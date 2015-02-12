/* --------------------------------------------------------------
	File:			AXProfiler.cpp
	Description:	Implementation for the AXProfiler class.  See AXProfiler.h for details.
	Date:			June 31, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXProfiler.h"
#include <cassert>

#include "..\\AXCore.h"

float AXProfileSample::_TimeRootBegin = 0.0f;
float AXProfileSample::_TimeRootEnd = 0.0f;
int AXProfileSample::_OpenSampleCount = 0;
int AXProfileSample::_LastOpenedSampleIndex = -1;
AXProfileSample::_ProfileSampleStats AXProfileSample::_ProfileSamples[MAX_PROFILER_SAMPLES];

AXProfileSample::AXProfileSample(std::string SampleName) {

	int StoreIndex = -1;

	//See if this sample already exists; if it doesn't store it in the first free slot we find
	for(int i=0; i<MAX_PROFILER_SAMPLES; i++) {
		if(!_ProfileSamples[i].IsValid) {
			if(StoreIndex<0) StoreIndex = i;
		}
		else if(_ProfileSamples[i].Name == SampleName) {
			assert(!_ProfileSamples[i].IsOpen && "Tried to profile a sample which was already being profiled!");

			_SampleIndex = i;
			_ParentIndex = _LastOpenedSampleIndex;

			_LastOpenedSampleIndex = i;

			_ProfileSamples[i].ParentCount = _OpenSampleCount;
			_OpenSampleCount++;

			_ProfileSamples[i].IsOpen = true;
			_ProfileSamples[i].CallCount++;
			_ProfileSamples[i].StartTime = AXBaseSystem::GetClock()->GetTime();

			if(_ParentIndex<0) _TimeRootBegin = _ProfileSamples[i].StartTime;

			return;
		}
	}

	assert(StoreIndex >= 0 && "Profiler has run out of sample slots!");


	_SampleIndex = StoreIndex;
	_ParentIndex = _LastOpenedSampleIndex;
	_LastOpenedSampleIndex = StoreIndex;

	_ProfileSamples[StoreIndex].ParentCount = _OpenSampleCount;
	_OpenSampleCount++;

	_ProfileSamples[StoreIndex].IsValid = true;
	_ProfileSamples[StoreIndex].Name = SampleName;
	_ProfileSamples[StoreIndex].IsOpen = true;
	_ProfileSamples[StoreIndex].CallCount = 1;

	_ProfileSamples[StoreIndex].AveragePc = 0.0f;
	_ProfileSamples[StoreIndex].MinPc = -1;
	_ProfileSamples[StoreIndex].MaxPc = -1;
	_ProfileSamples[StoreIndex].AverageCc = 0.0f;
	_ProfileSamples[StoreIndex].MinCc = -1;
	_ProfileSamples[StoreIndex].MaxCc = -1;

	_ProfileSamples[StoreIndex].TotalTime = 0.0f;
	_ProfileSamples[StoreIndex].ChildTime = 0.0f;
	_ProfileSamples[StoreIndex].StartTime = AXBaseSystem::GetClock()->GetTime();

	if(_ParentIndex<0) _TimeRootBegin = _ProfileSamples[StoreIndex].StartTime;
}

AXProfileSample::~AXProfileSample() {
	float fEndTime = AXBaseSystem::GetClock()->GetTime();
	_ProfileSamples[_SampleIndex].IsOpen = false;
	float fTimeTaken = fEndTime - _ProfileSamples[_SampleIndex].StartTime;

	if(_ParentIndex>=0) {
		_ProfileSamples[_ParentIndex].ChildTime += fTimeTaken;
	}
	else {
		//no parent, so this is the end of the main loop sample
		_TimeRootEnd = fEndTime;
	}
	_ProfileSamples[_SampleIndex].TotalTime += fTimeTaken;
	_LastOpenedSampleIndex = _ParentIndex;
	_OpenSampleCount--;
}


void AXProfileSample::Output() {

	AXLog->Write(MESSAGE_PROFILE, "\nAliquiX Profile Sample (%2.5f seconds)", _TimeRootEnd-_TimeRootBegin);
	AXLog->Write(MESSAGE_PROFILE, "pMin\t| pAvg\t| pMax\t| cMin\t| cAvg\t| cMax\t| Name");
	AXLog->Write(MESSAGE_PROFILE, "-----------------------------------------------------------------");

	for(int i=0; i<MAX_PROFILER_SAMPLES; i++) {
		if(_ProfileSamples[i].IsValid) {

		float SampleTime, Percentage;

		float TotalTimeTaken = _TimeRootEnd - _TimeRootBegin;
		if(TotalTimeTaken < 0.00000001f) TotalTimeTaken = 0.00000001f;

		// Calculate the time spend on the sample itself (excluding children)
		SampleTime = _ProfileSamples[i].TotalTime - _ProfileSamples[i].ChildTime;
		Percentage = (SampleTime / ( TotalTimeTaken )) * 100.0f;

		// Calculate percentage statistics
		float TotalPc;
		TotalPc = _ProfileSamples[i].AveragePc * _ProfileSamples[i].DataCount;
		TotalPc += Percentage;
		_ProfileSamples[i].AveragePc = TotalPc/(_ProfileSamples[i].DataCount+1);
		if((_ProfileSamples[i].MinPc == -1) || (Percentage < _ProfileSamples[i].MinPc)) _ProfileSamples[i].MinPc = Percentage;
		if((_ProfileSamples[i].MaxPc == -1) || (Percentage > _ProfileSamples[i].MaxPc)) _ProfileSamples[i].MaxPc = Percentage;

		// Calculate call count statistics
		float TotalCc;
		TotalCc = _ProfileSamples[i].AverageCc * _ProfileSamples[i].DataCount;
		TotalCc += _ProfileSamples[i].CallCount;
		_ProfileSamples[i].AverageCc = TotalCc/(float)(_ProfileSamples[i].DataCount+1);
		if((_ProfileSamples[i].MinCc == -1) || (Percentage < _ProfileSamples[i].MinCc)) _ProfileSamples[i].MinCc = (float)_ProfileSamples[i].CallCount;
		if((_ProfileSamples[i].MaxCc == -1) || (Percentage > _ProfileSamples[i].MaxCc)) _ProfileSamples[i].MaxCc = (float)_ProfileSamples[i].CallCount;

		_ProfileSamples[i].DataCount++;

		// Log the profile
		std::string IndentedName = "";
		for(int j=0; j<_ProfileSamples[i].ParentCount; j++) {
			IndentedName += " ";
		}
		IndentedName += _ProfileSamples[i].Name;

		AXLog->Write(MESSAGE_PROFILE, "%3.1f\t| %3.1f\t| %3.1f\t| %3.1f\t| %3.1f\t| %3.1f\t| %s (%f seconds)",
					_ProfileSamples[i].MinPc,
					_ProfileSamples[i].AveragePc,
					_ProfileSamples[i].MaxPc,
					_ProfileSamples[i].MinCc,
					_ProfileSamples[i].AverageCc,
					_ProfileSamples[i].MaxCc,
					IndentedName.c_str(),
					SampleTime);

		// Reset the sample for next time
		_ProfileSamples[i].CallCount = 0;
		_ProfileSamples[i].TotalTime = 0;
		_ProfileSamples[i].ChildTime = 0;

		}
	}

	AXLog->Write(MESSAGE_PROFILE, "-----------------------------------------------------------------");
}

void AXProfileSample::ResetSample(std::string Name) {
	for(int i=0; i<MAX_PROFILER_SAMPLES; i++) {
		if(_ProfileSamples[i].IsValid && _ProfileSamples[i].Name == Name) {
			_ProfileSamples[i].IsValid = 0;
		}
	}
}

void AXProfileSample::ResetAll() {
	for(int i=0; i<MAX_PROFILER_SAMPLES; i++) {
		_ProfileSamples[i].IsValid = 0;
	}
}


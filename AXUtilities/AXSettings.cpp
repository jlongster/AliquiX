/* --------------------------------------------------------------
	File:			AXSettings.cpp
	Description:	Implementation for the AXSettings class.  See AXSettings.h for details.
	Date:			July 1, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"

#include "..\\AXCommon.h"
#include "AXSettings.h"
#include "AXStringFunctions.h"

#include <fstream>
#include <vector>
using std::string;

string AXSettings::FileName = "settings.txt";

AXSettings::AXSettings() {
}

AXSettings::~AXSettings() {
}

void AXSettings::SetFileName(string Name) {
	FileName = Name;
}

const char* AXSettings::GetFileName() {
	return FileName.c_str();
}

AXResult AXSettings::ParseSettings(string SectionName) {
	std::ifstream in(FileName.c_str());
	char LineBuf[1024];

	string Name;
	boost::any Value;

	_SectionName = SectionName;

	if(!in.is_open())
		return AXFILENOTFOUND;

	while(!in.eof()) {
		in.getline(LineBuf, 1024);
		string Line(LineBuf);

		if(Line == "[" + _SectionName + "]") break;
	}

	if(!in.eof()) {
		while(!in.eof()) {
			in.getline(LineBuf, 1024);
			string Line(LineBuf);

			if(Line == "" || Line[0] == '[') break;

			string::size_type eqPos = Line.find_first_of('=');
			if(eqPos != string::npos) {
				Name = Line.substr(0, eqPos);
				Value = AXStringConvertToAny(Line.substr(eqPos + 1));
			}
			else {
				Name = Line;
				Value = 0;
			}
			//set the variable
			SetSetting(Name, Value);
		}
	}

	in.close();

	return AXSUCCESS;
}

AXResult AXSettings::WriteSettings() {
	std::ifstream in(FileName.c_str());
	char LineBuf[1024];

	if(_SectionName == "") { assert(0 && "settings has no name!"); }

	std::vector<string> File;
	std::vector<string>::iterator FileIt;

	if(!in.is_open()) {
		File.push_back("[" + _SectionName + "]");
	}
	else {
		int SectionFound = 0;
		while(!in.eof()) {
			in.getline(LineBuf, 1024);
			string Line(LineBuf);

			if(Line == "[" + _SectionName + "]") SectionFound = 1;
			if(Line != "") File.push_back(Line);
		}
		if(!SectionFound) File.push_back("[" + _SectionName + "]");

		in.close();
	}

	std::ofstream out(FileName.c_str());
	StringAnyMap::iterator it;

	if(!out.is_open())
		return AXFAILURE;

	FileIt = File.begin();
	while(FileIt != File.end()) {
		string Line = *FileIt;
		out << Line << "\n";
		FileIt++;
		
		if(Line == ("[" + _SectionName + "]")) {
			for(it = _SettingsMap.begin(); it != _SettingsMap.end(); it++) {
				string Value = AXStringConvertFromAny((*it).second);
				if(Value != "") out << (*it).first.c_str() << "=" << Value.c_str() << "\n";
			}
			while(FileIt != File.end() && (*FileIt) != "" && (*FileIt)[0] != '[') {
				FileIt++;
			}
		}

	}

	out.close();

	return AXSUCCESS;
}

void AXSettings::DebugSettings() {
	StringAnyMap::iterator it;
	for(it = _SettingsMap.begin(); it != _SettingsMap.end(); it++) {
		string Value = AXStringConvertFromAny((*it).second);
		if(Value != "") {
			LOG_DBG2("Setting: %s=%s", (*it).first.c_str(), Value.c_str());
		}
	}



}


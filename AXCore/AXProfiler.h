/* --------------------------------------------------------------
	File:			AXProfiler.h
	Description:	This is a class that enables easy and quick profiling of any parts of the system at once.

					It hinges on the lifetime of an instance.  To use it simply create an instance on the stack
					which is destroyed when the code block exits, and the report for that sample will show
					statistics on the code executed while the block is alive.  Hierarchal samples are
					supported, and parent blocks do not include children blocks in their sampling.

					You can control the lifetime of a sample with arbitrary code blocks.  A typical use
					may look like this:

					{
					PROFILE("Main Loop");

					for(int i=0; i<OBJECTS; i++) {
						objects[i].draw();
						...
					}

					}

					There is a handy macro named "PROFILE" that creates the sample on the stack for you.

					With every loop of the application, statistics are spit out to the file called "logProfile.txt".
					Never give credit to the stats at the very beginning or end of the program, as they are skewed
					because of how things run differently at the beginning and end of the program.
					

	Date:			July 31, 2005
	Author:			James Long
-------------------------------------------------------------- */


#if !defined(AXPROFILER_INCLUDE)
#define AXPROFILER_INCLUDE

static const int MAX_PROFILER_SAMPLES = 512;

class AXProfileSample {
public:
	AXProfileSample(string SampleName);
	~AXProfileSample();

	static void Output();

	static void ResetSample(string SampleName);
	static void ResetAll();

protected:
	//index into the array of samples
	int _SampleIndex;
	int _ParentIndex;

	static struct _ProfileSampleStats {
		_ProfileSampleStats() {
			IsValid = false;
			DataCount = 0;
			AveragePc = 0.0f;
			MinPc = MaxPc = -1;
			MinCc = MaxCc = -1;
		}

		bool IsValid;    //whether or not this sample is valid to be used
		bool IsOpen;     //is this sample currently being profiled?
		unsigned int CallCount; //number of times this sample has been executed, this frame
		string Name;     //name of the sample

		float StartTime;  //starting time on the clock, in seconds
		float TotalTime;  //total time recorded across all executions of this sample, this frame
		float ChildTime;  //total time taken by children of this sample, this frame

		int ParentCount;  //number of parents this sample has (useful for neat indenting)

		float AveragePc;  //average percentage of game loop time taken up
		float MinPc;      //minimum percentage of game loop time taken up
		float MaxPc;      //maximum percentage of game loop time taken up

		float AverageCc;  //average percentage of game loop time taken up
		float MinCc;      //minimum percentage of game loop time taken up
		float MaxCc;      //maximum percentage of game loop time taken up

		unsigned long DataCount; //number of times values have been stored since
								//sample creation/reset
	} _ProfileSamples[MAX_PROFILER_SAMPLES];

	static int _LastOpenedSampleIndex;
	static int _OpenSampleCount;
	static float _TimeRootBegin, _TimeRootEnd;
};

#ifdef _DEBUG
#define AXPROFILE(SampleName) AXProfileSample _Profile_Sample(SampleName);
#else
#define AXPROFILE(SampleName)
#endif

#endif


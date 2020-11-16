#ifndef _FILE_IO_H
#define _FILE_IO_H

#include <Windows.h>
#include <limits.h>
#include "DataDefinitions.h"

#ifdef _FILE_IO
static const char *INPUT_KEYWORDS[] =
{
 "MinCurrent",
 "MaxCurrent",
 "NumberOfCurrentPoints",
 "MaxTime",
 "NumberOfTimePoints",
 "WaveformFile",
 "WaveformCurrent",
 "LineZ",
 "LineLength",
 "LineDampingTau",
 "ResonatorLocation",
 "ResonatorFrequency",
 "ResonatorR",
 "ResonatorAlpha", 
 "ResonatorC",
 "ResonatorL",
 "LineNpoints",
 "NumberOfThreads",
 "JunctionLocation",
 "Resonator",
 "EndResonator"
};
static size_t NUMBER_OF_INPUT_KEYWORDS =
      sizeof(INPUT_KEYWORDS)/sizeof(*INPUT_KEYWORDS);

enum struct  INPUT_KEYWORD_INDEX {
	_MinCurrent,
	_MaxCurrent,
	_NumberOfCurrentPoints,
	_MaxTime,
	_NumberOfTimePoints,
	_WaveFormFile,
	_WaveFormCurrent,
	_LineZ,
	_LineLength,
	_LineDampingTau,
	_ResonatorLocation,
	_ResonatorFrequency,
	_ResonatorR,
	_ResonatorAlpha,
	_ResonatorC,
	_ResonatorL,
	_LineNpoints,
	_NumberOfThreads,
	_JunctionLocation,
	_Resonator,
	_EndResonator
	
};




static const char *ERROR_MESSAGES[] =
{
	"Wrong \"MinCurrent\" parameter",
	"Wrong \"MaxCurrent\" parameter",
	"Wrong \"NumberOfCurrentPoints\" parameter",
	"Wrong \"MaxTime\" parameter",
	"Wrong \"NumberOfTimePoints\" parameter",
	"Wrong \"WaveFormFile\" parameter",
	"Wrong \"WaveFormCurrent\" parameter",
	"Wrong \"LineZ\" parameter",
	"Wrong \"LineLength\" parameter",
	"Wrong \"LineDampingTau\" parameter",
	"Wrong \"ResonatorLocation\" parameter",
	"Wrong \"ResonatorFrequency\" parameter",
	"Wrong \"ResonatorR\" parameter",
	"Wrong \"ResonatorAlpha\" parameter",
	"Wrong \"ResonatorC\" parameter",
	"Wrong \"ResonatorL\" parameter",
	"Wrong \"LineNpoints\" parameter",
	"Error in Resonator Circuit definition",
	"Number of threads is too big",
	"Wrong \"JunctionLocation\" parameter"
};


static size_t NUMBER_OF_ERROR_MESSAGES = sizeof(ERROR_MESSAGES) / sizeof(*ERROR_MESSAGES);

enum struct  ERROR_MESSAGE_INDEX {
	_MinCurrent,
	_MaxCurrent,
	_NumberOfCurrentPoints,
	_MaxTime,
	_NumberOfTimePoints,
	_WaveFormFile,
	_WaveFormCurrent,
	_LineZ,
	_LineLength,
	_LineDampingTau,
	_ResonatorLocation,
	_ResonatorFrequency,
	_ResonatorR,
	_ResonatorAlpha,
	_ResonatorC,
	_ResonatorL,
	_LineNpoints,
	_ResonatorError,
	_NumberOfThreadsError,
	_JunctionLocation
};

static InputData InitialData =
{
	DBL_MAX,           // MinCurrent
	DBL_MAX,           // MaxCurrent
	INT_MAX,           // NumberOfCurrentPoints
	DBL_MAX,           // MaxTime
	INT_MAX,           // NumberOfTimePoints
	NULL,              // waveformFile     
	DBL_MAX,           // waveformCurrent
	DBL_MAX,           // lineZ;
	DBL_MAX,           // lineLength;
	DBL_MAX,           // lineDampingTau;
	INT_MAX,           // numberOfLinePoints;
	DBL_MAX,           // junctionLocation
	1,                 // numberOfThreads 
	0,                 // numberOfResonators
	NULL,              // resonators
	NULL,              // voltage
	NULL,              // current
	NULL,              // rd
	NULL,              // voltage0
	NULL,              // x
	NULL               // waveform
};



#endif
int SetupRead(char *filename, InputData *data);
const char *GetErrorMessage(size_t index);
size_t VerifyInputData(InputData *data, size_t **errors);
#endif



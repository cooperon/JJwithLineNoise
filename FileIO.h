#ifndef _FILE_IO_H
#define _FILE_IO_H
#endif
#include <Windows.h>
#include <limits.h>
#define _PI 3.141592653589793238462643
#define _2PI 6.283185307179586476925287
#define _PI_4 7.853981633974483e-01
#define _PI_2 1.5707963267948966192313
#define _TOL 1.e-10
#define _ROOT_STEP 0.1


typedef struct InputData
{
 double minCurrent;
 double maxCurrent;
 unsigned int numberOfCurrentPoints;
 double maxTime;
 unsigned int numberOfTimePoints;
 char* waveFormFile;
 unsigned int waveFormPoint;
 double lineZ;
 double lineLength;
 double lineDampingTau;
 double resonatorFrequency;
 double resonatorAlpha;
 double resonatorR;
 unsigned int numberOfLinePoints;
 double resonatorLocation;
} InputData;

typedef struct ComputationalData
{
	double *fi;
	double *fiOld;
	double *lineCurrent;
	double *hi;
	double *jjFunc;
	double *fiR;
	double *iR;
} ComputationalData;

typedef struct AuxiliaryData
{
	InputData *inputData;
	double dt;
	double current;
	unsigned int mResonator;

	double *a;
	double *al;
	double *ar;
	double *b;
	double *alpha;
	double *beta;
	double bFi;
	double bIminusI;
	double bmFiR;
	double bmIr;
	double bnFi;
	double bnI;
	double inDfi;
	double dIdFi;
	double irFiFiR;
	double irIr;
	double firIr;
	double b0JJ;
	double b0I1;
	double i1I1;
	double i1Dfi;
	
}AuxiliaryData;


#ifdef _FILE_IO
static const char *INPUT_KEYWORDS[] =
{
 "MinCurrent",
 "MaxCurrent",
 "NumberOfCurrentPoints",
 "MaxTime",
 "NumberOfTimePoints",
 "WaveFormFile",
 "WaveFormPoint",
 "LineZ",
 "LineLength",
 "LineDampingTau",
 "ResonatorLocation",
 "ResonatorFrequency",
 "ResonatorR",
 "ResonatorAlpha", 
 "ResonatorC",
 "ResonatorL",
 "LineNpoints"
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
	_WaveFormPoint,
	_LineZ,
	_LineLength,
	_LineDampingTau,
	_ResonatorLocation,
	_ResonatorFrequency,
	_ResonatorR,
	_ResonatorAlpha,
	_ResonatorC,
	_ResonatorL,
	_LineNpoints
	
};



static InputData InitialData =
{
 DBL_MAX,           // MinCurrent
 DBL_MAX,           // MaxCurrent
 INT_MAX,           // NumberOfCurrentPoints
 DBL_MAX,           // MaxTime
 INT_MAX,           // NumberOfTimePoints
 NULL,              // waveFormFile     
 UINT_MAX,          // waveFormPoint
 DBL_MAX,           // lineZ;
 DBL_MAX,           // lineLength;
 DBL_MAX,           // lineDampingTau;
 DBL_MAX,           // resonatorFrequency;
 DBL_MAX,           // resonatorAlpha;
 DBL_MAX,           // resonatorR;
 INT_MAX,           // numberOfLinePoints;
 DBL_MAX            // resonatorLocation;
};

static const char *ERROR_MESSAGES[] =
{
	"Wrong \"MinCurrent\" parameter",
	"Wrong \"MaxCurrent\" parameter",
	"Wrong \"NumberOfCurrentPoints\" parameter",
	"Wrong \"MaxTime\" parameter",
	"Wrong \"NumberOfTimePoints\" parameter",
	"Wrong \"WaveFormFile\" parameter",
	"Wrong \"WaveFormPoint\" parameter",
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
	"Error in Resonator Circuit definition"
};


static size_t NUMBER_OF_ERROR_MESSAGES = sizeof(ERROR_MESSAGES) / sizeof(*ERROR_MESSAGES);

enum struct  ERROR_MESSAGE_INDEX {
	_MinCurrent,
	_MaxCurrent,
	_NumberOfCurrentPoints,
	_MaxTime,
	_NumberOfTimePoints,
	_WaveFormFile,
	_WaveFormPoint,
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
	_ResonatorError
};


#endif
int SetupRead(char *filename, InputData *data);
const char *GetErrorMessage(size_t index);
size_t VerifyInputData(InputData *data, size_t **errors);



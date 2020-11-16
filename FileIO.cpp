#define _FILE_IO

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <float.h> 
#include <limits.h> 
#include <math.h>
#include "utils.h"
#include "fileio.h"



int SetupRead(char *filename, InputData *data)
{
	FILE *fp = NULL;
	int iret, i;
	char buffer[_FILE_INPUT_BUFFER_LENGTH + 1];
	char *str, *param;
	double dummy_f;
	int dummy_i;

	iret = 0;
	int index = -1;
	*data = InitialData;
	bool resonatorFlag = false;
	unsigned int nResonators = 0;
	Resonator *resonators = NULL;
	double resonatorC = DBL_MAX, resonatorL = DBL_MAX;
	if (!(fp = fopen(filename, "r"))) { iret = -1; goto _END; }
	while (fgets(buffer, _FILE_INPUT_BUFFER_LENGTH, fp))
	{
		str = RemoveBlanks(buffer, 0);
		if ((index = ParameterItem(str, INPUT_KEYWORDS, NUMBER_OF_INPUT_KEYWORDS, &param)) < 0)
			continue;
		INPUT_KEYWORD_INDEX keyword = (INPUT_KEYWORD_INDEX)index;
		switch (keyword)
		{
		case INPUT_KEYWORD_INDEX::_MinCurrent:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->minCurrent = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_MaxCurrent:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->maxCurrent = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_NumberOfCurrentPoints:
			if (ReadIntParam(param, &dummy_i) >= 0)
				if (dummy_i >= 0)
					data->numberOfCurrentPoints = (size_t)dummy_i;
			break;

		case INPUT_KEYWORD_INDEX::_MaxTime:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->maxTime = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_NumberOfTimePoints:
			if (ReadIntParam(param, &dummy_i) >= 0)
				if (dummy_i >= 0)
					data->numberOfTimePoints = (size_t)dummy_i;
			break;

		case INPUT_KEYWORD_INDEX::_WaveFormFile:
			if (!param) break;
			if (!*param) break;
			data->waveformFile = (char*)calloc(strlen(param) + 1, sizeof(char));
			strcpy(data->waveformFile, param);
			break;

		case INPUT_KEYWORD_INDEX::_WaveFormCurrent:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->waveformCurrent = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_LineZ:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->lineZ = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_LineLength:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->lineLength = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_LineDampingTau:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->lineDampingTau = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_ResonatorFrequency:
			if (!resonatorFlag) break;
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				resonators[nResonators - 1].frequency = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_ResonatorR:
			if (!resonatorFlag) break;
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				resonators[nResonators - 1].R = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_ResonatorAlpha:
			if (!resonatorFlag) break;
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				resonators[nResonators - 1].alpha = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_ResonatorC:
			if (!resonatorFlag) break;
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				resonators[nResonators - 1].C = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_ResonatorL:
			if (!resonatorFlag) break;
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				resonators[nResonators - 1].L = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_LineNpoints:
			if (ReadIntParam(param, &dummy_i) >= 0)
				if (dummy_i > 0)
					data->numberOfLinePoints = dummy_i;
			break;

		case INPUT_KEYWORD_INDEX::_NumberOfThreads:
			if (ReadIntParam(param, &dummy_i) >= 0)
				if (dummy_i > 0)
					data->numberOfThreads = dummy_i;
			break;

		case INPUT_KEYWORD_INDEX::_ResonatorLocation:
			if (!resonatorFlag) break;
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				resonators[nResonators - 1].location = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_JunctionLocation:
			if (ReadDoubleParam(param, &dummy_f) >= 0)
				data->junctionLocation = dummy_f;
			break;

		case INPUT_KEYWORD_INDEX::_Resonator:
			resonatorFlag = true;
			nResonators++;
			resonators = (Resonator*)realloc(resonators, nResonators * sizeof(Resonator));
			resonators[nResonators - 1].R = DBL_MAX;
			resonators[nResonators - 1].L = DBL_MAX;
			resonators[nResonators - 1].C = DBL_MAX;
			resonators[nResonators - 1].alpha = DBL_MAX;
			resonators[nResonators - 1].frequency = DBL_MAX;
			resonators[nResonators - 1].location = DBL_MAX;
			resonators[nResonators - 1].pointNumber = -1;
			break;

		case INPUT_KEYWORD_INDEX::_EndResonator:
			resonatorFlag = false;
			break;

		default: break;
		}
	}
	if (nResonators > 0)
	{
		for (i = 0; i < nResonators; i++)
		{
			if (resonators[i].R == DBL_MAX)
			{
				printf("Resonator # %d: resistance have not defined\n", i + 1);
				iret = -1;
			}
			else
			{
				if (resonators[i].L != DBL_MAX && resonators[i].C != DBL_MAX)
				{
					resonators[i].frequency = 1.L / sqrt(resonators[i].L*resonators[i].C);
					resonators[i].alpha = resonators[i].R*sqrt(resonators[i].C / resonators[i].L);
					printf("Resonator circuit #%d: frequency=%le, alpha=%le\n",
						i + 1, resonators[i].frequency, resonators[i].alpha);
				}
				else
					if (resonators[i].alpha != DBL_MAX && resonators[i].frequency != DBL_MAX)
					{
						resonators[i].L = resonators[i].R / (resonators[i].alpha*resonators[i].frequency);
						resonators[i].C = resonators[i].alpha*sqrt(resonators[i].frequency * resonators[i].R);
						printf("Resonator circuit #%d: L=%le C=%le\n",
							i + 1, resonators[i].L, resonators[i].C);
					}
					else
					{
						printf("Resonator # %d: parameters have not defined\n", i + 1);
						iret = -1;
					}
				data->resonators = resonators;
				data->numberOfResonators = nResonators;
			}
		}
	}
_END:
	if (fp) fclose(fp);
	return iret;
}

size_t VerifyInputData(InputData *data, size_t **errors)
{
	size_t nerrors = 0;
	*errors = NULL;
	SYSTEM_INFO si;
	int i;
	GetSystemInfo(&si);
	//si.dwNumberOfProcessors = 1;
	if ((data->maxCurrent == DBL_MAX) || (data->maxCurrent <= 0))
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_MaxCurrent;
		nerrors++;
	}


	if ((data->minCurrent == DBL_MAX) || (data->minCurrent <= 0))
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_MinCurrent;
		nerrors++;
	}

	if ((data->maxTime == DBL_MAX) || (data->maxTime <= 0))
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_MaxTime;
		nerrors++;
	}
	if ((data->waveformCurrent < data->minCurrent) || (data->waveformCurrent > data->maxCurrent))
	{
		if (data->waveformCurrent != DBL_MAX)
		{
			data->waveformCurrent = DBL_MAX;
			free(data->waveformFile);
			data->waveformFile = NULL;
			printf("Wrong WaveFormCurrent. No waveform will be written\n");
		}
	}

	if (data->numberOfTimePoints == 0)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_NumberOfTimePoints;
		nerrors++;
	}
	if (data->lineZ == DBL_MAX)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineZ;
		nerrors++;
	}

	if (data->lineLength == DBL_MAX)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineLength;
		nerrors++;
	}

	if (data->lineDampingTau == DBL_MAX)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineDampingTau;
		nerrors++;
	}

	if (data->numberOfLinePoints ==INT_MAX || data->numberOfLinePoints<3)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineNpoints;
		nerrors++;
	}

	
	if (data->lineLength == DBL_MAX || data->lineLength <0.)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineLength;
		nerrors++;
	}
	for (i = 0; i < data->numberOfResonators; i++)
	{
		if (data->resonators[i].location == DBL_MAX || data->resonators[i].location <0. || data->resonators[i].location> data->lineLength)
		{
			*errors = (size_t*)realloc(*errors, (nerrors + 1) * sizeof(**errors));
			(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_ResonatorLocation;
			nerrors++;
		}
	}

	if (data->junctionLocation == DBL_MAX || data->junctionLocation <0. || data->junctionLocation>data->lineLength)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1) * sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_JunctionLocation;
		nerrors++;
	}


	if (data->numberOfThreads > si.dwNumberOfProcessors)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1) * sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_NumberOfThreadsError;
		nerrors++;
	}



	return nerrors;

}



const char *GetErrorMessage(size_t index)
{
	if (index >= NUMBER_OF_ERROR_MESSAGES) return NULL;
	return ERROR_MESSAGES[index];
}






 
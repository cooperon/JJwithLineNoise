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
 int iret;
 char buffer[_FILE_INPUT_BUFFER_LENGTH + 1];
 char *str, *param;
 double dummy_f;
 int dummy_i;

 iret = 0;
 int index = -1;
 *data = InitialData;
 double resonatorC=DBL_MAX, resonatorL=DBL_MAX;
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
		 data->waveFormFile = (char*)calloc(strlen(param) + 1, sizeof(char));
		 strcpy(data->waveFormFile, param);
		 break;

	 case INPUT_KEYWORD_INDEX::_WaveFormPoint:
		 if (ReadIntParam(param, &dummy_i) >= 0)
		 {
			 data->waveFormPoint = dummy_i;
			 if (data->waveFormPoint < 0) data->waveFormPoint = 0;
		 }
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
		 if (ReadDoubleParam(param, &dummy_f) >= 0)
			 data->resonatorFrequency = dummy_f;
		 break;

	 case INPUT_KEYWORD_INDEX::_ResonatorR:
		 if (ReadDoubleParam(param, &dummy_f) >= 0)
			 data->resonatorR = dummy_f;
		 break;

	 case INPUT_KEYWORD_INDEX::_ResonatorAlpha:
		 if (ReadDoubleParam(param, &dummy_f) >= 0)
			 data->resonatorAlpha = dummy_f;
		 break;

	 case INPUT_KEYWORD_INDEX::_ResonatorC:
		 if (ReadDoubleParam(param, &dummy_f) >= 0)
			 resonatorC = dummy_f;
		 break;

	 case INPUT_KEYWORD_INDEX::_ResonatorL:
		 if (ReadDoubleParam(param, &dummy_f) >= 0)
			 resonatorL = dummy_f;
		 break;

	 case INPUT_KEYWORD_INDEX::_LineNpoints:
		 if (ReadIntParam(param, &dummy_i) >= 0)
		  if(dummy_i>0)
			 data->numberOfLinePoints = dummy_i;
		 break;

	 case INPUT_KEYWORD_INDEX::_ResonatorLocation:
		 if (ReadDoubleParam(param, &dummy_f) >= 0)
			 data->resonatorLocation = dummy_f;
		 break;

	 default: break;
	 }
 }
 if (data->resonatorR != DBL_MAX)
 {
	 if (resonatorL != DBL_MAX && resonatorC != DBL_MAX)
	 {
		 data->resonatorFrequency = 1.L / sqrt(resonatorL*resonatorC);
		 data->resonatorAlpha = data->resonatorR*sqrt(resonatorC / resonatorL);
		 printf("Resonator circuit: L-C circuit parameters: L=%le, C=%le\n", resonatorL, resonatorC);
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

	if (data->resonatorR == DBL_MAX )
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_ResonatorR;
		nerrors++;
	}

	if (data->resonatorAlpha == DBL_MAX || data->resonatorFrequency == DBL_MAX)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_ResonatorError;
		nerrors++;
	}

	if (data->lineLength == DBL_MAX || data->lineLength <0.)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineLength;
		nerrors++;
	}

	if (data->resonatorLocation == DBL_MAX || data->resonatorLocation <0. || data->resonatorLocation>data->lineLength)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_ResonatorLocation;
		nerrors++;
	}


	return nerrors;

}



const char *GetErrorMessage(size_t index)
{
	if (index >= NUMBER_OF_ERROR_MESSAGES) return NULL;
	return ERROR_MESSAGES[index];
}






 
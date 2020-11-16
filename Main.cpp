#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
#include <math.h>
#include <windows.h>
#include "FileIO.h"
#include "MainProg.h"
#include "CommentOutput.h"
#include "TimeFuncs.h"




int main(int argc, char *argv[])
{
	size_t i, nerrors, *errors = NULL;
	InputData inputData;
	FILE *fp = NULL;
	double dx;

	if (argc < 3)
	{
		printf("Usage: RSJline <input file> <output file>");
		exit(0);
	}

	if (SetupRead(argv[1], &inputData) < 0)
	{
		printf("ERROR: wrong input file\n");
		exit(0);
	}
	printf("Dimensionless RSJ model with tansmission line\n");
	InitializeTimeMeasure();

	if ((nerrors = VerifyInputData(&inputData, &errors)))
	{
		for (i = 0; i<nerrors; i++)
			printf("%s\n", GetErrorMessage(errors[i]));
		free(errors);
		exit(0);
	}
	CommentOutput(&inputData);

	inputData.voltage=(double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	inputData.current = (double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	inputData.rd = (double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	inputData.voltage0 = (double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	if (inputData.waveformFile)
	{
		inputData.x = (double*)calloc(inputData.numberOfLinePoints, sizeof(double));
		inputData.waveform = (double*)calloc(inputData.numberOfLinePoints, sizeof(double));
	}
	else
	{
		inputData.x = NULL;
		inputData.waveform = NULL;
	}

	if (!MainProg(&inputData))
	{
		fp = fopen(argv[2], "w");
		fprintf(fp, "Voltage Current Rd Voltage0 \n");
		for (i = 0; i < inputData.numberOfCurrentPoints; i++)
			fprintf(fp, "%le %le %le %le\n", inputData.voltage[i], inputData.current[i], inputData.rd[i], inputData.voltage0[i]);
		fclose(fp);
		if (inputData.waveformFile)
		{
			dx = inputData.lineLength / (inputData.numberOfLinePoints - 1);
			fp = fopen(inputData.waveformFile, "w");
			fprintf(fp, "X Voltage\n");
			for (i = 0; i < inputData.numberOfLinePoints; i++)
			{
				fprintf(fp, "%le %le\n", dx*i, inputData.waveform[i]);
			}
			fclose(fp);
		}
	}
	else
	 SetComment(_COMMENT_FILENAME, "===ERROR==\n");
	SetComment(_COMMENT_FILENAME, "==========================================\n");
	printf("Calculation time: %f s\n", GetRunTime());
	free(inputData.voltage);
	free(inputData.current);
	free(inputData.rd);
	free(inputData.voltage0);
	free(inputData.x);
	free(inputData.waveform);
	free(inputData.resonators);
	free(inputData.waveformFile);
	return 0;
}

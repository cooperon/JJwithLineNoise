#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
//#include <cfloat>
#include <math.h>
#include <windows.h>
#include "FileIO.h"
#include "Main.h"
#include "CommentOutput.h"
#include "Lineq.h"





void ParametersCalculation(AuxiliaryData *auxiliaryData);
int NextStep(int k, AuxiliaryData *aData, ComputationalData*cData);
void Derivative(double *function, int numberOfPoints, double step, double *derivative);



double fi_tmp = 0., hi_tmp=0.;
double *voltages_tmp = NULL;
double hi_tmp_old = 0.;
double il_tmp=0.;
double dx_tmp;

LARGE_INTEGER lpFrequency;

#define _NUMBER_OF_VECTOR_VARIABLES  3
#define _NUMBER_OF_SCALAR_VARIABLES 4
#define _SIZE_OF_DATA_BUFFER  inputData.numberOfLinePoints*_NUMBER_OF_VECTOR_VARIABLES+ _NUMBER_OF_SCALAR_VARIABLES



int main(int argc, char *argv[])
{
	double *voltages, *currents, *rDs;
	double dI;

	double *dataBuffer;
	double *dataBufferCorr;
	int iret = 0;
	double t = 0.;
	double v;
	size_t i, j, nerrors, *errors = NULL;
	LARGE_INTEGER lpPerformanceCount;
	LARGE_INTEGER initialCount;
	InputData inputData;
	AuxiliaryData auxiliaryData;
	ComputationalData computationalData, computationalDataCorr;
	double calculationTime;
	double hi_old = 0., hi0=0.;

	auxiliaryData.inputData = &inputData;

	printf("Dimensionless RSJ model with tansmission line\n");
	QueryPerformanceFrequency(&lpFrequency);
	QueryPerformanceCounter(&lpPerformanceCount);
	initialCount = lpPerformanceCount;
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


	if ((nerrors = VerifyInputData(&inputData, &errors)))
	{
		for (i = 0; i<nerrors; i++)
			printf("%s\n", GetErrorMessage(errors[i]));
		free(errors);
		exit(0);
	}

	CommentOutput(argv[1], argv[2]);
	ParametersCalculation(&auxiliaryData);
	if (inputData.numberOfCurrentPoints > 1)
		dI = (inputData.maxCurrent - inputData.minCurrent) / (inputData.numberOfCurrentPoints - 1);
	else dI = 0.;
	currents = (double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	voltages = (double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	voltages_tmp = (double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	rDs = (double*)calloc(inputData.numberOfCurrentPoints, sizeof(double));
	double *waveform = NULL;
	if (inputData.waveFormFile)
		waveform = (double*)calloc(inputData.numberOfTimePoints, sizeof(double));

	dataBuffer = (double*)calloc(_SIZE_OF_DATA_BUFFER, sizeof(double));
	dataBufferCorr = (double*)calloc(_SIZE_OF_DATA_BUFFER, sizeof(double));
	computationalData.fi = dataBuffer;
	computationalData.fiOld = computationalData.fi + inputData.numberOfLinePoints;
	computationalData.lineCurrent = computationalData.fiOld + inputData.numberOfLinePoints;
	computationalData.hi = computationalData.lineCurrent + inputData.numberOfLinePoints;
	computationalData.jjFunc = computationalData.hi + 1;
	computationalData.fiR = computationalData.jjFunc + 1;
	computationalData.iR = computationalData.fiR + 1;
	computationalDataCorr.fi = dataBufferCorr;
	computationalDataCorr.fiOld = computationalDataCorr.fi + inputData.numberOfLinePoints;
	computationalDataCorr.lineCurrent = computationalDataCorr.fiOld + inputData.numberOfLinePoints;
	computationalDataCorr.hi = computationalDataCorr.lineCurrent + inputData.numberOfLinePoints;
	computationalDataCorr.jjFunc = computationalDataCorr.hi + 1;
	computationalDataCorr.fiR = computationalDataCorr.jjFunc + 1;
	computationalDataCorr.iR = computationalDataCorr.fiR + 1;

	auxiliaryData.current = inputData.minCurrent;
	hi_tmp_old = hi_old = 0.;



		for (i = 0; i < inputData.numberOfCurrentPoints; i++)
		{
			*computationalData.hi = hi_old;

			t = 0.;
			for (j = 0; j < inputData.numberOfTimePoints; j++)
			{
				if (iret = NextStep(i, &auxiliaryData, &computationalData)) break;
				//			if (inputData.waveFormFile && i == inputData.waveFormPoint)
				//				waveform[j] = computationalData.fi[0];
				t += auxiliaryData.dt;
				/*

							if (j == 20000)
							{
								int k;
								FILE *fp = fopen("Fi100.dat", "w");
								fprintf(fp, "X Fi I\n");
								for (k = 0; k < inputData.numberOfLinePoints; k++)
									fprintf(fp, "%le %le %le\n", k*inputData.lineLength / (inputData.numberOfLinePoints - 1), computationalData.fi[k], computationalData.lineCurrent[k]);
									fclose(fp);
									exit(0);
							}
				*/
			}

			//		double tMin = 0.;
					/*
					int addNpoints;

					double dt;
					double period;
					dt = auxiliaryData.dt;
					v = (*computationalData.hi- hi_old )/ t;
					if (fabs(v) > _V_MIN)
					{
						t = 0.;
						period = 2 * _PI / v;
						addNpoints = (int)(period / auxiliaryData.dt);
						if (addNpoints < _MIN_POINT_FOR_PERIOD)
						{
							addNpoints = _MIN_POINT_FOR_PERIOD;
							dt = period / (addNpoints - 1);
						}
						memcpy(dataBufferCorr, dataBuffer, _SIZE_OF_DATA_BUFFER*sizeof(double));
						for (j = 0; j < addNpoints; j++)
						{
							if (iret = NextStep(&auxiliaryData, &computationalData)) break;
							t += dt;
							if (computationalData.fi[0] < computationalDataCorr.fi[0])
							{
								tMin = t;
								memcpy(dataBufferCorr, dataBuffer, _SIZE_OF_DATA_BUFFER*sizeof(double));
							}

						}
						memcpy(dataBuffer, dataBufferCorr, _SIZE_OF_DATA_BUFFER*sizeof(double));

					}

				*/

				//	if ((j / 1000) * 1000 == j) printf(".");
				//printf("\n");


			currents[i] = auxiliaryData.current;
			voltages[i] = (*computationalData.hi - hi_old) / (inputData.maxTime /*+ tMin*/);
			voltages_tmp[i] = (hi_tmp - hi_tmp_old) / inputData.maxTime;
			hi_tmp_old = hi_tmp;
			hi_old = *computationalData.hi;
			auxiliaryData.current += dI;
			 // if ((i / 10) * 10 == i)
				printf(".");
		}
		printf("\n");
		if (iret)
		{
			printf("Error in computations\n");
			// goto _END;
		}

	Derivative(voltages, inputData.numberOfCurrentPoints, dI, rDs);
	FILE *fp = fopen(argv[2], "w");
	fprintf(fp, "Voltage Current Rd Tmp\n");
	for (i = 0; i < inputData.numberOfCurrentPoints; i++)
		fprintf(fp, "%le %le %le %le\n", voltages[i], currents[i], rDs[i], voltages_tmp[i]);
	fclose(fp);
	if (inputData.waveFormFile)
	{
		fp = fopen(inputData.waveFormFile, "w");
		fprintf(fp, "Time Voltage\n");
		for (i = 0; i < inputData.numberOfTimePoints; i++)
		{
			t = auxiliaryData.dt*i;
			fprintf(fp, "%le %le\n", t, waveform[i]);
		}
		fclose(fp);
	}
	SetComment(_COMMENT_FILENAME, "==========================================\n");
_END:
	free(voltages);
	free(currents);
	free(rDs);
	free(dataBuffer);
	free(dataBufferCorr);
	free(waveform);
	free(inputData.waveFormFile);
	free(auxiliaryData.a);
	free(auxiliaryData.al);
	free(auxiliaryData.ar);
	free(auxiliaryData.b);	
	free(auxiliaryData.alpha);
	free(auxiliaryData.beta);
	QueryPerformanceCounter(&lpPerformanceCount);
	calculationTime = (double)(lpPerformanceCount.QuadPart - initialCount.QuadPart) / (double)lpFrequency.QuadPart;
	printf("Calculation time: %f s\n", calculationTime);

	getchar();
	exit(0);
}

void ParametersCalculation(AuxiliaryData *auxiliaryData)
{
	int iret = 0, i;
	double dXdT;
	double dampCoeff;
	double dXdT2;
	double zDxDt;
	double resonatorDenom;
	double alpha_OmegaDt;
	double ai;
	double omegaDt;
	double omegaDt2;


	InputData *inputData = auxiliaryData->inputData;

	auxiliaryData->a = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
	auxiliaryData->al = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
	auxiliaryData->ar = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
	auxiliaryData->b = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
	auxiliaryData->alpha = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
	auxiliaryData->beta = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));

	auxiliaryData->dt = inputData->maxTime / (inputData->numberOfTimePoints - 1);
	dXdT = inputData->lineLength / ((inputData->numberOfLinePoints - 1)*auxiliaryData->dt);
	dx_tmp = inputData->lineLength / (inputData->numberOfLinePoints - 1);
	dXdT2 = dXdT*dXdT;
	dampCoeff = 1. + auxiliaryData->dt / inputData->lineDampingTau;
	ai = 2. + dXdT2*dampCoeff;
	zDxDt = inputData->lineZ * dXdT;
	omegaDt = inputData->resonatorFrequency*auxiliaryData->dt;
	omegaDt2 = omegaDt*omegaDt;
	alpha_OmegaDt = inputData->resonatorAlpha / omegaDt;
	resonatorDenom = 1. / (1. + alpha_OmegaDt + 1. / omegaDt2);
	auxiliaryData->mResonator = (unsigned int)((inputData->numberOfLinePoints-1)*(inputData->resonatorLocation / inputData->lineLength));

	for (i = 0; i < inputData->numberOfLinePoints; i++)
	{
		auxiliaryData->a[i] = ai;
		auxiliaryData->al[i] = 1;
		auxiliaryData->ar[i] = 1;
	}
	auxiliaryData->a[0] =  1. + zDxDt*dampCoeff;
	auxiliaryData->a[inputData->numberOfLinePoints - 1] = 1. + dXdT2*dampCoeff;
	auxiliaryData->al[0] = auxiliaryData->ar[inputData->numberOfLinePoints - 1] = 0.;
	//auxiliaryData->a[auxiliaryData->mResonator] = ai + dampCoeff*resonatorDenom * alpha_OmegaDt*zDxDt / inputData->resonatorR;

	auxiliaryData->b0JJ = zDxDt*dampCoeff;
	auxiliaryData->b0I1 = -zDxDt;
	auxiliaryData->bFi = dXdT2*dampCoeff;
	auxiliaryData->bIminusI = zDxDt;
	auxiliaryData->bmFiR = dampCoeff*resonatorDenom*zDxDt*alpha_OmegaDt / inputData->resonatorR;
	auxiliaryData->bmIr = -dampCoeff*resonatorDenom*zDxDt / omegaDt2;
	auxiliaryData->bnFi = dXdT2*dampCoeff;
	auxiliaryData->bnI = zDxDt;
	auxiliaryData->dIdFi = auxiliaryData->inDfi = dXdT / inputData->lineZ;
	auxiliaryData->irFiFiR = resonatorDenom*alpha_OmegaDt / inputData->resonatorR;
	auxiliaryData->irIr = resonatorDenom / omegaDt2;
	auxiliaryData->firIr = inputData->resonatorR / alpha_OmegaDt;
	auxiliaryData->i1I1 = 1./ dampCoeff;
	auxiliaryData->i1Dfi = 1. / zDxDt;

}


int NextStep(int k, AuxiliaryData *aData, ComputationalData*cData)
{
	int i, iret = 0;
	*cData->jjFunc = aData->current - sin(*cData->hi);
/*
	cData->fi[0] = *cData->jjFunc;
	*cData->hi += cData->fi[0] * aData->dt;
	return 0.;
*/
	for (i = 1; i < aData->inputData->numberOfLinePoints - 1; i++)
	{
		//if (i == aData->mResonator ) continue;
		aData->b[i] = aData->bFi*cData->fi[i] + aData->bIminusI*(cData->lineCurrent[i] - cData->lineCurrent[i + 1]);
	}
	aData->b[0] = aData->b0JJ* (*cData->jjFunc) + aData->b0I1* cData->lineCurrent[1];
//	aData->b[aData->mResonator] += aData->bmFiR* *cData->fiR + aData->bmIr* *cData->iR;
	aData->b[aData->inputData->numberOfLinePoints - 1] = aData->bnFi* cData->fi[aData->inputData->numberOfLinePoints - 1] +
		aData->bnI* cData->lineCurrent[aData->inputData->numberOfLinePoints - 1];
	memcpy(cData->fiOld, cData->fi, aData->inputData->numberOfLinePoints*sizeof(double));
	if (iret = LinearSystem(aData->a, aData->ar, aData->al, aData->b, aData->inputData->numberOfLinePoints,
		aData->alpha, aData->beta, cData->fi)) return iret;
	if (k == -1)
	{
		FILE *fp = fopen("MatrixOld.dat", "w");
		fprintf(fp, "a0 al ar b fi fiOld\n");
		for (i = 0; i < aData->inputData->numberOfLinePoints; i++)
			fprintf(fp, "%le %le %le %le %le %le\n", aData->a[i], aData->al[i], aData->ar[i], aData->b[i], cData->fi[i], cData->fiOld[i]);
		fclose(fp);
	}

	*cData->hi += cData->fi[0] * aData->dt;
	// 	*cData->hi = 0.; 
//	*cData->jjFunc = aData->current - sin(*cData->hi);
	*cData->iR = aData->irFiFiR*(cData->fi[aData->mResonator] - *cData->fiR) + aData->irIr* *cData->iR;
	*cData->fiR += aData->firIr* *cData->iR;
	cData->lineCurrent[1] = aData->i1I1*cData->lineCurrent[1] + aData->i1Dfi*(cData->fi[0]- cData->fi[1]);
	for (i = 2; i < aData->inputData->numberOfLinePoints; i++)
	{
		cData->lineCurrent[i] = cData->lineCurrent[i - 1] - aData->inDfi*(cData->fi[i-1] - cData->fiOld[i-1]);
		//if (i == aData->mResonator) cData->lineCurrent[i] -= *cData->iR;
	}

	return 0;
}


/*
int NextStep(AuxiliaryData *aData, ComputationalData*cData)
{
	int i, iret = 0;
	{

		//fi_tmp = aData->current- sin(hi_tmp);
		// hi_tmp += fi_tmp * aData->dt;
		hi_tmp += (aData->current - sin(hi_tmp)) * aData->dt;
	}
	for (i = 1; i < aData->inputData->numberOfLinePoints - 1; i++)
	{
		//if (i == aData->mResonator ) continue;
		aData->b[i] = aData->bFi*cData->fi[i] + aData->bIminusI*(cData->lineCurrent[i] - cData->lineCurrent[i + 1]);
	}
	aData->b[0] = aData->b0JJ* *cData->jjFunc + aData->b0I1* cData->lineCurrent[1];
	aData->b[aData->mResonator ] += aData->bmFiR* *cData->fiR + aData->bmIr* *cData->iR;
	aData->b[aData->inputData->numberOfLinePoints - 1] = aData->bnFi* cData->fi[aData->inputData->numberOfLinePoints - 1] + 
		     aData->bnI* cData->lineCurrent[aData->inputData->numberOfLinePoints - 1];
	memcpy(cData->fiOld, cData->fi, aData->inputData->numberOfLinePoints*sizeof(double));
	if (iret = LinearSystem(aData->a, aData->ar, aData->al, aData->b, aData->inputData->numberOfLinePoints,
		aData->alpha, aData->beta, cData->fi)) return iret;
	
	*cData->hi += cData->fi[0] * aData->dt;
// 	*cData->hi = 0.; // !!!
	*cData->jjFunc = aData->current - sin(*cData->hi);
	*cData->iR = aData->irFiFiR*(cData->fi[aData->mResonator ]- *cData->fiR) + aData->irIr* *cData->iR;
	*cData->fiR += aData->firIr* *cData->iR;
	cData->lineCurrent[1] = *cData->jjFunc - cData->fi[0];
	for (i = 2; i < aData->inputData->numberOfLinePoints; i++)
	{
		cData->lineCurrent[i] = cData->lineCurrent[i - 1] - aData->inDfi*(cData->fi[i] - cData->fiOld[i]);
		if (i == aData->mResonator) cData->lineCurrent[i] -= *cData->iR;
	}
	return 0;
}
*/




void Derivative(double *function, int numberOfPoints, double step, double *derivative)
{
	int i;
	derivative[0] = (4.*function[1] - 3.*function[0] - function[2]) / step*0.5;
	derivative[numberOfPoints - 1] = (function[numberOfPoints - 3] - 4.*function[numberOfPoints - 2] +
		3.*function[numberOfPoints - 1]) / step*0.5;
	for (i = 1; i<numberOfPoints - 1; i++)
		derivative[i] = (function[i + 1] - function[i - 1]) / step*0.5;
}
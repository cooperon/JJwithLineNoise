#define _COMMENT_OUTPUT
#include "CommentOutput.h"
#include "stdio.h"
#include "utils.h"



void CommentOutput(char *inputFileName, char *outputFileName)
{
	FILE *cFp, *iFp;
	char dateString[100], timeString[100];
	char inputBuffer[_FILE_INPUT_BUFFER_LENGTH];
	int i;
	switch (i=CheckFileStatus(_COMMENT_FILENAME))
	{
	case -1:
		printf("Comment file \" %s \" can't be created\n", _COMMENT_FILENAME);
		return;
	case 1: 
		printf("New comment file \" %s \"  created\n", _COMMENT_FILENAME);
		break;
	case 0:
	default:
		break;
	}
	cFp = fopen(_COMMENT_FILENAME, "a");
	if (!(iFp = fopen(inputFileName, "r")))
	{
		printf("Input file name: %s can't be opened\n", inputFileName);
	}
	fprintf(cFp, "==========================================\n");

	DateTime(dateString, timeString);
	fprintf(cFp, "Date: %s\tTime: %s\n", dateString, timeString);
	fprintf(cFp, "Output file name: %s\n\n", outputFileName);
	fprintf(cFp, "-----------------------------------------\n");
	while (fgets(inputBuffer, _FILE_INPUT_BUFFER_LENGTH - 1, iFp))
		fprintf(cFp, "%s", inputBuffer);
	fclose(iFp);
	fclose(cFp);
	return;

}

void SetComment(char *commentFileName, char *comment)
{
	switch (CheckFileStatus(commentFileName))
	{
	 default:
		printf("Comment file \" %s \" absent\n", commentFileName);
		return;
	case 0:
		break;
	}
	FILE *cFp = fopen(commentFileName, "a");
	fprintf(cFp, "%s\n", comment);
	fclose(cFp);
	return;
}



#include "csv.h"

/*
 * DEBUG
 * 0-false
 * !0-true
 */
#define DEBUG 1
#if DEBUG
#define PRINTF printf
#else
#define PRINTF //printf
#endif

void** csvParse(int** data, FILE* csvFile, const int DIM)
{
	// buffer c
	int c = 0;
	//read data from file as chars
	fseek(csvFile, 0, SEEK_SET);
	int lengthOfCsv = 0;
	for(; (c = fgetc(csvFile)) != EOF; lengthOfCsv++)
	{
	}
	PRINTF("%d\n", lengthOfCsv);
	fseek(csvFile, 0, SEEK_SET);
	char* rawCsv = malloc(lengthOfCsv);
	for(int i = 0; (c = fgetc(csvFile)) != EOF; i++)
	{
		rawCsv[i] = c;
	}
	PRINTF("Data:\n%s\n", rawCsv);
	//done with csvFile. MUST CLOSE IN MAIN WHEN DONE
	//process rawCsv
	int numLines = 0;
	for(int i = 0; i < lengthOfCsv; i++)
	{
		if(rawCsv[i] == '\n')
		{
			numLines++;
		}
	}
	PRINTF("numLines: %d\n", numLines);
	//split rawCsv into rows. Stored in dataByLine
	char** dataByLine  = malloc(numLines*sizeof(long int));
	int counter = 0;
	int sizeDataByLine[numLines];
	char* tmp = NULL;
	int endOfLine = 0;
	for(int i = 0; i < numLines; i++)
	{
		dataByLine[i] = malloc(16);
		sizeDataByLine[i] = 16;
	}
	for(int i = 0; i < numLines; i++)
	{
		for(int j = 0; rawCsv[counter] != '\n'; j++)
		{
			if(j > (sizeDataByLine[j]-2))
			{
				tmp = realloc(dataByLine[i], sizeDataByLine[i]*2);
				sizeDataByLine[i] = sizeDataByLine[i]*2;
				if(tmp == NULL)
				{
					exit(1);
				}
				else
				{
					sizeDataByLine[i] = sizeDataByLine[i]*2;
					dataByLine[i] = tmp;
				}
			}
			dataByLine[i][j] = rawCsv[counter];
			counter++;
			endOfLine = j;
		}
		dataByLine[i][(endOfLine)+1] = '\0';
		counter++;
	}
	//data is in rows. Done with rawCsv
	free(rawCsv);
	//remove delimeters from dataByLine and convert to int**
	for(int i = 0; i < numLines; i++)
	{
		PRINTF("%s-----\n", dataByLine[i]);
	}
	//check number of delimiters
	int numDelimit = 0;
	for(int i = 0; i < numLines; i++)
	{
		numDelimit = 0;
		for(int j = 0; dataByLine[i][j] != '\0'; j++)
		{
			if(dataByLine[i][j] == ',')
			{
				numDelimit++;
			}
		}
		if((numDelimit+1) != DIM)
		{
			printf("Given DIM does not match\n");
			exit(1);
		}
	}
	//convert char array to int array
	int** delimitIndex = malloc(numLines*sizeof(long int));
	for(int i = 0; i < numLines; i++)
	{
		delimitIndex[i] = malloc(DIM*sizeof(int));
	}
	for(int i = 0; i < numLines; i++)
	{
		for(int j = 0; j < (DIM-1); j++)
		{
			delimitIndex[i][j] = 0;
		}
	}
	for(int i = 0; i < numLines; i++)
	{
		for(int j = 0; dataByLine[i][j] != '\0'; j++)
		{
			if(dataByLine[i][j] == ',')
			{
				for(int l = 0; l < DIM-1; l++)
				{
					if(delimitIndex[i][l] == 0)
					{
						delimitIndex[i][l] = j;
						break;
					}
				}
			}
		}
	}
	for(int i = 0; i < numLines; i++)
	{
		for(int l = 0; l < (DIM-1); l++)
		{
			PRINTF("%d\n", delimitIndex[i][l]);
		}
	}
	//clean up
	for(int i = 0; i < numLines; i++)
	{
		free(dataByLine[i]);
	}
	free(dataByLine);
	for(int i = 0; i < numLines; i++)
	{
		free(delimitIndex[i]);
	}
	free(delimitIndex);
	printf("csvParse: %s\n", strerror(errno));
	return 0;
}

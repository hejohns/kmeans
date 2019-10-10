#include "csv.h"

/*
 * DEFINE
 * 0-false
 * !0-true
 */
#define DEBUG 0
#if DEBUG
#define PRINTF(...); printf(__VA_ARGS__);
#else
#define PRINTF(...);
#endif

double csvParse(double*** data, FILE* csvFile, const int DIM)
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
	char* restrict rawCsv = malloc(lengthOfCsv);
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
	char** restrict dataByLine = malloc(numLines*sizeof(long int));
	int counter = 0;
	int sizeDataByLine[numLines];
	char* restrict tmp = NULL;
	int endOfLine = 0;
	for(int i = 0; i < numLines; i++)
	{
		dataByLine[i] = malloc(32);
		sizeDataByLine[i] = 32;
	}
	for(int i = 0; i < numLines; i++)
	{
		for(int j = 0; rawCsv[counter] != '\n'; j++)
		{
			if((j*sizeof(char)) > (sizeDataByLine[i]-8))
			{
				tmp = realloc(dataByLine[i], sizeDataByLine[i]*2);
				if(tmp == NULL)
				{
					exit(1);
				}
				else
				{
					sizeDataByLine[i] *= 2;
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
			printf("Given DIM does not match: %d-%d\n", numDelimit, DIM);
			exit(1);
		}
	}
	//create array for which cells are negative
	int** restrict coe = malloc(numLines*sizeof(long int));
	for(int i = 0; i < numLines; i++)
	{
		coe[i] = malloc(DIM*sizeof(int));
		for(int l = 0; l < DIM; l++)
		{
			coe[i][l] = 1;
		}
	}
	(*data) = malloc(numLines*sizeof(long int));
	for(int i = 0; i < numLines; i++)
	{
		 (*data)[i] = calloc((size_t)DIM, DIM*sizeof(int));
	}
	//convert char array to double
	{
	int stop = 0;
	int l = 0;
	for(int i = 0; i < numLines; i++)
	{
nextLine:
		l = 0;
		for(int j = 0;; j++)
		{
			if(((dataByLine[i][j]-'0') <= 9) && ((dataByLine[i][j]-'0') >= 0))
			{
				char containsDecimal = 0;
				for(int u=j;(dataByLine[i][u]!=',')&&(dataByLine[i][u]!='\0');u++)
				{
					if(dataByLine[i][u] == '.')
					{
						containsDecimal = 1;
						goto jump;
					}
				}
jump:
				if(containsDecimal)
				{
	//shifted for readability
	for(stop = j; (dataByLine[i][stop] != ',') && (dataByLine[i][stop] != '\0');stop++){}
	PRINTF("Digit-%d-%d-%d-%f\n", dataByLine[i][j]-'0', i, j, pow(10, stop-j-2));
	(*data)[i][l] = (*data)[i][l] + ((dataByLine[i][j]-'0')*pow(10,(stop-j-2)));
				}
				else
				{
	for(stop = j; (dataByLine[i][stop] != ',') && (dataByLine[i][stop] != '\0');stop++){}
	PRINTF("Digit-%d-%d-%d-%f\n", dataByLine[i][j]-'0', i, j, pow(10, stop-j-1));
	(*data)[i][l] = (*data)[i][l] + ((dataByLine[i][j]-'0')*pow(10,(stop-j-1)));
				}
			}
			else if(dataByLine[i][j] == '-')
			{
				coe[i][l] = coe[i][l]*-1;
				PRINTF("Negative-\n");
			}
			else if(dataByLine[i][j] == '.')
			{
				PRINTF("Decimal-\n");
	for(stop = j; (dataByLine[i][stop] != ',') && (dataByLine[i][stop] != '\0');stop++){}
	coe[i][l] = coe[i][l]*pow(10, stop-j-1);
			}
			else if(dataByLine[i][j] == ',')
			{
				PRINTF("Delimit-\n");
				l++;
			}
			else if(dataByLine[i][j] == '\0')
			{
				PRINTF("Delimit-\n");
				i++;
				if(i==numLines)
				{
					break;
				}
				goto nextLine;
			}
			else
			{
				printf("Invalid char on [%d][%d]\n", i, j);
			}
		}
	}
	}
	//done conversion
	//data=data*coe
	for(int i = 0; i < numLines; i++)
	{
		for(int l = 0; l < DIM; l++)
		{
			 (*data)[i][l] = (*data)[i][l]/coe[i][l];
		}
	}
	for(int i = 0; i < numLines; i++)
	{
		for(int j = 0; j < DIM; j++)
		{
			PRINTF("%f,", (*data)[i][j]);
		}
		PRINTF("\n");
	}
	//clean up
	for(int i = 0; i < numLines; i++)
	{
		free(dataByLine[i]);
	}
	free(dataByLine);
	for(int i = 0; i < numLines; i++)
	{
		free(coe[i]);
	}
	free(coe);
	PRINTF("csvParse: %s\n", strerror(errno));
	return numLines;
}

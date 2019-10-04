#include "csv.h"

/*
 * DEBUG
 * 0-false
 * !0-true
 */
#define DEBUG 0
#if DEBUG
#define PRINTF printf
#else
#define PRINTF //printf
#endif

int powi(int base, int exponent);
int powi(int base, int exponent)
{
	int buf = 1;
	for(int i = 0; i < exponent; i++)
	{
		buf=base*buf;
	}
	return buf;
}

int csvParse(int*** data, FILE* csvFile, const int DIM)
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
	char** dataByLine = malloc(numLines*sizeof(long int));
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
jump2:
		for(int j = 0;; j++)
		{
			if(dataByLine[i][j] == '\0')
			{
				delimitIndex[i][DIM-1] = j;
				i++;
				if(i == numLines)
				{
					goto breakout2;
				}
				goto jump2;
			}
		}
	}
breakout2:
	for(int i = 0; i < numLines; i++)
	{
		for(int l = 0; l < DIM; l++)
		{
			PRINTF("%d\n", delimitIndex[i][l]);
		}
	}
	//create array for which cells are negative
	int** negCoe = malloc(numLines*sizeof(long int));
	for(int i = 0; i < numLines; i++)
	{
		negCoe[i] = malloc(DIM*sizeof(int));
		for(int l = 0; l < DIM; l++)
		{
			negCoe[i][l] = 1;
		}
	}
	(*data) = malloc(numLines*sizeof(long int));
	for(int i = 0; i < numLines; i++)
	{
		 (*data)[i] = malloc(DIM*sizeof(int));
	}
	int l = 0;
	for(int i = 0; i < numLines; i++)
	{
		for(int j = 0; dataByLine[i][j] != '\0'; j++)
		{
jump:
			if(l == (DIM-1 ))
			{
				l=0;
				i++;
				j=0;
				if(i == numLines)
				{
					goto breakout;
				}
				else
				{
					goto jump;
				}
			}

			if(((dataByLine[i][j]-'0') <= 9) && ((dataByLine[i][j]-'0') >= 0))
			{
//shifted for readability
PRINTF("Triggered1-%d-%d-%d-%d\n", dataByLine[i][j]-'0', i, j, powi(10, delimitIndex[i][l]-j-1));
 (*data)[i][l] = (*data)[i][l] + ((dataByLine[i][j]-'0')*powi(10,(delimitIndex[i][l]-j-1)));
			}
			else if(dataByLine[i][j] == '-')
			{
				negCoe[i][l] = -1;
PRINTF("Triggered2\n");
			}
			else if(dataByLine[i][j] == ',')
			{
				j++;
				l++;
				goto jump;
			}
			else
			{
				printf("Invalid char on [%d][%d]\n", i, j);
			}
		}
	}
breakout:
	l = (DIM-1);
	for(int i = 0; i < numLines; i++)
	{
		for(int j = delimitIndex[i][l-1]; dataByLine[i][j] != '\0'; j++)
		{
			if(((dataByLine[i][j]-'0') <= 9) && ((dataByLine[i][j]-'0') >= 0))
			{
//shifted for readability
PRINTF("Triggered3-%d-%d-%d-%d\n", dataByLine[i][j]-'0', i, j, powi(10, delimitIndex[i][l]-j-1));
 (*data)[i][l] = (*data)[i][l] + ((dataByLine[i][j]-'0')*powi(10,(delimitIndex[i][l]-j-1)));
			}
			else if(dataByLine[i][j] == '-')
			{
				negCoe[i][l] = -1;
				PRINTF("Triggered4\n");
			}
			else if(dataByLine[i][j] == ',')
			{
			}
			else
			{
				printf("Invalid char on [%d][%d]\n", i, j);
			}
		}
	}
	//data=data*negCoe
	for(int i = 0; i < numLines; i++)
	{
		for(int l = 0; l < DIM; l++)
		{
			 (*data)[i][l] = (*data)[i][l]*negCoe[i][l];
		}
	}
	for(int i = 0; i < numLines; i++)
	{
		for(int j = 0; j < DIM; j++)
		{
			PRINTF("%d,", (*data)[i][j]);
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
		free(delimitIndex[i]);
	}
	free(delimitIndex);
	for(int i = 0; i < numLines; i++)
	{
		free(negCoe[i]);
	}
	free(negCoe);
	printf("csvParse: %s\n", strerror(errno));
	return numLines;
}

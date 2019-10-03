#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

int main(int argc, char** argv)
{
	//fail immedietly on incorrect usage
	if(argc != 3)
	{
		printf("Usage: \n    ./kmeans [NUM_THREADS] [DATA_PATH]\n");
		exit(1);
	}
	//global buffer c
	int c;
	//read data
	const char* NUM_THREADS = argv[1];
	const char* DATA_PATH = argv[2];
	printf("NUM_THREADS = %s\nDATA_PATH = %s\n", NUM_THREADS, DATA_PATH);
	FILE* data;
	data = fopen(DATA_PATH, "r");
	if(data == NULL)
	{
		printf("%s\n", strerror(errno));
		exit(1);
	}
	int lengthOfData = 0;
	int* k = &lengthOfData;
	for(; (c = fgetc(data)) != EOF; (*k)++)
	{
	}
	printf("%d\n", lengthOfData);
	fseek(data, 0, SEEK_SET);
	char* input = malloc(lengthOfData);
	for(int i = 0; (c = fgetc(data)) != EOF; i++)
	{
		input[i] = c;
	}
	printf("Data:\n%s\n", input);
	//done with data file
	fclose(data);
	//process input from data file
	int numLines = 0;
	k = &numLines;
	for(int i = 0; i < lengthOfData; i++)
	{
		if(input[i] == '\n')
		{
			(*k)++;
		}
	}
	printf("numLines: %d\n", numLines);
	char** dataByLine  = malloc(numLines*sizeof(long int));
	int counter = 0;
	k = &counter;
	int sizeDataByLine[numLines];
	char* tmp;
	int endOfLine;
	int* nullTerm = &endOfLine;
	//split data into rows
	for(int i = 0; i < numLines; i++)
	{
		dataByLine[i] = malloc(32);
		for(int i = 0; i < numLines; i++)
		{
			sizeDataByLine[i] = 32;
		}
		for(int j = 0; input[*k] != '\n'; j++)
		{
			if(j > (sizeDataByLine[j]-2))
			{
				tmp = realloc(dataByLine[i], sizeDataByLine[i]*2);
				sizeDataByLine[i] = sizeDataByLine[i] + 1;
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
			dataByLine[i][j] = input[*k];
			(*k)++;
			*nullTerm = j;
		}
		dataByLine[i][(*nullTerm)+1] = '\0';
		(*k)++;
	}
	free(input);
	//remove delimeters and convert digit chars to ints
	for(int i = 0; i < numLines; i++)
	{
		printf("%s-----\n", dataByLine[i]);
	}
	for(int i =0; i < numLines; i++)
	{
		for(int j = 0;*(dataByLine[i]+j) != '\0'; j++)
		{
		}
			printf("HIT EOF\n");
	}
	//begin kmeans
	//clean up
	printf("%s\n", strerror(errno));
	for(int i = 0; i < numLines; i++)
	{
		free(dataByLine[i]);
	}
	free(dataByLine);
	return 0;
}

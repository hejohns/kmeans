#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include "csv.h"

/*
 * DEFINE
 * 0-false
 * !0-true
 */

#define DEBUG 0
#if DEBUG
#define PRINTF(...); printf(_VA_ARGS_);
#else
#define PRINTF(...);
#endif

#define OUTPUT_DATA 0
#if OUTPUT_DATA
#define PRINTF_DATAONLY(...); printf(_VA_ARGS_);
#else
#define PRINTF_DATAONLY(...);
#endif

int main(int argc, char** argv)
{
	//fail immedietly on incorrect usage
	if(argc != 4)
	{
		printf("Usage: \n    ./kmeans [NUM_THREADS] [DATA_PATH] [DIM(data)]]\n");
		exit(1);
	}
	//read data
	const int NUM_THREADS = argv[1][0] - '0';
	const char* DATA_PATH = argv[2];
	const int DIM = argv[3][0] - '0';
	PRINTF("NUM_THREADS = %d\nDATA_PATH = %s\nDIM= %d\n", NUM_THREADS, DATA_PATH, DIM);
	FILE* datafp;
	datafp = fopen(DATA_PATH, "r");
	if(datafp == NULL)
	{
		printf("%s\n", strerror(errno));
		exit(1);
	}
	double** data = NULL;
	int rows = 0;
	rows = csvParse(&data, datafp, DIM);
	for(int i = 0; i < rows; i++)
	{
		for(int l=0; l < DIM; l++)
		{
			PRINTF_DATAONLY("%f", data[i][l]);
			if(l!=(DIM-1))
			{
				PRINTF_DATAONLY(",");
			}
		}
		PRINTF_DATAONLY("\n");
	}
	//clean up
	data = malloc(1);
	free(data);
	for(int i = 0; i < rows; i++)
	{
		data[i] = malloc(1);
		free(data[i]);
	}	
	PRINTF("kmeans: %s\n", strerror(errno));
	return 0;
}

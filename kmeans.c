#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <omp.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "csv.h"
#include "kmeans_funcs.h"

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

#define OUTPUT_DATA 0
#if OUTPUT_DATA
#define PRINTF_DATAONLY(...); printf(__VA_ARGS__);
#else
#define PRINTF_DATAONLY(...);
#endif

int main(int argc, char** argv)
{
	//fail immedietly on incorrect usage
	if(argc != 6)
	{
		printf("Usage: \n    ./kmeans [DATA_PATH] [DIM(data)] [K] [ITERATIONS] [NUM_THREADS]\n");
		exit(1);
	}
	//read data
	int stop;
	int num_threads = 0;
	for(stop = 0; argv[5][stop] != '\0'; stop++){}
	for(int i = 0; argv[5][i] != '\0'; i++)
	{
		num_threads = num_threads + (argv[5][i]-'0')*pow(10,(stop-i-1));
	}
	const int NUM_THREADS = num_threads;
	const char* DATA_PATH = argv[1];
	int dim = 0;
	for(stop = 0; argv[2][stop] != '\0'; stop++){}
	for(int i = 0; argv[2][i] != '\0'; i++)
	{
		dim= dim + (argv[2][i]-'0')*pow(10,(stop-i-1));
	}
	const int DIM = dim;
	int numCenters = 0;
	for(stop = 0; argv[3][stop] != '\0'; stop++){}
	for(int i = 0; argv[3][i] != '\0'; i++)
	{
		numCenters = numCenters + (argv[3][i]-'0')*pow(10,(stop-i-1));
	}
	const int K = numCenters;
	int decimalPos = 0;
	char decimalAfter = 0;
	double iterations = 0;
	for(stop = 0; argv[4][stop] != '\0'; stop++){}
	for(int j = 0; argv[4][j] != '\0';j++)
	{
		if(argv[4][j] == '.')
		{
			decimalPos = j;
			goto hurdle;
		}
	}
hurdle:
	for(int i = 0; argv[4][i] != '\0'; i++)
	{
		decimalAfter = 0;
		if(((argv[4][i]-'0') <= '9') && ((argv[4][i]-'0')>=0))
		{
		for(int j = i; argv[4][j] != '\0';j++)
		{
			if(argv[4][j] == '.')
			{
				decimalAfter = 1;
				goto hurdle2;
			}
		}
hurdle2:
		if(decimalAfter)
		{
			iterations = iterations + (argv[4][i]-'0')*pow(10,(stop-i-2));
		}
		else
		{
			iterations = iterations + (argv[4][i]-'0')*pow(10,(stop-i-1));
		}
		}
		else if(argv[4][i] == '.')
		{
			decimalPos = stop-i;
		}
		else
		{
		}
	}
	if(decimalPos != 0){iterations/=(pow(10,decimalPos-1));}
	const double ITERATIONS = iterations;
	PRINTF("NUM_THREADS = %d\nDATA_PATH = %s\nDIM = %d\nK = %d", NUM_THREADS, DATA_PATH, DIM, K);
	FILE* datafp = fopen(DATA_PATH, "r");
	if(datafp == NULL)
	{
		printf("%s\n", strerror(errno));
		exit(1);
	}
	double*** restrict data = malloc(sizeof(long int));
	int rows = 0;
	rows = csvParse(data, datafp, DIM);
	//
	omp_set_num_threads(NUM_THREADS);
	double finalCenters[K][DIM];
	double*** centers = malloc(sizeof(long int));//[K][DIM]
	(*centers) = malloc(K*sizeof(long int));
	for(int i=0; i<K;i++)
	{
		(*centers)[i] = malloc(DIM*sizeof(double));
	}
	double*** ownership = malloc(sizeof(long int));//[rows][2]
	(*ownership) = malloc(rows*sizeof(long int));
	for(int i=0; i<rows; i++)
	{
		(*ownership)[i] = malloc(2*sizeof(double));
	}
	int** numElem = malloc(sizeof(long int));//[K]
	(*numElem) = malloc(K*sizeof(int));
	double error = 1;
	double error2 = 2;
	double minError = 0;
	initializeCenters(data, centers, ownership, rows, DIM, K);
	//done setting initials
	for(int m = 0; m < ITERATIONS; m++)
	{
		for(int j = 0; fabs(error-error2) > 0; j++)
		{
			error = error2;
			/*
			printf("%f-%f,%f>%f:%d\n",error, error2, fabs(error-error2), iterations, fabs(error-error2)>iterations);
			printf("%d\n",j);
			*/
#pragma omp parallel
		{
			//calculate ownership
			calculateOwnership(data, centers, ownership, rows, DIM, K);
#pragma omp barrier
			//calculate total error
		}
			error2 = totalError(ownership, rows);
#pragma omp parallel
		{
			//calculate new centers
			newCenters(data, centers, ownership, numElem, rows, K, DIM);
		}//end of parallel
		}//return to serial
		if(minError>error2 || m==0)
		{
			minError = error2;
			for(int u=0;u<K;u++)
			{
				for(int p=0;p<DIM;p++)
				{
					finalCenters[u][p] = (*centers)[u][p];
				}
			}
		}
	}
for(int u = 0; u<K; u++){
for(int p=0;p<DIM;p++){
printf("center%d,%d:%f--%i\n", u, p, (*centers)[u][p], (*numElem)[u]);
}}
	//
	//
	for(int i = 0; i < rows; i++)
	{
		for(int l=0; l < DIM; l++)
		{
			PRINTF_DATAONLY("%f", (*data)[i][l]);
			if(l!=(DIM-1))
			{
				PRINTF_DATAONLY(",");
			}
		}
		PRINTF_DATAONLY("\n");
	}
	//
	//clean up
	//malloc_stats();
	for(int i = 0; i < rows; i++)
	{
		free((*data)[i]);
	}
	free(*data);
	free(data);//[rows][DIM]
	for(int i=0; i<K; i++)
	{
		free((*centers)[i]);
	}
	free(*centers);
	free(centers);//[K][DIM]
	for(int i=0; i<rows; i++)
	{
		free((*ownership)[i]);
	}
	free(*ownership);
	free(ownership);//[rows][2]
	free(*numElem);
	free(numElem);//[K]
	//malloc_stats();
	PRINTF("kmeans: %s\n", strerror(errno));
	return 0;
}

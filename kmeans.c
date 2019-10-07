#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <omp.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "csv.h"
#include "kmeans_math.h"

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
		printf("Usage: \n    ./kmeans [DATA_PATH] [DIM(data)] [K] [EPSILON] [NUM_THREADS]\n");
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
	double epsilon = 0;
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
			epsilon = epsilon + (argv[4][i]-'0')*pow(10,(stop-i-2));
		}
		else
		{
			epsilon = epsilon + (argv[4][i]-'0')*pow(10,(stop-i-1));
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
	if(decimalPos != 0){epsilon/=(pow(10,decimalPos-1));}
	const double EPSILON = epsilon;
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
	double centers[K][DIM];
	double ownership[rows][2];
	int numElem[K];
	double error = 0;
	double error2 = 0;
	//set initial centers in parallel
#pragma omp parallel
	{
#pragma omp for schedule(auto) collapse(2)
	for(int i=0; i<K; i++)
	{
		for(int j=0; j<DIM; j++)
		{
			centers[i][j] = (*data)[(i+1)*(rows/K)-1][j];
		}
	}
#pragma omp for schedule(auto)
	for(int i=0; i<rows;i++)
	{
		ownership[i][0] = 0;
		ownership[i][1] = distance_sqed((*data)[i], centers[0], DIM);
	}
	}
	//done setting initials
	epsilon=-1;
	for(int j = 0; fabs(error-error2) > epsilon; j++)
	{
		printf("%f-%f,%f>%f:%d\n",error, error2, fabs(error-error2), epsilon, fabs(error-error2)>epsilon);
		error=error2;
		printf("%d\n",j);
	//enter iterative parallel
#pragma omp parallel
	{
		//calculate ownership
#pragma omp for schedule(auto) collapse(2)
		for(int i = 0; i<rows; i++)
		{
			for(int u = 0; u<K; u++)
			{
				if (ownership[i][1] > distance_sqed((*data)[i], centers[u], DIM))
				{
					ownership[i][0] = u;
					ownership[i][1] = distance_sqed((*data)[i], centers[u], DIM);
				}
			}
		}
#pragma omp barrier
		//calculate total error
		error2=0;
#pragma omp for schedule(auto) reduction(+:error2)
		for(int i=0;i<rows;i++)
		{
			error2+=ownership[i][1];
		}
		//calculate new centers
#pragma omp for schedule(auto)
		for(int u=0; u<K;u++)
		{
			numElem[u] = 0;
		}
#pragma omp barrier
#pragma omp for schedule(auto) collapse(2)
		for(int u=0; u<K;u++)
		{
			for(int i=0; i<rows;i++)
			{
				if(ownership[i][0] == u)
				{
#pragma omp atomic
					numElem[u] += 1;
				}
			}
		}
#pragma omp barrier
#pragma omp for schedule(auto) collapse(2)
		for(int u=0; u<K;u++)
		{
			for(int p=0; p<DIM;p++)
			{
				centers[u][p] = 0;
			}
		}
#pragma omp barrier
#pragma omp for schedule(auto) collapse(3)
		for(int u=0; u<K;u++)
		{
			for(int p=0; p<DIM;p++)
			{
				for(int i = 0; i<rows;i++)
				{
					if(ownership[i][0] == u)
					{
#pragma omp atomic
						centers[u][p] += (*data)[i][p];
					}
				}
			}
		}
#pragma omp barrier
#pragma omp for schedule(auto) collapse(2)
		for(int u=0; u<K;u++)
		{
			for(int p=0; p<DIM;p++)
			{
				centers[u][p] /= numElem[u];
			}
		}
	}//end of parallel
	//return to serial
	epsilon = EPSILON;
	}
for(int u = 0; u<K; u++){
for(int p=0;p<DIM;p++){
printf("center%d,%d:%f--%i\n", u, p, centers[u][p], numElem[u]);
}}
for(int i=0;i<rows;i++){
}
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
	for(int i = 0; i < rows; i++)
	{
		free((*data)[i]);
	}
	free(*data);
	//malloc_stats();
	PRINTF("kmeans: %s\n", strerror(errno));
	return 0;
}

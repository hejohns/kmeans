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

#define ITER_PER_THREAD 1024

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
	if(argc != 6){
		printf("Usage: \n    ./kmeans [DATA_PATH] [DIM(data)] [K] [ITERATIONS] [NUM_THREADS]\n");
		exit(1);
	}
	//read data
	const char* DATA_PATH = argv[1];
	const unsigned int DIM = str2uint(argv[2]);
	const unsigned int K = str2uint(argv[3]);
	const unsigned int iterations = str2uint(argv[4]);
	const unsigned int ITERATIONS = (iterations-iterations%ITER_PER_THREAD)+ITER_PER_THREAD;
	const unsigned int NUM_THREADS = str2uint(argv[5]);
	PRINTF("ITERATIONS=%d\nNUM_THREADS = %d\nDATA_PATH = %s\nDIM = %d\nK = %d\n", ITERATIONS, NUM_THREADS, DATA_PATH, DIM, K);
	FILE* datafp = fopen(DATA_PATH, "r");
	if(datafp == NULL){
		printf("%s\n", strerror(errno));
		exit(1);
	}
	double*** restrict data = malloc(sizeof(void*));
	long unsigned int rows = csvParse(data, datafp, DIM);
	fclose(datafp);
	
	double*** finalCenters = malloc(ITERATIONS*sizeof(void*));//[ITERATIONS][K][DIM+1]
	double* errors = malloc(ITERATIONS*sizeof(double));//[ITERATIONS];
	for(int i=0; i<ITERATIONS; i++)
	{
		finalCenters[i] = malloc(K*sizeof(void*));
		for(int j=0; j<K; j++)
		{
			finalCenters[i][j] = malloc((DIM+1)*sizeof(double));
		}
	}
	long unsigned int index=0;
	omp_set_num_threads(NUM_THREADS);
#pragma omp parallel for schedule(auto)
for(int m=0; m < ITERATIONS/ITER_PER_THREAD; m++)
{
	double*** centers = malloc(sizeof(void*));//[K][DIM]
	(*centers) = malloc(K*sizeof(void*));
	for(int i=0; i<K;i++)
	{
		(*centers)[i] = malloc(DIM*sizeof(double));
	}
	double*** ownership = malloc(sizeof(void*));//[rows][2]
	(*ownership) = malloc(rows*sizeof(void*));
	for(int i=0; i<rows; i++)
	{
		(*ownership)[i] = malloc(2*sizeof(double));
	}
	int** numElem = malloc(sizeof(void*));//[K]
	(*numElem) = malloc(K*sizeof(int));
	for(int n=0; n<ITER_PER_THREAD; n++)
	{
		long unsigned int thread_index;
#pragma omp critical
		{
		index++;
		thread_index = index-1;
		}
		//printf("%ld\n",thread_index);
		double error = 0;
		double error2 = 0;
		if(index-1==0){
			initializeCentersSpaced(data, centers, rows, DIM, K);
		}
		else{
			initializeCentersRand(data, centers, rows, DIM, K);
		}
		for(int j = 0; (fabs(error-error2) > 0) || (j==0); j++)
		{
			error = error2;
			if(j==0){
				initializeOwnership(data, centers, ownership, rows, DIM, K);
				calculateOwnership(data, centers, ownership, rows, DIM, K);
			}
			else{
				calculateOwnership(data, centers, ownership, rows, DIM, K);
			}
			error2 = totalError(ownership, rows);
			newCenters(data, centers, ownership, numElem, rows, K, DIM);
			errors[thread_index] = error2;
		}//done finding one set of centers
		for(int u=0; u<K; u++)
		{
			for(int p=0; p<DIM; p++)
			{
				finalCenters[thread_index][u][p] = (*centers)[u][p];
			}
			finalCenters[thread_index][u][DIM] = (*numElem)[u];
		}
		//printf("%ld\n",thread_index);
	}//thread done
	for(int i=0; i<K; i++){
		free((*centers)[i]);
	}
	free(*centers);
	free(centers);//[K][DIM]
	for(int i=0; i<rows; i++){
		free((*ownership)[i]);
	}
	free(*ownership);
	free(ownership);//[rows][2]
	free(*numElem);
	free(numElem);//[K]
}//return to serial
	//
	for(int i = 0; i < rows; i++){
		free((*data)[i]);
	}
	free(*data);
	free(data);//[rows][DIM]
	double finalFinalCenters[K][DIM+1];
	double minimal = errors[0];
	for(int u=0; u<K; u++){
		for(int p=0; p<(DIM+1); p++){
			finalFinalCenters[u][p] = finalCenters[0][u][p];
	}}
	for(int m=0;m<ITERATIONS;m++)
	{
		if(errors[m] < minimal)
		{
			for(int u=0; u<K;u++)
			{
					for(int p=0;p<(DIM+1);p++)
					{
						finalFinalCenters[u][p] = finalCenters[m][u][p];
					}
			}
			minimal = errors[m];
		}
	}
	for(int u=0; u<K;u++){
		for(int p=0;p<DIM;p++)
		{
			printf("%f", finalFinalCenters[u][p]);
			printf(",");
		}
		printf("%ld\n", (long unsigned int)finalFinalCenters[u][DIM]);
	}
	//clean up
	for(int i=0; i<ITERATIONS; i++)
	{
		for(int j=0; j<(DIM+1); j++)
		{
			free(finalCenters[i][j]);
		}
		free(finalCenters[i]);
	}
	free(errors);//[ITERATIONS];
	free(finalCenters);//[ITERATIONS][K][DIM+1]
	//malloc_stats();
	PRINTF("kmeans: %s\n", strerror(errno));
	return 0;
}

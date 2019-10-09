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

double distance_sqed(double* x1, double* x2, int DIM)
{
	double tmp = 0;
	for(int i = 0; i<DIM; i++)
	{
		tmp = tmp+pow(x1[i]-x2[i],2);
	}
	return tmp;
}

void initializeCenters(double*** data, double*** centers, double*** ownership, int rows, const int DIM, const int K)
{
#pragma omp parallel
        {
#pragma omp for schedule(auto) collapse(2)
        for(int i=0; i<K; i++)
        {
                for(int j=0; j<DIM; j++)
                {
                        (*centers)[i][j] = (*data)[(i+1)*(rows/K)-1][j];
                }
        }
#pragma omp for schedule(auto)
        for(int i=0; i<rows;i++)
        {
                (*ownership)[i][0] = 0;
                (*ownership)[i][1] = distance_sqed((*data)[i], (*centers)[0], DIM);
        }
        }
}

void calculateOwnership(double*** data, double*** centers, double*** ownership, int rows, const int DIM, const int K)
{
#pragma omp for schedule(auto) collapse(2)
		for(int i = 0; i<rows; i++)
		{
			for(int u = 0; u<K; u++)
			{
				if ((*ownership)[i][1] > distance_sqed((*data)[i], (*centers)[u], DIM))
				{
					(*ownership)[i][0] = u;
					(*ownership)[i][1] = distance_sqed((*data)[i], (*centers)[u], DIM);
				}
			}
		}
}

double totalError(double*** ownership,int rows)
{
	double error=0;
#pragma omp parallel for schedule(auto) reduction(+:error)
	for(int i=0;i<rows;i++)
	{
		error+=(*ownership)[i][1];
	}
	return error;
}

void newCenters(double*** data, double*** centers, double*** ownership, int** numElem, int rows, const int K, const int DIM)
{
#pragma omp for schedule(auto)
	for(int u=0; u<K;u++)
	{
		(*numElem)[u] = 0;
	}
#pragma omp barrier
#pragma omp for schedule(auto) collapse(2)
	for(int u=0; u<K;u++)
	{
		for(int i=0; i<rows;i++)
		{
			if((*ownership)[i][0] == u)
			{
#pragma omp atomic
				(*numElem)[u] += 1;
			}
		}
	}
#pragma omp barrier
#pragma omp for schedule(auto) collapse(2)
	for(int u=0; u<K;u++)
	{
		for(int p=0; p<DIM;p++)
		{
			(*centers)[u][p] = 0;
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
				if((*ownership)[i][0] == u)
				{
#pragma omp atomic
					(*centers)[u][p] += (*data)[i][p];
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
			(*centers)[u][p] /= (*numElem)[u];
		}
	}
#pragma omp barrier
}

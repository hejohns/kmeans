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

double distance_sqed(double* x1, double* x2, int dim)
{
	double tmp = 0;
	for(int i = 0; i<dim; i++)
	{
		tmp = tmp+pow(x1[i]-x2[i],2);
	}
	return tmp;
}

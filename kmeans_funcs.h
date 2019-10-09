#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

double distance_sqed(double* x1, double* x2, int DIM);
void initializeCenters(double*** data, double*** centers, double*** ownership, int rows, const int DIM, const int K);
void calculateOwnership(double*** data, double*** centers, double*** ownership, int rows, const int DIM, const int K);
double totalError(double*** ownership,int rows);
void newCenters(double*** data, double*** centers, double*** ownership, int** numElem, int rows, const int K, const int DIM);

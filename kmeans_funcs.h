#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>

unsigned int str2uint(char* str);
double distance_sqed(double* x1, double* x2, int DIM);
void initializeCentersRand(double*** data, double*** centers, int rows, int DIM, int K);
void initializeCentersSpaced(double*** data, double*** centers, int rows, int DIM, int K);
void initializeOwnership(double*** data, double*** centers, double*** ownership, int rows, int DIM, int K);
void calculateOwnership(double*** data, double*** centers, double*** ownership, int rows, int DIM, int K);
double totalError(double*** ownership,int rows);
void newCenters(double*** data, double*** centers, double*** ownership, int** numElem, int rows, int K, int DIM);

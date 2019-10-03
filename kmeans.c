#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("Usage: \n    ./kmeans [NUM_THREADS] [DATA_PATH]\n");
		exit(1);
	}
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
	int c;
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
	//begin kmeans
	//clean up
	printf("%s\n", strerror(errno));
	free(input);
	fclose(data);
	return 0;
}

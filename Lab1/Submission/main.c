#include <pthread.h>
#include "lab1_IO.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Global variables */
int p;
int n;
int **A;
int **B;
int **C;
double start, end;

/* Parallel function */
void *Pth_mat_vect(void *rank);

int main(int argc, char *argv[])
{
	GET_TIME(start);
	/* Allocate memory for C */
	Lab1_loadinput(&A, &B, &n);
	int i;
	C = malloc(n * sizeof(int *));
	for (i = 0; i < n; i++)
		C[i] = malloc(n * sizeof(int));

	/* Create threads */
	long thread;
	pthread_t *thread_handles;
	p = atoi(argv[1]);
	thread_handles = malloc(p * sizeof(pthread_t));
	for (thread = 0; thread < p; thread++)
		pthread_create(&thread_handles[thread], NULL,
					   Pth_mat_vect, (void *)thread);

	/* Join */
	for (thread = 0; thread < p; thread++)
		pthread_join(thread_handles[thread], NULL);


	/* Free memory */
	
	GET_TIME(end);
	double elapsed_time = start - end;
	Lab1_saveoutput(C, &n, elapsed_time);

	for (i = 0; i < n; i++)
	{
		free(A[i]);
		free(B[i]);
		free(C[i]);
	}
	free(A);
	free(B);
	free(C);



	return 0;
}

/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiply an nxn matrix by another nxn matrix
 * In arg:         rank
 * Global in vars: A, B, n, p
 * Global out var: C
 */
void *Pth_mat_vect(void *rank)
{
	int i, j, x, y, r;

	int k = (long)rank;
	x = floor(k / sqrt(p));
	y = k % (int)sqrt(p);

	int first_row = (n * x) / sqrt(p);
	int last_row = ((n * (x + 1)) / sqrt(p)) - 1;
	int first_column = n * y / sqrt(p);
	int last_column = ((n * (y + 1)) / sqrt(p)) - 1;

	for (i = first_row; i <= last_row; i++)
	{
		for (j = first_column; j <= last_column; j++)
		{
			C[i][j] = 0;
			for (r = 0; r < n; r++)
			{
				C[i][j] += A[i][r] * B[r][j];
			}
		}
	}

	return NULL;
}

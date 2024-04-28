/* File:    trap.c
 * Purpose: Calculate definite integral using trapezoidal 
 *          rule.
 *
 * Input:   a, b, n
 * Output:  Estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -o trap trap.c
 * Usage:   ./trap
 *
 * Note:    The function f(x) is hardwired.
 *
 * IPP2:    Section 3.2.1 (pp. 101 and ff.) and 5.2 (p. 228)
 */

#include <stdio.h>
#include <pthread.h>
#include "timer.h"
#include <stdlib.h>
#include <math.h>

const int MAX_THREADS = 4;

double  integral;   /* Store result in integral   */
double  a, b;       /* Left and right endpoints   */
int     n;          /* Number of trapezoids       */
double  h;          /* Height of trapezoids       */
int thread_count;
long trap_count;
pthread_mutex_t mutex;

double f(double x);    /* Function we're integrating */
double Serial_trap(double a, double b, int n, double h);
void* Thread_sum(void* rank);

int main(void) { 

    long       thread;  /* Use long in case of a 64-bit system */
    pthread_t* thread_handles;
    double start, finish, elapsed;

    printf("Enter the number of threads:\n");
    scanf("%d", &thread_count);

    if (thread_count <= 0 || thread_count > MAX_THREADS){
        printf("Enter the number of threads:\n");
        scanf("%d", &thread_count);
    }

    printf("Enter a, b, and n. (a<b, n>100)\n");
    scanf("%lf", &a);
    scanf("%lf", &b);
    scanf("%d", &n);

    if(a>=b){
    printf("Enter a, b, a should less than b\n");
    scanf("%lf", &a);
    scanf("%lf", &b);
    }

    if( n< 10000){
        printf("Enter n, should great than 10000\n");
        scanf("%d", &n);
    }

    thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    integral = 0.0;
    h = (double)(b-a)/n;
    trap_count = n/thread_count;
    if(thread_count == 1){
        GET_TIME(start);
        integral = Serial_trap(a,b,n,h);
        GET_TIME(finish);
        elapsed = finish - start;
        printf("With n = %d trapezoids, our estimate\n", n);
    }else{
        GET_TIME(start);
        for (thread = 0; thread < thread_count; thread++){
            pthread_create(&thread_handles[thread], NULL, Thread_sum, (void*)thread);
        }
        for (thread = 0; thread < thread_count; thread++){
            pthread_join(thread_handles[thread], NULL);
            }
        GET_TIME(finish);
        elapsed = finish - start;
        integral = h*integral;
        printf("With n = %d trapezoids, our estimate\n", n);
    }
    printf("of the integral from %f to %f = %.15f\n",
      a, b, integral);
    printf("The elapsed time is %e seconds\n", elapsed);
    pthread_mutex_destroy(&mutex);
    free(thread_handles);

   return 0;
}  /* main */

/*------------------------------------------------------------------
 * Function:    Trap
 * Purpose:     Estimate integral from a to b of f using trap rule and
 *              n trapezoids
 * Input args:  a, b, n, h
 * Return val:  Estimate of the integral 
 */

void* Thread_sum(void* rank){
    long my_rank = (long) rank;
    long double x;
    long double my_integral = 0.0;
    long double start = a+trap_count*my_rank*h;
    long double end = a+trap_count*(my_rank+1)*h;

    my_integral = (f(start) + f(end))/2.0;
    int roll;
    roll = trap_count/16;
    for (long long i = 1; i <= roll; i++)
    {
        //x = start+i*h;
        my_integral += f(start+i*h*16+0);
        my_integral += f(start+i*h*16+1);
        my_integral += f(start+i*h*16+2);
        my_integral += f(start+i*h*16+3);
        my_integral += f(start+i*h*16+4);
        my_integral += f(start+i*h*16+5);
        my_integral += f(start+i*h*16+6);
        my_integral += f(start+i*h*16+7);
        my_integral += f(start+i*h*16+8);
        my_integral += f(start+i*h*16+9);
        my_integral += f(start+i*h*16+10);
        my_integral += f(start+i*h*16+11);
        my_integral += f(start+i*h*16+12);
        my_integral += f(start+i*h*16+13);
        my_integral += f(start+i*h*16+14);
        my_integral += f(start+i*h*16+15);
    }

    pthread_mutex_lock(&mutex);
    integral += my_integral;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

double Serial_trap(double a, double b, int n, double h) {
   double integral;
   int k;

   integral = (f(a) + f(b))/2.0;
   for (k = 1; k <= n-1; k++) {
     integral += f(a+k*h);
   }
   integral = integral*h;

   return integral;
}  /* Trap */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input args:  x
 */
double f(double x) {
   double return_val;

   return_val = x*x;
   return return_val;
}  /* f */

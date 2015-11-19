#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#define MIN_X 1.0
#define MAX_X 4.0


struct thread_arg {
    double start;
    long sections;
    double h;
    double result;
};

double f(double x) {
    return x * x * x - x * x + x;
}

double integrate(double start, long sections, double h) {
    double sum = f(start);
    sum += f(start + sections * h);
    sum *= 0.5;
    long i;
    double x;
    for (i = 1; i < sections; i++) {
        x = start + i * h;
        sum += f(x);
    }
    return sum * h;
}

void *thread_integrate_wrap(void *arg) {
    struct thread_arg *data = (struct thread_arg *) arg;
    double result = integrate(data->start, data->sections, data->h);
    data->result = result;
}

int main(int argc, char *argv[]) {
    assert(argc > 2);
    long sections = atol(argv[1]);
    long size = atol(argv[2]);
    assert(sections > 0);
    assert(sections >= size);
    double h = (MAX_X - MIN_X) / sections;
    
    long i;
    long sections_per_thread = sections / size;
    printf("sections_per_thread %ld\n", sections_per_thread);
    struct thread_arg *thread_arguments = (struct thread_arg *) malloc(sizeof(struct thread_arg) * size);
    pthread_t **threads = malloc(sizeof(pthread_t *) * size);
    for (i = 0; i < size; i ++) {
        thread_arguments[i].start = MIN_X + h * (sections % size + sections_per_thread * i);
        thread_arguments[i].sections = sections_per_thread;
        thread_arguments[i].h = h;
        threads[i] = malloc(sizeof(pthread_t));
        if (pthread_create(threads[i], NULL, thread_integrate_wrap, &thread_arguments[i]) != 0) {
            printf("Couldn't create thread %ld\n", i);
            return 1;
        }
    }
    double sum = 0.0;
    if (sections % size != 0)
        sum += integrate(MIN_X, sections % size, h);
    printf("additional sum %lf\n", sum);
    for (i = 0; i < size; i ++) {
        if (pthread_join(*threads[i], NULL) != 0) {
            printf("Couldn't join thread %ld\n", i);
            return 1;
        }
        free(threads[i]);
        printf("%d-th thread returned %lf\n", i, thread_arguments[i].result);
        sum += thread_arguments[i].result;
    }
    printf("sum is %lf\n", sum);
    free(threads);
    free(thread_arguments);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>

void *sleeeeeeep(void *arg) {
    while (1) {
        sleep(10000);
    }
}

int main() {
/*    struct rlimit lim = {100, 100};
    getrlimit(RLIMIT_NPROC, &lim);
    printf("%llu\n", lim.rlim_cur);
    printf("%llu\n", lim.rlim_max);*/

    pthread_t thread;

    char *line = "Waiting for a cancellation...";

    int thread_count = 0;
    while (1) {
        int creation_code = pthread_create(&thread, NULL, sleeeeeeep, line);
        thread_count++;
        if (0 != creation_code) {
            fprintf(stderr, "Cannot create a thread %d: %d - %s\n", thread_count, creation_code, strerror(creation_code));
            //return EXIT_FAILURE;
            break;
        }
    }

    pthread_exit(EXIT_SUCCESS);

    printf("(main thread): Child thread was canceled!\n");
    return EXIT_SUCCESS;
}
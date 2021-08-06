#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define TH_INVALID_ARG (void*) 2
#define TH_EXIT_SUCCESS (void*) 0

#define SLEEP_TIME 2 /*Time in seconds*/

void *print_lines(void *arg) {
    const char *line = (char *) arg;
    if (NULL == line) {
        return TH_INVALID_ARG;
    }
    size_t line_length = strlen(line);
    if (0 == line_length) {
        return TH_INVALID_ARG;
    }

    int line_number = 1;
    while (strlen(line)) {
        printf("[%d] %s\n", line_number++, line);
        pthread_testcancel();
    }

    return TH_EXIT_SUCCESS;
}

int main() {
    pthread_t thread;

    char *line = "Waiting for a cancellation...";

    int creation_code = pthread_create(&thread, NULL, print_lines, line);
    if (0 != creation_code) {
        fprintf(stderr, "Cannot create a thread: %s\n", strerror(creation_code));
        return EXIT_FAILURE;
    }

    unsigned int remaining_sleep_time = SLEEP_TIME;
    while (remaining_sleep_time > 0) {
        remaining_sleep_time = sleep(SLEEP_TIME);
    }

    int cancellation_code = pthread_cancel(thread);
    if (0 != cancellation_code) {
        fprintf(stderr, "Cannot cancel the thread: %s\n", strerror(cancellation_code));
    }

    void *return_value;
    int join_code = pthread_join(thread, &return_value);
    if (0 != join_code) {
        fprintf(stderr, "Cannot join the child thread: %s\n", strerror(join_code));
        return EXIT_FAILURE;
    }

    if (TH_INVALID_ARG == return_value) {
        fprintf(stderr, "Illegal thread argument!\n");
        return EXIT_FAILURE;
    }
    if (PTHREAD_CANCELED != return_value) {
        fprintf(stderr, "Thread wasn't canceled!\n");
        return EXIT_FAILURE;
    }
    printf("(main thread): Child thread was canceled!\n");

    return EXIT_SUCCESS;
}
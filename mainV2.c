#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define EXECUTE 1
#define TH_INVALID_ARG (void*) 2
#define TH_EXIT_SUCCESS (void*) 0

#define SLEEP_TIME 2 /*Time in seconds*/
#define EXPECTED_ARG_COUNT 2

void print_err(int err_code, char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(err_code));
}

void final_routine(void *arg) {
    printf("(child thread): Current thread was canceled!\n");
}

void *print_lines(void *arg) {
    char *line = arg;
    if (NULL == line) {
        fprintf(stderr, "Illegal thread argument, expected NOT NULL string!\n");
        return TH_INVALID_ARG;
    }

    size_t line_length = strlen(line);
    if (0 == line_length) {
        fprintf(stderr, "Illegal thread argument, expected NOT EMPTY string!\n");
        return TH_INVALID_ARG;
    }

    int line_number = 1;
    pthread_cleanup_push(final_routine, NULL)
        while (strlen(line)) {
            printf("[%d] %s\n", line_number++, line);
            pthread_testcancel();
        }
    pthread_cleanup_pop(EXECUTE)

    return TH_EXIT_SUCCESS;
}

int stop_thread(pthread_t thread) {
    int cancellation_code = pthread_cancel(thread);
    if (0 != cancellation_code) {
        print_err(cancellation_code, "Cannot cancel the thread");
    }

    void *return_value;
    int join_code = pthread_join(thread, &return_value);
    if (0 != join_code) {
        print_err(join_code, "Cannot join the child thread");
        return EXIT_FAILURE;
    }

    if (PTHREAD_CANCELED != return_value) {
        fprintf(stderr, "(main thread): Child thread wasn't canceled!\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    pthread_t thread;

    if(argc > EXPECTED_ARG_COUNT){
        fprintf(stderr, "Expected only %d argument\n", EXPECTED_ARG_COUNT - 1);
        return EXIT_FAILURE;
    }
    char *line = argv[1];

    int creation_code = pthread_create(&thread, NULL, print_lines, line);
    if (0 != creation_code) {
        print_err(creation_code, "Cannot create a thread");
        free(line);
        return EXIT_FAILURE;
    }

    unsigned int remaining_sleep_time = SLEEP_TIME;
    while (remaining_sleep_time > 0) {
        remaining_sleep_time = sleep(SLEEP_TIME);
    }

    int stopping_code = stop_thread(thread);
    if(stopping_code != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    printf("(main thread): Child thread was canceled!\n");
    return EXIT_SUCCESS;
}
/*
 * UVic CSC 360, Summer 2021
 * This code copyright 2021: Roshan Lasrado, Mike Zastre
 *
 * Assignment 2: Task 1
 * --------------------
 * Simulate a Vaccination Center with `1` Registration Desk and `N` 
 * Vaccination Stations.
 * 
 * Input: Command Line args
 * ------------------------
 * ./vaccine <num_vaccination_stations `N`> <input_test_case_file>
 * e.g.
 *      ./vaccine 10 test1.txt
 * 
 * Input: Test Case file
 * ---------------------
 * Each line corresponds to person arrive for vaccinationn 
 * and is formatted as:
 *
 * <person_id>:<arrival_time>,<service_time>
 * 
 * NOTE: All times represented in `Tenths of a Second`.
 * 
 * Expected Sample Output:
 * -----------------------
 * Person 1: Arrived at 3.
 * Person 1: Added to the queue.
 * Vaccine Station 1: START Person 1 Vaccination.
 * Vaccine Station 1: FINISH Person 1 Vaccination.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"


/* 
 * Some compile-time constants related to assignment description.
 */
#define MAX_VAC_STATIONS 10
#define MAX_INPUT_LINE 100
#define TENTHS_TO_SEC 100000


Queue_t *queue;
pthread_mutex_t queue_mutex;
pthread_cond_t queue_condvar;
unsigned int num_vac_stations;
unsigned int is_vac_completed = false;


/*
 * Function: reg_desk
 * ------------------
 *  Registration Desk Thread.
 *  Reads the input file and adds the vaccination persons to the
 *  queue as per their arrival times.
 * 
 *  arg: Input file name
 *  
 *  returns: null
 */
void *reg_desk(void *arg) {
    char *filename = (char *)arg;

    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "File \"%s\" does not exist.\n", filename);
        exit(1);
    }

    char line[MAX_INPUT_LINE];
    unsigned int current_time = 0;

    while (fgets(line, sizeof(line), fp)) {
        int person_id;
        int person_arrival_time;
        int person_service_time;

        int vars_read = sscanf(line, "%d:%d,%d", &person_id, 
            &person_arrival_time, &person_service_time);

        if (vars_read == EOF || vars_read != 3) {
            fprintf(stderr, "Error reading from the file.\n");
            exit(1);
        }

        if (person_id < 0 || person_arrival_time < 0 || 
            person_service_time < 0)
        {
            fprintf(stderr, "Incorrect file input.\n");
            exit(1);
        }

        int arrival_time = person_arrival_time;

        // Sleep to simulate the persons arrival time.
        usleep((arrival_time - current_time) * TENTHS_TO_SEC);
        fprintf(stdout, "Person %d: arrived at time %d.\n", 
            person_id, arrival_time);

        // Update the current time based on simulated time elapsed.
        current_time = arrival_time;

        pthread_mutex_lock(&queue_mutex);
        PersonInfo_t *person_now = new_person();
        person_now->id = person_id;
        person_now->arrival_time = arrival_time;
        person_now->service_time = person_service_time;
        
        enqueue(queue, person_now);
        fprintf(stdout, "Person %d: Added to the queue\n", person_now->id);
	
        pthread_cond_signal(&queue_condvar);
        pthread_mutex_unlock(&queue_mutex);

    }

    fclose(fp);

    pthread_mutex_lock(&queue_mutex);
    is_vac_completed = true;
    pthread_cond_signal(&queue_condvar);
    pthread_cond_broadcast(&queue_condvar);
    pthread_mutex_unlock(&queue_mutex);

    return NULL;
}


/*
 * Function: vac_station
 * ---------------------
 *  Vaccination Station Thread.
 *  Vaccinate the persons from the queue as per their service times.
 *
 *  arg: Vaccination station number
 *
 *  returns: null
 *
 */
void *vac_station(void *arg) {
    int station_num = *((int *)arg);
    int current_service_time, current_id;


    while (true) {

        while (queue_size(queue) < 1 && is_vac_completed == false) {
	    pthread_cond_wait(&queue_condvar, &queue_mutex);
	}

	if ((is_vac_completed == true) && (is_empty(queue))) {
		pthread_cond_signal(&queue_condvar);
		pthread_mutex_unlock(&queue_mutex);
		return NULL;
	}
	
	current_service_time = queue->start->service_time;
	current_id = queue->start->id;
        fprintf(stdout, "Vaccine Station %d: START Person %d vaccination \n", station_num, current_id);
        dequeue(queue);
        pthread_cond_signal(&queue_condvar);
        pthread_mutex_unlock(&queue_mutex);

	usleep(current_service_time * TENTHS_TO_SEC);
	fprintf(stdout, "Vaccine Station %d: FINISH Person %d vaccination \n", station_num, current_id);
        
	if ((is_vac_completed == true) && (is_empty(queue))) {
		pthread_cond_signal(&queue_condvar);
		return NULL;
	}
    }
    
    return NULL;
}


/*
 * Function: validate_args
 * -----------------------
 *  Validate the input command line args.
 *
 *  argc: Number of command line arguments provided
 *  argv: Command line arguments
 */
void validate_args(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid number of input args provided! "
                        "Expected 2, received %d\n", 
                        argc - 1);
        exit(1);
    }

    num_vac_stations = atoi(argv[1]);
    if ((num_vac_stations <= 0) || (num_vac_stations > MAX_VAC_STATIONS)) {
        fprintf(stderr, "Vaccine stations exceeded the MAX LIMIT.%d\n", 
            argc - 1);
        exit(1);
    }

}

/*
 * Function: initialize_vars
 * -------------------------
 *  Initialize the mutex, conditional variable and the queue.
 */
void initialize_vars() {
    int cond_status;
    int mutex_status;
    mutex_status = pthread_mutex_init(&queue_mutex, NULL);
    if (mutex_status != 0) {
        fprintf(stderr, "Error creating mutex 'queue_mutex' \n");
        exit(1);
    }
    cond_status = pthread_cond_init(&queue_condvar, NULL);
    if (cond_status != 0) {
        fprintf(stderr, "Error creating condition variable 'queue_cond' \n");
        exit(1);
    }
    queue = init_queue();

}


/*
 * Function: main
 * -------------------------
 * argv[1]: Number of Vaccination Stations 
 * argv[2]: Input file/test case.
 */
int main(int argc, char *argv[]) {
    int i, status, thread_size;
    char* inputfile = argv[2];
    
    validate_args(argc, argv);

    initialize_vars();
    thread_size = num_vac_stations + 1;

    int vac_number[thread_size];	
    pthread_t threads[thread_size];
    
    pthread_create(&threads[0], NULL, reg_desk, inputfile);
    
    for (i = 1; i < thread_size; i++) {
	vac_number[i] = i;    
        status = pthread_create(&threads[i], NULL, vac_station, &vac_number[i]); //[1]
	
	if (status) {
		fprintf(stderr, "Error: unable to create thread %d", status);
		exit(1);
	}
    }
    
    pthread_join(threads[0], NULL); //[2]
    for (i = 1; i < thread_size; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_exit(NULL);
    return 0;
}

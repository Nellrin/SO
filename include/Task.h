#ifndef TASK_H
#define TASK_H

#include <sys/time.h>
struct timeval;

#include "Prog.h"





typedef enum {
    EXECUTING,
    SCHEDULED,
    COMPLETED
} Task_Status;


typedef struct {
    int id;
    
    short amount_programs; 
    Prog ** programs;
        
    struct timeval estimated_duration;
    struct timeval real_duration;
    struct timeval start_time;

    Task_Status status;
} Task;


Task * create_Task(int id, short amount_programs, char ** path_to_programs, short * amount_args, char *** args, char * estimated_duration);
void destroy_Task(Task *Task);
void execute_Task(Task *Task);
void print_Task_status(Task *Task);
Task **get_Tasks();
void print_task_debug(Task * x);
#endif
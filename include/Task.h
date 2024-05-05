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
    int pid;
    char * pipe_flag;
    
    short amount_programs; 
    Prog ** programs;
        
    struct timeval estimated_duration;
    struct timeval real_duration;
    struct timeval start_time;

    Task_Status status;
} Task;


Task * create_Task(int pid, char * pipe_flag, short amount_programs, char ** path_to_programs, short * amount_args, char *** args, char * estimated_duration);
void set_ids(Task * x, int id, char * output_file);
void destroy_Task(Task *Task);
void execute_Task(Task * x, char * output_file);
char * print_Task_status(Task *Task);
Task **get_Tasks(char * output_folder, int amount);
void print_task_debug(Task * x);


void write_Task(Task * x, int file);
Task * read_Task(int file);
#endif
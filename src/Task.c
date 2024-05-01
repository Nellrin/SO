

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/Task.h"
#include "../include/Queue.h"

Task * create_Task(int id, int pid, char * pipe_flag, short amount_programs, char ** path_to_programs, short * amount_args, char *** args, char * estimated_duration){
    Task * x = malloc(sizeof(Task));

    if (x == NULL) {
        perror("Não deu para alocar memória ... cringe");
        exit(EXIT_FAILURE);
    }

    x->id = id;
    x->pid = pid;
    x->pipe_flag = strdup(pipe_flag);

    x->estimated_duration.tv_sec = strtod(estimated_duration,NULL) / 1000;
    x->estimated_duration.tv_usec = (((int) strtod(estimated_duration,NULL)) % 1000) * 1000;

    x->programs = malloc(sizeof(Prog *) * amount_programs);
    x->amount_programs = amount_programs;

    for(int i = 0; i < amount_programs; i++)
    x->programs[i] = create_Prog(path_to_programs[i],amount_args[i],args[i]);

    x->status = SCHEDULED;

    return x;
}
void destroy_Task(Task * x){
    for(int i = 0; i< x->amount_programs; i++)
    destroy_Prog(x->programs[i]);
    
    free(x->programs);
    free(x->pipe_flag);
    free(x);
}
void execute_Task(Task * x, char * output_file){

    gettimeofday(&(x->start_time), NULL);

        char * filename = malloc(sizeof(char) * 128);
        sprintf(filename, "%s/%d.txt", output_file, x->id);
        int output = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);

            x->status = EXECUTING;

            // snprintf(filename,128,"%s/done_tasks.bin",output_file);
            // int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            // lseek(done, 0, SEEK_END);
            // write(done, x, sizeof(Task));
        


    dup2(output, STDOUT_FILENO);
    dup2(output, STDERR_FILENO);


        if(!strcmp(x->pipe_flag,"-u"))
        execute_single_Prog(x->programs[0]);

        if(!strcmp(x->pipe_flag,"-p"))
        execute_multiple_Prog(x->programs, x->amount_programs);


        close(output);
        free(filename);


        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        timersub(&current_time, &(x->start_time), &(x->real_duration));



            x->status = COMPLETED;

            // lseek(done, 0, SEEK_END);
            // write(done, x, sizeof(Task));
}
Task **get_Tasks() {
    
    int fd = open("output_folder/done.bin", O_RDONLY | O_CREAT);

    lseek(fd, 0, SEEK_SET);

    Task ** list_of_tasks = NULL;
    

    Task *task = malloc(sizeof(Task));

    for(int num_tasks = 0; read(fd, task, sizeof(Task)) > 0; num_tasks++) {
        list_of_tasks = realloc(list_of_tasks, (num_tasks + 1) * sizeof(Task *));
        if (list_of_tasks == NULL) {
            perror("Ficheiro bem mid ngl");
            close(fd);
            return NULL;
        }

        task = malloc(sizeof(Task));
        list_of_tasks[num_tasks] = task;
    }


    close(fd);
    free(task);

    return list_of_tasks;
}

void print_Task_status(Task *x){
    char * lista = malloc(sizeof(char) * 2048 * 4);
    snprintf(lista,2048*4,"%d %s", x->id, x->programs[0]->path_to_program);

    for (int i = 1; i < x->amount_programs; ++i) 
    sprintf(lista," | %s", x->programs[i]->path_to_program);

    if(x->status == COMPLETED){
        double ms = x->real_duration.tv_sec * 1000;
        ms += x->real_duration.tv_usec / 1000;
   
        sprintf(lista," %d ms", (int) ms);
    }

    sprintf(lista," \n");

    write(STDOUT_FILENO, lista, strlen(lista));
}
Task* grabTask(TTL *queue) {
    // Modificar el puntero original
    Task* taskToExec = queue->task; // Nuevo valor del puntero
    TTL *kill = queue;
    queue = queue->next;
    free(kill);
    return taskToExec;
}
void print_task_debug(Task * x){
    printf("\n\n──────────────────────────────────\n");
    printf("[TASK %03d]\n",x->id);
    printf("(%d) PROGRAMS\n",x->amount_programs);

    for(int i = 0; i < x->amount_programs; i++){
        printf("\n    +──────────────────────────────────\n");

        printf("    |%s (%d)\n    |\n",x->programs[i]->path_to_program, x->programs[i]->amount_args);
        for(int j = 0; j < x->programs[i]->amount_args; j++)
        printf("    |%s\n",x->programs[i]->args[j]);

        printf("    +──────────────────────────────────\n");
    }

    printf("\nSTATUS: ");
    switch (x->status){
    case COMPLETED:
        printf("COMPLETE\n");
        break;

    case EXECUTING:
        printf("EXECUTING\n");
        break;

    case SCHEDULED:
        printf("SCHEDULED\n");
        break;
    
    default:
        break;
    }
    printf("──────────────────────────────────\n");
}
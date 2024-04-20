#include "../../include/Entidades/Task.h"
#include "../../include/Entidades/Prog.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>


Task * create_Task(int id, short amount_programs, char ** path_to_programs, short * amount_args, char *** args, char * estimated_duration){
    Task * x = malloc(sizeof(Task));

    if (x == NULL) {
        perror("Não deu para alocar memória ... cringe");
        exit(EXIT_FAILURE);
    }

    x->id = id;

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
    free(x);
}
void execute_Task(Task * x){

    gettimeofday(&(x->start_time), NULL);

        char * filename = malloc(sizeof(char) * 128);
        sprintf(filename, "output_folder/%d.bin", x->id);
        int errors = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        
        snprintf(filename,128,"output_folder/done_tasks.bin");
        int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);




    dup2(errors, STDOUT_FILENO);
    dup2(errors, STDERR_FILENO);


        for(int i = 0; i < x->amount_programs; i++)
        execute_Prog(x->programs[i]);


        close(errors);
        free(filename);


        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        timersub(&current_time, &(x->start_time), &(x->real_duration));



            x->status = COMPLETED;

            lseek(done, 0, SEEK_END);
            write(done, x, sizeof(Task));
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
        if (task == NULL) {
            perror("Oops, task vazia :v");
            close(fd);
            free_tasks(list_of_tasks, num_tasks);
            return NULL;
        }

        list_of_tasks[num_tasks] = task;
    }


    close(fd);
    free(task);

    return list_of_tasks;
}

void print_Task_status(Task *x){
    printf("%d",x->id);

    printf(" %s", x->programs[0]->path_to_program);
    
    for (int i = 1; i < x->amount_programs; ++i) 
    printf(" | %s", x->programs[i]->path_to_program);
    
    if(x->status == COMPLETED){
        double ms = x->real_duration.tv_sec * 1000;
        ms += x->real_duration.tv_usec / 1000;
   
        printf(" %d ms", ms);
    }

    printf("\n");
}
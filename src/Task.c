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

Task * create_Task(int pid, char * pipe_flag, short amount_programs, char ** path_to_programs, short * amount_args, char *** args, char * estimated_duration){
    Task * x = malloc(sizeof(Task));

    if (x == NULL) {
        perror("Não deu para alocar memória ... cringe");
        exit(EXIT_FAILURE);
    }

    x->id = 0;
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
void set_ids(Task * x, int id, char * output_file){
    x->id = id;

    char * filename = malloc(sizeof(char) * 128);
    snprintf(filename,128,"%s/done_tasks.bin",output_file);
    int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);

    lseek(done, 0, SEEK_END);
    write(done, x, sizeof(Task));

    close(done);
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

            x->status = EXECUTING;

            // snprintf(filename,128,"%s/done_tasks.bin",output_file);
            // int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            // lseek(done, 0, SEEK_END);
            // write(done, x, sizeof(Task));
        


    // dup2(output, STDERR_FILENO);

    // printf("flag = %s\n %s = %d\n\n",x->pipe_flag,x->pipe_flag, !strcmp(x->pipe_flag,"-u"));


        if(!strcmp(x->pipe_flag,"-u"))
        execute_single_Prog(x->programs[0],x->id, output_file);

        if(!strcmp(x->pipe_flag,"-p"))
        execute_multiple_Prog(x->programs, x->amount_programs,x->id,output_file);




        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        timersub(&current_time, &(x->start_time), &(x->real_duration));


            x->status = COMPLETED;

            // lseek(done, 0, SEEK_END);
            // write(done, x, sizeof(Task));
}
Task **get_Tasks(char * output_folder, int amount){

    char * folder = malloc(sizeof(char) * 512);
    snprintf(folder,512,"%s/done_task.bin",output_folder);

    int fd = open(folder, O_RDONLY | O_CREAT);


    Task ** list_of_tasks = malloc(sizeof(Task *) * amount);
    printf("\n\n%d\n\n",amount);
    

    // Task *task = malloc(sizeof(Task));

    // for(int num_tasks = 0; num_tasks < amount; num_tasks++){
    //     list_of_tasks[num_tasks] = task;
    //     read(fd, task, sizeof(Task));
    // }
///////////////////////////////////////////////////////////////////////////////
    lseek(fd, 0, SEEK_SET);
    
    for (int i = 0; i < amount; i++){
        list_of_tasks[i] = create_Task(0,NULL,0,NULL, NULL,NULL,NULL);
        read(fd, list_of_tasks[i], sizeof(Task));
        print_task_debug(list_of_tasks[i]);
    }

    close(fd);

    return list_of_tasks;
}

char * print_Task_status(Task *x){
    char * lista = malloc(sizeof(char) * 256 * 4);
    snprintf(lista,256*4,"%d %s", x->id, x->programs[0]->path_to_program);

    for (int i = 1; i < x->amount_programs; ++i) 
    sprintf(lista," | %s", x->programs[i]->path_to_program);

    if(x->status == COMPLETED){
        double ms = x->real_duration.tv_sec * 1000;
        ms += x->real_duration.tv_usec / 1000;
   
        sprintf(lista," %d ms", (int) ms);
    }

    sprintf(lista," \n");

    return lista;
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

void write_Task(Task * x, int file){
    write(file, &x->id, sizeof(int));
    write(file, &x->pid, sizeof(int));


    size_t pipe_flag_size = strlen(x->pipe_flag) + 1;
    write(file, &pipe_flag_size, sizeof(size_t));
    write(file, x->pipe_flag, pipe_flag_size);


    write(file, &x->amount_programs, sizeof(short));

    for(int i = 0; i < x->amount_programs; i++)
    write_Prog(x->programs[i],file);


    write(file, &x->estimated_duration, sizeof(struct timeval));
    write(file, &x->real_duration, sizeof(struct timeval));
    write(file, &x->start_time, sizeof(struct timeval));
    write(file, &x->status, sizeof(Task_Status));
}
Task * read_Task(int file){
    Task * x = malloc(sizeof(Task));


    read(file, &(x->id), sizeof(int));
    read(file, &(x->pid), sizeof(int));

    size_t pipe_flag_size;
    read(file, &pipe_flag_size, sizeof(size_t));
    x->pipe_flag = malloc(pipe_flag_size);
    read(file, x->pipe_flag, pipe_flag_size);


    read(file, &(x->amount_programs), sizeof(short));

    
    x->programs = malloc(sizeof(Prog) * x->amount_programs);

    
    for(int i = 0; i < x->amount_programs; i++)
    x->programs[i] = read_Prog(file);


    read(file, &(x->estimated_duration), sizeof(struct timeval));
    read(file, &(x->real_duration), sizeof(struct timeval));
    read(file, &(x->start_time), sizeof(struct timeval));
    read(file, &(x->status), sizeof(Task_Status));


    return x;
}
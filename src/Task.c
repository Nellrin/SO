#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/Task.h"
#include "../include/Prog.h"
#include "../include/Queue.h"
#include "../include/Auxiliares.h"

Task * create_Task(int pid, char * pipe_flag, short amount_programs, char ** path_to_programs, short * amount_args, char *** args, char * estimated_duration){
    Task * x = malloc(sizeof(Task));

    x->id = 0;
    x->pid = pid;
    x->pipe_flag = strdup(pipe_flag);

    x->estimated_duration = strtod(estimated_duration,NULL);
    x->real_duration = 0;

    x->programs = malloc(sizeof(Prog *) * amount_programs);
    x->amount_programs = amount_programs;

    for(int i = 0; i < amount_programs; i++)
    x->programs[i] = create_Prog(path_to_programs[i],amount_args[i],args[i]);

    x->status = SCHEDULED;

    return x;
}
void set_ids(Task * x, int id, char * output_file){

    char * filename = malloc(sizeof(char) * 128);
    snprintf(filename,128,"%s/done_tasks.bin",output_file);
    int done = open(filename, O_RDWR, 0644), y = id;;

    x->id = id;

    lseek(done,0,SEEK_SET);
    write(done,&y,sizeof(int));

    lseek(done, 0, SEEK_END);
    write_Task(x,done);

    close(done);
    free (filename) ;
}
void destroy_Task(Task * x){
    for(int i = 0; i< x->amount_programs; i++)
    destroy_Prog(x->programs[i]);

    free(x->programs);
    free(x->pipe_flag);
    free(x);
}
long execute_Task(Task * x, char * output_file){

    struct timeval start_time, current_time, result;
    gettimeofday(&start_time, NULL);

            x->status = EXECUTING;

        if(!strcmp(x->pipe_flag,"-u"))
        execute_single_Prog(x->programs[0],x->id, output_file);

        if(!strcmp(x->pipe_flag,"-p"))
        execute_multiple_Prog(x->programs, x->amount_programs,x->id,output_file);


    gettimeofday(&current_time, NULL);


        timersub(&current_time, &start_time, &result);

        long res = (result.tv_sec * 1000) + (result.tv_usec / 1000);


    return res;
}
Task **get_Tasks(char * output_folder, int amount){

    char *folder = malloc(sizeof(char) * 512);
    snprintf(folder, 512, "%s/done_tasks.bin", output_folder);

    int fd = open(folder, O_RDONLY);

    Task ** list_of_tasks = malloc(sizeof(Task *) * (amount));
    lseek(fd, 4, SEEK_SET);

    for (int i = 0; i < (amount); i++)
    list_of_tasks[i] = read_Task(fd);

    close(fd);

    return list_of_tasks;
}

char * print_Task_status(Task *x){
    char * lista = malloc(sizeof(char) * 256 * 4);
    snprintf(lista,256*4,"%d %s", x->id, x->programs[0]->path_to_program);

    for (int i = 1; i < x->amount_programs; ++i)
    sprintf(lista + strlen(lista)," | %s", x->programs[i]->path_to_program);

    if(x->status == COMPLETED){
        sprintf(lista + strlen(lista)," %d ms", (int) (x->real_duration));
    }

    sprintf(lista + strlen(lista)," \n");

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
        printf("COMPLETE\n%lds\n", x->real_duration);
        break;

    case EXECUTING:
        printf("EXECUTING\n%lds\n", x->estimated_duration);
        break;

    case SCHEDULED:
        printf("SCHEDULED\n%lds\n",x->estimated_duration);
        break;

    default:
        break;
    }
    printf("──────────────────────────────────\n");
}

void write_Task(Task * x, int file){
    write(file, &(x->id), sizeof(int));
    write(file, &(x->pid), sizeof(int));


    size_t pipe_flag_size = strlen(x->pipe_flag) + 1;
    write(file, &(pipe_flag_size), sizeof(size_t));
    write(file, x->pipe_flag, pipe_flag_size);


    write(file, &(x->amount_programs), sizeof(short));

    for(int i = 0; i < x->amount_programs; i++)
    write_Prog(x->programs[i],file);


    write(file, &(x->estimated_duration), sizeof(long));
    write(file, &(x->real_duration), sizeof(long));
    write(file, &(x->status), sizeof(Task_Status));

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


    read(file, &(x->estimated_duration), sizeof(long));
    read(file, &(x->real_duration), sizeof(long));
    read(file, &(x->status), sizeof(Task_Status));


    return x;
}
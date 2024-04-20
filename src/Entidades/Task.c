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

    for(int i = 0; i < amount_args; i++)
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
        
        freopen(filename, "w", stdout);
        freopen(filename, "w", stderr);

       
       
        int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        write(fd, x, sizeof(Task));



        for(int i = 0; i < x->amount_programs; i++)
        execute_Prog(x->programs[i]);



        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        timersub(&current_time, &(x->start_time), &(x->real_duration));



            x->status = COMPLETED;

            lseek(fd, 0, SEEK_SET);
            write(fd, x, sizeof(Task));
            


        close(fd);
        free(filename);
}
Task * read_Task_from_ID(int id) {

    const char *filename;
    asprintf(&filename, "%d.bin", id);

    int fd = open(filename, O_RDONLY);

    free(filename);


    if (fd == -1) {
        perror("O ficheiro não existe");
        exit(EXIT_FAILURE);
    }

    Task * x = malloc(sizeof(Task));
    read(fd, x, sizeof(Task));
    close(fd);

    return x;
}
void print_Task_status(Task *x){
    printf("%d",x->id);

    printf(" %s", x->programs[0]);
    
    for (int i = 1; i < x->amount_programs; ++i) 
    printf(" | %s", x->programs[i]->path_to_program);
    
    if(x->status == COMPLETED){
        double ms = x->real_duration.tv_sec * 1000;
        ms += x->real_duration.tv_usec / 1000;
   
        printf("%d ms", ms);
    }

    printf("\n");
}
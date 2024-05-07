#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/Prog.h"

Prog * create_Prog(char * path_to_program, short amount_args, char ** args){
    Prog * x = malloc(sizeof(Prog));

    x->path_to_program = strdup(path_to_program);
    x->amount_args = amount_args;

    x->args = malloc(sizeof(char *) * amount_args);

    for(int i = 0; i < amount_args; i++)
    x->args[i] = strdup(args[i]);

    return x;
}
void execute_single_Prog(Prog * x, int id, char * output_file){

    char * filename = malloc(sizeof(char) * 128);
    sprintf(filename, "%s/%d.txt", output_file, id);
    int output = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644), status;

    pid_t pid = fork();

    if(pid == 0){

    dup2(output, STDERR_FILENO);
    dup2(output, STDOUT_FILENO);
        execvp(x->path_to_program, x->args);
        perror(x->path_to_program);
        _exit(0);
    }

    else
    waitpid(pid, &status, 0);

}
void execute_multiple_Prog(Prog ** x, int amount, int id, char * output_file){

    char * filename = malloc(sizeof(char) * 128);
    sprintf(filename, "%s/%d.txt", output_file, id);
    int output = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644), pipes[amount - 1][2];



    dup2(output, STDERR_FILENO);
    dup2(output, STDOUT_FILENO);
    close(output);
    free (filename) ;



    for(int i = 0; i < amount - 1; i++)
    pipe(pipes[i]);




    for(int i = 0; i < amount; i++){

        if(fork() == 0){
            for(int j = 0; j < amount; j++){
                if(j != i && j != i - 1){
                    close(pipes[j][1]);
                    close(pipes[j][0]);
                }
            }


                if(i < amount - 1){
                    close(pipes[i][0]);

                    dup2(pipes[i][1], STDOUT_FILENO);
                    close(pipes[i][1]);
                }

                if(i > 0){
                    close(pipes[(i - 1)][1]);
                    dup2(pipes[(i - 1)][0], STDIN_FILENO);
                    close(pipes[(i - 1)][0]);
                }

                execvp(x[i]->path_to_program, x[i]->args);
                perror(x[i]->path_to_program);
                _exit(0);
        }
        else{
            if(i > 0)
            close(pipes[i-1][1]);
        }

    }

    for (int i = 0; i < amount - 1; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}
void destroy_Prog(Prog * x){
    free(x->path_to_program);
    x->path_to_program = NULL ;
    for(int i = 0; i<x->amount_args; i++)
    {
        free(x->args[i]);
        x->args[i] = NULL ;
    }
    free(x->args);
    x->args = NULL ;
    free(x);
    x = NULL ;
}
void write_Prog(Prog * x, int file){
    size_t path_size = strlen(x->path_to_program) + 1;
    write(file, &path_size, sizeof(size_t));
    write(file, x->path_to_program, path_size);

    write(file, &(x->amount_args), sizeof(short));

    for(int i = 0; i < x->amount_args; i++){
        size_t size_arg = strlen(x->args[i]) + 1;
        write(file, &size_arg, sizeof(size_t));
        write(file, x->args[i], size_arg);
    }
}
Prog * read_Prog(int file){
    Prog * x = malloc(sizeof(Prog));

    size_t path_size;
    read(file, &path_size, sizeof(size_t));

    x->path_to_program = malloc(path_size);
    read(file, x->path_to_program, path_size);


    read(file, &(x->amount_args), sizeof(short));


    x->args = malloc(sizeof(char *) * x->amount_args);
    for (int i = 0; i < x->amount_args; i++) {
        size_t size_arg;
        read(file, &size_arg, sizeof(size_t));

        x->args[i] = malloc(size_arg);
        read(file, x->args[i], size_arg);
    }


    return x;
}
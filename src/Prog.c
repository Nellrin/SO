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
void execute_single_Prog(Prog * x){

    pid_t pid = fork();
    int status;
    
    if(pid == 0){
        execvp(x->path_to_program, x->args);
        perror(x->path_to_program); 
        _exit(1);
    }
    
    else
    waitpid(pid, &status, 0);

}

void execute_multiple_Prog(Prog ** x, int amount){
    int status;
    int pipes[amount - 1][2];
    
    for (int i = 0; i < amount - 1; i++)
    pipe(pipes[i]);

    for (int i = 0; i < amount; i++){
        if (fork() == 0){
            if (i < amount - 1){
                close(pipes[i][0]);
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
            }

            if (i > 0) {
                close(pipes[(i - 1)][1]);
                dup2(pipes[(i - 1)][0], STDIN_FILENO);
                close(pipes[(i - 1)][0]);
            }

            execvp(x[i]->path_to_program, x[i]->args);
            perror(x[i]->path_to_program); 
            _exit(1);
        }
        else
        close(pipes[i][1]);
        
    }
    
    for (int i = 0; i < amount - 1; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

void destroy_Prog(Prog * x){
    free(x->path_to_program);
    for(int i = 0; i<x->amount_args; i++)
    free(x->args[i]);
    free(x->args);
    free(x);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../../include/Entidades/Prog.h"

Prog * create_Prog(char * path_to_program, short amount_args, char ** args){
    Prog * x = malloc(sizeof(Prog));

    if (x == NULL) {
        perror("Não deu para alocar memória ... cringe");
        exit(EXIT_FAILURE);
    }

    x->path_to_program = strdup(path_to_program);
    x->amount_args = amount_args;

    x->args = malloc(sizeof(char *) * amount_args);

    for(int i = 0; i < amount_args; i++)
    x->args[i] = args[i];

    return x;
}
void execute_Prog(Prog * x){

    pid_t pid = fork();

    if(pid == -1){
        perror("O garfo não garfou");
        exit(EXIT_FAILURE);
    }
    
    else if(pid == 0)
    execvp(x->path_to_program, x->args);
    
    else{
        int status;
        waitpid(pid, &status, 0);

        if(WIFEXITED(status));
    }

}
void destroy_Prog(Prog * x){
    free(x->path_to_program);
    for(int i = 0; i<x->amount_args; i++)
    free(x->args[i]);
    free(x->args);
    free(x);
}
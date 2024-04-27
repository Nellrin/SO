#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/Auxiliares.h"

void parse_string(char *mode, char *input, char ***path_to_programs, char ****args, short *amount_programs, short **amount_args) {
    char *input_copy = strdup(input);
    char *token;

    if(strcmp(mode, "-u") == 0) {

        (*amount_programs) = 1;
        (*path_to_programs) = malloc((*amount_programs) * sizeof(char *));
        //printf("check\n");
    
        token = strtok(input_copy, " ");
        (*path_to_programs)[0] = strdup(token);
        //printf("check\n");

        (*amount_args)[0] = 0;
        (*args) = malloc((*amount_programs) * sizeof(char ***));
        (*args)[0] = malloc((*amount_args)[0] * sizeof(char **));
        //printf("check\n");
        

        for(int i = 0; token != NULL; i++) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                (*amount_args)[0] = i+1;
                (*args)[0] = realloc((*args)[0], (*amount_args)[0] * sizeof(char **));
                (*args)[0][i] = strdup(token);
                //printf("%s\n", (*arguments)[i]); // Corregido: Acceso a arguments como puntero a puntero
            }
        }
        
    }

    //$ ./client execute 3000 -p "prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n"
    if (strcmp(mode, "-p") == 0) {
        for (int i = 0; (token = strtok(input_copy, "|")) != NULL; i++) { //prog-a arg-1 (...) arg-n
            token = strtok(NULL, " "); //prog-a
            (*amount_programs) = i+1;
            //realloc((*path_to_programs), (*amount_programs) * sizeof(char *));
        }

        //POR HACER
        //POR HACER
        //POR HACER Y COLOCAR ESTA FUNCION EN OTRO ARCHIVO

    }

    free(input_copy);
}
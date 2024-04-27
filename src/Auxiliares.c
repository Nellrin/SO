#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

void parse_string(char *mode, char *input, char ***path_to_programs, char ****args, short *amount_programs, short **amount_args) {
    char *input_copy = strdup(input);
    char *token;

    if(strcmp(mode, "-u") == 0) {

        (*amount_programs) = 1;
        (*path_to_programs) = malloc((*amount_programs) * sizeof(char *));
        //printf("check\n");
    
        token = strtok(input_copy, " ");
        (*path_to_programs)[0] = strdup(token);
        printf("%s\n", (*path_to_programs)[0]);

        //printf("check\n");
        (*amount_args)[0] = 0;
        //printf("xd\n");
        (*args) = malloc((*amount_programs) * sizeof(char ***));
        (*args)[0] = malloc((*amount_args)[0] * sizeof(char **));
        //printf("check\n");
        

        for(int i = 0; token != NULL; i++) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                (*amount_args)[0] = i+1;
                (*args)[0] = realloc((*args)[0], (*amount_args)[0] * sizeof(char **));
                (*args)[0][i] = strdup(token);
                //printf("%s\n", (*args)[0][i]); // Corregido: Acceso a arguments como puntero a puntero
            }
        }
        
    }

    //$ ./client execute 3000 -p "prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n"
    if (strcmp(mode, "-p") == 0) {
        for (int i = 0; (token = strsep(&input_copy, "|")) != NULL; i++) { //prog-a arg-1 (...) arg-n
            //if (input_copy == NULL) break;
            
            if(i != 0) token = token+1;
            printf("%s\n", token);
            //input_copy = strtok(NULL, " "); //prog-a
            //input_copy = strsep(&input_copy, "|") //prog-a
            //input_copy = input_copy+1;
            //printf("%s\n", input_copy);

            (*amount_programs) = i+1;
            (*path_to_programs) = malloc((*amount_programs) * sizeof(char *));
            token = strtok(token, " ");
            (*path_to_programs)[i] = strdup(token); //prog-a
            printf("%s\n", (*path_to_programs)[i]);

            (*amount_args)[i] = 0;
            (*args) = malloc((*amount_programs) * sizeof(char ***));
            (*args)[i] = malloc((*amount_args)[i] * sizeof(char **));

            for(int j = 0; token != NULL; j++) {
                token = strtok(NULL, " "); //arg-1
                if (token != NULL) {
                    (*amount_args)[i] = j+1;
                    (*args)[i] = realloc((*args)[i], (*amount_args)[i] * sizeof(char **));
                    (*args)[i][j] = strdup(token);
                    printf("%s\n", (*args)[i][j]); // Corregido: Acceso a arguments como puntero a puntero
                }
            }

            //input_copy = token;
            printf("\n");
        }
    }
    

    free(input_copy);
}

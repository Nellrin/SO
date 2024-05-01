#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/Task.h"

int amount_chars(char * string, char c){
    int amount = 0;

    for(int i = 0; i<strlen(string); i++)
    amount += (string[i] == c);

    return amount;
}

/*
typedef struct {
    [DONE] int id,
    [DONE] short amount_programs, 
    [DONE] char **path_to_programs, 
    short *amount_args, 
    char ***args, 
    char *estimated_duration
} Task;

*/
static char ** string_to_array(char *str, int *amount) {
    *amount = 1+ amount_chars(str,' ');
    char ** list = malloc(sizeof(char *) * (*amount));


    char * new_str = strdup(str);
    char * token;
    int i = 0;
    
    while((token = strsep(&new_str, " \0")) != NULL && i < (*amount)) {
        if(*token != '\0') {
            list[i] = strdup(token);
            i++;
        }
    }

    free(new_str);
    
    if(token!=NULL)
    free(token);

    return list;
}

// void parse_string(char *mode, char *input, char ***path_to_programs, char ****args, short *amount_programs, short **amount_args) {
Task * parse_string(int id, char * pipe_flag, char * time, char *argv, char * output_file){
    short amount_programs = 1 + amount_chars(argv,'|');
    char ** path_to_programs = malloc(sizeof(char *) * amount_programs);
    short * amount_args = calloc(sizeof(short), amount_programs);
    char *** args = malloc(sizeof(char **) * amount_programs);

    int amount_palavras = 0;
    char ** palavras = string_to_array(argv, &amount_palavras);


    for(int i = 0; i < amount_programs;)
    for(int j = 0; j < amount_palavras;j++){
        if(j == 0 || !strcmp("|",palavras[j - 1])){
            path_to_programs[i] = strdup(palavras[j]);



            for(int k = 0; k + j + 1 < amount_palavras && (!strcmp(palavras[k+j+1],"|") == 0); k++){
                // printf("\nK = %d\n\n",amount_palavras);
                // printf("%s\n",palavras[k]);
                amount_args[i]++;
            }

            args[i] = malloc(sizeof(char *) * amount_args[i]);

            int m = 0;
            for(int k = 0; k + j + 1 < amount_palavras && (!strcmp(palavras[k+j+1],"|") == 0); k++){
                args[i][k] = strdup(palavras[1 + k + j]); m++;
            }
                printf("%d\n", amount_args[i]);
            i++;
        }
    
    }



   //$ ./client execute 3000 -p "prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n"

    Task * x = create_Task(id,pipe_flag, amount_programs, path_to_programs, amount_args, args, time);

    char * filename = malloc(sizeof(char) * 128);
    snprintf(filename,128,"%s/done_tasks.bin",output_file);
    int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);

    lseek(done, 0, SEEK_END);
    write(done, x, sizeof(Task));



    for(int i = 0; i < amount_programs; i++) free(path_to_programs[i]);
        free(path_to_programs);
        for(int i = 0; i < amount_palavras; i++) free(palavras[i]);
        free(palavras);
        for (int i = amount_programs-1; 0 <= i; i--) {
            for(int j = 0; j < amount_args[i]; j++) {
                free(args[i][j]);
            }
            free(args[i]);
        }
        free(args);
        free(amount_args);

    return x;
}
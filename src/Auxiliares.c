#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/Task.h"

char *remove_quotes(const char *input) {
    size_t input_length = strlen(input);
    char *output = (char *)malloc(input_length + 1); // +1 para o caractere nulo no final
    if (output == NULL) {
        // Tratar erro de alocação de memória
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < input_length; i++) {
        if (input[i] != '\"') {
            output[j++] = input[i];
        }
    }
    output[j] = '\0'; // Adiciona o caractere nulo ao final da string

    return output;
}

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

/*
    int id;
    int pid;
    char * output;
*/
Task * parse_string(int pid, char * pipe_flag, char * time, char *argv){
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
            amount_args[i]++;

            args[i] = malloc(sizeof(char *) * amount_args[i] + 1);
            args[i][0] = strdup(path_to_programs[i]);

            int m = 0;
            for(int k = 0; k + j + 1 < amount_palavras && (!strcmp(palavras[k+j+1],"|") == 0); k++){
                args[i][k+1] = strdup(palavras[1 + k + j]); m++;
            }
                //printf("%d\n", amount_args[i]);
            i++;
        }
    
    }



   //$ ./client execute 3000 -p "prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n"

    Task * x = create_Task(pid,pipe_flag, amount_programs, path_to_programs, amount_args, args, time);


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

void new_status(char *folder, int task_id, Task_Status estado){
    char *filename = malloc(sizeof(char) * 128);
    snprintf(filename, 128, "%s/done_tasks.bin", folder);
    int fd = open(filename, O_RDWR, 0644);

    lseek(fd, sizeof(int), SEEK_SET);

    Task *temp_task;
    off_t offset = sizeof(int);

    while((temp_task = read_Task(fd)) != NULL){
        if(temp_task->id == task_id){
            temp_task->status = estado;

            lseek(fd, offset, SEEK_SET);
            write_Task(temp_task, fd);

            lseek(fd, 0, SEEK_END);
            destroy_Task(temp_task);
            break;
        }

        offset = lseek(fd, 0, SEEK_CUR);
        destroy_Task(temp_task);
    }

    close(fd);
}

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

            args[i] = malloc(sizeof(char *) * amount_args[i]);

            int m = 0;
            for(int k = 0; k + j + 1 < amount_palavras && (!strcmp(palavras[k+j+1],"|") == 0); k++){
                args[i][k] = strdup(palavras[1 + k + j]); m++;
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

void new_status(char * folder, int task_id, Task_Status estado){
    char * filename = malloc(sizeof(char) * 128);
    snprintf(filename,128,"%s/done_tasks.bin",folder);
    int fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0644), r; //r => read xd
    Task * tk = malloc(sizeof(Task));
    while ((r = read(fd, tk, sizeof(Task))) > 0){//le alguma coisa
        if (tk->id == task_id){
            tk->status = estado;
            lseek(fd, -sizeof(Task), SEEK_CUR);
            // lseek é uma chamada de sistema que é usada para alterar a posição do cursor de leitura ou gravação em um arquivo. Nesse caso, fd é o descritor de arquivo associado ao arquivo binário.
            // -sizeof(Task) é o deslocamento negativo que você deseja mover o cursor. sizeof(Task) retorna o tamanho da estrutura struct Task em bytes, e o sinal negativo indica que queremos retroceder essa quantidade de bytes.
            // SEEK_CUR é um argumento que indica que o deslocamento deve ser relativo à posição atual do cursor no arquivo.

            // Escrevendo a Task atualizada
            write(fd, tk, sizeof(Task));
        }
    }   
}

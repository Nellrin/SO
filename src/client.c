#include "../include/Auxiliares.h"
#include "../include/client.h"
#include "../include/Task.h"
#include "../include/Prog.h"



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
//#include "defs.h"

//short amount_programs, char ** path_to_programs, short * amount_args, char *** args

int main(int argc, char* argv[]){

    // printf("%d\n", argc);
    // for(int i = 0; i < argc; i++) {
    //     printf("%s\n", argv[i]);
    // }

    for(int i = 0; i < argc;i++)
    printf("[%d] %s\n",i,argv[i]);

 	if (argc < 2 || 5 < argc || (argc != 2 && argc != 5)){
		printf("Missing argument.\n");
		_exit(1);
	}

    switch(argc) {
        case 2:
            if (strcmp(argv[1],"status") == 0) {
                //printf("recibi status\n");


                
            }
            else perror("Não foi possivel interrogar os programas, dado que o argumento número 1 recebido não foi: status\n");
            break;
        case 5:
            if (strcmp(argv[1],"execute") == 0) {
                //printf("recibi execute\n");
                if (strcmp(argv[3],"-u") == 0 || strcmp(argv[3],"-p") == 0) {

                    Task *newTask = parse_string(0, argv[2], argv[4]);
                    int in = open("inbound", O_WRONLY), out = open("outbound", O_RDONLY), idTask;
                    char *msg = malloc(sizeof(char) * 128);
                    write(in, newTask, sizeof(Task));//manda a task para o server

                    read(out, &idTask, sizeof(int));//recebe o id da task enviada
                    sprintf(msg, "TASK %d Received", idTask);
                    write(1, msg, sizeof(msg));//escreve na terminal uma msg a dar perceber que o server recebeu a task (o id da task é gerada pelo server)

                    // printf("listo\n");
                    printf("listo\n");


                    print_task_debug(newTask);

                    



                }
                else perror("Não foi possivel interrogar os programas, dado que o argumento número 3 recebido não foi: -u / -p\n");
                    
            }
            else perror("Não foi possivel interrogar os programas, dado que o argumento número 1 recebido não foi: execute\n");
            break;
    }

    return 0;
}
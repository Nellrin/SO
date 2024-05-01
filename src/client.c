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


 	if (argc < 2 || 5 < argc || (argc != 2 && argc != 5)){
		printf("Missing argument.\n");
		_exit(1);
	}

    switch(argc) {
        case 2:
            if (!strcmp(argv[1],"STATUS")) {
                //printf("recibi status\n");


                
            }
            else perror("Não foi possivel interrogar os programas, dado que o argumento número 1 recebido não foi: status\n");
            break;
        case 5:
            if (!strcmp(argv[1],"EXECUTE") && (!strcmp(argv[3],"-u") || !strcmp(argv[3],"-p"))) {

                    Task *newTask = parse_string(0, argv[3],argv[2], argv[4], "output_file");

                    // $ ./client execute 100 -u "prog-a arg-1 (...) arg-n
                    int in = open("inbound", O_WRONLY), out = open("outbound", O_RDONLY), idTask;
                    char *msg = malloc(sizeof(char) * 128);
                    write(in, newTask, sizeof(Task));//manda a task para o server

                    read(out, &idTask, sizeof(int));//recebe o id da task enviada
                    sprintf(msg, "TASK %d Received", idTask);
                    write(1, msg, sizeof(msg));//escreve na terminal uma msg a dar perceber que o server recebeu a task (o id da task é gerada pelo server)

                    printf("listo\n");


                    print_task_debug(newTask);

                    
                    
            }
            else perror("Argumentos inválidos\n");
            break;
    }

    return 0;
}
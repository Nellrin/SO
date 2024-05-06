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
		perror("Missing argument.\n");
		_exit(1);
	}

    int pid = getpid(),
        in = open("pipes/server", O_WRONLY);


    switch(argc) {
        case 2:
            if(!strcasecmp(argv[1],"status")){
                char line_out[1024], line_in[1024], line[1024];
                sprintf(line_out,"pipes/%d",pid);
                mkfifo(line_out, 0600);

                sprintf(line_in,"%spipes/%d",argv[1],pid);

                write(in,line_in,1024);

                int out = open(line_out, O_RDONLY);
                if(read(out, line, (1024)) > 0)
                write(1, line, strlen(line));
            }

            if(!strcasecmp(argv[1],"break"))
            write(in, argv[1], 1024);

            break;
        case 5:
            if(!strcasecmp(argv[1],"execute") && 
                  ((!strcmp(argv[3],"-u") && amount_chars(argv[4],'|') == 0)
                || (!strcmp(argv[3],"-p") && amount_chars(argv[4],'|') > 0))){


    


                    char pipe_name[20];
                    sprintf(pipe_name, "pipes/%d", pid);
                    mkfifo(pipe_name, 0600);
                    char buff[1024];
                    sprintf(buff, "%d %s %s \"%s\"", pid, argv[3],argv[2], argv[4]);
                    //printf("%s\n", buff);

                    //Task *newTask = parse_string(pid,argv[3],argv[2], argv[4]);

                    // // $ ./client execute 100 -u "prog-a arg-1 (...) arg-n
                    write(in, buff, 1024);//manda a task para o server

                    //print_task_debug(newTask);

                    int out = open(pipe_name, O_RDONLY), idTask;
                    if(read(out, &idTask, sizeof(int)) > 0)
                    if(idTask >= 0){
                        //printf("entrei no if\n");
                        char ret[30];
                        sprintf(ret, "TASK %d Received\n", idTask);
                        //dup2(1,STDOUT_FILENO);
                        write(1, ret, sizeof(ret));
                    }
            }
            else perror("Argumentos inválidos\n");
            break;
    }

    return 0;
}
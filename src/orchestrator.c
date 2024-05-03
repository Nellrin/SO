#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#include "../include/Queue.h"
#include "../include/Task.h"
#include "../include/Auxiliares.h"

typedef struct orchestrator{
    TTL* queue;
    int active_tasks;
    int log;
    
    char * output_folder;
    int parallel_tasks;
    char * sched_policy;
} Server ;



int main (int argc, char * argv []){
    if (argc != 4)
    perror("Invalid Number of Arguments");

    Server * Big_Guy = malloc(sizeof(Server));

    Big_Guy->output_folder = strdup(argv[1]);
    Big_Guy->parallel_tasks = atoi(argv[2]);
    Big_Guy->sched_policy = strdup(argv[3]);

    Big_Guy->queue = NULL;
    Big_Guy->active_tasks = 0;
    Big_Guy->log = 0;

    if(fork() == 0){
        mkdir(Big_Guy->output_folder, 0700);
        char * filename = malloc(sizeof(char) * 128);
        sprintf(filename, "%s/done_tasks.bin", Big_Guy->output_folder);

        open(filename, O_CREAT, 0644);
        _exit(0);
    }

            // char * filename = malloc(sizeof(char) * 128);
            // snprintf(filename,128,"%s/done_tasks.bin",argv[1]);
            // int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);


// $ ./client execute 100 -u "prog-a arg-1 (...) arg-n"

    mkfifo ("server" , 0600);

    int x, fdout = 0;
    int fdin = open("server" , O_RDONLY) ;

    Task * task = malloc(sizeof(Task));
    char client_path[10];
    char buff[1024];
    char *buff_cpy;
    int pid, active = 1;
    char * pipe_flag, *time, *args, *token;


    ssize_t r;
    while(active)
    while((r = read (fdin , buff, 1024)) > 0){

        if(!strcmp(buff,"BREAK"))
        active--;

        else{

        buff_cpy = strdup(buff);
        
        for(int i = 0; i < 3 && (token = strsep(&buff_cpy, " ")) != NULL; i++) {
            switch(i) {
                case 0:
                    pid = atoi(token);
                    break;
                case 1:
                    pipe_flag = strdup(token);
                    break;
                case 2:
                    time = strdup(token);
                    break;
            }
        }

        token = strsep(&buff_cpy, "\0");
        //printf("%s\n", token);
        args = strdup(remove_quotes(token));
        //printf("%s\n", args);

        task = parse_string(pid,pipe_flag,time, args);
        print_task_debug(task);

        free(buff_cpy);
        free(pipe_flag);
        free(time);
        free(args);

        Big_Guy->log++;
        x = Big_Guy->log;
        set_ids(task,Big_Guy->log,Big_Guy->output_folder);
        sprintf(client_path, "%d", task->pid);
        fdout = open(client_path , O_WRONLY);
        write(fdout, &x, sizeof(int));
        //printf("check\n");


        // if(/*blah blah blah*/ 0){
        //     /*imprime status*/
        // }

        // else{
        //     // lseek(done, 0, SEEK_END);
        //     // write(done, task, sizeof(Task));

            if (Big_Guy->active_tasks < Big_Guy->parallel_tasks){
                Big_Guy->active_tasks++;
                if(fork() == 0){
                    //atualizar estado da tarefa no ficheiro bin como: executing
                    new_status(Big_Guy->output_folder, task->id, EXECUTING);

                    execute_Task(task, Big_Guy->output_folder);
                    //printf("executou a tarefa\n");

                    // atualizar estado da tarefa no ficheiro bin como: finish
                    new_status(Big_Guy->output_folder, task->id, COMPLETED);
                    
                    
                    while(Big_Guy->queue != NULL){
                        new_status(Big_Guy->output_folder, task->id, EXECUTING);
                        execute_Task(grabTask(Big_Guy->queue), argv[1]) ;
                        new_status(Big_Guy->output_folder, task->id, COMPLETED);
                    }

                    Big_Guy->active_tasks--;
                    _exit (0) ;
                }
                // else //PAI
                // {
                //     //Handle Daddy Issues
                // }
            }
            else
            Big_Guy->queue = add_task(Big_Guy->queue, task, Big_Guy->sched_policy);
        
        //}
        }
    }
        //saiu do fifo
    return 0;
}

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


            char * filename = malloc(sizeof(char) * 128);
            snprintf(filename,128,"%s/done_tasks.bin",argv[1]);
            int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);


// $ ./client execute 100 -u "prog-a arg-1 (...) arg-n"

    mkfifo ("server" , 0600);

    int x, fdout = 0;
    int fdin = open("server" , O_WRONLY) ;

    Task * task = malloc(sizeof(Task));
    char client_path[10];

    ssize_t r;

    while((r = read (fdin , task, sizeof (Task))) > 0){
        
        Big_Guy->log++;
        x = Big_Guy->log;
        set_ids(task,Big_Guy->log,Big_Guy->output_folder);
        sprintf(client_path, "%d", task->pid);
        fdout = open(client_path , O_RDONLY);
        write(fdout, &x, sizeof(int));


        if(/*blah blah blah*/ 0){
            /*imprime status*/
        }

        else{
            lseek(done, 0, SEEK_END);
            write(done, task, sizeof(Task));

            if (Big_Guy->active_tasks < Big_Guy->parallel_tasks){
                Big_Guy->active_tasks++;
                if(fork() == 0){
                    //atualizar estado da tarefa no ficheiro bin como: executing
                    new_status(Big_Guy->output_folder, task->id, EXECUTING);

                    execute_Task(task, argv[1]);

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
        
        }
    }
        //saiu do fifo
    return 0;
}

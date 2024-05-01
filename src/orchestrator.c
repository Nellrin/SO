#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#include "Task.h"
#include "Queue.h"
#include "Auxiliares.h"

typedef struct orchestrator{
    TTL* queue;
    int active_tasks;
    
    char * output_folder;
    int parallel_tasks;
    char * sched_policy;
} Server ;



int main (int argc, char * argv []){
    if (argc != 3)
    perror("Invalid Number of Arguments");

    Server * Big_Guy = malloc(sizeof(Server));

    Big_Guy->output_folder = strdup(argv[1]);
    Big_Guy->parallel_tasks = atoi(argv[2]);
    Big_Guy->sched_policy = strdup(argv[3]);

    Big_Guy->queue = NULL;
    Big_Guy->active_tasks = 0;



    //opening all fifos
    mkfifo ("inbound" , 0600);
    //mkfifo ("outbound" , 0600);

    int fdin = open ("inbound" , O_RDONLY) ;
    //int fdout = open ("outbound" , O_WRONLY) ;

    //store tasks
    Task * task = NULL;

    //initialize queue
    TTL * queue ;

    ssize_t r ;

    pid_t pid ;

    int ParallelProcessesRunning = 0 ;

    while(1)
    {
        while ((r = read (fdin , task, sizeof (Task))) > 0)
        {
            //guardar a tarefa no ficheiro bin com o estado: em espera
            char * filename = malloc(sizeof(char) * 128);
            snprintf(filename,128,"%s/done_tasks.bin",argv[1]);
            int done = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            lseek(done, 0, SEEK_END);
            write(done, task, sizeof(Task));
            //ja foi guardado a task no ficheiro com o estado: em espera

            if (ParallelProcessesRunning < Big_Guy->parallel_tasks)
            {
                ParallelProcessesRunning ++ ;
                pid = fork () ; //CRIA UM FILHO

                if (pid < 0)
                {
                    perror ("Error creating forking") ;
                }
                else if (pid == 0) //FILHO
                {
                    //atualizar estado da tarefa no ficheiro bin como: executing
                    new_status(Big_Guy->output_folder, task->id, EXECUTING);

                    execute_Task (task, argv[1]) ;

                    //atualizar estado da tarefa no ficheiro bin como: finish
                    new_status(Big_Guy->output_folder, task->id, COMPLETED);
                    
                    while (queue != NULL)
                    {
                        execute_Task (grabTask (queue), argv[1]) ;
                    }

                    ParallelProcessesRunning -- ;
                    _exit (0) ;
                }
                // else //PAI
                // {
                //     //Handle Daddy Issues
                // }
            }
            else
            {
                queue = add_task (queue, task, Big_Guy->sched_policy) ;
            } 
        }
        //saiu do fifo
    }
    return 0;
}

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
mkfifo ("outbound" , 0600);

int fdin = open ("inbound" , O_RDONLY) ;
int fdout = open ("inbound" , O_WRONLY) ;

    //store tasks
    Task * task = NULL;

    //initialize queue
    TTL * queue ;

    ssize_t r ;

    pid_t pid ;

    int ParallelProcessesRunning = 0 ;

    // while(1)
    // {
    //     while ((r = read (fdin , task, sizeof (Task *))) > 0)
    //     {
    //         if (ParallelProcessesRunning < MaxParallelTasks)
    //         {
    //             ParallelProcessesRunning ++ ;
    //             pid = fork () ; //CRIA UM FILHO

    //             if (pid < 0)
    //             {
    //                 perror ("Error creating forking") ;
    //             }
    //             else if (pid == 0) //FILHO
    //             {
    //                 execute_Task (task, argv[1]) ;
                    
    //                 while (queue != NULL)
    //                 {
    //                     execute_Task (grabTask (queue , argv[3]), argv[1]) ;
    //                 }

    //                 ParallelProcessesRunning -- ;
    //                 _exit (0) ;
    //             }
    //             else //PAI
    //             {
    //                 //Handle Daddy Issues
    //             }
    //         }
    //         else
    //         {
    //             queue = addTask (task , queue) ;
    //         } 
    //     }
        // saiu do fifo
    // }
    return 0;
}

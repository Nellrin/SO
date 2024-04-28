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
    char * output_folder;
    int parallel_tasks;
    char * sched_policy;
} Server ;



int main (int argc, char * argv [])
{
    if (argc != 3)
    {
        perror ("Wrong number of arguments given when calling el Orquestador") ;
    }

    // output_folder = argv [1]
    // parallel-tasks = argv [2]
    // shced policy = argv [3]

    // int MaxParallelTasks = atoi (argv [2]) ;

    // int running = 1 ;


    //opening all fifos
    if (mkfifo ("inbound" , 0600) == -1)
	{
		perror ("error creating inbound") ;
	}

	if (mkfifo ("outbound" , 0600) == -1)
	{
		perror ("error creating outbound") ;
	}

	int fdin = open ("inbound" , O_RDONLY) ;
	if (fdin == -1)
    {
        perror ("error opening inbound") ;
        return 1 ;
    }

	int fdout = open ("inbound" , O_WRONLY) ;
	if (fdout == -1)
    {
        perror ("error opening outbound") ;
        return 1 ;
    }

    // //store tasks
    // Task * task = NULL;

    // //initialize queue
    // TTL * queue ;

    // ssize_t r ;

    // pid_t pid ;

    // int ParallelProcessesRunning = 0 ;

    // while (running)
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
    //     // saiu do fifo
    // }
    return 0;
}

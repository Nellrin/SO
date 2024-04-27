#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "Task.h"

typedef struct TaskLinkedList
{
    Task * task ;
    TTL * next ;

} TTL ;

TTL * addTask (Task * task , TTL * queue)
{
    if (queue == NULL)
    {
        queue = malloc (sizeof (TTL *)) ;
        queue -> task = task ;
        queue -> next = NULL ;
        return queue ;
    }
    else
    {
        TTL * newqueue = malloc (sizeof (TTL *)) ;
        newqueue -> next = queue ;
        newqueue -> task ;
        return newqueue ;
    }  
}

Task * grabTask (TTL * queue , int method)
{
    //method 1 -> fifo
    //method 2 -> ordered by time

    if (method == 1)
    {
        TTL * last = NULL ;
        while (queue -> next != NULL)
        {
            last = queue ;
            queue = queue -> next ;
        }

        Task * returne = queue -> task ;

        last -> next = NULL ;
        free (queue) ;
        queue = NULL ;

        return returne ;
    }

    if (method == 2)
    {
        struct timeval mintime = queue -> task -> estimated_duration ;
        Task * mintask = queue -> task ;

        struct timeval temptime ;
        while (queue != NULL)
        {
            temptime = queue -> task -> estimated_duration ;

            if (temptime.tv_sec < mintime.tv_sec || (temptime.tv_sec == mintime.tv_sec && temptime.tv_usec < mintime.tv_usec))
            {
                mintime = temptime ;
                mintask = queue -> task ;
            }

            queue = queue -> next ;
        }
        return mintask ;
    }
}

int orchestrator (char * argv [] , int argc)
{
    if (argc != 3)
    {
        perror ("Wrong number of arguments given when calling el Orquestador") ;
    }

    // output_folder = argv [1]
    // parallel-tasks = argv [2]
    // shced policy = argv [3]

    int MaxParallelTasks = atoi (argv [2]) ;

    int running = 1 ;


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

    //store tasks
    Task * task ;

    //initialize queue
    TTL * queue ;

    ssize_t r ;

    pid_t pid ;

    int ParallelProcessesRunning = 0 ;

    while (running)
    {
        while (r = read (fdin , task, sizeof (Task *)) > 0)
        {
            if (ParallelProcessesRunning < MaxParallelTasks)
            {
                ParallelProcessesRunning ++ ;
                pid = fork () ; //CRIA UM FILHO

                if (pid < 0)
                {
                    perror ("Error creating forking") ;
                }
                else if (pid = 0) //FILHO
                {
                    execute_Task (task) ;
                    
                    while (queue != NULL)
                    {
                        execute_Task (grabtask (queue , 1)) ;
                    }

                    ParallelProcessesRunning -- ;
                    _exit (0) ;
                }
                else //PAI
                {
                    //Handle Daddy Issues
                }
            }
            else
            {
                queue = addTask (task , queue) ;
            } 
        }
        // saiu do fifo
    }
}

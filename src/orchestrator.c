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
} Server;

Server * orchestrator_set_up(char * argv[]){
    Server * Big_Guy = malloc(sizeof(Server));

    Big_Guy->output_folder = strdup(argv[1]);
    Big_Guy->parallel_tasks = atoi(argv[2]);
    Big_Guy->sched_policy = strdup(argv[3]);

    Big_Guy->queue = NULL;
    Big_Guy->active_tasks = 0;
    Big_Guy->log = 0;

    char * filename = malloc(sizeof(char) * 1024);
    sprintf(filename, "%s/done_tasks.bin", Big_Guy->output_folder);    

    if(fork() == 0){
        mkdir(Big_Guy->output_folder, 0700);
        open(filename, O_CREAT, 0644);

        _exit(0);
    }

    int z = open(filename, O_RDWR, 0644), y = lseek(z, 0, SEEK_END);
    
    if(y > 0){
        lseek(z, 0, SEEK_SET);
        read(z,&(Big_Guy->log),sizeof(int));
    }

    else{
        lseek(z, 0, SEEK_SET);
        write(z,&(Big_Guy->log),sizeof(int));
    }

    close(z);

    return Big_Guy;
}


int main (int argc, char * argv []){
    if (argc != 4)
    perror("Invalid Number of Arguments");

    Server * Big_Guy = orchestrator_set_up(argv);

    mkfifo("pipes/server" , 0600);

    int x, fdout = 0, pid, active = 1;
    int fdin = open("pipes/server" , O_RDONLY) ;

    Task * task = malloc(sizeof(Task));
    char client_path[20], buff[1024];
    char * buff_cpy;
    char * pipe_flag, *time, *args, *token;


    ssize_t r;
    while(active){
        if((r = read (fdin , buff, 1024)) > 0){

            if(!strcmp(buff,"done")){
                Big_Guy->active_tasks--;
                printf("Done\n");
                continue;
            }

            if(!strcmp(buff,"break")){
                active--;
                continue;
            }

            if(!strncmp(buff,"status", 6)){
                if(fork() == 0){

                    Task ** x = get_Tasks(Big_Guy->output_folder, Big_Guy->log);
                    char * list = malloc(sizeof(char) * Big_Guy->log * 300);

                    for(Task_Status status = EXECUTING; status <= COMPLETED; status++){

                        switch(status){
                            case EXECUTING:
                                snprintf(list,(Big_Guy->log * 300),"Executing\n");
                                break;
                            
                            case SCHEDULED:
                                sprintf(list + strlen(list),"\nScheduled\n");
                                break;
                            case COMPLETED:
                                sprintf(list + strlen(list),"\nCompleted\n");
                                break;
                            }
                            

                                for(int i = 0; i < Big_Guy->log; i++)
                                    if(x[i]->status == status){
                                        char * nova = print_Task_status(x[i]);

                                        sprintf(list + strlen(list),"%s",nova);
                                        free(nova);
                                    }
                    }


                    const char * out = buff + 6;

                    fdout = open(out , O_WRONLY);
                    write(fdout, list, Big_Guy->log * 300);

                    for(int i = 0; i < Big_Guy->log; i++)
                    destroy_Task(x[i]);

                    free(x);
                    free(list);

                    print_queue(Big_Guy->queue);

                    _exit(0);
                }

                // else 
                // continue;
            }

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
                args = strdup(remove_quotes(token));

                task = parse_string(pid,pipe_flag,time, args);
                // print_task_debug(task);

                free(buff_cpy);
                free(pipe_flag);
                free(time);
                free(args);

                Big_Guy->log++;

                x = Big_Guy->log;
                set_ids(task,Big_Guy->log,Big_Guy->output_folder);
                sprintf(client_path, "pipes/%d", task->pid);
                fdout = open(client_path , O_WRONLY);
                write(fdout, &x, sizeof(int));

                if (Big_Guy->active_tasks < Big_Guy->parallel_tasks){
                    Big_Guy->active_tasks++;
                    if(fork() == 0){
                        //atualizar estado da tarefa no ficheiro bin como: executing
                        new_status(Big_Guy->output_folder, task->id, EXECUTING, task->real_duration);

                        // printf("[%d]", task->id);

                        long time = execute_Task(task, Big_Guy->output_folder);
                        // printf("[%d - %ldms|%d]\n", task->id, time, getgid());

                        // atualizar estado da tarefa no ficheiro bin como: finish
                        new_status(Big_Guy->output_folder, task->id, COMPLETED,time);
                        
                        

                        
                        char done[1024];
                        snprintf(done,1024,"done");
                        
                        int out = open("pipes/server",O_WRONLY);
                        write(out,done,1024);
                        close(out);
                        
                        _exit(0);
                    }
                }


                else
                Big_Guy->queue = add_task(Big_Guy->queue, task, Big_Guy->sched_policy);

            }           
            continue;
        }

        if(Big_Guy->queue != NULL && Big_Guy->active_tasks < Big_Guy->parallel_tasks){
            
            Big_Guy->active_tasks++;
            Task * x = grabTask(&Big_Guy->queue);
            
            if(fork() == 0){
                new_status(Big_Guy->output_folder, x->id, EXECUTING, x->real_duration);

                    printf("[Lido %d]\n", x->id);
                long time = execute_Task(x, argv[1]);
                    // printf("[%d - %ldms|%d]\n", x->id, time, getgid());

                new_status(Big_Guy->output_folder, x->id, COMPLETED, time);



                char done[1024];
                snprintf(done,1024,"done");
                
                int out = open("pipes/server",O_WRONLY);
                write(out,done,1024);
                close(out);
                close(fdin);

                destroy_Task(x);
                
                _exit(0);
            }

            else
            destroy_Task(x);


        }
    }

    return 0;
}

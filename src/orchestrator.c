#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
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

        long amount_forks;
        char * output_folder;
        int parallel_tasks;
        char * sched_policy;
    } Server;

Server * orchestrator_set_up(char * argv[]){
    Server * Big_Guy = malloc(sizeof(Server));

    Big_Guy->output_folder = strdup(argv[1]);
    Big_Guy->parallel_tasks = atoi(argv[2]);
    Big_Guy->sched_policy = strdup(argv[3]);

    Big_Guy->amount_forks = 1;
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
    free (filename) ;

    return Big_Guy;
}
void server_exec_task(Server * Big_Guy, Task ** task){

    Big_Guy->active_tasks++;
    Big_Guy->amount_forks++;

        printf("[%d]", (*task)->id);




    if(fork() == 0){
        new_status(Big_Guy->output_folder, (*task)->id, EXECUTING, (*task)->real_duration);
        long time = execute_Task((*task), Big_Guy->output_folder);
        new_status(Big_Guy->output_folder, (*task)->id, COMPLETED,time);


        char * done = malloc(sizeof(char) * 1024);
        snprintf(done,1024,"done");

        int out = open("pipes/server",O_WRONLY);
        write(out,done,1024);
        close(out);
        free(done);

        _exit(0);
    }


    else
    destroy_Task(*task);

}
void server_task_setter(Server * Big_Guy, Task ** task, char * buff){

    char * buff_cpy = strdup(buff),
         * token,
         * pipe_flag,
         * time,
         * args,
         * client_path = malloc(sizeof(char) * 1024);

    char * buff_cpy2 = buff_cpy ;

    int pid = 0,
        x = 0,
        fdout = 0;

        for(int i = 0; i < 3 && (token = strsep(&buff_cpy, " ")) != NULL; i++){
            switch(i){
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
        args = remove_quotes(token);

        (*task) = parse_string(pid,pipe_flag,time, args);



        Big_Guy->log++;

        x = Big_Guy->log;



        set_ids((*task),x,Big_Guy->output_folder);
        sprintf(client_path, "pipes/%d", (*task)->pid);
        fdout = open(client_path , O_WRONLY);
        write(fdout, &x, sizeof(int));




        close(fdout);
        free(buff_cpy2);
        free(pipe_flag);
        free(time);
        free(args);
        free(client_path);
}
void destroy_server(Server * Big_Guy){
    for(int i = 0; i < Big_Guy->amount_forks; i++)
    wait(NULL);

    free_queue(Big_Guy->queue);
    free(Big_Guy->output_folder);
    free(Big_Guy->sched_policy);
    //Big_Guy ->sched_policy = NULL ;

    free(Big_Guy);
}
void server_status(Server * Big_Guy, char * buff){

    Big_Guy->amount_forks++;

    if(fork() == 0){
        Task ** x = get_Tasks(Big_Guy->output_folder, Big_Guy->log);
        char * list = malloc(sizeof(char) * Big_Guy->log * 1024);
        int fdout = 0;


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
        write(fdout, list, Big_Guy->log * 1024);




        for(int i = 0; i < Big_Guy->log; i++)
        destroy_Task(x[i]);

        free(x);
        free(list);

        print_queue(Big_Guy->queue);

        _exit(0);
    }

}


int main (int argc, char * argv []){
    if (argc != 4)
    perror("Invalid Number of Arguments");

    mkfifo("pipes/server" , 0600);



    Server * Big_Guy = orchestrator_set_up(argv);
    Task * task = malloc(sizeof(Task)); // not freed
    char * buff = malloc(sizeof(char) * 1024);
    int fdin = open("pipes/server" , O_RDONLY), active = 1;
    ssize_t r;




    while(active){
        if((r = read (fdin , buff, 1024)) > 0){

            if(!strcasecmp(buff,"done")){
                wait(NULL);
                Big_Guy->active_tasks--;
                printf("Done\n");
                continue;
            }

            if(!strcasecmp(buff,"break")){
                active--;
                continue;
            }

            if(!strncasecmp(buff,"status", 6))
            server_status(Big_Guy,buff);


            else{
                server_task_setter(Big_Guy,&task,buff);

                if(Big_Guy->active_tasks < Big_Guy->parallel_tasks)
                server_exec_task(Big_Guy,&task);

                else
                Big_Guy->queue = add_task(Big_Guy->queue, task, Big_Guy->sched_policy);

            continue;
            }
        }



        if(Big_Guy->queue != NULL && Big_Guy->active_tasks < Big_Guy->parallel_tasks){
            Task * x = grabTask(&Big_Guy->queue);
            server_exec_task(Big_Guy, &x);
        }
    }


    free(buff);
    destroy_server(Big_Guy);

    return 0;
}
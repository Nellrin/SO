#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#include "../include/Queue.h"

Task* grabTask(TTL **queue) {
    if (*queue == NULL) return NULL;

    Task* taskToExec = (*queue)->task; // Novo valor do ponteiro
    TTL *kill = *queue;
    *queue = (*queue)->next;
    free(kill);
    return taskToExec;
}


static TTL* create_node(Task *data){
    TTL* newNode = malloc(sizeof(TTL));
    newNode->task = data;
    newNode->next = NULL;
    return newNode;
}

TTL* add_task(TTL* head, Task *data, char * order){
    if(!strcasecmp(order,"FCFS")){
        TTL* newNode = create_node(data);
        if (head == NULL)
        return newNode;

        TTL* current = head;

        while (current->next != NULL)
        current = current->next;

        current->next = newNode;
        return head;
    }
    if(!strcasecmp(order,"SJF")){
        TTL* newNode = create_node(data);
        
        if (head == NULL || ((head->task->estimated_duration) > (data->estimated_duration))) {
            newNode->next = head;
            return newNode;
        }
        
        TTL* current = head;

        while (current->next != NULL && ((current->next->task->estimated_duration) < (data->estimated_duration)))
        current = current->next;

        newNode->next = current->next;
        current->next = newNode;
        
        return head;
    }

    return head;
}

TTL* remove_task(TTL* head, int id){
    if (head == NULL)
    return NULL;

    if (head->task->id == id){
        TTL* temp = head;
        head = head->next;
        
        destroy_Task(temp->task);
        free(temp);

        return head;
    }

    TTL* current = head;
    while (current->next != NULL && current->next->task->id != id)
    current = current->next;


    if (current->next != NULL){
        TTL* temp = current->next;
        current->next = current->next->next;
        
        destroy_Task(temp->task);
        free(temp);
    }
    return head;
}

Task * look_up_task(TTL* head, int id){
    TTL* current = head;

    while (current != NULL){

        if (current->task->id == id)
        return current->task;

        current = current->next;
    }

    return NULL;
}

void print_queue(TTL* head){

    printf("+───────+\n"
           "| Queue |\n"
           "+───────+\n");
    TTL* current = head;

    while(current != NULL){
        printf("|Task %2d|\n", current->task->id);
        current = current->next;
    }

    printf("+───────+\n");

}

void free_queue(TTL* head){
    TTL* current = head;
    while (current != NULL) {
        TTL* temp = current;
        current = current->next;
        destroy_Task(temp->task);
        free(temp);
    }
}
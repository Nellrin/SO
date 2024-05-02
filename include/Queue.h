#ifndef QUEUE_H
#define QUEUE_H

#include "Task.h"

typedef struct TaskLinkedList
{
    Task * task ;
    struct TaskLinkedList * next ;

} TTL ;

Task* grabTask(TTL *queue) ;
TTL* add_task(TTL* head, Task *data, char * order);
TTL* remove_task(TTL* head, int id);
Task * look_up_task(TTL* head, int id);
void print_queue(TTL* head);
void free_queue(TTL* head);

#endif
#ifndef PROG_H
#define PROG_H

typedef struct {
    char * path_to_program;
    
    short amount_args;
    char ** args;
} Prog;


Prog * create_Prog(char * path_to_program, short amount_args, char ** args);
void destroy_Prog(Prog * x);
void execute_Prog(Prog * x);

#endif
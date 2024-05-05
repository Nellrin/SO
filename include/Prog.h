#ifndef PROG_H
#define PROG_H

typedef struct {
    char * path_to_program;
    
    short amount_args;
    char ** args;
} Prog;


Prog * create_Prog(char * path_to_program, short amount_args, char ** args);
void destroy_Prog(Prog * x);
void execute_single_Prog(Prog * x, int id, char * output_file);
void execute_multiple_Prog(Prog ** x, int amount, int id, char * output_file);

void write_Prog(Prog * x, int file);
Prog * read_Prog(int file);

#endif
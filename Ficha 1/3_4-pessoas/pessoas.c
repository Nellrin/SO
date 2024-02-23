#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "person.h"


int main(int argc, char* argv[]){
    switch (argc){

    case 3:
        if(!strcmp(argv[1],"-l"))
        list(atoi(argv[2]));
        
        break;

    case 4:
        if(!strcmp(argv[1],"-i"))
        add(argv[2], atoi(argv[3]));

        if(!strcmp(argv[1],"-u"))
        change_age_v1(argv[2],atoi(argv[3]));

        if(!strcmp(argv[1],"-o"))
        change_age_v2(atoi(argv[2]),atoi(argv[3]));
        
        break;
    
    default:        
        printf("Usage:\n");
        printf("Add new person: ./pessoas -i [name] [age]\n");
        printf("List N persons: ./pessoas -l [N]\n");
        printf("Change person age: ./pessoas -u [name] [age]\n");
        printf("Change person age (v2): ./pessoas -o [position] [age]\n");

        break;
    }

    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include "person.h"


#define MAX 256

struct Pessoa {
    char nome[MAX];
    int idade;
};


void add(char * name, int age){
    const char * file = "pessoas.bin";
    int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    int ofd = 1;
    struct Pessoa joe;

    strcpy(joe.nome,name);
    joe.idade = age;

    char * string = malloc(sizeof(char) * 10);
    off_t currentPosition = lseek(fd, 0, SEEK_END) / sizeof(struct Pessoa);
    snprintf(string,10,"%ld\n",currentPosition);


    write(fd, &joe, sizeof(struct Pessoa));
    write(ofd,string,4);

    close(fd);
}
void list(int amount){
    const char * file = "pessoas.bin";
    int fd = open(file, O_RDONLY | O_CREAT, 0644);
    int ofd = 1;
    char * string = malloc(sizeof(char) * 64);

    struct Pessoa joe;

    for(int i = 0; i < amount ; i++){
        read(fd,&joe,sizeof(struct Pessoa));

        snprintf(string, 271, "(%3d) %s\n", joe.idade, joe.nome);
        write(ofd,string,strlen(string));
    }

    free(string);
    close(fd);
}
void change_age_v1(char * name, int age){
    const char * file = "pessoas.bin";
    int fd = open(file, O_RDWR);

    lseek(fd, 0, SEEK_SET);

    struct Pessoa joe;
    
    while(read(fd,&joe,sizeof(struct Pessoa)) > 0){
        if(!strcmp(joe.nome,name)){
            joe.idade = age;
        
            lseek(fd, -sizeof(struct Pessoa), SEEK_CUR);
            write(fd,&joe,sizeof(struct Pessoa));
        }

    }

    close(fd);
}
void change_age_v2(int position, int age){
    const char * file = "pessoas.bin";
    int fd = open(file, O_RDWR);

    lseek(fd,0, SEEK_SET);

    struct Pessoa joe;
    
    for(int i = 0; i <= position; i++)
    read(fd,&joe,sizeof(struct Pessoa));

    joe.idade = age;
    
    lseek(fd, -1 * sizeof(struct Pessoa), SEEK_CUR);
    write(fd,&joe,sizeof(struct Pessoa));
    
    close(fd);
}

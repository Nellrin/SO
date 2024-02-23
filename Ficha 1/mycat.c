#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */

#define MAX 128

int main(int argc, char * argv[]){
    
    char buffer[MAX];
    int n;

    int ifd = 0;
    int ofd = 1;

    while((n = read(ifd,buffer,MAX))>0)
        write(ofd,buffer,n);

    close(ifd);
    close(ofd);

    return 0;
}


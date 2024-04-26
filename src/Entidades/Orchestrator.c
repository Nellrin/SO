#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "Prog.h"

int orchestrator (char * argv [] , int argc)
{
    // output_folder = argv [1]
    // parallel-tasks = argv [2]
    // shced policy = argv [3]

    int running = 1 ;

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

    Prog * programa ;

    ssize_t r ;

    while (running)
    {
        while (r = read (fdin , programa, sizeof (Prog)) > 0) // 0 == STDIN
        {
            //nao sei ao certo o nome do ficheiro mas tipo vai aqui
            char * output = OutputFileName (programa -> pid) ; //o Prog nao sei se devia ter um pid ou algo assim...

            execute_Prog (programa , output) ; // isto vai rodar o programa e meter o output num ficheiro
        }

        // falta alguma forma de mandar o server (orchestrator) abaixo (running = 0)
    }


}


char * OutputFileName (int pid)
{
    char* filename = malloc(sizeof(char) * 16) ;

    int bytatas = sprintf (filename , "OutputFile%d.txt" , pid) ;

    if (bytatas < 0)
    {
        free (filename) ;
        return NULL ;
    }

    return filename;
}
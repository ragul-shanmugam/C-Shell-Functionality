/*

Name: Ragul Shanmugam
ID: 1001657250

*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10    // Mav shell only supports ten arguments

pid_t child_pid = 0;
int exec_ret;
int status,n;
int a,b = 0;
int tempListpids;
int listpids[15];
char *tempHistory;
char *userHistory[15];

static void handle_signal (int sig )
{
  /*
   Determine which of the signals were caught and
   print an appropriate message.
  */
  switch( sig )
  {
    //do nothing - dummy loop - action implemented in bg
  }
}

int main(int argc, char* argv[])
{
    char* usr_str =  (char*) malloc( MAX_COMMAND_SIZE );
    struct sigaction act;

    /*
    Zero out the sigaction struct
    */
    memset (&act, '\0', sizeof(act));

    /*
    Set the handler to use the function handle_signal()
    */
    act.sa_handler = &handle_signal;

    /*
    Install the handler for SIGINT and SIGTSTP and check the
    return value.
    */
    if (sigaction(SIGINT , &act, NULL) < 0)
    {
        perror ("sigaction: ");
        return 1;
    }
    if (sigaction(SIGTSTP , &act, NULL) < 0)
    {
        perror ("sigaction: ");
        return 1;
    }

    while(1)
    {
        printf("msh>");
        while( !fgets (usr_str, MAX_COMMAND_SIZE, stdin) );

        //This block of code is used to add the user input value to an array
        //and to display it during history command
        if(strcmp(usr_str,"") != 0)
        {
            tempHistory= strdup(usr_str);
            if (userHistory[a] == NULL)
            userHistory[a] = tempHistory;
            a++;
        }

        //Getting the PIDs and adding it to an array to display it during listpids command
        tempListpids = getpid();
        for(b = 0; b<15; b++)
        {
            listpids[b] = tempListpids;
        }

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];
        int   token_count = 0;

        // Pointer to point to the token
        // parsed by strsep
        char *arg_ptr;
        char *working_str  = strdup( usr_str );

        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;

        // Tokenize the input strings with whitespace used as the delimiter
        while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL)
                                        && (token_count<MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
            if( strlen( token[token_count] ) == 0 )
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

        //Check if the user command is empty or null and simply print msh>
        if( token[0] == '\0')
        {
            continue;
        }

        /*
        Comparing the user input for exit or quit, bg, cd, listpids and history
        and perform the corresponding operations
        */
        else  if ( (strcmp(token[0],"quit") == 0) || (strcmp(token[0],"exit") == 0) )
        {
            exit(0);
        }
        else if ( strcmp(token[0],"bg") == 0 )
        {
            kill(child_pid,SIGCONT);
            continue;
        }
        else if ( strcmp(token[0],"cd") == 0 )
    	{
      		chdir(token[1]);
    		continue;
        }
        else if ( strcmp(token[0],"listpids") == 0 )
        {
            for(n =0 ; n<15 ; n++)
            {
                if(userHistory[n] != NULL)
                printf("%d: %d\n", n, listpids[n]);
            }
            continue;
        }
        else if( strcmp(token[0],"history") == 0 )
        {
            for(n =0 ; n<15 ; n++)
            {
                if(userHistory[n] != NULL)
                printf("%d: %s", n, userHistory[n]);
            }
            continue;
        }
        else if( strcmp(token[0], "!") == 0 )
        {
            execvp(token[1], token);
        }
        else
        {
            //Creating a child process to execute the user command
            //using execvp function
            child_pid = fork();
            if( child_pid == -1 )
            {
                perror("fork failed: ");
                fflush(NULL);
                exit( EXIT_FAILURE );
            }
            else if(child_pid == 0 )
            {
                exec_ret = execvp (token[0], token);
                if(exec_ret != 0)
                {
                    printf("%s : Command not found.\n", token[0]);
                }
                fflush(NULL);
            }
            else
            {
                waitpid( child_pid, &status, 0 );
                fflush(NULL);
            }
        }
        free( working_root );
    }
    return 0;
}




/*
* File:		A2.c
* Author:	Andrew Mercer	B00768289
* Date:		2020/5/29
* Version:	1.0
*
* Purpose:	Creating a shell for use with linux
*
* Notes:	Did not have time to implement the background process function.
*/


#include<stdio.h> 
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<syscall.h>


#define TOKEN_TABLE_SIZE 10
#define CMD_TABLE_SIZE 20
#define PIPE_TOKEN_TABLE_SIZE 20
#define PID_ERROR_NUM -1
#define TEMP_PIPE_SIZE 2
#define MAX_CMD_NAME_SIZE 81


char *token_table[TOKEN_TABLE_SIZE];
char *cmd_table[CMD_TABLE_SIZE];
char *pipe_token_table[PIPE_TOKEN_TABLE_SIZE];

/*
* Name:			Start_ecec
* Purpose:		Executes the the tokens and arguments in token_table  
*
* Arguments:	Void 
* Output:		outputs the result of the command to the terminal  
* Modifies:		
* Returns:		Void  
*
* Assumptions:	None 
* Bugs:			None 
* Notes:		
*
*
*/
void start_ecec(void)
{                           
    int status;
    int error;
    pid_t pid;
    pid = fork();            
    if(pid == 0)
    {       
       error = execvp(token_table[0],token_table);
    
    }else if(pid == PID_ERROR_NUM)
    {
        printf("Error Creating Child Process\n");
        exit(1);
    }else
    {
        while(waitpid(pid, &status, WNOHANG) != pid)
        {
            
        }
        
    }

    if(error < 0)
    {
        printf("Execution Failed\n");
        exit(1);
    }
    //return;
}

/*
* Name:			pipe_start_ecec
* Purpose:		Pipes the contents of token_table in to pipe_token_table  
*
* Arguments:	Void 
* Output:		outputs the result of the command to the terminal  
* Modifies:		
* Returns:		Void  
*
* Assumptions:	None 
* Bugs:			when this is executed the terminal will exit for some reason  
* Notes:		
*
*
*/
void pipe_start_ecec(void)
{    
    int temp_pipe[TEMP_PIPE_SIZE2];                       
    int status;
    int error;
    pipe(temp_pipe);
    pid_t pid;
    pid = fork();            
    
    
    if(pid == 0)
    {   
	/*If child process, pipe secondary process into file pointer*/    
        dup2(temp_pipe[1],STDOUT_FILENO);
        close(temp_pipe[0]);
        close(temp_pipe[1]);
        error = execvp(token_table[0],token_table);
    
    }
    else 
    {
	/*Fork process, pipe primary process into file pointer */
        pid = fork();
        dup2(temp_pipe[0],STDIN_FILENO);
        close(temp_pipe[0]);
        close(temp_pipe[1]);
        error = execvp(pipe_token_table[0],pipe_token_table);
    }
    close(temp_pipe[0]);
    close(temp_pipe[1]);
    waitpid(PID_ERROR_NUM, NULL ,0);
    /*Wait for processes to finish */ 
    while(waitpid(pid, &status, WNOHANG) != pid)
    {
            
    }
    //return;
}


int main()
{ 
    char * cmd;
    char * token;
    char * pipe_tokenL;
    char * pipe_tokenR;
    char cmd_name[MAX_CMD_NAME_SIZE];
    char *end;
    int semicolon_count = 0;
    int count = 0;
    int token_count = 0;
    int pipe_count = 0;
    while(1)
    {
        
        printf("$: ");
        fgets(cmd_name, MAX_CMD_NAME_SIZE , stdin);
        /* Used to changed last enter character to a nul character*/
        end = strchr(cmd_name,'\n');
        if(end != NULL)
        {
            *end = '\0';
        }
          
        semicolon_count = 0; 
        cmd = strtok(cmd_name, ";");  
        /* Tokenize with ';' character as delimiter*/            
        while(cmd != NULL)
        {
            cmd_table[semicolon_count] = cmd; 
            cmd = strtok(NULL, ";");
            semicolon_count++; 
        }


        

        count = 0;
        /* Loop to go though all tokens found after tokenizing with a delimiter of ';' */ 
        while(count < semicolon_count)
        {
            char *rest_of_token;
            /* If token involves piping*/
            if(strchr(cmd_table[count], '|') != NULL)
            {
                pipe_count = 0;
                pipe_tokenL = strtok_r(cmd_table[count], "|", &rest_of_token);
                pipe_tokenR = strtok(pipe_tokenL, " ");
                /*tokenize first portion*/
                while(pipe_tokenR != NULL)
                {
                    token_table[pipe_count] = pipe_tokenR;
                    pipe_count++;
                    pipe_tokenR = strtok(NULL, " ");
                }
                pipe_count = 0;
                pipe_tokenL = strtok_r(rest_of_token, "|", &rest_of_token);
                pipe_tokenR = strtok(pipe_tokenL, " ");
                /*Tokenize second portion*/
                while(pipe_tokenR != NULL )
                {
                    pipe_token_table[pipe_count] = pipe_tokenR;
                    pipe_count++;
                    pipe_tokenR = strtok(NULL, "|");
                }
                pipe_start_ecec();
            }
            token = strtok(cmd_table[count], " ");
            token_count = 0;
            /* If not piping*/
            while(token != NULL)
            {
                
                token_table[token_count] = token;
                token = strtok(NULL, " ");
                token_count++;
            }                                                              
            count++;
            /* Check if exit command was input*/
            if((strcmp(token_table[0], "exit") == 0))
            {
                return 0;
            }
            start_ecec();
            
           
            
        }   
          /*Reset memory in all global arrays */
          memset(token_table, 0, sizeof(token_table));
          memset(cmd_table, 0, sizeof(cmd_table));
          memset(pipe_token_table, 0, sizeof(pipe_token_table));
    }
    return 0;
}

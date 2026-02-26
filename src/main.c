#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


#define MAX_ARGS 64
#define MAX_INPUT 1024


//structure for critical pipe information
typedef struct{
    char *commands[MAX_ARGS];
    int num_commands;
} PipeInfo;


//structure to hold redirection info
typedef struct {
    char *input_file;
    char *output_file;
    int append;
} Redirect;


void parse_command(char *input, char **args, Redirect *redirect) {
    // initialize redirect
    redirect->input_file = NULL;
    redirect->output_file = NULL;
    redirect->append = 0;

    int i = 0;
    char *token = strtok(input, " \t");

    while (token != NULL && i < MAX_ARGS - 1) {
    	if (redirect != NULL && strcmp(token, ">") == 0){
    	    // Next token is output file
    	    token = strtok(NULL, " \t");
    	    redirect->output_file = token;
    	    redirect->append = 0;
	}
        //append output redirection
	else if (redirect != NULL && strcmp(token, ">>") == 0) {
	    token = strtok(NULL, " \t");
	    redirect->output_file = token;
	    redirect->append = 1;
	}
        //input redirection
	else if (redirect != NULL && strcmp(token, "<") == 0) {
	    token = strtok(NULL, " \t");
  	    redirect->input_file = token;
	}
	else {
        args[i] = token;
        i++;
        }
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
}


//check if array has |
int has_pipe(char *input){
    for (int i = 0; input[i] != '\0'; i++){
	if (input[i] == '|') {
	    return 1; }} return 0;}


//parse input with |
void parse_pipe(char *input, PipeInfo *pipe_info){
    pipe_info->num_commands = 0;

    char *token = strtok(input, "|");
    while (token != NULL && pipe_info->num_commands < MAX_ARGS){
	while (*token == ' ' || *token == '\t') token++;

	char *end = token + strlen(token) - 1;
	while (end > token && (*end == ' ' || *end == '\t')){
	    *end = '\0';
	    end--; }

	pipe_info->commands[pipe_info->num_commands] = token;
	pipe_info->num_commands++;
	token = strtok(NULL, "|");}
}


void execute_pipe(PipeInfo *pipe_info) {
    if (pipe_info->num_commands != 2) {
	printf("Only single pipes suppoerted\n");
	return; }


    char cmd1[MAX_INPUT], cmd2[MAX_INPUT];
    strcpy(cmd1, pipe_info->commands[0]);
    strcpy(cmd2, pipe_info->commands[1]);

    //parse the commands
    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];

    int i = 0;
    char *token= strtok(cmd1, " \t");
    while (token != NULL && i < MAX_ARGS - 1) {
	args1[i++] = token;
	token = strtok(NULL, " \t");}
    args1[i] = NULL;



    i = 0;
    token = strtok(cmd2, " \t");
    while (token != NULL && i < MAX_ARGS - 1){
	args2[i++] = token;
	token = strtok(NULL, " \t");}
    args2[i] = NULL;


    //create the pipes
    int pipefd[2];
    if (pipe(pipefd) == -1) {
	perror("Pipe failed");
	return; }


    //child 1 for left command (EX: ls)
    pid_t pid1 = fork();
    if (pid1 == 0) {

	close(pipefd[0]);
	dup2(pipefd[1], 1);
	close(pipefd[1]);

	execvp(args1[0], args1);
	perror("Command 1 failed");
	exit(1); }


     //child 2 for right command (EX: grep)
     pid_t pid2 = fork();
     if (pid2 == 0) {

	close(pipefd[1]);
	dup2(pipefd[0], 0);
	close(pipefd[0]);

	execvp(args2[0], args2);
	perror("Command 2 failed");
	exit(1); }


	close(pipefd[0]);
	close(pipefd[1]);

	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);}

void execute_command(char **args, Redirect *redirect){
    pid_t pid = fork();

    if (pid < 0) {
        // if fork fails
        perror("Fork Failed");
        return; }

    if (pid == 0){
       //child process
       //input redirection
       if (redirect->input_file != NULL){
           int fd = open(redirect->input_file, O_RDONLY);
           if (fd < 0) {
               perror("Input file error");
               exit(1);
           }
           dup2(fd, 0); //replace stdin with file
           close(fd);
       }

       //output redirection
       if (redirect->output_file != NULL){
           int flags = O_WRONLY | O_CREAT;
           if (redirect->append){
               flags |= O_APPEND;
           } else {
               flags |= O_TRUNC;
           }
           int fd = open(redirect->output_file, flags, 0644);
           if (fd < 0) {
               perror("Output file error");  //if fd fails
               exit(1);
           }
           dup2(fd, 1);
           close(fd);
	}
	//execute with redirections applied
       if (execvp(args[0], args) == -1){
           perror("Command not found");}
       exit(1);} //if execvp fails
    else{
      //parent process
      int status;
      waitpid(pid, &status, 0);}
}


//check if the command is built in
int is_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0)   return 1;
    if (strcmp(args[0], "pwd") == 0)  return 1;
    if (strcmp(args[0], "help") == 0) return 1;
    return 0; }



//run the built in commands
void run_builtin(char **args){
    //runs cd part, change directory
    if (strcmp(args[0], "cd") == 0){
        if (args[1] == NULL){
            chdir(getenv("HOME"));}
          else{
            if (chdir(args[1]) != 0){
                perror("cd failed"); }}
    }
    //runs pwd part, show directory
    else if (strcmp(args[0], "pwd") == 0){
       char cwd[1024];
       if (getcwd(cwd, sizeof(cwd)) != NULL) {
           printf("%s\n", cwd); }
         else {
           perror("pwd failed"); }
     }
     //help to show the user the built in commands functions
     else if (strcmp(args[0], "help") == 0){
         printf("\n=== AiromShell - Available commands ===\n");
         printf("cd <dir>  - Change directory\n");
         printf("pwd       - Print current directory\n");
         printf("help      - Show this help message\n");
         printf("exit      - Exits the shell\n");
         printf("Other commands tun as a system command\n");
         printf("=====================================\n\n");
     }
}


//program starts
int main() {
    char input[MAX_INPUT];

    while (1) {
        // Print the prompt
        printf("airomsShell> ");
        fflush(stdout);

        // Read user input
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }

        // Remove the newline character at the end
        input[strcspn(input, "\n")] = '\0';

        // Exit if user types "exit"
        if (strcmp(input, "exit") == 0) {
            printf("Leaving already?? Goodbye\n");
            break;
        }

        // skipping empty input
        if (strlen(input) == 0){
           continue;}



	//checks if the input has pipe by calling functions
	if (has_pipe(input)){
	    PipeInfo pipe_info;
	    parse_pipe(input, &pipe_info);
	    execute_pipe(&pipe_info);
	    continue; }

        //call the parse function
        char *args[MAX_ARGS];
        Redirect redirect;
        parse_command(input, args, &redirect);


        //print the results of the function
        if (args[0] != NULL) {
            //check if its a built-in command
            if (is_builtin(args)) {
                run_builtin(args);
            }
            //runs if its not a built-in command
	    else {
                execute_command(args, &redirect);
            }
        }
    }


    return 0;
}

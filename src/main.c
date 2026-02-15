#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAX_ARGS 64
#define MAX_INPUT 1024



void parse_command(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \t");

    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
}



void execute_command(char **args){
    pid_t pid = fork();

    if (pid < 0) {
        // if fork fails
        perror("Fork Failed");
        return; }

    if (pid == 0){
       //child process
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
    if (strcmp(args[0], "cd") == 0){
        if (args[1] == NULL){
            chdir(getenv("HOME"));}
          else{
            if (chdir(args[1]) != 0){
                perror("cd failed"); }}
    }

    else if (strcmp(args[0], "pwd") == 0){
       char cwd[1024];
       if (getcwd(cwd, sizeof(cwd)) != NULL) {
           printf("%s\n", cwd); }
         else {
           perror("pwd failed"); }
     }

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

        //call the parse function
        char *args[MAX_ARGS];
        parse_command(input, args);


        //print the results of the function
        if (args[0] != NULL) {
            //check if its a built-in command
            if (is_builtin(args)) {
                run_builtin(args);
            } else {
                execute_command(args);
            }
        }
    }


    return 0;
}

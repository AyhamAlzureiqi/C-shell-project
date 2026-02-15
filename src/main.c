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
            execute_command(args);
        }
}


    return 0;
}

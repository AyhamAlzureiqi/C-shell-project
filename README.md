# AiromsShell - Simple Unix Shell in C

A custom command-line shell implementation in C, featuring command execution, built-in commands, I/O redirection, and pipes.

## Features

-  **Command Execution**: Run any system command with arguments
-  **Built-in Commands**: `cd`, `pwd`, `help`, `exit`
-  **I/O Redirection**: 
  - Output: `command > file`
  - Append: `command >> file`
  - Input: `command < file`
-  **Pipes**: Connect commands with `cmd1 | cmd2`

## Building
```bash
make
```

Or compile manually:
```bash
gcc src/main.c -o myshell
```

## Usage
```bash
./myshell
```

### Example Commands
```bash
# Basic commands
airomsShell> ls -la
airomsShell> pwd
airomsShell> cd /home

# I/O Redirection
airomsShell> ls > files.txt
airomsShell> cat < input.txt
airomsShell> echo "log entry" >> log.txt

# Pipes
airomsShell> ls | grep txt
airomsShell> cat file.txt | wc -l
airomsShell> ps aux | grep myshell

# Built-in commands
airomsShell> help
airomsShell> exit
```

## Technical Details

### Architecture

The shell operates in a Read-Eval-Print Loop (REPL):
1. Display prompt
2. Read user input
3. Parse input into commands and arguments
4. Execute commands using `fork()` and `execvp()`
5. Wait for completion and return to step 1

### Key System Calls

- `fork()` - Create child processes for command execution
- `execvp()` - Replace child process with command program
- `waitpid()` - Wait for child process completion
- `pipe()` - Create inter-process communication channels
- `dup2()` - Redirect file descriptors for I/O redirection
- `open()`, `close()` - File operations for redirection

## Project Structure
```
myshell/
├── src/
│   └── main.c          # Main shell implementation
├── README.md           # This file
└── Makefile           # Build configuration
```

## Implementation Highlights

- **Process Management**: Uses `fork()` to create child processes, keeping the main shell process alive
- **Built-in Commands**: Implemented in the shell process itself (not forked) to modify shell state
- **Pipe Implementation**: Creates pipe, forks two children, redirects stdout→pipe→stdin
- **Error Handling**: Checks return values of all system calls

## Limitations

- Single pipe support only (no chained pipes like `a | b | c`)
- No background process support (`&`)
- No command history
- No environment variable expansion
- No glob pattern matching (`*`, `?`)

## Learning Outcomes

This project demonstrates understanding of:
- Unix process model and system calls
- File descriptors and I/O redirection
- Inter-process communication (IPC)
- Systems programming in C
- Memory management and resource cleanup

## Author

Ayham AlZureiqi
Computer Engineering Student
@German Jordanian University

## Acknowledgments

Built during the winter break as a learning project to understand Unix shell internals and systems programming concepts,
While also demonestrating understanding of various C language concepts and ideas 

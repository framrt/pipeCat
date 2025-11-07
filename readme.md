# File Cat with Pipe Communication

A C program that demonstrates inter-process communication using pipes to read and display file contents with detailed byte-level information.

## Overview

This program creates a parent-child process pair that communicates via a Unix pipe. The child process reads a text file and sends its contents through the pipe, while the parent process receives and displays the data with detailed read statistics.

## Features

- **Automatic .txt Extension Handling**: Automatically appends `.txt` extension if not present in the filename
- **Pipe-Based IPC**: Demonstrates Unix pipe communication between parent and child processes
- **Detailed Read Statistics**: Shows byte counts for each read operation
- **Signal Handling**: Optional SIGINT (Ctrl+C) interrupt handling
- **Buffer Overflow Protection**: Safe string handling with bounded buffers

## Compilation

```bash
gcc -o filecat filecat.c -Wall
```

## Usage

```bash
./filecat <filename>
```

**Examples:**
```bash
# Will automatically look for "document.txt"
./filecat document

# Will use "myfile.txt" as provided
./filecat myfile.txt
```

## Program Flow

1. **Initialization**
   - Displays the parent process PID
   - Validates and processes the input filename
   - Adds `.txt` extension if missing
   - Opens the specified file

2. **Pipe Creation**
   - Creates a pipe with two file descriptors (read and write ends)

3. **Process Forking**
   - **Child Process**: Reads the file in chunks (1000 bytes) and writes to the pipe
   - **Parent Process**: Reads from the pipe and displays content with statistics

4. **Output**
   - Each read operation is labeled with read number and byte count
   - Total bytes read is displayed at the end

## Example Output

```
My PID is: 12345
input file from user: ./filecat
Added .txt extension: document.txt
parent reciving: 
[Read #1: 1000 bytes] <file content here>
[Read #2: 523 bytes] <remaining content>
===========================================
Parent: Pipe closed by child (EOF reached)

[TOTAL BYTES FILE: 1523]
```

## Configuration

### Buffer Size
```c
#define CHAR_BUFFER 1000
```
Defines the chunk size for file reading operations. Can be adjusted based on needs.

### Signal Handling
```c
#define HANDLE_INT
```
Enables SIGINT handling. Comment out to disable interrupt handling.

## Key Functions

### `check_and_add_txt_extension(char *filename)`
- **Returns**: `1` if `.txt` extension already exists, `0` if it was added
- **Purpose**: Ensures the filename has a `.txt` extension
- **Safety**: Uses `strrchr()` to find the last dot in the filename

## Error Handling

The program handles several error conditions:
- Pipe creation failure
- Fork failure
- File read errors
- Invalid file access

## Technical Details

- **IPC Method**: Unix pipes (`pipe()` system call)
- **Process Creation**: `fork()` system call
- **File I/O**: Standard C library (`fread()`)
- **Low-level I/O**: Unix system calls (`read()`, `write()`)

## Improvements Suggested

1. More check on user exception like ctrl+z && kill 

## Dependencies

- POSIX-compliant system (Linux, macOS, Unix)
- Standard C library
- System headers: `unistd.h`, `sys/wait.h`, `sys/types.h`, `signal.h`

## License

Educational/Academic use

## Author

Created by framrt as a demonstration of Unix IPC concepts and process management.

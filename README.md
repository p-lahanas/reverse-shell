### Technical assessment assignment – Embedded Developer
# Task
Build a software system composed of a client and server application where the client sends instructions for the server to execute. The server then acknowledges receipt of instructions and then acts on them.

The purpose of this exercise is to evaluate your ability to perform systems development – as such you are restricted to only use standard libraries.

As part of this assessment, we expect you to be able to articulate decisions made in the process of building your system – please be prepared to discuss why you made design decisions in follow-on interviews.

## Requirements
- Use UDP for client -> server communication
- Each instruction received by the server should result in an acknowledgement message being sent back to the client (also via UDP)
- Both client and server are to be written in C and are to both run on Linux – any modern distribution is acceptable (please note what distribution you tested on to assist with our assessment)
- The server supports the following instructions
  - Test
    - Server takes no action – just responds with acknowledgement
  - Run command
    - Execute a shell command
  - Stop server
    - Shuts down the server

## Deliverables
- Source code for both client and server
- Build instructions
- Usage instructions
- A short (maximum 1-page) document explaining your design
  - This should also document the protocol used to communicate between client and server

# Submission

## Build Instructions
The requirements are:

- CMake 3.17 or better;
- A C23 compatible compiler (gcc preferrable)

To build run the following in the project root directory:

```bash
mkdir build
```

```bash
cd build
```

```bash
cmake ..
```

```bash
cmake --build .
```

## Usage
### Server
From the build directory, to start the server, run the executable.
```bash
./apps/server
```
### Client
To run the client, specify an instruction using the corresponding flag. The run instruction requires an additional parameter which is the command to run. You must specify **EXACTLY ONE** flag when running the client. 
```bash
./apps/client [flag]
```
Flags
```
Option (only use ONE flag at a time):
  -r <command>  Send a run packet with the corresponding <command>
  -t            Send a test packet
  -s            Send a stop packet

```
**Example:** Send a test instruction
```bash
./client -t
```
**Example:** Run ls
```bash
./client -r ls
```
**Example:** Run ls with additional flags
```bash
./client -r "ls -al"
```

## TODO:
- Add multithreading into the server to support multiple concurrent connections
- Add sequence numbers in RTP so multiple packets can be sent at once, allowing for faster communication

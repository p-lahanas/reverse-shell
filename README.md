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

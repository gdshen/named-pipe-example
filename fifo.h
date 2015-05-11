#ifndef FIFO_H_
# define FIFO_H_

# include <stdlib.h>

// The basename for the various FIFOs used in the programs.
// They are used in the macos below.
# define SERVER_FIFO_FILENAME    "ServerFifo__"
# define CLIENT_FIFO_READER_NAME "ClientReaderFifo__"
# define CLIENT_FIFO_WRITER_NAME "ClientWriterFifo__"

// Some handy macros for creating file names of the FIFOs.
// _BUF_ is a char *
// _PID_ is an int (HINT: you may need to cast getpid())
# define CREATE_SERVER_FIFO_NAME(_BUF_) sprintf(_BUF_,"%s/%s"\
        ,getenv("HOME"),SERVER_FIFO_FILENAME);
# define CREATE_CLIENT_READER_NAME(_BUF_, _PID_) sprintf(_BUF_,"%s/%s%d"\
        ,getenv("HOME"),CLIENT_FIFO_READER_NAME,_PID_);
# define CREATE_CLIENT_WRITER_NAME(_BUF_, _PID_) sprintf(_BUF_,"%s/%s%d"\
        ,getenv("HOME"),CLIENT_FIFO_WRITER_NAME,_PID_);

// These are permissions you should use to create your new FIFOs.
# define FIFO_PERMISSIONS  S_IRUSR | S_IWUSR

// This is a list of the available commands for the client.
// You can get more about them by typing 'help' into the client.
# define CMD_EXIT         "exit"
# define CMD_CHDIR        "cd"
# define CMD_LOCAL_CHDIR  "lcd"
# define CMD_DIR          "dir"
# define CMD_LOCAL_DIR    "ldir"
# define CMD_PWD          "pwd"
# define CMD_LOCAL_PWD    "lpwd"
# define CMD_PUT          "put"
# define CMD_GET          "get"
# define CMD_HELP         "help"

// Might be handy
# define NEW_LINE         '\n'

// The prompt the client shows.
# define CLIENT_PROMPT    ">>> "

// When the server has finished sending over the text for
// a directory, this is sent.
# define RETURN_OKAY      "OKAY\n"

// When the client server is unhappy about something.
# define RETURN_ERROR     "SERVER ERROR\n"

// This is the command to use with popen() to get a
// direcctory listing.
# define CMD_LS_POPEN    "ls -lF"

#endif /* FIFO_H_ */

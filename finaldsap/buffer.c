#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>   //https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
//Input-output system calls in C 
#include<errno.h>  //perror,strerror
#include"buffer.h"
#include<ncurses.h>
#include<unistd.h> //standard symbolic constants and types
#include"gui.h"
int total_lines;

// Function to initialize a buffer
void init_buffer(buffer **bf) {
    (*bf) = (buffer *)malloc(sizeof(buffer));
    (*bf)->prev = NULL;
    (*bf)->next = NULL;
    (*bf)->cur_line = 0;
    (*bf)->num_chars = 0;
    (*bf)->cur_X = 0;
    Lineinit(*bf);
    return;
}

// Function to initialise line
void Lineinit(buffer *bf) {
    bf->line = (char *)malloc(sizeof(char) * LINEMAX);
    bf->line = memset(bf->line, '\0', LINEMAX); 
    return;
}

// Function to check the provided file is exist or not.
int fileexist(char *filename) {  //file descriptor is integer that uniquely identifies an open file of the process.
    int fd;
    fd = open(filename, O_RDWR); //Open(included in fcntl.h) for reading and writing
    if(fd == -1)
        return 0;
    close(fd);
    return 1;
}


// Function to create next buffer
void buf_create_next(buffer *bf) {
    bf->next = (buffer *)malloc(sizeof(buffer));
    bf->next->prev = bf;
    bf->next->cur_line = bf->cur_line + 1;
    bf->next->num_chars = 0;
    bf->next->cur_X = 0;
    bf = bf->next;
    Lineinit(bf);
}

// Function to load a buffer from existing file
void buffer_load(int fd, buffer *bf) { 
    char ch;
    int err, linenumber = 0, i = 0;

    while(err = read(fd, &ch, 1)) {
        if(err == -1) {
            printf("Read error :( ");
            exit(0);
        }
        if(i < LINEMAX) {
            if(ch == '\n') {
                buf_create_next(bf);
                bf->line[i] = ch;
                bf->cur_line = linenumber;
                bf->num_chars = i + 1;
                bf = bf->next;
                linenumber++;
                i = 0;
                continue;
            }
            else {
                bf->line[i] = ch;
                bf->num_chars = i; 
            }
        }
        else { //directly go into next line
            buf_create_next(bf);
            bf->line[i] = ch;
            bf->cur_line = linenumber;
            bf->num_chars = i + 1;
            bf = bf->next;
            linenumber++;
            i = 0;
            continue; 
        }
        i++;
    }
    total_lines = linenumber;
    return;
}

// implimented to check that is the buffer loads all the characters into it from an existing file . for testing purpose only.
// this function prints the buffer content to the stdout
void buf_print_stdout(buffer *bf) {
    while(1) {
        if(bf == NULL) 
            return;
        else {
            fputs(bf->line, stdout);
            bf = bf->next;
        }
    }
}


/*destroys (clears all data in them) all the buffers after and including current buffer. */
void distroy_buffer(buffer *bf){
	buffer *temp;

	while(bf != NULL){
		temp = bf;
		bf = bf->next;
		free(temp->line);
		free(temp);
	}

}

// It will insert a character at a given location means at cursor position.
void charInsert(buffer *bf, char ch, int loc) {
    if(loc == LINEMAX) {
        return;
    }
    else if(loc == bf->num_chars) {
        bf->line[loc] = ch;
        bf->num_chars++;
        bf->cur_X = loc+1;
    }
    else if(bf->num_chars == 0) {
        bf->line[0] = ch;
        bf->num_chars++;
        bf->cur_X++;
    }
    else {
        memmove(bf->line + loc + 1, bf->line + loc, bf->num_chars - loc); //memmove() is used to copy a block of memory from a location to another. It is declared in string.h
        bf->line[loc] = ch;
        bf->num_chars++;
        bf->cur_X++;  
    }
}

// Function to save buffer content into file
void bufSave(int fd, buffer *bf) {    
    while(bf != NULL) {
        write(fd, bf->line, bf->num_chars); //////
        if(write(fd, bf->line, bf->num_chars) == -1) {
            printf("Error while writing :(");
        }
        bf = bf->next;
    }
}

//funtion to increase the line number by given number of all the buffers starting from the given buffer
void buf_Incr_lineno(buffer *bf, int no) {
    buffer *temp = bf;
    while(temp->next != NULL) {
        temp->cur_line = temp->cur_line + no;
        temp = temp->next;
    }
    (temp->cur_line)++;
}

// Function to decrement the line number by given number of all buffers stating form the given buffer
void buf_Decr_lineno(buffer *bf, int no) {
    buffer *temp = bf;
    while(temp->next != NULL) {
        temp->cur_line = temp->cur_line - no;
        temp = temp->next;
    }
    (temp->cur_line)--;
}

// Function to insert line between two lines or next line
void bufInsert_betw(buffer *bf) {
    if(bf->next == NULL) {
        buf_create_next(bf);
        return;
    }
    buf_Incr_lineno(bf->next, 1);
    buffer *temp;
    temp = (buffer *)malloc(sizeof(buffer));
    temp->next = bf->next;
    temp->prev = bf;
    bf->next->prev = temp;
    bf->next = temp;
    bf->next->cur_line = bf->cur_line + 1;
    bf->next->num_chars = 0;
    Lineinit(bf->next);
}
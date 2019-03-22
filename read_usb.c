#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>


/*
This code configures the file descriptor for use as a serial port.
*/
void configure(int fd) {
  struct termios pts;
  tcgetattr(fd, &pts);
  cfsetospeed(&pts, 9600);
  cfsetispeed(&pts, 9600);

  /*
  // You may need to un-comment these lines, depending on your platform.
  pts.c_cflag &= ~PARENB;
  pts.c_cflag &= ~CSTOPB;
  pts.c_cflag &= ~CSIZE;
  pts.c_cflag |= CS8;
  pts.c_cflag &= ~CRTSCTS;
  pts.c_cflag |= CLOCAL | CREAD;
  pts.c_iflag |= IGNPAR | IGNCR;
  pts.c_iflag &= ~(IXON | IXOFF | IXANY);
  pts.c_lflag |= ICANON;
  pts.c_oflag &= ~OPOST;
  */

  tcsetattr(fd, TCSANOW, &pts);

}


int main(int argc, char* argv[]) {

  if (argc < 2) {
    printf("Please specify the name of the serial port (USB) device file!\n");
    exit(0);
  }

  // get the name from the command line
  char* filename = argv[1];
  
  // try to open the file for reading and writing
  // you may need to change the flags depending on your platform
  int fd = open(filename, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (fd < 0) {
    perror("Could not open file\n");
    exit(1);
  }
  else {
    printf("Successfully opened %s for reading and writing\n", filename);
  }

  configure(fd);

  /*
    Write the rest of the program below, using the read and write system calls.
  */
    char msg[100];
    int msgind = 0;
    while(1){
        char start, end;
        char buf[100];
        int bytes_read = read(fd, buf, 100);
        int i;
        for(i = 0; i < bytes_read; i++){
            if(buf[i]=='\n'){
                msg[msgind] = buf[i];
                msg[msgind+1] = '\0';
                printf("%s", msg);
                msgind = 0;
            }
            else{
                msg[msgind] = buf[i];
                msgind++;
            }
        }
    }

}

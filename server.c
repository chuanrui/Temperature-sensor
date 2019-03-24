/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
//extern from read_usb
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
//////
//extern from read_usb
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
//////
}
int start_server(int PORT_NUMBER)
{

    //////
    //extern from read_usb
    char* filename = "/dev/cu.usbmodem146201";
    
    // try to open the file for reading and writing
    // you may need to change the flags depending on your platform
    int fd2 = open(filename, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (fd2 < 0) {
        perror("Could not open file\n");
        exit(1);
    }
    else {
        printf("Successfully opened %s for reading and writing\n", filename);
    }
    
    configure(fd2);
//////
      // structs to represent the server and client
      struct sockaddr_in server_addr,client_addr;    
      
      int sock; // socket descriptor

      // 1. socket: creates a socket descriptor that you later use to make other system calls
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Socket");
	exit(1);
      }
      int temp;
      if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
	perror("Setsockopt");
	exit(1);
      }

      // configure the server
      server_addr.sin_port = htons(PORT_NUMBER); // specify port number
      server_addr.sin_family = AF_INET;         
      server_addr.sin_addr.s_addr = INADDR_ANY; 
      bzero(&(server_addr.sin_zero),8); 
      
      // 2. bind: use the socket and associate it with the port number
      if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	perror("Unable to bind");
	exit(1);
      }

      // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
      if (listen(sock, 1) == -1) {
	perror("Listen");
	exit(1);
      }
          
      // once you get here, the server is set up and about to start listening
      printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
      fflush(stdout);
     

      // 4. accept: wait here until we get a connection on that port
      int sin_size = sizeof(struct sockaddr_in);
      int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
      if (fd != -1) {
	printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
      
	// buffer to read data into
	char request[1024];
	
	// 5. recv: read incoming message (request) into buffer
	int bytes_received = recv(fd,request,1024,0);
	// null-terminate the string
	request[bytes_received] = '\0';
	// print it to standard out
	printf("This is the incoming request:\n%s\n", request);

	// this is the message that we'll send back
	char *reply = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><p>Temperature</p></html>";

	// 6. send: send the outgoing message (response) over the socket
	// note that the second argument is a char*, and the third is the number of chars	
	send(fd, reply, strlen(reply), 0);
	//////
    //extern from read_usb
          char msg[100];
          char reply2[100];
          int msgind = 0;
          while(1){
              char start, end;
              char buf[100];
              int bytes_read = read(fd2, buf, 100);
              int i;
              for(i = 0; i < bytes_read; i++){
                  if(buf[i]=='\n'){
                      msg[msgind] = buf[i];
                      msg[msgind+1] = '\0';
                      reply2[0] = '\0';
                      strcat(reply2, "\n<html><p>");
                      strcat(reply2,msg);
                      strcat(reply2,"</p></html>");
                      send(fd, reply2, strlen(reply2), 0);
                      msgind = 0;
                  }
                  else{
                      msg[msgind] = buf[i];
                      msgind++;
                  }
              }
          }
    //////
	// 7. close: close the connection
	close(fd);
	printf("Server closed connection\n");
      }

      // 8. close: close the socket
      close(sock);
      printf("Server shutting down\n");
  
      return 0;
} 



int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 2) {
      printf("\nUsage: %s [port_number]\n", argv[0]);
      exit(-1);
  }

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }

  start_server(port_number);
}


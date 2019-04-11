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
#include <pthread.h>
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
}
//////
pthread_mutex_t lock;
int shut = 0;
//char reply2[200];
char msg[100];
int rear = 3599;
double data[3600];
int full = 0;
double min = 200;
double max = 0;
double average = 0;
int is_connect = 1;
int afd=-1;

void build_file(char* reply, char* filename){
    FILE* thefile = fopen(filename,"r");
    if(thefile == NULL){
        printf("cannot open file\n");
    }
    char* line = malloc(30000*sizeof(char));
    while(fgets(line, 30000, thefile)!=NULL){
        //line[strlen(line)-1] = '\0';
        strcat(reply, line);
        free(line);
        line = malloc(30000*sizeof(char));
    }
    free(line);
    fclose(thefile);
    //printf("%s", reply);
}
//////
int start_server(int PORT_NUMBER)
{
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
    while(!shut){
      int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
        printf("socket fd is %d\n", fd);
        
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
            //char *reply = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><p>Temperature</p></html>";
            // 6. send: send the outgoing message (response) over the socket
            // note that the second argument is a char*, and the third is the number of chars
            //send(fd, reply, strlen(reply), 0);
          char* copy_request = malloc(1024*sizeof(char));
          strcpy(copy_request, request);
          char* token = strtok(copy_request," ");
          token = strtok(NULL, " ");
          if(strcmp(token, "/test.html")==0){
              char reply[2000];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
              char* html = "index.html";
              build_file(reply, html);
              send(fd, reply, strlen(reply), 0);
          }
          else if(strcmp(token, "/jquery.js")==0){
              char* reply = malloc(90000*sizeof(char));
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/javascript\n\n");
              char* jquery = "jquery.js";
              build_file(reply, jquery);
              send(fd, reply, strlen(reply), 0);
              free(reply);
          }
          else if(strcmp(token, "/Project.js")==0){
              char reply[2000];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/javascript\n\n");
              char* js = "Project.js";
              build_file(reply, js);
              send(fd, reply, strlen(reply), 0);
          }
          /*else if(strcmp(token, "/jquery.canvasjs.min.js")==0){
              char* reply = malloc(500000*sizeof(char));
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/javascript\n\n");
              char* graph = "jquery.canvasjs.min.js";
              build_file(reply, graph);
              send(fd, reply, strlen(reply), 0);
              free(reply);
          }*/
          else if(strcmp(token, "/real")==0){
              char reply[100];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(100*sizeof(char));
              sprintf(string,"{\"real\":%f,\"is_connect\":%d}", data[rear], is_connect);
              strcat(reply, string);
              free(string);
              printf("msg is %s\n", reply);
              send(fd, reply, strlen(reply), 0);
          }
          else if(strcmp(token, "/min")==0){
              char reply[100];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(100*sizeof(char));
              sprintf(string,"{\"min\":%f,\"is_connect\":%d}", min, is_connect);
              strcat(reply, string);
              free(string);
              send(fd, reply, strlen(reply), 0);
          }
          else if(strcmp(token, "/max")==0){
              char reply[100];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(100*sizeof(char));
              sprintf(string,"{\"max\":%f,\"is_connect\":%d}", max, is_connect);
              strcat(reply, string);
              free(string);
              send(fd, reply, strlen(reply), 0);
          }
          else if(strcmp(token, "/average")==0){
              char reply[100];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(100*sizeof(char));
              sprintf(string,"{\"average\":%f,\"is_connect\":%d}", average, is_connect);
              strcat(reply, string);
              free(string);
              send(fd, reply, strlen(reply), 0);
          }
          else if(strcmp(token, "/updateall")==0){
              char reply[200];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(200*sizeof(char));
              sprintf(string,"{\"real\":%f,\"min\":%f,\"max\":%f,\"average\":%f,\"is_connect\":%d}", data[rear],min,max,average,is_connect);
              strcat(reply, string);
              free(string);
              send(fd, reply, strlen(reply), 0);
          }
          else if(strcmp(token, "/graph")==0){
              char* reply = malloc(100000*sizeof(char));
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              strcat(reply, "{\"array\":[");
              char* tmp;
              int i;
              for(i = 0; i < 3599; i++){
                  tmp = malloc(100*sizeof(char));
                  sprintf(tmp, "%f,", data[i]);
                  strcat(reply, tmp);
                  free(tmp);
              }
              tmp = malloc(300*sizeof(char));
              sprintf(tmp, "%f],\"rear\":%d,\"full\":%d}",data[i], rear, full);
              strcat(reply, tmp);
              free(tmp);
              send(fd, reply, strlen(reply), 0);
              free(reply);
          }
          else if(strcmp(token, "/light")==0){
              char reply[100];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(100*sizeof(char));
              sprintf(string,"{\"is_connect\":%d}", is_connect);
              strcat(reply, string);
              free(string);
              send(fd, reply, strlen(reply), 0);
              write(afd,"light",5);
          }
          else if(strcmp(token, "/c")==0){
              char reply[100];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(100*sizeof(char));
              sprintf(string,"{\"is_connect\":%d}", is_connect);
              strcat(reply, string);
              free(string);
              send(fd, reply, strlen(reply), 0);
              write(afd,"c",1);
          }
          else if(strcmp(token, "/f")==0){
              char reply[100];
              reply[0] = '\0';
              strcat(reply, "HTTP/1.1 200 OK\nContent-Type: application/json\n\n");
              char* string = malloc(100*sizeof(char));
              sprintf(string,"{\"is_connect\":%d}", is_connect);
              strcat(reply, string);
              free(string);
              send(fd, reply, strlen(reply), 0);
              write(afd,"f",1);
          }
          free(copy_request);
            /*pthread_mutex_lock(&lock);
            send(fd, reply2, strlen(reply2), 0);
            pthread_mutex_unlock(&lock);*/
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
void* update(void* r){
    //extern from read_usb
    char* filename = "/dev/cu.usbmodem146201";
    
    // try to open the file for reading and writing
    // you may need to change the flags depending on your platform
    afd = open(filename, O_RDWR | O_NOCTTY | O_NDELAY);
    printf("arduino fd is %d\n", afd);
    if (afd < 0) {
        perror("Could not open file\n");
        exit(1);
    }
    else {
        printf("Successfully opened %s for reading and writing\n", filename);
    }
    configure(afd);
    
    int msgind = 0;
    int times = 0;
    while(!shut){
        char start, end;
        char buf[100];
        int fd2 = open(filename, O_RDWR | O_NOCTTY | O_NDELAY);
        //printf("arduino fd2 is %d\n", fd2);
        close(fd2);
        if(fd2<0){
            close(fd2);
            close(afd);
            afd = -1;
            is_connect = -1;
            continue;
        }
        else{
            if(afd>0);
            else{
                afd = open(filename, O_RDWR | O_NOCTTY | O_NDELAY);
                configure(afd);
                is_connect = 1;
            }
        }
        int bytes_read = read(afd, buf, 100);
        int i;
        for(i = 0; i < bytes_read; i++){
            if(buf[i]=='\n'){
                
                msg[msgind] = buf[i];
                msg[msgind+1] = '\0';
                times++;
                if(msg[0]!='\n'&&msg[0] == 'T'&&times>3){
                    //process the incoming data
                    pthread_mutex_lock(&lock);
                    /*reply2[0] = '\0';
                    strcat(reply2, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><p>");
                    strcat(reply2,msg);
                    strcat(reply2,"</p></html>");*/
                    char* copy_msg = msg;
                    while(*copy_msg<'0' || *copy_msg>'9'){
                        copy_msg++;
                    }
                    double current = strtof(copy_msg, NULL);
                    rear = (rear+1)%3600;
                    if(rear == 3599){
                        full = 1;
                    }
                    if(current > max){
                        max = current;
                    }
                    if(current < min){
                        min = current;
                    }
                    if(full==0){
                        average = (average*rear+current)/(rear+1);
                    }
                    else{
                        average = (3600*average - (rear+3599)%3600 + current)/3600;
                    }
                    data[rear] = current;
                    //printf("msg is %f\n", data[rear]);
                    /*printf("msg is %s", msg);
                    printf("max temp is %f\n", max);
                    printf("min temp is %f\n\n", min);
                    printf("average temp is %f\n", average);*/
                    pthread_mutex_unlock(&lock);
                    //process end
                }
                msgind = 0;
            }
            else{
                msg[msgind] = buf[i];
                msgind++;
            }
        }
    }
    return r;
}
//shutting down
void* check(void* r){
    char tmp[100];
    while(1){
        scanf("%s", tmp);
        if (strcmp(tmp, "q\n")) {
            shut = 1;
            break;
        }
    }
    return r;
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
    pthread_t t1, t2;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&t1, NULL, &check, NULL);
    pthread_create(&t2, NULL, &update, NULL);
    start_server(port_number);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_mutex_destroy(&lock);
}


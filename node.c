//This program is to represent each node in the ring
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"

int main(int argc, char const *argv[]){

  int id, port, neighborPort, pid, pipe1[2];
  struct message msg;

  srand(time(0));

  id = (rand()%1000) + 1;

  port = id + 1024;

  msg.id = id;
  msg.port = port;
  sprintf(msg.buffer, " %d", msg.id);

  printf("Node ID = %d\n", id);
  printf("Node port = %d\n", port);

  //Create a pipe for communication between the child and parent
  if(pipe(pipe1) < 0){
    printf("Error on pipe creation");
    exit(EXIT_FAILURE);
  }

  pid = fork();

  //The child is used to run the socket server
  if(pid == 0){
    int server_fd, new_socket;
    struct sockaddr_in address;
    struct message msg1;
    int addrlen = sizeof(address);

    close(pipe1[0]);

    server_fd = createSocket();

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
      {
        perror("bind failed");
        exit(EXIT_FAILURE);
      }
    if (listen(server_fd, 3) < 0)
      {
        perror("listen");
        exit(EXIT_FAILURE);
      }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
      {
        perror("accept");
        exit(EXIT_FAILURE);
      }

    //Receive the Election message and send to parent so this node's ID can get added to the message
    if(recv( new_socket , &msg1, sizeof(msg1), 0) < 0){
      perror("Error on recv");
      exit(EXIT_FAILURE);
    }
    if(write(pipe1[1], &msg1, sizeof(msg1)) < 0){
      perror("Error on write");
      exit(EXIT_FAILURE);
    }

      //Receive the Coordinator message
    if(recv( new_socket , &msg1, sizeof(msg1), 0) < 0){
      perror("Error on recv");
      exit(EXIT_FAILURE);
    }
    if(write(pipe1[1], &msg1, sizeof(msg1)) < 0){
      perror("Error on write");
      exit(EXIT_FAILURE);
    }

      //Close the socket
      close(new_socket);
  }
  else{
    int neighborSock = 0;
    struct sockaddr_in serv_addr;
    struct message msg2;

    close(pipe1[1]);

    //Read the Election message coming in from neighboring node
    if(read(pipe1[0], &msg2, sizeof(msg2)) < 0){
      perror("Error on read");
      exit(EXIT_FAILURE);
    }

    //Want to make sure the buffer isn't empty before continuing. If it's empty that means binding failed
    if(msg2.buffer[0] == '\0'){
      exit(1);
    }
    printf("E(%s)\n",msg2.buffer);

    //Add this nodes ID to the Election message
    strcat(msg2.buffer, msg.buffer);
    msg2.list[msg2.values] = msg.id;
    msg2.values = msg2.values + 1;

    printf("enter neighbor port: ");
    scanf("%d", &neighborPort);

    neighborSock = createSocket();

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(neighborPort);

    if (connect(neighborSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      {
        perror("\nConnection Failed \n");
        exit(EXIT_FAILURE);
      }

    //Send the Election message to the next node in the ring
    if(send(neighborSock , &msg2 , sizeof(msg) , 0 ) < 0){
      perror("Error on send");
      exit(EXIT_FAILURE);
    }

    //Read the Coordinator message and pass it along to next node in the ring
    if(read(pipe1[0], &msg2, sizeof(msg2)) < 0){
      perror("Error on read");
      exit(EXIT_FAILURE);
    }
    printf("C(%s)\n", msg2.buffer);
    if(send(neighborSock , &msg2 , sizeof(msg) , 0) < 0){
      perror("Error on send");
      exit(EXIT_FAILURE);
    }

    //Close the neighborSock since we are done with it
    close(neighborSock);

    //Determines the largest ID in the Coordinator message
    int max = largest(msg2.list, msg2.values);

    //Check to make sure there are 6 total processes, if not exit program
    if(msg2.values != 6){
      printf("invalid number of processes!\n");
      exit(2);
    }

    //Fork here and execl either ./diningPhilosophers or ./coordinator program
    int pid1 = fork();

    if(pid1 == 0){
      //If the processes ID is the max, start up Coordinator process
      if(msg.id == max){
        startCoordinator(msg2, msg, msg.port);
      }
      //Else start up Philosopher process
      else{
        startPhilosopher(msg, max);
      }
    }

    //Parent process waits for child to complete
    else{
      int returnStatus;
      if(waitpid(pid1, &returnStatus, 0) < 0){
        perror("Error on waitpid");
        exit(EXIT_FAILURE);
      }
    }

  return 0;
  }
}

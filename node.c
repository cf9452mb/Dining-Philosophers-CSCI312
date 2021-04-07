//This program is to represent each node in the ring
#include <stdio.h>
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
    printf("Error on pipe creation: %d\n", errno);
    exit(0);
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
        exit(1);
      }
    if (listen(server_fd, 3) < 0)
      {
        perror("listen");
        exit(2);
      }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
      {
        perror("accept");
        exit(3);
      }

    //Receive the Election message and send to parent so this node's ID can get added to the message
      recv( new_socket , &msg1, sizeof(msg1), 0);
      write(pipe1[1], &msg1, sizeof(msg1));

      //Receive the Coordinator message
      recv( new_socket , &msg1, sizeof(msg1), 0);
      write(pipe1[1], &msg1, sizeof(msg1));

      //Close the socket
      close(new_socket);
  }
  else{
    int neighborSock = 0;
    struct sockaddr_in serv_addr;
    struct message msg2;

    close(pipe1[1]);

    //Read the Election message coming in from neighboring node
    read(pipe1[0], &msg2, sizeof(msg2));
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
        exit(4);
      }

    //Send the Election message to the next node in the ring
    send(neighborSock , &msg2 , sizeof(msg) , 0 );

    //Read the Coordinator message and pass it along to next node in the ring
    read(pipe1[0], &msg2, sizeof(msg2));
    printf("C(%s)\n", msg2.buffer);
    send(neighborSock , &msg2 , sizeof(msg) , 0 );

    //This variable contains the ID of the Coordinator
    int max = atoi(msg2.buffer);

    //Check to make sure there are 6 total processes, if not exit program
    if(msg2.values != 6){
      printf("invalid number of processes!\n");
      exit(5);
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
      waitpid(pid1, &returnStatus, 0);
    }

  return 0;
  }
}

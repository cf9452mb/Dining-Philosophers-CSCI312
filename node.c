//This program is to represent each node in the ring
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
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
    exit(1);
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

      recv( new_socket , &msg1, sizeof(msg1), 0);
      write(pipe1[1], &msg1, sizeof(msg1));

      recv( new_socket , &msg1, sizeof(msg1), 0);
      write(pipe1[1], &msg1, sizeof(msg1));
  }
  else{
    int neighborSock = 0;
    struct sockaddr_in serv_addr;
    struct message msg2;

    close(pipe1[1]);

    read(pipe1[0], &msg2, sizeof(msg2));
    printf("E(%s)\n",msg2.buffer);
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
        printf("\nConnection Failed \n");
        return -1;
      }

    send(neighborSock , &msg2 , sizeof(msg) , 0 );

    read(pipe1[0], &msg2, sizeof(msg2));
    printf("C(%s)\n", msg2.buffer);
    send(neighborSock , &msg2 , sizeof(msg) , 0 );

    int max = atoi(msg2.buffer);


  return 0;
  }
}

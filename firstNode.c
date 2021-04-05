//This program is for the First node in the ring
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

  //Initialize the message structure
  msg.id = id;
  msg.port = port;
  msg.values = 1;
  msg.list[0] = msg.id;
  sprintf(msg.buffer, "%d", msg.id);

  printf("Node ID = %d\n", id);
  printf("Node port = %d\n", port);

  //Create a pipe for communication between parent and child
  if(pipe(pipe1) < 0){
    printf("Error on pipe creation: %d\n", errno);
    exit(1);
  }

  pid = fork();

  //Child process is used to run the socket server
  if(pid == 0){
    int server_fd, new_socket;
    struct sockaddr_in address;
    struct message msg1;
    int addrlen = sizeof(address);

    //Close reading pipe since it will only be used for writing
    close(pipe1[0]);

    server_fd = createSocket();

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons( port );

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

      //Receive the election message from neighboring node and send it to the parent through a pipe
      recv( new_socket , &msg1, sizeof(msg1), 0);
      write(pipe1[1], &msg1, sizeof(msg1));

      //Receive the coordinator message from neighboring node
      recv( new_socket , &msg1, sizeof(msg1), 0);
      write(pipe1[1], &msg1, sizeof(msg1));
  }

  //Parent code
  else{
    close(pipe1[1]);

    printf("enter neighbor port: ");
    scanf("%d", &neighborPort);

    int neighborSock = 0;
    struct sockaddr_in serv_addr;
    struct message msg2;

    neighborSock = createSocket();

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(neighborPort);

    if (connect(neighborSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      {
        printf("\nConnection Failed \n");
        return -1;
      }


    send(neighborSock , &msg , sizeof(msg) , 0 );
    read(pipe1[0], &msg2, sizeof(msg2));
    printf("E(%s)\n", msg2.buffer);

    int max = largest(msg2.list, msg2.values);
    printf("Largest ID = %d\n", max);

    sprintf(msg2.buffer, "%d", max);
    send(neighborSock, &msg2, sizeof(msg2), 0);

    read(pipe1[0], &msg2, sizeof(msg2));


    //Fork here and execl either dininingPhilosphers or coordinator



    return 0;
  }
}

//Function to return the largest element in the arr[] array
int largest(int arr[], int size)
{
  int i;

  // Initialize maximum element
  int max = arr[0];

  // Traverse array elements from second and
  // compare every element with current max
  for (i = 1; i < size; i++)
    if (arr[i] > max)
      max = arr[i];

  return max;
}

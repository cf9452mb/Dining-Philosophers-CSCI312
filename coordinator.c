//This is the program that the coordinator runs. It is used to communicate with each of the 5 Philosophers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"

int main(int argc, char *argv[]){

  int first, second, third, fourth, fifth, sixth, port;

  //Extract all of the values in the Node ring
  first = atoi(argv[1]);
  second = atoi(argv[2]);
  third = atoi(argv[3]);
  fourth = atoi(argv[4]);
  fifth = atoi(argv[5]);
  sixth = atoi(argv[6]);
  port = atoi(argv[7]);

  printf("I am the Coordinator\n");

  printf("My port number is = %d\n", port);
  printf("value = %d\n", first);
  printf("value = %d\n", second);
  printf("value = %d\n", third);
  printf("value = %d\n", fourth);
  printf("value = %d\n", fifth);
  printf("value = %d\n", sixth);

  //Code to create the socket connection to communicate with the Philosophers
  /*
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  server_fd = createSocket();

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons( port );

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
  */

  return 0;

}

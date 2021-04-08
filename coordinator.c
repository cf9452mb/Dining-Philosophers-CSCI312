//This is the program that the coordinator runs. It is used to communicate with each of the 5 Philosophers
#include <stdlib.h>
#include <string.h>
#include "shared.h"

int main(int argc, char *argv[]){

  int nodes[N+1], size, id, port;

  //Extract all of the values in the Node ring, this is the order they are in
  for(int i = 0; i < (N+1); i++){
    nodes[i] = atoi(argv[i+1]);
  }
  id = atoi(argv[7]);
  port = atoi(argv[8]);

  size = deleteElement(nodes,(N+1), id);

  printf("I am the Coordinator\n");
  printf("My ID is = %d\n", id);
  printf("My port number is = %d\n", port);
  printf("The processes are in this order:\n");
  for(int i = 0; i < size; i++){
    printf("%d\n", nodes[i]);
  }

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

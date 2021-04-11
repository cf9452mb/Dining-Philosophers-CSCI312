//This is the program that the coordinator runs. It is used to communicate with each of the 5 Philosophers
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#include <errno.h>
#include "shared.h"

int main(int argc, char *argv[]){

  int nodes[N+1], client_sockets[5], new_socket, valread, size, id, port, max_sd, sd;
  bool chopsticksTaken[5] = {false, false, false, false, false};
  struct sockaddr_in address;
  struct message msg;
  struct queueList queue[3];
  int addrlen = sizeof(address);
  fd_set readfds;
  int entries = 0;

  //Extract all of the values in the Node ring, this is the order they are in
  for(int i = 0; i < (N+1); i++){
    nodes[i] = atoi(argv[i+1]);
  }
  id = atoi(argv[7]);
  port = atoi(argv[8]);

  //Delete the coordinator out of the ring since it will not be participating in the dining philosophers
  size = deleteElement(nodes,(N+1), id);

  printf("I am the Coordinator\n");
  printf("My ID is = %d\n", id);
  printf("My port number is = %d\n", port);
  printf("The processes are in this order:\n");
  for(int i = 0; i < size; i++){
    printf("%d\n", nodes[i]);
  }

  for(int i = 0; i < N; i++){
    client_sockets[i] = 0;
  }

  int coordSocket = createSocket();

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(coordSocket, (struct sockaddr *)&address, sizeof(address))<0)
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }

  //try to specify maximum of 5 pending connections for the master socket
  if (listen(coordSocket, 5) < 0)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }

  while(1)
    {
      //clear the socket set
      FD_ZERO(&readfds);

      //add coord socket to set
      FD_SET(coordSocket, &readfds);
      max_sd = coordSocket;

      //add child sockets to set
      for (int i = 0 ; i < N ; i++)
        {
          //socket descriptor
          sd = client_sockets[i];

          //if valid socket descriptor then add to read list
          if(sd > 0)
            FD_SET( sd , &readfds);

          //highest file descriptor number, need it for the select function
          if(sd > max_sd)
            max_sd = sd;
        }

      //wait for an activity on one of the sockets , timeout is NULL ,
      //so wait indefinitely
      int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

      if ((activity < 0) && (errno!=EINTR))
        {
          printf("select error");
        }

      //If something happened on the coordSocket ,
      //then its an incoming connection
      if (FD_ISSET(coordSocket, &readfds))
        {
          if ((new_socket = accept(coordSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
              perror("accept");
              exit(EXIT_FAILURE);
            }

          //add new socket to array of sockets
          for (int i = 0; i < N; i++)
            {
              //if position is empty
              if( client_sockets[i] == 0 )
                {
                  client_sockets[i] = new_socket;
                  printf("Adding to list of sockets as %d\n" , i);

                  break;
                }
            }
        }

      //else its some IO operation on some other socket
      for (int i = 0; i < N; i++)
        {
          sd = client_sockets[i];

          if (FD_ISSET( sd , &readfds))
            {
              //Read the incoming message and determine which type of message it is
              valread = read( sd , &msg, sizeof(msg));

              if(strcmp(msg.buffer, "REQUEST") == 0){
                printf("%d is requesting CS\n", msg.id);
                int spot;
                for(int i = 0; i < N; i++){
                  if(nodes[i] == msg.id){
                    spot = i;
                    break;
                  }
                }
                if(chopsticksTaken[spot] == false && chopsticksTaken[((spot+4)%5)] == false){
                  strcpy(msg.buffer, "OK");
                  printf("Sending OK to %d\n", msg.id);
                  send(sd, &msg, sizeof(msg), 0);
                  chopsticksTaken[spot] = true;
                  chopsticksTaken[((spot+4)%5)] = true;
                }
                else{
                  //Stick the calling process on the Queue
                  struct queueList entry;
                  entry.id = msg.id;
                  entry.socketDesc = sd;
                  queue[entries] = entry;
                  entries++;
                }
              }

              else if(strcmp(msg.buffer, "RELEASE") == 0){
                printf("Releasing CS from %d\n", msg.id);
                int remove = 4, spot;
                for(int i = 0; i < N; i++){
                  if(nodes[i] == msg.id){
                    spot = i;
                    break;
                  }
                }
                chopsticksTaken[spot] = false;
                chopsticksTaken[((spot+4)%5)] = false;
                for(int i = 0; i < entries; i++){
                  int spot1;
                  for(int j = 0; j < N; j++){
                    if(nodes[j] == queue[i].id){
                      spot1 = j;
                      break;
                    }
                  }
                  if(chopsticksTaken[spot1] == false && chopsticksTaken[((spot1+4)%5)] == false){
                    strcpy(msg.buffer, "OK");
                    printf("Sending OK to %d\n", msg.id);
                    send(queue[i].socketDesc, &msg, sizeof(msg), 0);
                    chopsticksTaken[spot1] = true;
                    chopsticksTaken[((spot1+4)%5)] = true;
                    remove = spot1;
                  }
                }
                //Delete element from Queue if needed and take 1 off entries
                if(remove != 4){
                  int size = deleteQueue(queue, entries, nodes[remove]);
                  entries--;
                }
              }
              else{
                printf("default\n");
                printf("Buffer = %s\n", msg.buffer);
              }



            }
        }
    }

  return 0;

}

//This is the shared header file for Project5
#ifndef SHARED_H
#define SHARED_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#define N 5  //Number of philosophers

//Message structure used in sending through sockets
struct message{
  int id;
  int port;
  int values;
  int list[N + 1];
  char buffer[1024];
};

struct queueList{
  int id;
  int socketDesc;
};

//Function to send message
void sendMsg(int socketfd, struct message m) {
  char * msg = (char * )&m; //Might have to do this differently, not sure yet.
  int s;
  s = send(socketfd, msg, strlen(msg), 0);
  if(s < 0) {
    perror("ERROR: Message failed to send.\n");
    exit(-1);
  }
  return;
}


//Function to recieve message
struct message recMsg(int socketfd) {
  char * buffer[1024];
  struct message msg;
  int readResult = read(socketfd, buffer, 1023);
  if(readResult < 0) {
    perror("ERROR: Could not receive.\n");
    exit(-1);
  }
  memcpy(&msg, buffer, sizeof(msg)); //Basically, convert buffer back into a msg.

  return msg;
}

//Find largest element in an array
int largest(int arr[], int size){

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

//Function to delete an element from an array
int deleteElement(int arr[], int n, int x)
{
  // Search x in array
  int i;
  for (i=0; i<n; i++)
    if (arr[i] == x)
      break;

  // If x found in array
  if (i < n)
    {
      // reduce size of array and move all
      // elements on space ahead
      n = n - 1;
      for (int j=i; j<n; j++)
        arr[j] = arr[j+1];
    }

  return n;
}

int deleteQueue(struct queueList arr[], int n, int x)
{
  // Search x in array
  int i;
  for (i=0; i<n; i++)
    if (arr[i].id == x)
      break;

  // If x found in array
  if (i < n)
    {
      // reduce size of array and move all
      // elements on space ahead
      n = n - 1;
      for (int j=i; j<n; j++){
        arr[j].id = arr[j+1].id;
        arr[j].socketDesc = arr[j+1].socketDesc;
      }
    }

  return n;
}

//Create a socket
int createSocket() {
  int socketConnection;

  socketConnection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(socketConnection < 0) {
    perror("client: socket creation failed");
    exit(1);
  }

  return socketConnection;
}

//Function to start up the Coordinator process. Passes values in msg.list[] and the port to the process
void startCoordinator(struct message msg, struct message msgID, int value){

  char first[4], second[4], third[4], fourth[4], fifth[4], sixth[4];
  char id[4], port[4];

  sprintf(first, "%d", msg.list[0]);
  sprintf(second, "%d", msg.list[1]);
  sprintf(third, "%d", msg.list[2]);
  sprintf(fourth, "%d", msg.list[3]);
  sprintf(fifth, "%d", msg.list[4]);
  sprintf(sixth, "%d", msg.list[5]);
  sprintf(id, "%d", msgID.id);
  sprintf(port, "%d", (value + 1000));  //Add 1000 to the port to make sure its unique

  if(execl("./coordinator", "coordinator", first, second, third, fourth, fifth, sixth, id, port, (char *)NULL) < 0){
    perror("Error on execl of ./coordinator");
    exit(EXIT_FAILURE);
  }

}

//Function to start up the Dining Philosophers process. Passes the processes ID and the Coordinator's port to the program
void startPhilosopher(struct message msg, int value){

  char id[4], port[4];
  sprintf(id, "%d", msg.id);
  sprintf(port, "%d", (value + 2024));  //Add 2024 to match the Coordinator's port

  if(execl("./diningPhilosophers", "diningPhilosophers", id, port, (char *)NULL) < 0){
    perror("Error on execl of ./diningPhilosophers");
    exit(EXIT_FAILURE);
  }

}

#endif

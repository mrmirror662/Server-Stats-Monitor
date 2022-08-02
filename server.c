#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include <unistd.h>

const char *header = "#loadinfo 12345";

typedef struct Load
{
   float avg1, avg5, avg15;
   int thread, totalthread;
} Load;
Load getLoadInfo()
{
   float avg1, avg5, avg15;
   int threads, totalthreads, garbage;
   FILE *f = fopen("/proc/loadavg", "r");
   fscanf(f, "%f %f %f %d/%d %d", &avg1, &avg5, &avg15, &threads, &totalthreads, &garbage);
   Load l;
   l.avg15 = avg15;
   l.avg5 = avg5;
   l.avg1 = avg1;
   l.thread = threads;
   l.totalthread = totalthreads;
   return l;
}

int main(int argc, char *argv[])
{
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n;

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0)
   {
      perror("ERROR opening socket");
      exit(1);
   }

   /* Initialize socket structure */
   bzero((char *)&serv_addr, sizeof(serv_addr));
   portno = 18000;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR on binding");
      exit(1);
   }

   /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
   while (1)
   {
      listen(sockfd, 5);
      clilen = sizeof(cli_addr);

      /* Accept actual connection from the client */
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      Load l = getLoadInfo();
      if (newsockfd < 0)
      {
         perror("ERROR on accept");
         exit(1);
      }

      /* If connection is established then start communicating */
      bzero(buffer, 256);
      n = read(newsockfd, buffer, 255);

      if (n < 0)
      {
         perror("ERROR reading from socket");
         exit(1);
      }

      int res = strcmp(buffer, header);
      /* Write a response to the client */
      if (res == 0)
      {
         printf("\n***load header found***\n");
         char msg[128];
         memset(msg, 0, sizeof(msg));
         sprintf(msg, "%s[%f,%f,%f,%d,%d]\n", header, l.avg1, l.avg5, l.avg15, l.thread, l.totalthread);
         n = write(newsockfd, msg, strlen(msg));

         if (n < 0)
         {
            perror("ERROR writing to socket");
            //     exit(1);
         }
      }
   }
   close(newsockfd);
   close(sockfd);
   return 0;
}

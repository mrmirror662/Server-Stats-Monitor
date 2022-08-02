#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "util.h"
#include "load.h"

#define PORT 18000
#define SA struct sockaddr_in

int sendMsg(char *ip, char *msg, char *buffer, size_t N)
{

    int sock = 0, valread, client_fd;
    SA serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 50000;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof timeout) < 0)
        perror("setsockopt failed\n");

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                   sizeof timeout) < 0)
        perror("setsockopt failed\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        printf(
            "Invalid address/ Address not supported \n");
        return -1;
    }
    if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr,
                             sizeof(serv_addr))) < 0)
    {
        printf("Connection Failed \n");
        return -1;
    }
    send(sock, msg, strlen(msg), 0);
    valread = read(sock, buffer, N);

    // closing the connected socket
    close(client_fd);
}
void readFile(char *name, char *buff, int N)
{
    FILE *f = fopen(name, "r+");
    if (f == NULL)
    {
        printf("couldnt open %s\n", name);
        return;
    }
    int i = 0;
    while (1)
    {

        if (i > N - 2)
            break;

        char c = getc(f);
        if (c == EOF)
            break;
        buff[i] = c;
        i++;
    }
    fclose(f);
}
void appendToFile(char *name, char *val)
{
    FILE *f = fopen(name, "a+");
    fprintf(f, "%s\n", val);
    fclose(f);
}
void clearFile(char *name)
{
    FILE *f = fopen(name, "w");
    fclose(f);
}

int main(int argc, char const *argv[])
{

    char *ip_file = "ips";
    char *header = "#loadinfo 12345";

    char buffer[1024] = {0};

    const int N = 1024;
    char buff[N];
    readFile(ip_file, buff, N);
    int op;
    printf("*options*\n-add ips(1)-\n-list ip(2)-\n-clear ips(3)-\n-ping servers(4)-\nenter op:");
    scanf(" %d", &op);
    switch (op)
    {
    case 1:
    {
        printf("enter ip to be inserted:");
        char ip[32];

        scanf("%s", ip);
        appendToFile(ip_file, ip);
    }
    break;
    case 2:

        printf("%s\n", buff);
        break;
    case 3:
        clearFile(ip_file);
        break;
    case 4:
    {
        FILE *f = fopen(ip_file, "r+");
        char *str = buff;
        while (1)
        {
            if (strlen(str) < 1)
                break;

            char ip[32] = {0};
            int done = 0;
            int count = 0;
            for (int i = 0;; i++)
            {
                if (str[i] == '\0')
                {
                    done = 1;
                    break;
                }
                if (str[i] == '\n')
                {
                    ip[i] = '\0';
                    break;
                }
                ip[i] = str[i];

                count++;
            }
            if (done)
                break;
            str = str + count + 1;
            printf("ip:%s\n", ip);
            if (strlen(ip) < 1)
                break;

            if (sendMsg(ip, header, buffer, 1024) < 0)
                continue;

            Load l;
            sscanf(buffer, "#loadinfo 12345[%f,%f,%f,%d,%d]\n", &l.avg1, &l.avg5, &l.avg15, &l.thread, &l.totalthread);
            printLoad(l);
        }
        fclose(f);
    }
    break;
    default:
        printf("invalid selection!\n");
        break;
    }
    return 0;
}
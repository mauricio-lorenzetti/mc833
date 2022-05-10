#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define PROTOCOL 0
#define PORT 9002

int inet_addr(char[256]);

struct sockaddr_in server_address;

void cli(int network_socket) {

    char buff[1024], buff_in[1024];
    int n, r;

    bzero(buff_in, sizeof(buff_in));
    read(network_socket, buff_in, sizeof(buff_in));
    printf("%s", buff_in);

    while (1) {

        bzero(buff_in, sizeof(buff_in));
        r = read(network_socket, buff_in, sizeof(buff_in));
        printf("%s", buff_in);

        bzero(buff, sizeof(buff));
        n = 0;
        
        while ((buff[n++] = getchar()) != '\n');
        write(network_socket, buff, sizeof(buff));
    }

}

int main() {

    int network_socket;

    network_socket = socket(AF_INET, SOCK_STREAM, PROTOCOL);

    if (network_socket == -1) {
        printf("socket creation failed...\n");
        return(0);
    }
    else {
        printf("socket successfully created..\n");
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) != 0) {
        printf("connection failed...\n");
        return(0);
    }
    else {
        printf("successfully connected..\n");
    }

    cli(network_socket);

    close(network_socket);

    return 0;
}


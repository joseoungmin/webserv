#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("usage: %s <message>\n", argv[0]);
        exit(1);
    }

    int len;
    string message;
    char line[256];
	int sd = socket(AF_INET, SOCK_STREAM, 0);

// set up the address structure
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4242);
// end of address structure setup

// "localhost" to IP address conversion
    struct addrinfo *res;
    int result = getaddrinfo ("localhost", NULL, NULL, &res);
    if (result == 0) {
        memcpy(&(server_addr.sin_addr), &((struct sockaddr_in *) res->ai_addr)->sin_addr,sizeof(struct in_addr));
        freeaddrinfo(res);
    }
	if(result != 0)
		inet_pton(PF_INET, "localhost", &(server_addr.sin_addr));
	else
		server_addr.sin_addr.s_addr = INADDR_ANY;
// end of "localhost" to IP address conversion

	if (sd < 0) {
		perror("socket() failed");
		exit(-1);
	}
    if (connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
        message = argv[1];
        
		write(sd ,(char *)message.c_str(), message.size());
        
		printf("sent - %s\n", message.c_str());
        len = read(sd ,line, sizeof(line));
        line[len] = '\0';
        printf("received - %s\n", line);
    }
	else {
		perror("Could not connect to server");
	}
    exit(0);
}
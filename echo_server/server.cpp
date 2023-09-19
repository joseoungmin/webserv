#include <stdio.h>
#include <cstdlib>
#include "tcpacceptor.hpp"
#include <iostream>


int main(int argc, char** argv)
{
    if (argc < 2 || argc > 4) {
        printf("usage: server <port> [<ip>]\n");
        exit(1);
    }

    fd_set	readfds;
    int 	clientSockets[MAX_CLIENTS] = {0, }; // 클라이언트 소켓 배열
	char	buffer[BUFFER_SIZE];
	int		clientSocket;

    TCPStream* stream = NULL;
    TCPAcceptor* acceptor = NULL;
    if (argc == 3)
        acceptor = new TCPAcceptor(atoi(argv[1]), argv[2]);
    else 
        acceptor = new TCPAcceptor(atoi(argv[1]));

    if (acceptor->start() == 0) {
		int maxSocket = acceptor->get_sd();
        while (1) {
			FD_ZERO(&readfds);
        	FD_SET(acceptor->get_sd(), &readfds); // 서버 소켓을 모니터링 대상으로 추가
        	for (int i = 0; i < MAX_CLIENTS; ++i) {
        	    if (clientSockets[i] > 0) {
        	        FD_SET(clientSockets[i], &readfds); // 클라이언트 소켓을 모니터링 대상으로 추가
        	    }
        	    if (clientSockets[i] > maxSocket) {
        	        maxSocket = clientSockets[i];
        	    }
        	}
        	int activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        	if (activity == -1) {
        	    perror("select");
        	    exit(EXIT_FAILURE);
        	}

 			if (FD_ISSET(acceptor->get_sd(), &readfds)) {
        	    // 클라이언트 연결 수락
        	    if ((clientSocket = accept(acceptor->get_sd(), NULL, NULL)) < 0) {
        	        perror("accept");
        	        exit(EXIT_FAILURE);
        	    }

        	    // 클라이언트 소켓을 배열에 추가
        	    for (int i = 0; i < MAX_CLIENTS; ++i) {
        	        if (clientSockets[i] == 0) {
        	            clientSockets[i] = clientSocket;
        	            break;
        	        }
        	    }
        	}

        // 클라이언트와의 통신
        	for (int i = 0; i < MAX_CLIENTS; ++i) {
        	    if (clientSockets[i] > 0 && FD_ISSET(clientSockets[i], &readfds)) {
        	        int bytesRead = recv(clientSockets[i], buffer, BUFFER_SIZE, 0);
	
					if (bytesRead <= 0) {
        	            // 클라이언트 연결 종료 또는 오류 발생
        	            close(clientSockets[i]);
        	            clientSockets[i] = 0;
        	        } else {
        	            // 클라이언트로부터 데이터를 받아서 처리
        	            buffer[bytesRead] = '\0';
        	            std::cout << "stcket id - " << clientSockets[i] << " rcv_len - "<< strlen(buffer) << " received - \"" << buffer << "\"" << std::endl;
						write(clientSockets[i], buffer, strlen(buffer));
        	        }
				delete stream;
            	}
        	}
        }
    }
    perror("Could not start the server");
    exit(-1);
}
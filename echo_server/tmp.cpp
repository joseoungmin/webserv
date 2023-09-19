#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>

const int PORT = 4242;
const int MAX_CLIENTS = 5; // 동시에 처리할 클라이언트 수
const int BUFFER_SIZE = 42;

int main() {
    int serverSocket, clientSocket, maxSocket;
    fd_set readfds;
    int clientSockets[MAX_CLIENTS] = {0}; // 클라이언트 소켓 배열
    char buffer[BUFFER_SIZE];

    // 서버 소켓 생성 및 설정
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    maxSocket = serverSocket;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds); // 서버 소켓을 모니터링 대상으로 추가
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

        if (FD_ISSET(serverSocket, &readfds)) {
            // 클라이언트 연결 수락
            if ((clientSocket = accept(serverSocket, NULL, NULL)) < 0) {
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
                    std::cout << clientSockets[i] << "클라이언트 메시지: " << buffer << std::endl;
                }
            }
        }
    }

    // 서버 소켓 닫기
    close(serverSocket);

    return 0;
}

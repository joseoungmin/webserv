#ifndef TCPACCEPTOR_HPP
#define TCPACCEPTOR_HPP

#include <string>
#include <netinet/in.h>
#include "tcpstream.hpp"

using namespace std;

class TCPAcceptor
{
    int    m_lsd;
    string m_address;
    int    m_port;
    bool   m_listening;

  public:
    TCPAcceptor(int port, const char* address="");
    ~TCPAcceptor();

    int        start();
    TCPStream* accept();
    	int			get_sd() { return m_lsd; }

  private:
    TCPAcceptor() {}
};

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024


TCPAcceptor::TCPAcceptor(int port, const char* address)
    : m_lsd(0), m_port(port), m_address(address), m_listening(false) {}

TCPAcceptor::~TCPAcceptor()
{
    if (m_lsd > 0) {
        close(m_lsd);
    }
}

int TCPAcceptor::start()
{
    if (m_listening == true) {
        return 0;
    }

    m_lsd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(m_port);
    address.sin_addr.s_addr = INADDR_ANY;

    //int optval = 1;
    //setsockopt(m_lsd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    int result = bind(m_lsd, (struct sockaddr*)&address, sizeof(address));
    if (result != 0) {
        perror("bind() failed");
        return result;
    }
    result = listen(m_lsd, MAX_CLIENTS);
    if (result != 0) {
        perror("listen() failed");
        return result;
    }
    m_listening = true;
    return result;
}

TCPStream* TCPAcceptor::accept()
{
    if (m_listening == false) {
        return NULL;
    }

    struct sockaddr_in address;
    socklen_t len = sizeof(address);
    memset(&address, 0, sizeof(address));
    int sd = ::accept(m_lsd, (struct sockaddr*)&address, &len);
    if (sd < 0) {
        perror("accept() failed");
        return NULL;
    }
    return new TCPStream(sd, &address);
}

#endif // !
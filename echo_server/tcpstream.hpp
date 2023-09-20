#ifndef TCPSTREAM_HPP
#define TCPSTREAM_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string>


class TCPStream
{
	int			m_clientSocket;
	std::string	m_peerIP;
	int			m_peerPort;

public:
	friend class TCPAcceptor;
	
	~TCPStream();
	
	ssize_t 	send(char* buffer, size_t len);
	ssize_t 	receive(char* buffer, size_t len);
	int			get_sd() { return m_clientSocket; }
	//std::string getPeerIP();
	//int 		getPeerPort();

private:
	TCPStream(int sd, struct sockaddr_in* address);
	TCPStream();
	TCPStream(const TCPStream& stream);
};

#include <arpa/inet.h>

TCPStream::TCPStream(int sd, struct sockaddr_in* address) : m_clientSocket(sd) {
    char ip[50];
    inet_ntop(PF_INET, (struct in_addr*)&(address->sin_addr.s_addr),
              ip, sizeof(ip)-1);
    m_peerIP = ip;
    m_peerPort = ntohs(address->sin_port);
}

TCPStream::~TCPStream()
{
    close(m_clientSocket);
}

ssize_t	TCPStream::send(char* buffer, size_t len)
{
	return write(m_clientSocket, buffer, len);
}

ssize_t	TCPStream::receive(char* buffer, size_t len)
{
	return read(m_clientSocket, buffer, len);
}

#endif
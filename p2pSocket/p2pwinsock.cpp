#include "p2psock.hpp"
#include <vector>
#include <stack>

#include "winsock2.h"
#include "ws2tcpip.h"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27117"

namespace p2p {
    std::vector<SOCKET*> sockets;
    std::stack<int> freedSockets;

    WSADATA wsaData;
	addrinfo hints;
    addrinfo* result, *ptr;

    void initiate() {
        if(WSAStartup(MAKEWORD(2,2), &wsaData))
            throw("P2P: Failed To Initialize WSAStartup 2.2, WinSock 2.2 May Not Be Supported.");
    }
    
    void shutdownNetwork() {
        freeaddrinfo(result);
        freeaddrinfo(ptr);
        
        // Loop Through List and Close All Sockets Via destroySocket():
        for(unsigned int i = 0; i < sockets.size() - 1; ++i)
            destroySocket(i);
        
        WSACleanup();
    }

	void closeConnection(const int &socketHandle, p2pFlags flags) {
		if(flags & P2P_SEND && !(flags & P2P_RECV))
			shutdown(*sockets[socketHandle], SD_SEND); // P2P_SEND
		else if(flags & P2P_RECV && !(flags & P2P_SEND))
			shutdown(*sockets[socketHandle], SD_RECEIVE); // P2P_SEND
		else if(flags & P2P_SEND && flags & P2P_RECV) {
			shutdown(*sockets[socketHandle], SD_BOTH);
		}
		else
			throw("P2P: Attempted to Close Socket Connection Without Passing Valid Flags.");
	}
    
    int createSocket(p2pFlags flags) {
        // Handle Flags:
        if(!(flags & P2P_CLIENT) && !(flags & P2P_HOST))
            throw("P2P: Missing Initialization Flags - Declare Whether Client or Host.");
        
        // Check For Desired IPV# in Flags:
        if(flags & P2P_IPV4 && !(flags & P2P_IPV6))
            hints.ai_family = AF_INET; // IPV4
        else if(flags & P2P_IPV6 && !(flags & P2P_IPV4))
            hints.ai_family = AF_INET6; // IPV6
        else if(!(flags & P2P_HOST))
            hints.ai_family = AF_UNSPEC; // Either IPV4 or IPV6 is Allowed (Client Only).
        else
            throw("P2P: Host Must Supply a Single IPV# Flag.");
        
        // Check For Desired Protocol, Default to TCP:
        if((flags & P2P_TCP || !(flags & P2P_TCP)) && !(flags & P2P_UDP)) {
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_socktype = SOCK_STREAM;
        }
        else if(flags & P2P_UDP && !(flags & P2P_TCP)) {
            hints.ai_protocol = IPPROTO_UDP;
            hints.ai_socktype = SOCK_DGRAM;
        } else
            throw("P2P: Both Internet Protocols Were Passed.");
        
        // Set Host AI_PASSIVE:
        if(flags & P2P_HOST)
            hints.ai_flags = AI_PASSIVE;

        // Add New Socket:
        int hndl;
        if(!freedSockets.empty()) {
			// Use a Free Handle in vector::sockets:
            hndl = freedSockets.top();
			freedSockets.pop();
            sockets[hndl] = new SOCKET;
        } else {
            sockets.push_back(new SOCKET);
            hndl = sockets.size() - 1;
        }

        *sockets[hndl] = INVALID_SOCKET;
        return hndl;
    }
    
    void destroySocket(const int &socketHandle) {
        closesocket(*sockets[socketHandle]);
        delete sockets[socketHandle];
        sockets[socketHandle] = NULL;
        freedSockets.push(socketHandle); // Push Handle To Stack For Reuse.
    }
    
    void socketBind(const int &socketHandle) {
        getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
        *sockets[socketHandle] = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // Setup Socket.
        bind(*sockets[socketHandle], result->ai_addr, (int)result->ai_addrlen); // Bind Socket.
        freeaddrinfo(result);
    }

    void socketListen(const int &socketHandle) {
        if(listen(*sockets[socketHandle], SOMAXCONN) == SOCKET_ERROR)
            throw("P2P: SOCKET_ERROR in socketListen function.");
    }

    int socketAccept(const int &socketHandle) {
        // Add New Socket:
        int hndl;
        if(!freedSockets.empty()) {
            hndl = freedSockets.top();
			freedSockets.pop();
            sockets[hndl] = new SOCKET;
        } else {
            sockets.push_back(new SOCKET);
            hndl = sockets.size() - 1;
        }

        *sockets[hndl] = INVALID_SOCKET;
        *sockets[hndl] = accept(*sockets[socketHandle], NULL, NULL);
        return hndl;
    }
    
    void socketConnect(const int &socketHandle, const char* ipAddress) {
        getaddrinfo(ipAddress, DEFAULT_PORT, &hints, &result); // Definitely Client, Pass IP.
        *sockets[socketHandle] = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // Setup Socket.
        
        // Attempt to connect to an address until one succeeds
        for(ptr = result; ptr != NULL; ptr = ptr->ai_next) {
            // Create a SOCKET for connecting to server
            *sockets[socketHandle] = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (*sockets[socketHandle] == INVALID_SOCKET)
                throw("P2P: Failure to Validate Socket in socketConnect.");

            // Connect to server.
            if(connect(*sockets[socketHandle], ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
                closesocket(*sockets[socketHandle]);
                *sockets[socketHandle] = INVALID_SOCKET;
                continue;
            }
            break;
        }
        
        freeaddrinfo(result);
    }

	int read(const int &socketHandle, char* byteArray, const int &len) {
		return recv(*sockets[socketHandle], byteArray, len, 0);
	}

	void write(const int &socketHandle, const char* byteArray, const int &len) {
		if(send(*sockets[socketHandle], byteArray, len, 0) == SOCKET_ERROR)
			throw("P2P: SOCKET_ERROR in write function.");
	}
}
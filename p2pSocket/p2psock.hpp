#ifndef P2PSOCKET_HPP
#define P2PSOCKET_HPP

namespace p2p {
    enum p2pFlags {
        P2P_NULL    = 0x00, // 0000 0000 0000 0000
        P2P_IPV4    = 0x01, // 0000 0000 0000 0001
        P2P_IPV6    = 0x02, // 0000 0000 0000 0010
        P2P_TCP     = 0x04, // 0000 0000 0000 0100
        P2P_UDP     = 0x08, // 0000 0000 0000 1000
        P2P_HOST    = 0x10, // 0000 0000 0001 0000
        P2P_CLIENT  = 0x20, // 0000 0000 0010 0000
        P2P_SEND	= 0x40, // 0000 0000 0100 0000
        P2P_RECV	= 0x80  // 0000 0001 0000 0000
    };

    // Needed For Bitwise ORing (|) p2pFlags Together.
    inline p2pFlags operator |(const p2pFlags a, const p2pFlags b) {
        return static_cast<p2pFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    void initiate(); // Initiates APIs.
    void shutdownNetwork(); // Shuts down APIs and clears sockets/flushes buffers.
    void closeConnection(const int &socketHandle, p2pFlags flags = P2P_NULL); // Closes Specified Network Sub-Systems.
    int createSocket(p2pFlags flags = P2P_NULL); // Returns integer ID handle to socket.
    void destroySocket(const int &socketHandle);
    void socketBind(const int &socketHandle);
    void socketListen(const int &socketHandle);
    int socketAccept(const int &socketHandle);
    void socketConnect(const int &socketHandle, const char* ipAddress);

    int read(const int &socketHandle, char* byteArray, const int &len);
    void write(const int &socketHandle, const char* byteArray, const int &len);
}

#endif
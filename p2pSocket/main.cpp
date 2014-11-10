#include <iostream>
#include "p2psock.hpp"
     
#define CLIENT
#ifndef CLIENT
#include <string>
int main(int argc, char* argv) {
        try     {
                char* snt;
                char c[4];
				std::string text;
     
                p2p::initiate();
                int handle = p2p::createSocket(p2p::P2P_IPV4 | p2p::P2P_TCP | p2p::P2P_CLIENT);
                p2p::socketConnect(handle, "71.83.101.177");
                bool sentinel = true;
     
                while(sentinel) {

						std::cin >> text;
                        unsigned int length = text.length();

						snt = new(char[length + 1]);

						for(unsigned int counter = 0; counter < length; ++counter) {
							snt[counter] = text[counter];
						}

						snt[length] = '\0';

                        for(int it = 0; it < 4; it += 1) {
                                c[it] = char(0xFF & (length >> (it * 8)));
                        }
     
     
                        p2p::write(handle, c, 4);
                        p2p::write(handle, snt, strlen(snt));
                        if(snt == "quit")
							sentinel = false;

						delete snt;
						text.clear();
                }
     
                p2p::shutdownNetwork();
     
        }
        catch(const char* e) {
                std::cout << e << "\n";
                p2p::shutdownNetwork();
        }
    return 0;
}
#else
int main(int argc, char* argv) {
    try {
        char* snt;
        char* c;
        unsigned int i = 0x00000000;
     
        p2p::initiate();
        int temp = p2p::createSocket(p2p::P2P_IPV4 | p2p::P2P_TCP | p2p::P2P_HOST);
        p2p::socketBind(temp);
        p2p::socketListen(temp);
        int handle = p2p::socketAccept(temp);
        p2p::destroySocket(temp);
        bool sentinel = true;
		std::cout << "Connected - P2P System Initiated...." << std::endl;

        while(sentinel) {
			// Set i equal to the number of chars in next string:
			c = new(char[4]);
            p2p::read(handle, c, 4);
            for(int it = 3; it > -1; it -= 1) {
                    i |= unsigned int(c[it]);        // possible type cast needed?
                    i <<= (it * 8);
            }


			std::cout << "Read " << i << " Bytes" << std::endl;
			snt = new(char[i + 1]);
            p2p::read(handle, snt, i);
			snt[i] = '\0';
            std::cout << snt << std::endl;;
     
            if(snt == "quit") {
                    sentinel = false;
            }

			delete c;
			delete snt;
			i = 0;
        }
     
        p2p::shutdownNetwork();
        return 0;
    }
    catch(const char* e) {
		std::cout << e << std::endl;
        p2p::shutdownNetwork();
    }
}
#endif
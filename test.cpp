#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <cstring>

const int PORT = 12345;
const int BUFFER_SIZE = 256;

void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

void cleanup(SDLNet_SocketSet socketSet, UDPsocket udpSocket) {
    SDLNet_FreeSocketSet(socketSet);
    SDLNet_UDP_Close(udpSocket);
    SDLNet_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initSDL();

    // Initialize SDL_net
    if (SDLNet_Init() < 0) {
        std::cerr << "SDLNet_Init Error: " << SDLNet_GetError() << std::endl;
        cleanup(nullptr, nullptr);
        return 1;
    }

    // Create a UDP socket
    UDPsocket udpSocket = SDLNet_UDP_Open(0);
    if (!udpSocket) {
        std::cerr << "SDLNet_UDP_Open Error: " << SDLNet_GetError() << std::endl;
        cleanup(nullptr, udpSocket);
        return 1;
    }

    // Set up the broadcast address
    IPaddress broadcastAddress;
    if (SDLNet_ResolveHost(&broadcastAddress, "192.168.178.255", PORT) < 0) {
        std::cerr << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << std::endl;
        cleanup(nullptr, udpSocket);
        return 1;
    }

    // Create a datagram to send
    UDPpacket* packet = SDLNet_AllocPacket(BUFFER_SIZE);
    if (!packet) {
        std::cerr << "SDLNet_AllocPacket Error: " << SDLNet_GetError() << std::endl;
        cleanup(nullptr, udpSocket);
        return 1;
    }

    // Set the data for the broadcast message
    strcpy(reinterpret_cast<char*>(packet->data), "DISCOVER");
    packet->len = strlen(reinterpret_cast<char*>(packet->data));

    // Broadcast the message
    packet->address = broadcastAddress;  // Set the destination address for the packet
    if (SDLNet_UDP_Send(udpSocket, -1, packet) == 0) {
        std::cerr << "SDLNet_UDP_Send Error: " << SDLNet_GetError() << std::endl;
        SDLNet_FreePacket(packet);
        cleanup(nullptr, udpSocket);
        return 1;
    }

    std::cout << "Broadcast message sent. Waiting for responses..." << std::endl;

    // Create a socket set for receiving responses
    SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
    if (!socketSet) {
        std::cerr << "SDLNet_AllocSocketSet Error: " << SDLNet_GetError() << std::endl;
        SDLNet_FreePacket(packet);
        cleanup(socketSet, udpSocket);
        return 1;
    }

    SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(udpSocket));

    // Wait for responses (timeout: 5 seconds)
    if (SDLNet_CheckSockets(socketSet, 5000) > 0) {
        if (SDLNet_SocketReady(udpSocket)) {
            // Prepare a packet for receiving
            UDPpacket* recvPacket = SDLNet_AllocPacket(BUFFER_SIZE);
            if (!recvPacket) {
                std::cerr << "SDLNet_AllocPacket Error: " << SDLNet_GetError() << std::endl;
                SDLNet_FreeSocketSet(socketSet);
                SDLNet_FreePacket(packet);
                cleanup(socketSet, udpSocket);
                return 1;
            }

            // Receive a packet
            if (SDLNet_UDP_Recv(udpSocket, recvPacket)) {
                std::cout << "Received response from " 
                          << SDLNet_Read32(&recvPacket->address.host) << ":" 
                          << SDLNet_Read16(&recvPacket->address.port) << std::endl;
                std::cout << "Message: " << recvPacket->data << std::endl;
            } else {
                std::cerr << "SDLNet_UDP_Recv Error: " << SDLNet_GetError() << std::endl;
            }

            SDLNet_FreePacket(recvPacket);
        } else {
            std::cout << "No response received within the timeout period." << std::endl;
        }
    } else {
        std::cout << "No responses within the timeout period." << std::endl;
    }

    // Clean up
    SDLNet_FreePacket(packet);
    cleanup(socketSet, udpSocket);

    return 0;
}
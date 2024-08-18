#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include <iostream>
#include <csignal>
#include <atomic>

#define PORT 12345
#define BUFFERSIZE 1024


std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    running = false;
}


int main(int argc, char **argv) {
    signal(SIGINT, signalHandler);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (SDLNet_Init() == -1) {
        std::cout << "SDLNet_Init error: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    //basic echo server

    UDPsocket server;
    UDPpacket *packet;

    server = SDLNet_UDP_Open(PORT);
    if (server == NULL) {
        std::cout << "SDLNet_UDP_Open error: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    packet = SDLNet_AllocPacket(BUFFERSIZE);
    if (packet == NULL) {
        std::cout << "SDLNet_AllocPacket error: " << SDLNet_GetError() << std::endl;
        return -1;
    }


    std::cout << "Server is running on " << PORT << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;

    while (running) {
        if (SDLNet_UDP_Recv(server, packet) > 0) {
            std::cout << "Received packet from " << SDLNet_ResolveIP(&packet->address) << std::endl;
            std::cout << "Data: " << packet->data << std::endl;
            SDLNet_UDP_Send(server, -1, packet);
        }

        SDL_Delay(100);
    }

    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(server);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include <iostream>

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (SDLNet_Init() == -1) {
        std::cout << "SDLNet_Init error: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    //basic echo server

    int port = 12345;
    int bufferSize = 1024;

    UDPsocket server;
    UDPpacket *packet;

    server = SDLNet_UDP_Open(port);
    if (server == NULL) {
        std::cout << "SDLNet_UDP_Open error: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    packet = SDLNet_AllocPacket(bufferSize);
    if (packet == NULL) {
        std::cout << "SDLNet_AllocPacket error: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        if (SDLNet_UDP_Recv(server, packet) > 0) {
            std::cout << "Received packet from " << packet->address.host << std::endl;
            std::cout << "Data: " << packet->data << std::endl;
            SDLNet_UDP_Send(server, -1, packet);
        }

        SDL_Delay(100);
    }


    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
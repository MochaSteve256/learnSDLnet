#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <host> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);

    // Initialize SDL2 and SDL2_net
    if (SDL_Init(0) == -1) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (SDLNet_Init() == -1) {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Resolve host
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, host, port) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Open a UDP socket
    UDPsocket client = SDLNet_UDP_Open(0);  // 0 means let SDLNet choose an available port
    if (!client) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Allocate memory for the packet
    UDPpacket *packet = SDLNet_AllocPacket(BUFFER_SIZE);
    if (!packet) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(client);
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    while (true) {
        // Get input from stdin
        // Get a line of input from the user. The input is stored in the
        // packet->data buffer, which is a char array of size BUFFER_SIZE.
        // The fgets() function reads characters from stdin and stores them
        // in the packet->data buffer until a newline character is found or
        // the buffer is full (whichever comes first). The newline character
        // is included in the buffer. If the user enters more characters than
        // the buffer can hold, the excess characters are left in the input
        // stream.
        printf("Enter message: ");
        if (fgets((char *)packet->data, BUFFER_SIZE, stdin) == NULL) {
            // If fgets() returns NULL, it means an error occurred, such as
            // an end-of-file condition. In this case, we break out of the loop.
            break;
        }

        // Prepare the packet
        packet->len = strlen((char *)packet->data);
        packet->address.host = ip.host; // Set the destination IP
        packet->address.port = ip.port; // Set the destination port

        // Send the packet to the server
        if (SDLNet_UDP_Send(client, -1, packet) == 0) {
            fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
            break;
        }

        // Wait for a response from the server (with timeout)
        Uint32 timeout = SDL_GetTicks() + 5000; // 5 second timeout
        memset(packet->data, 0, BUFFER_SIZE);
        bool rcv = true;
        while (!SDLNet_UDP_Recv(client, packet)) {
            if (SDL_GetTicks() > timeout) {
                fprintf(stderr, "Timeout waiting for server response\n");
                rcv = false;
                break;
            }
            SDL_Delay(100); // Wait a bit before trying again
        }
        if (rcv) {
            printf("Received from server: %s", (char *)packet->data);
        } else {
            fprintf(stderr, "Received empty packet or no response\n");
        }

        // Clear the packet data for the next iteration
        memset(packet->data, 0, BUFFER_SIZE);
    }

    // Clean up
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(client);
    SDLNet_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

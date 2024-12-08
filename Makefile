all:
	.\udp_echo.exe

echo:
	g++ .\udp_echo.cpp -ISDL2/net/include -ISDL2/include -LSDL2/lib -LSDL2/net/lib -lSDL2_net -lSDL2main -lSDL2 -static-libgcc -static-libstdc++ -o .\udp_echo.exe

client:
	g++ .\udp_client.cpp -ISDL2/net/include -ISDL2/include -LSDL2/lib -LSDL2/net/lib -lSDL2_net -lSDL2main -lSDL2 -static-libgcc -static-libstdc++ -o .\udp_client.exe
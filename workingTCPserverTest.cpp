//
// Created by itay on 30/12/2021.
//

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>

void srvmsg(std::string msg) {
    std::cout << "SRV->" << msg << std::endl;
}

int sockets[2];
bool startListening = false;

int settings(int port) {
    // create socket
    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket == -1)
    {
        srvmsg( "Can't create a socket!" );
        return -1;
    }
    // bind socket to ip/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    srvmsg( "my port is " + std::to_string( hint.sin_port) );
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    int binding = bind(mySocket, (sockaddr*)&hint, sizeof(hint));
    if (binding < 0) {
        srvmsg( "Can't bind a socket! (" + std::to_string( binding ) + ")" );
        return -2;
    }
    // telling winsock
    int listening = listen(mySocket, SOMAXCONN);
    if (listening < 0) {
        srvmsg( "Can't listen! (" + std::to_string( listening ) + ")" );
        return -3;
    }
    /*
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];*/
    sockets[0] = mySocket;
    return mySocket;
}

void listenToClients() {
    while(!startListening);

    int mySocket = sockets[0];
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = accept(mySocket, (sockaddr*)&client, &clientSize);
    if (clientSocket < 0) {
        srvmsg( "Can't accept! (" + std::to_string(clientSocket) + ")" );
        return;
    }
    int bufSize = 4096;
    char buf[bufSize];
    srvmsg( "starting listening" );
    std::string clientInput = "";
    while (clientInput != "f"){
        memset(buf, 0, bufSize);
        int byteRcv = recv(clientSocket, buf, bufSize, 0);
        if (byteRcv < 0) {
            srvmsg( "connection issue" );
            break;
        }
        if (byteRcv == 0) {
            srvmsg( "client disconnected" );
            break;
        }
        clientInput = std::string (buf, 0, byteRcv);
        srvmsg("CLIENT:_" + std::string (buf, 0, byteRcv) + "_");
        if (clientInput == "f\n" || clientInput == "f") {
            char okBye[] = "ok bye\n";
            send(clientSocket, okBye, sizeof(okBye), 0);
            break;
        }
        else {
            char gotYou[] = "got you fam\n";
            send(clientSocket, gotYou, sizeof(gotYou), 0);
        }
    }
    sockets[1] = clientSocket;
}

void connect(int port) {
    int socket = settings(port);
    if (socket >= 0) {
        sockets[0] = socket;
        startListening = true;
        srvmsg("started thread ");
    }
}

void disconnect(std::thread* p) {
    if (p != nullptr) p->join();
    close(sockets[0]);
    close(sockets[1]);
    srvmsg( "closed" );
}

int main() {
    /*
     g++ -pthread -o srv.out thServer.cpp
     ./srv.out
     */
    srvmsg( "enter port:");
    int port;
    std::cin >> port;
    std::thread t(listenToClients);
    connect(port);
    srvmsg("living life biatch");
    disconnect(&t);
}

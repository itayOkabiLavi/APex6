/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <sys/socket.h>
#include <thread>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};


// you can add helper classes here and implement on the cpp file


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler{
	public:
    virtual void handle(int clientID){
        cout<< "!!!handling " << clientID << " !!!" << endl;
    }
};


// implement on Server.cpp
class Server {
	thread* myThread; // the thread to run the start() method in
    int myPort;
	// you may add data members
public:
	Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
	void stop();
};

#endif /* SERVER_H_ */

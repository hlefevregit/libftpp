/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 11:37:45 by hulefevr          #+#    #+#             */
/*   Updated: 2025/10/14 12:03:12 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "message.hpp"
#include "client.hpp"
#include <unordered_map>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>     
#include <sys/select.h> 
#include <sys/socket.h>
#include <iostream>
#include <cstring>

class Server {
public:
	Server();
	~Server();

	void start(const size_t& p_port);
	void defineAction(const Message::Type& messageType, const std::function<void(long long& clientID, const Message& msg)>& action);
	void sendTo(const Message& message, long long clientID);
	void sendToArray(const Message& message, std::vector<long long> clientIDs);
	void sendToAll(const Message& message);
	void update();
	void stop();
private:
	int _listeningSocket;
	std::atomic<bool> _isRunning;
	std::thread _updateThread;
	std::mutex _clientsMutex;
	std::unordered_map<long long, int> _clients; // clientID -> socket
	std::mutex _actionsMutex;
	std::unordered_map<Message::Type, std::function<void(long long& clientID, const Message&
		 msg)>> _actions; // messageType -> action
	long long _nextClientID;
    std::vector<std::pair<long long, Message>> _pendingMessages;
	
	
	void acceptNewClient();
	void receiveFromClient(long long clientID, int clientSocket);
	void updateLoop();
	void closeClient(long long clientID);
	bool setNonBlocking(int socket);
		
};
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 11:46:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/10/14 12:04:04 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

Server::Server() : _listeningSocket(-1), _isRunning(false), _nextClientID(1) {}

Server::~Server() {
	stop();
}

void Server::start(const size_t& p_port) {
	if (_isRunning.load()) {
		std::cerr << "Server is already running." << std::endl;
		return;
	}

	_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listeningSocket < 0) {
		std::cerr << "Failed to create socket." << std::endl;
		return;
	}

	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(p_port);

	if (bind(_listeningSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		std::cerr << "Failed to bind socket." << std::endl;
		close(_listeningSocket);
		_listeningSocket = -1;
		return;
	}

	if (listen(_listeningSocket, SOMAXCONN) < 0) {
		std::cerr << "Failed to listen on socket." << std::endl;
		close(_listeningSocket);
		_listeningSocket = -1;
		return;
	}

	if (!setNonBlocking(_listeningSocket)) {
		std::cerr << "Failed to set listening socket to non-blocking." << std::endl;
		close(_listeningSocket);
		_listeningSocket = -1;
		return;
	}

	_isRunning.store(true);
	_updateThread = std::thread(&Server::updateLoop, this);
	std::cout << "Server started on port " << p_port << "." << std::endl;
}

void Server::defineAction(const Message::Type& messageType, const std::function<void(long long& clientID, const Message& msg)>& action) {
	std::lock_guard<std::mutex> lock(_actionsMutex);
	_actions[messageType] = action;  // ✅ Déjà correct avec Message::Type
}

void Server::sendTo(const Message& message, long long clientID) {
	std::lock_guard<std::mutex> lock(_clientsMutex);
	auto it = _clients.find(clientID);
	if (it != _clients.end()) {
		int clientSocket = it->second;
		const std::vector<uint8_t>& data = message.getData();
		size_t totalSent = 0;
		while (totalSent < data.size()) {
			ssize_t sent = send(clientSocket, reinterpret_cast<const char*>(data.data() + totalSent), data.size() - totalSent, 0);
			if (sent <= 0) {
				std::cerr << "Failed to send message to client " << clientID << "." << std::endl;
				closeClient(clientID);
				return;
			}
			totalSent += sent;
		}
	} else {
		std::cerr << "Client " << clientID << " not found." << std::endl;
	}
}

void Server::sendToArray(const Message& message, std::vector<long long> clientIDs) {
	for (long long clientID : clientIDs) {
		sendTo(message, clientID);
	}
}

void Server::sendToAll(const Message& message) {
	std::lock_guard<std::mutex> lock(_clientsMutex);
	for (const auto& pair : _clients) {
		sendTo(message, pair.first);
	}
}

void Server::update() {
    std::vector<std::pair<long long, Message>> toProc;
    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        toProc.swap(_pendingMessages);
        _pendingMessages.clear();
    }

    for (auto& process : toProc) {
        long long clientID = process.first;
        Message& msg = process.second;

        std::function<void(long long&, const Message&)> action;
        {
            std::lock_guard<std::mutex> lock(_actionsMutex);
            // ✅ FIX: Utiliser getType() au lieu de type() pour cohérence
            auto it = _actions.find(msg.getType());
            if (it != _actions.end()) {
                action = it->second;
            }
        }

        if (action) {
            action(clientID, msg);
        } else {
            std::cerr << "No action defined for message type " << msg.getType() << "." << std::endl;
        }
    }
}

void Server::stop() {
	if (!_isRunning.load()) {
		return;
	}

	_isRunning.store(false);
	if (_updateThread.joinable()) {
		_updateThread.join();
	}

	{
		std::lock_guard<std::mutex> lock(_clientsMutex);
		for (const auto& pair : _clients) {
			close(pair.second);
		}
		_clients.clear();
	}

	if (_listeningSocket >= 0) {
		close(_listeningSocket);
		_listeningSocket = -1;
	}

	std::cout << "Server stopped." << std::endl;
}

void Server::acceptNewClient() {
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(_listeningSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			std::cerr << "Failed to accept new client." << std::endl;
		}
		return;
	}

	if (!setNonBlocking(clientSocket)) {
		std::cerr << "Failed to set client socket to non-blocking." << std::endl;
		close(clientSocket);
		return;
	}

	long long clientID = _nextClientID++;
	{
		std::lock_guard<std::mutex> lock(_clientsMutex);
		_clients[clientID] = clientSocket;
	}
	std::cout << "New client connected with ID " << clientID << "." << std::endl;
}

bool Server::setNonBlocking(int socket) {
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1) {
		return false;
	}
	if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		return false;
	}
	return true;
}

void Server::receiveFromClient(long long clientID, int clientSocket) {
	uint8_t buffer[4096];
	ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		std::vector<uint8_t> data(buffer, buffer + bytesRead);
		size_t offset = 0;

		while (offset + Message::HEADER_SIZE <= data.size()) {
			int msgType;
			uint32_t msgSize;
			std::memcpy(&msgType, data.data() + offset, sizeof(int));
			std::memcpy(&msgSize, data.data() + offset + sizeof(int), sizeof(uint32_t));
			msgSize = ntohl(msgSize);

			if (offset + Message::HEADER_SIZE + msgSize > data.size()) {
				break;
			}
			Message msg;
			msg.setType(msgType);
			msg.appendData(data.data() + offset + Message::HEADER_SIZE, msgSize);

			{
				std::lock_guard<std::mutex> lock(_clientsMutex);
				_pendingMessages.emplace_back(clientID, msg);
			}
			offset += Message::HEADER_SIZE + msgSize;
		}
	} else if (bytesRead == 0) {
		std::cout << "Client " << clientID << " disconnected." << std::endl;
		closeClient(clientID);
	} else {
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			std::cerr << "Failed to receive data from client " << clientID << "." << std::endl;
			closeClient(clientID);
		}
	}
}

void Server::updateLoop() {
	while (_isRunning.load()) {
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(_listeningSocket, &readfds);
		int maxfd = _listeningSocket;

		{
			std::lock_guard<std::mutex> lock(_clientsMutex);
			for (const auto& pair : _clients) {
				FD_SET(pair.second, &readfds);
				if (pair.second > maxfd) {
					maxfd = pair.second;
				}
			}
		}

		timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int activity = select(maxfd + 1, &readfds, nullptr, nullptr, &timeout);
		if (activity < 0) {
			if (errno != EINTR) {
				std::cerr << "Select error." << std::endl;
			}
			continue;
		}

		if (FD_ISSET(_listeningSocket, &readfds)) {
			acceptNewClient();
		}

		std::vector<long long> clientsToReceive;
		{
			std::lock_guard<std::mutex> lock(_clientsMutex);
			for (const auto& pair : _clients) {
				if (FD_ISSET(pair.second, &readfds)) {
					clientsToReceive.push_back(pair.first);
				}
			}
		}

		for (long long clientID : clientsToReceive) {
			int clientSocket;
			{
				std::lock_guard<std::mutex> lock(_clientsMutex);
				auto it = _clients.find(clientID);
				if (it != _clients.end()) {
					clientSocket = it->second;
				} else {
					continue;
				}
			}
			receiveFromClient(clientID, clientSocket);
		}
	}
}

void Server::closeClient(long long clientID) {
	std::lock_guard<std::mutex> lock(_clientsMutex);
	auto it = _clients.find(clientID);
	if (it != _clients.end()) {
		close(it->second);
		_clients.erase(it);
		std::cout << "Closed connection with client " << clientID << "." << std::endl;
	}
}


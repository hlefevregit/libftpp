/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 20:18:11 by hulefevr          #+#    #+#             */
/*   Updated: 2025/10/10 12:59:53 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "message.hpp"

#include <unistd.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

class Client {
public:
	Client() : Client(-1, "127.0.0.1", 0) {}
	Client(int socket_fd, const std::string& ip_address, uint16_t port);
	~Client();

	int getSocketFd() const;
	std::string getIpAddress() const;

	void connect(const std::string& address, const size_t& port);
	void disconnect();

	void defineAction(int messageType, const std::function<void(const Message& msg)>& action);
	void defineAction(const Message::Type& messageType, const std::function<void(const Message& msg)>& action);
	void send(const Message& message);
	
	void handleMessage(const Message& message);
	void	update();
	
	


private:
	int _socket_fd;
	std::string _ip_address;
	uint16_t _port;
	bool _connected;

	std::unordered_map<Message::Type, std::function<void(const Message& msg)>> _actions;

	std::vector<uint8_t> _recv_buffer;
	std::mutex _send_mutex;
	std::mutex _recv_mutex;
	std::thread _recv_thread;
	bool _stop_recv_thread;

	void recvLoop();
	
};
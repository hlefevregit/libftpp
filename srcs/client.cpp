/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 11:29:41 by hulefevr          #+#    #+#             */
/*   Updated: 2025/10/08 12:06:00 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/network.hpp"


Client::Client(int socket_fd, const std::string& ip_address, uint16_t port)
	: _socket_fd(socket_fd), _ip_address(ip_address), _port(port), _connected(false),
	  _recv_buffer(), _send_mutex(), _recv_mutex(), _recv_thread(), _stop_recv_thread(false) {
	if (socket_fd != -1)
		_connected = true;
}

Client::~Client() {
	disconnect();
}

int Client::getSocketFd() const {
	return _socket_fd;
}

std::string Client::getIpAddress() const {
	return _ip_address;
}

void Client::connect(const std::string& address, const size_t& port) {
	if (_connected) {
		std::cerr << "Client is already connected." << std::endl;
		return;
	}

	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1) {
		std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
		return;
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, address.c_str(), &server_addr.sin_addr) <= 0) {
		std::cerr << "Invalid address: " << address << std::endl;
		close(_socket_fd);
		_socket_fd = -1;
		return;
	}

	if (::connect(_socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		std::cerr << "Connection failed: " << strerror(errno) << std::endl;
		close(_socket_fd);
		_socket_fd = -1;
		return;
	}

	_ip_address = address;
	_port = port;
	_connected = true;

	_stop_recv_thread = false;
	_recv_thread = std::thread(&Client::recvLoop, this);
}


void Client::disconnect() {
	if (!_connected) {
		return;
	}

	_stop_recv_thread = true;
	if (_recv_thread.joinable()) {
		_recv_thread.join();
	}

	close(_socket_fd);
	_socket_fd = -1;
	_connected = false;
}

void Client::defineAction(const Message::Type& messageType, const std::function<void(const Message& msg)>& action) {
	_actions[messageType] = action;
}

void Client::send(const Message& message) {
	if (!_connected) {
		std::cerr << "Client is not connected. Cannot send message." << std::endl;
		return;
	}

	std::lock_guard<std::mutex> lock(_send_mutex);
	const std::vector<uint8_t>& data = message.rawData();
	size_t total_sent = 0;
	while (total_sent < data.size()) {
		ssize_t sent = ::send(_socket_fd, data.data() + total_sent, data.size() - total_sent, 0);
		if (sent == -1) {
			std::cerr << "Failed to send message: " << strerror(errno) << std::endl;
			return;
		}
		total_sent += sent;
	}
}

void Client::update() {
	std::lock_guard<std::mutex> lock(_recv_mutex);
	while (true) {
		if (_recv_buffer.size() < Message::HEADER_SIZE) {
			break;
		}

		int msg_type;
		uint32_t msg_size;
		std::memcpy(&msg_type, _recv_buffer.data(), sizeof(int));
		std::memcpy(&msg_size, _recv_buffer.data() + sizeof(int), sizeof(uint32_t));
		msg_size = ntohl(msg_size);

		if (_recv_buffer.size() < Message::HEADER_SIZE + msg_size) {
			break;
		}

		Message message;
		message.setType(msg_type);
		message.clear();
		message.ensureCapacity(msg_size);
		message.appendData(_recv_buffer.data() + Message::HEADER_SIZE, msg_size);

		_recv_buffer.erase(_recv_buffer.begin(), _recv_buffer.begin() + Message::HEADER_SIZE + msg_size);

		handleMessage(message);
	}
}

void Client::recvLoop() {
	while (!_stop_recv_thread) {
		uint8_t buffer[4096];
		ssize_t bytes_received = recv(_socket_fd, buffer, sizeof(buffer), 0);
		if (bytes_received > 0) {
			std::lock_guard<std::mutex> lock(_recv_mutex);
			_recv_buffer.insert(_recv_buffer.end(), buffer, buffer + bytes_received);
		} else if (bytes_received == 0) {
			std::cerr << "Server closed the connection." << std::endl;
			disconnect();
			break;
		} else {
			if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
				std::cerr << "Receive error: " << strerror(errno) << std::endl;
				disconnect();
				break;
			}
		}
	}
}

void Client::handleMessage(const Message& message) {
	auto it = _actions.find(static_cast<Message::Type>(message.getType()));
	if (it != _actions.end()) {
		it->second(message);
	} else {
		std::cerr << "No action defined for message type: " << message.typeToString() << std::endl;
	}
}


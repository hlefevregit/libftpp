/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   persistant_worker.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 15:17:41 by hulefevr          #+#    #+#             */
/*   Updated: 2025/10/06 15:39:16 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "thread_safe_iostream.hpp"
#include <string>
#include <functional>
#include <unordered_map> 
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

class PersistantWorker {
public:
	PersistantWorker();
	~PersistantWorker();

	void addTask(const std::string& name, const std::function<void()>& jobToExecute);
	
	void removeTask(const std::string& name);

private:

	void workerThread();
	
	PersistantWorker(const PersistantWorker& other);
	PersistantWorker& operator=(const PersistantWorker& other);

	std::thread _worker;
	std::atomic<bool> _stop;
	std::mutex _tasksMutex;
	std::unordered_map<std::string, std::function<void()>> _tasks;
	std::atomic<size_t> _activeTasks;
	std::chrono::milliseconds _sleepDuration;
	
};
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/30 15:23:26 by hulefevr          #+#    #+#              #
#    Updated: 2025/10/06 15:25:53 by hulefevr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++11

NAME = includes/pool.hpp
INCLUDES = 	includes/pool.hpp includes/pool.tpp \
			includes/data_buffer.hpp includes/data_buffer.tpp \
			includes/memento.hpp includes/memento.tpp \
			includes/observer.hpp includes/observer.tpp \
			includes/singleton.hpp includes/singleton.tpp \
			includes/state_machine.hpp includes/state_machine.tpp \
			includes/thread_safe_iostream.hpp includes/thread_safe_iostream.tpp \
			includes/thread.hpp includes/threading.hpp includes/design_patterns.hpp \
			includes/thread_safe_queue.hpp includes/thread_safe_queue.tpp \
			includes/worker_pool.hpp includes/persistant_worker.hpp \
			
SRCS = srcs/data_buffer.cpp srcs/memento.cpp srcs/thread_safe_iostream.cpp \
	   srcs/thread.cpp srcs/worker_pool.cpp srcs/persistant_worker.cpp
OBJS = $(SRCS:.cpp=.o)


.PHONY: all clean fclean re tester

all: $(NAME)

$(NAME): $(INCLUDES)
	@echo "Template library headers ready"

%.o: %.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -I includes -c $< -o $@

clean:
	rm -f $(OBJS)
	
fclean: clean
	rm -f tester

re: fclean all

tester: $(OBJS)
	$(CC) $(CFLAGS) -I includes $(OBJS) test.cpp -o tester
	./tester

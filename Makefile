# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/30 15:23:26 by hulefevr          #+#    #+#              #
#    Updated: 2025/09/30 18:38:45 by hulefevr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++11

NAME = includes/pool.hpp
INCLUDES = includes/pool.hpp includes/pool.tpp includes/data_buffer.hpp includes/data_buffer.tpp includes/memento.hpp includes/memento.tpp includes/observer.hpp includes/observer.tpp includes/singleton.hpp includes/singleton.tpp includes/state_machine.hpp includes/state_machine.tpp
SRCS = srcs/data_buffer.cpp srcs/memento.cpp
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

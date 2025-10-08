
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/30 15:23:26 by hulefevr          #+#    #+#              #
#    Updated: 2025/10/07 18:06:21 by hulefevr         ###   ########.fr        #
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
        includes/message.hpp includes/message.tpp \
        includes/client.hpp includes/network.hpp includes/threading.hpp
        
SRCS = srcs/data_buffer.cpp srcs/memento.cpp srcs/thread_safe_iostream.cpp \
    srcs/thread.cpp srcs/worker_pool.cpp srcs/persistant_worker.cpp \
    srcs/message.cpp srcs/client.cpp
OBJS = $(SRCS:.cpp=.o)

LIBFTPP_DIR = libftpp
LIBFTPP_MAINS = $(wildcard $(LIBFTPP_DIR)/main_*.cpp)

.PHONY: all clean fclean re tester test-all compile-all clean-tests list-tests help

all: $(NAME)

$(NAME): $(INCLUDES)
	@echo "Template library headers ready"

%.o: %.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -I includes -c $< -o $@

clean:
	rm -f $(OBJS)
    
fclean: clean
	rm -f tester
	$(MAKE) clean-tests

re: fclean all

tester: $(OBJS)
	$(CC) $(CFLAGS) -I includes $(OBJS) test.cpp -o tester
	./tester

test-all: $(OBJS)
	@echo "=== Running all individual tests from $(LIBFTPP_DIR)/ ==="
	@echo "Found test files: $(notdir $(LIBFTPP_MAINS))"
	@echo ""
	@for main in $(LIBFTPP_MAINS); do \
        test_name=$$(basename $$main .cpp); \
        echo "=========================================="; \
        echo "Testing: $$test_name"; \
        echo "=========================================="; \
        $(CC) $(CFLAGS) -I includes $(OBJS) $$main -o $$test_name 2>/dev/null || \
        { echo "❌ Compilation failed for $$test_name"; continue; }; \
        if [ -f $$test_name ]; then \
            echo "✅ Running $$test_name..."; \
            ./$$test_name || echo "❌ Test $$test_name failed with exit code: $$?"; \
            echo ""; \
        fi; \
    done
	@echo "=== All individual tests completed ==="

test-%: $(OBJS)
	@if [ ! -f "$(LIBFTPP_DIR)/main_$*.cpp" ]; then \
        echo "❌ Test file $(LIBFTPP_DIR)/main_$*.cpp not found!"; \
        echo "Available tests:"; \
        ls $(LIBFTPP_DIR)/main_*.cpp 2>/dev/null | sed 's|$(LIBFTPP_DIR)/main_||g' | sed 's|\.cpp||g' | sed 's|^|  - |g' || echo "  No test files found"; \
        exit 1; \
    fi
	@echo "=== Testing: $* ==="
	$(CC) $(CFLAGS) -I includes $(OBJS) $(LIBFTPP_DIR)/main_$*.cpp -o main_$*
	@echo "✅ Compiled main_$*"
	@echo "🚀 Running main_$*..."
	./main_$*
	@echo "✅ Test $* completed"

compile-all: $(OBJS)
	@echo "=== Compiling all individual tests ==="
	@success=0; total=0; \
    for main in $(LIBFTPP_MAINS); do \
        test_name=$$(basename $$main .cpp); \
        total=$$((total + 1)); \
        printf "Compiling %-30s ... " "$$test_name"; \
        if $(CC) $(CFLAGS) -I includes $(OBJS) $$main -o $$test_name 2>/dev/null; then \
            echo "✅ OK"; \
            success=$$((success + 1)); \
        else \
            echo "❌ FAILED"; \
        fi; \
    done; \
    echo ""; \
    echo "Compilation Summary: $$success/$$total tests compiled successfully"

clean-tests:
	@echo "Cleaning test binaries..."
	@for main in $(LIBFTPP_MAINS); do \
        test_name=$$(basename $$main .cpp); \
        rm -f $$test_name; \
    done
	@echo "Test binaries cleaned"

list-tests:
	@echo "Available individual tests:"
	@if [ -n "$(LIBFTPP_MAINS)" ]; then \
        for main in $(LIBFTPP_MAINS); do \
            test_name=$$(basename $$main .cpp | sed 's/main_//'); \
            echo "  - $$test_name"; \
        done; \
        echo ""; \
        echo "Usage:"; \
        echo "  make test-<name>     # Run specific test"; \
        echo "  make test-all        # Run all tests"; \
        echo "  make compile-all     # Compile all tests"; \
        echo "  make list-tests      # Show this list"; \
    else \
        echo "  No test files found in $(LIBFTPP_DIR)/"; \
    fi

help:
	@echo "libftpp Makefile - Available commands:"
	@echo ""
	@echo "Main targets:"
	@echo "  all              Build library headers"
	@echo "  tester           Run comprehensive test suite"
	@echo "  clean            Remove object files"
	@echo "  fclean           Remove all generated files"
	@echo "  re               Rebuild everything"
	@echo ""
	@echo "Individual tests (from $(LIBFTPP_DIR)/):"
	@echo "  test-all         Run all individual tests"
	@echo "  test-<name>      Run specific test (e.g., test-data_buffer)"
	@echo "  compile-all      Compile all tests without running"
	@echo "  list-tests       List available individual tests"
	@echo "  clean-tests      Remove test binaries"
	@echo ""
	@echo "Examples:"
	@echo "  make test-data_buffer    # Test DataBuffer specifically"
	@echo "  make test-pool           # Test Pool specifically"
	@echo "  make test-all            # Run all individual tests"

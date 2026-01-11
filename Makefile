# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vhasanov <vhasanov@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/11 04:52:26 by vhasanov          #+#    #+#              #
#    Updated: 2026/01/11 04:52:32 by vhasanov         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Name of the final executable
NAME = philo

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g
# -Wall -Wextra -Werror: Show all warnings and treat them as errors
# -pthread: Enable pthread library (for threads)
# -g: Include debugging symbols (useful for debugging with gdb/valgrind)

# Remove command
RM = rm -f

# Source files (.c files)
SRCS = main.c \
       utils.c \
       init.c \
       routine.c \
       monitor.c \
       cleanup.c

# Object files (.o files, compiled from .c files)
OBJS = $(SRCS:.c=.o)

# Default rule: make all
all: $(NAME)

# Rule to create the executable
# Links all object files together
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# Rule to compile .c files to .o files
# %.o means "any .o file"
# %.c means "corresponding .c file"
# $< is the source file (.c)
# $@ is the target file (.o)
%.o: %.c philo.h
	$(CC) $(CFLAGS) -c $< -o $@

# Remove object files
clean:
	$(RM) $(OBJS)

# Remove object files AND executable
fclean: clean
	$(RM) $(NAME)

# Rebuild everything from scratch
re: fclean all

# Phony targets (not actual files)
.PHONY: all clean fclean re

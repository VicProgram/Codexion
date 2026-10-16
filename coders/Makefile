# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vabad-ro <vabad-ro@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/16 13:53:00 by vabad-ro          #+#    #+#              #
#    Updated: 2026/10/16 18:33:17 by vabad-ro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread

SRC = main.c
OBJ = $(SRC:.c=.o)

HEADERS = codexion.h

INCLUDE = codexion.h

RM = rm -f

all: $(NAME)

	$(NAME): $(OBJ) $(HEADERS)
	cc $(OBJ) -o $(NAME)


clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re : fclean all

.PHONY: all clean fclean re

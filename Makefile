NAME = ft_ping

CC = cc
CFLAGS = -g -Wall -Wextra -Werror
LDFLAGS = -lm

SRC = src/main.c \
	  src/init.c \
	  src/packet.c \
	  src/checksum.c \
	  src/recv.c \
	  src/rtt.c \
      src/utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
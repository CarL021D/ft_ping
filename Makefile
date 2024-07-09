NAME = ft_ping

SRCS = srcs/ft_ping.c		\
       srcs/valuesinit.c

OBJS = ${SRCS:.c=.o}

CFLAGS = -Wall -Werror -Wextra -g3
CC = gcc

RM = rm -f

INCS = includes

.PHONY: all clean fclean re

all: ${NAME}

.c.o:
	@$(CC) ${CFLAGS} -I${INCS} -c $< -o $@

${NAME}: ${OBJS}
	sudo ${CC} ${CFLAGS} ${OBJS} -o ${NAME}
	@printf "%s\e[0;32m ft_ping : Compiling object file into executable\n\e[0m" "-"

clean:
	@${RM} ${OBJS}
	@printf "%s\e[0;31m ft_ping : Delete all object files\n\e[0m" "-"

fclean: clean
	@${RM} ${NAME}
	@printf "%s\e[0;31m ft_ping : Delete executable\n\e[0m" "-"

re: fclean ${NAME}
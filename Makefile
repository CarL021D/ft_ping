NAME = ft_ping

SRCS = srcs/ft_ping.c			    \
       srcs/values_init.c	        \
       srcs/packet_managment.c      \
       srcs/error_handler.c	        \
       srcs/ping_options_manager.c	\
	   srcs/utils.c

OBJS = ${SRCS:.c=.o}

CFLAGS = -Wall -Werror -Wextra -lm -g3  \
    -O0                             \
    -fsanitize=address              \
    -fsanitize=undefined            \
    -fno-omit-frame-pointer         \
    -fstack-protector-strong        \
    -fno-optimize-sibling-calls
	
CC = gcc

RM = rm -f

INCS = includes

.PHONY: all clean fclean re

all: ${NAME}

.c.o:
	@$(CC) ${CFLAGS} -I${INCS} -c $< -o $@

${NAME}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${NAME}
	@printf "%s\e[0;32m ft_ping : Compiling object file into executable\n\e[0m" "-"

clean:
	@${RM} ${OBJS}
	@printf "%s\e[0;31m ft_ping : Delete all object files\n\e[0m" "-"

fclean: clean
	@${RM} ${NAME}
	@printf "%s\e[0;31m ft_ping : Delete executable\n\e[0m" "-"

re: fclean ${NAME}
NAME 				= philo
SRCS 				= main.c
CFLAGS	 			= -Wall -Werror -Wextra	-g
OBJS        		= $(SRCS:.c=.o)
CC = cc

all: $(NAME)


$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -rf $(OBJS)
	
fclean: clean
	rm -rf $(NAME)


re: fclean all

.PHONY: all clean fclean re

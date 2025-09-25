NAME = webserv

CC = c++

CFLAGS = -Wall -Wextra -Werror -Wshadow -std=c++98

FILENAMES = $(addsuffix .cpp, 	main			\
								Cgi				\
								Client			\
								Location		\
								Parse			\
								Request			\
								Response		\
								Server			\
								ServerConfig	\
								ServerHandler	\
								Utils)

EXCEPTIONS = $(addprefix exceptions/, $(addsuffix .cpp,	ConfigException				\
														InvalidRequestException		\
														ServerConnectionException	\
														ServerRunningException))

SRCS = $(addprefix srcs/, $(FILENAMES) $(EXCEPTIONS))

OBJS = $(SRCS:%.cpp=%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(NAME)
	
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

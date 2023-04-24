NAME     = ircserv
CXX      = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS = ./test/main.cpp \
#	./srcs/server/IRCServer.cpp \
#	./srcs/socket/ServerSocket.cpp \
#	./srcs/reactor/SocketReactor.cpp \
#	./srcs/session/Session.cpp

OBJS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re

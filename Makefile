NAME     = ircserv
CXX      = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS = ./srcs/main.cpp \
	./srcs/server/IRCServer.cpp \
	./srcs/socket/ServerSocket.cpp \
	./srcs/session/Session.cpp \
	./srcs/session/SessionManager.cpp \
	./srcs/packet/PacketManager.cpp \
	./srcs/message/IRCMessage.cpp \
	./srcs/client/ClientManager.cpp \
	./srcs/client/Client.cpp \
	./srcs/channel/ChannelManager.cpp \
	./srcs/channel/Channel.cpp \
	./srcs/oper/OperatorManager.cpp



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

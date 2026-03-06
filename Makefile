# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdiaz-he <jdiaz-he@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/24 19:48:59 by jdiaz-he          #+#    #+#              #
#    Updated: 2026/02/17 19:20:02 by jdiaz-he         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


# Colors
CRESET					:=	\033[0m
CGRAY					:=	\033[30m
CRED					:=	\033[31m
CGREEN					:=	\033[32m
CYELLOW					:=	\033[33m
CBLUE					:=	\033[34m
CPURPLE					:=	\033[35m
CBLUE_LIGHT				:=	\033[36m
CWHITE					:=	\033[37m
CBLACK_BG				:=	\033[40m
CRED_BG					:=	\033[41m
CGREEN_BG				:=	\033[42m
CYELLOW_BG				:=	\033[43m
CBLUE_BG				:=	\033[44m
CPURPLE_BG				:=	\033[45m
CBLUE_LIGHT_BG			:=	\033[46m
CWHITE_BG				:=	\033[47m

# Makefile flags
MAKEFLAGS += --no-print-directory

# Commons dirs
DIR_INC					:=	inc/
DIR_SRC					:=	src/
DIR_OBJ					:=	obj/

# ircserv
NAME					:=	ircserv
HEADER					:=	$(DIR_INC)Server.hpp

SRC						:=	main.cpp \
							Server.cpp \
							Client.cpp \
							Channel.cpp \
							Message.cpp \
							Reply.cpp \
							CommandManager.cpp \
							cmd/PassCommand.cpp \
							cmd/NickCommand.cpp \
							cmd/UserCommand.cpp \
							cmd/JoinCommand.cpp \
							cmd/PartCommand.cpp \
							cmd/PrivmsgCommand.cpp \
							cmd/QuitCommand.cpp \
							cmd/InviteCommand.cpp \
							cmd/KickCommand.cpp \
							cmd/TopicCommand.cpp \
							cmd/ModeCommand.cpp \
							cmd/AwayCommand.cpp

OBJ						:=	$(addprefix $(DIR_OBJ), $(SRC:cpp=o))

# Compiler (Linux default)
CXX						:=	c++
CXX_FLAGS				:=	-Wall -Werror -Wextra -std=c++98
DATA_RACE				:=	#-fsanitize=thread -g3 -pthread
LEAKS_FLAGS				:=	-g3 -fsanitize=address#,leak
OPTIMIZE_FLAGS			:=	-O3
PREPROCESS_FLAG			:=	-E
ASSEMBLY_FLAG			:=	-S
INC_LIB					:=	-I$(DIR_INC)

# Debug Flag
# DEBUG_VALUE			:=	0
# DEBUG					:=	-DDEBUG=$(DEBUG_VALUE)
DEBUG					:=	-DDEBUG=0

# Files
all						:	$(NAME)

$(DIR_OBJ)%.o			:	$(DIR_SRC)%.cpp $(HEADER)
						@if [ ! -e $@ ]; then \
							echo "$(CBLUE)ircserv: Compilando $(notdir $@):	\c"; \
						else \
							echo "$(CBLUE)ircserv: $(CBLUE_BG)Re$(CRESET)$(CBLUE)compilando $(notdir $@):	\c"; \
						fi
						@mkdir -p $(@D)
						@$(CXX) $(CXX_FLAGS) $(INC_LIB) $(OPTIMIZE_FLAGS) -c $< -o $@ $(LEAKS_FLAGS) $(DATA_RACE) $(DEBUG)
						@echo "$(CGREEN) Archivo compilado.$(CRESET)"

$(NAME)					:	$(OBJ)
						@echo "$(CYELLOW)ircserv: Compilando $(notdir $@):	$(CRESET)"
						@$(CXX) $(CXX_FLAGS) $(OPTIMIZE_FLAGS) $(OBJ) $(LEAKS_FLAGS) -o $@ $(DATA_RACE) $(DEBUG)
						@echo "$(CGREEN)************************************$(CRESET)"
						@echo "$(CGREEN)* $(CGREEN_BG)       ircserv compilado        $(CRESET)$(CGREEN) *$(CRESET)"
						@echo "$(CGREEN)************************************$(CRESET)"

# Debug rules

debug_eval				:
						@$(eval DEBUG := -DDEBUG=1)

debug					:	debug_eval re

# debug					:	
# 						$(MAKE) DEBUG="-DDEBUG=1" re

# Clean rules
clean					:	
						@rm -rf $(OBJ) $(DIR_OBJ)
						@echo "$(CYELLOW_BG)ircserv:$(CRESET)$(CYELLOW) Objetos eliminados.$(CRESET)"

fclean					:	clean
						@rm -rf $(NAME)
						@echo "$(CYELLOW_BG)ircserv:$(CRESET)$(CYELLOW) Ejecutable eliminado.$(CRESET)"

re						:	fclean all

.PHONY					:	all clean fclean re debug debug_eval .SILENT
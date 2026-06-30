# ----- Target -----
NAME		=	codexion
CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -pthread -I$(HDR_DIR)

# ----- Directories -----
# Root Directory Name
SRC_DIR		=	./srcs/
OBJ_DIR		=	./objs/
HDR_DIR		=	./hdrs/

# Sub Directory Name
MAIN_DIR		=	main/
INIT_DIR		=	init/
CORE_DIR		=	core/
CODER_DIR		=	coder/
HEAP_DIR		=	heap/
UTILS_DIR		=	utils/

# ----- Source Files -----
SRCS		=	$(SRCS_MAIN) \
				$(SRCS_INIT) \
				$(SRCS_CORE) \
				$(SRCS_CODER) \
				$(SRCS_HEAP) \
				$(SRCS_UTILS)
SRCS_MAIN	=	main.c \
				simulation.c
SRCS_INIT	=	init.c \
				init_utils.c \
				parse.c
SRCS_CORE	=	arbiter.c \
				supervisor.c
SRCS_CODER	=	coder.c \
				coder_method.c
SRCS_HEAP	=	pop_heap.c \
				push_heap.c \
				heap_utils.c
SRCS_UTILS	=	utils.c \
				print.c \
				cleanup.c

# --- Object Files ---
OBJS		=	$(addprefix $(OBJ_DIR), $(SRCS:.c=.o))

# --- Headers File ---
HDR_NAME	=	codexion.h
HDRS		=	$(addprefix $(HDR_DIR), $(HDR_NAME))

vpath %.c	$(SRC_DIR) \
			$(SRC_DIR)$(MAIN_DIR) \
			$(SRC_DIR)$(INIT_DIR) \
			$(SRC_DIR)$(CORE_DIR) \
			$(SRC_DIR)$(CODER_DIR) \
			$(SRC_DIR)$(HEAP_DIR) \
			$(SRC_DIR)$(UTILS_DIR) \

# Mandatory Part
all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)%.o:	%.c $(HDRS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re

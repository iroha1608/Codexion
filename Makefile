# --- Target ---
TARGET		=	codexion
CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -pthread -I$(HDR_DIR)

# --- Directories ---
# Root Directory Name
SRC_DIR		=	./srcs/
OBJ_DIR		=	./objs/
HDR_DIR		=	./hdrs/

# Sub Directory Name
MAIN_DIR		=	main/
CODER_DIR		=	coder/
SCHEDULER_DIR	=	scheduler/


# --- Source Files ---
SRCS		=	$(SRCS_MAIN) $(SRCS_CODER) $(SRCS_SCHEDULER)
SRCS_MAIN	=	main.c \
				init.c \
				heap.c
# SRCS_CODER	=
# SRCS_SCHEDULER	=

# --- Object Files ---
OBJS		=	$(addprefix $(OBJ_DIR), $(SRCS:.c=.o))

# --- Headers File ---
HDR_NAME	=	codexion.h
HDRS		=	$(addprefix $(HDR_DIR), $(HDR_NAME))

vpath %.c	$(SRC_DIR) \
			$(SRC_DIR)$(MAIN_DIR) 

# Mandatory Part
all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)%.o:	%.c $(HDRS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

fclean:	clean
	rm -f $(TARGET)

re:	fclean all

.PHONY: all clean fclean re

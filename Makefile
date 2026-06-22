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
MAIN_DIR	=	main/
SORT_DIR	=	sort/
UTILS_DIR	=	utils/
ERROR_DIR	=	error/
OPE_DIR		=	operations/
BONUS_DIR	=	bonus/


# --- Source Files ---
SRCS		=	$(SRCS_MAIN) $(SRCS_SORT) $(SRCS_UTILS) \
				$(SRCS_ERROR) $(SRCS_OPE) $(SRCS_BONUS)
BONUS_SRCS	=	$(SRCS_BONUS) $(SRCS_SORT) $(SRCS_UTILS) \
				$(SRCS_ERROR) $(SRCS_OPE)
SRCS_MAIN	=	main.c
SRCS_BONUS	=	checker.c
SRCS_SORT	=	sort.c \
				sort_min.c \
				sort_to.c \
				sort_initialize_a.c \
				sort_initialize_b.c \
				sort_turk.c
SRCS_UTILS	=	stack_list.c \
				stack_find.c \
				stack_initialize.c
SRCS_ERROR	=	error.c \
				error_int.c
SRCS_OPE	=	swap.c \
				push.c \
				rotate.c \
				reverse.c


# --- Object Files ---
OBJS		=	$(addprefix $(OBJ_DIR), $(SRCS:.c=.o))

# --- Headers File ---
HDR_NAME	=	codexion.h
HDRS		=	$(addprefix $(HDR_DIR), $(HDR_NAME))

vpath %.c	$(SRC_DIR) \
			$(SRC_DIR)$(MAIN_DIR) \
			$(SRC_DIR)$(SORT_DIR) \
			$(SRC_DIR)$(UTILS_DIR) \
			$(SRC_DIR)$(ERROR_DIR) \
			$(SRC_DIR)$(OPE_DIR) \
			$(SRC_DIR)$(BONUS_DIR)

# Mandatory Part
all:	$(TARGET)


# Mandatory Part
all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lft

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

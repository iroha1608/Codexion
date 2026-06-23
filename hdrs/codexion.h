/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 16:52:11 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 22:36:06 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>
# include <stdbool.h>
# include <limits.h>

typedef struct s_data	t_data;
typedef struct s_coder	t_coder;
typedef struct s_heap	t_heap;

// Dongle state
typedef enum e_dongle_state {
	AVAILABLE,
	IN_USE,
	COOLDOWN
}	t_dongle_state;

// Dongle
typedef struct s_dongle {
	int				id;
	t_dongle_state	state;
	long long		available_time;
}	t_dongle;

// Actor: Coder
struct s_coder {
	int			id; // 1 ~ N
	pthread_t	thread_id;
	int			left_dongle_id;
	int			right_dongle_id;

	long long	last_compile_start;
	int			compile_count;
	long long	request_time; // FIFO
	long long	deadline; // EDF
	int			in_queue;
	t_data		*data;

	void		*(*run)(void *arg);
	bool		(*request_dongles)(t_coder *self);
	void		(*release_dongles)(t_coder *self);
	bool		(*print_status)(t_coder *self, const char *status);
};

// Heap (Queue)
struct s_heap {
	t_coder	**data;
	int		size;
	int		capacity;
	int		scheduler_type; // 0 : FIFO, 1 : EDF
};

// System Data
struct s_data {
	// Argument data
	int			num_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	int			num_compiles_required;
	long long	dongle_cooldown;
	int			scheduler_type; // 0 : FIFO, 1 : EDF

	// running state
	long long	simulation_start_time;
	bool		is_simulation_running; // 0 : All thread complete
	bool		init_error;

	// Array
	t_coder		*coders;
	t_dongle	*dongles;
	t_heap		*wait_queue; // Queue

	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	*dongle_conds;
	pthread_mutex_t	time_mutex;
	pthread_cond_t	sv_cond;
	pthread_cond_t	exit_cond;
	pthread_mutex_t	print_mutex;
};

// --------------- main.c ---------------

// --------------- parse.c ---------------
bool	parse_arguments(int argc, char **argv, t_data *data);
// static bool parse_scheduler(char *arg, t_data *data);
// static bool assign_arguments(char **argv, t_data *data);

// --------------- init.c ---------------
void		cleanup_data(t_data *data);
// int			init_data(t_data *data);

// --------------- heap.c ---------------
int			is_higher_priority(t_coder *a, t_coder *b, int scheduler_type);
t_heap		*init_heap(int capacity, int scheduler_type);
void		free_heap(t_heap *heap);
void		push_heap(t_heap *heap, t_coder *coder);
t_coder		*pop_heap(t_heap *heap);
int			is_empty_heap(t_heap *heap);

// --------------- print.c ---------------
int			print_error(const char *msg);
bool		coder_print_status(t_coder *self, const char *status);

// --------------- utils.c ---------------
long long	get_time(void);
void		set_timespec(struct timespec *ts, long long time);
void		precise_sleep(long long sleep_time, t_data *data);
bool		ft_atol(const char *str, long long *result);

#endif

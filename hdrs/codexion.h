/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 16:52:11 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 19:10:23 by nsato            ###   ########.fr       */
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

// Dongle state
typedef enum e_dongle_state {
	AVAILABLE,
	IN_USE,
	COOLDOWN
}	t_dongle_state;

// Dongle
typedef struct s_dongle {
	int	id;
	t_dongle_state	state;
	long long	available_time;
}	t_dongle;

// Coder
typedef struct s_coder {
	int	id; // 1 ~ N
	pthread_t	thread_id;
	int	left_dongle_id;
	int	right_dongle_id;

	long long	last_compile_start;
	int	compile_count;

	long long	request_time; // FIFO
	long long	deadline; // EDF

	struct s_data	*data; // All data access
}	t_coder;

typedef struct s_heap {
	t_coder	**data;
	int	size;
	int	capacity;
	int	scheduler_type; // 0 : FIFO, 1 : EDF
}	t_heap;

// Data
typedef struct s_data {
	// Argument data
	int	num_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	int	num_compiles_required;
	long long	dongle_cooldown;
	int	scheduler_type; // 0 : FIFO, 1 : EDF

	// running state
	long long	simulation_start_time;
	int	is_simulation_running; // 0 : All thread complete

	// Array
	t_coder	*coders;
	t_dongle	*dongles;

	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	*dongle_conds;

	t_heap	*wait_queue; // 中央集権キュー

	pthread_mutex_t	time_mutex;
	pthread_cond_t	sv_cond;

	pthread_mutex_t	print_mutex;
}	t_data;

// main.c
int	parse_arguments(int argc, char **argv, t_data *data);
int	init_data(t_data *data);

// init.c
void	cleanup_data(t_data *data);
int	print_error(const char *msg);

// heap.c
int	is_higher_priority(t_coder *a, t_coder *b, int scheduler_type);
t_heap	*init_heap(int capacity, int scheduler_type);
void	free_heap(t_heap *heap);
void	push_heap(t_heap *heap, t_coder *coder);
t_coder	*pop_heap(t_heap *heap);
int	is_empty_heap(t_heap *heap);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 16:52:11 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 18:19:46 by nsato            ###   ########.fr       */
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

	long long	request_time; // fifo
	long long	deadline; // edf

	struct s_data	*data; // All data access
}	t_coder;

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
	int	scheduler_type; // 0 : fifo, 1 : edf

	// running state
	long long	simulation_start_time;
	int	is_simulation_running; // 0 : All thread complete

	// Array
	t_coder	*coders;
	t_dongle	*dongles;

	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	*dongle_conds;

	pthread_mutex_t	time_mutex;
	pthread_cond_t	sv_cond;

	pthread_mutex_t	print_mutex;
}	t_data;

int	parse_arguments(int argc, char **argv, t_data *data);
int	init_data(t_data *data);
void	cleanup_data(t_data *data);
int	print_error(const char *msg);

#endif

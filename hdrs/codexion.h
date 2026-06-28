/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 16:52:11 by nsato             #+#    #+#             */
/*   Updated: 2026/06/29 03:00:36 by nsato            ###   ########.fr       */
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
typedef enum e_dongle_state
{
	AVAILABLE,
	IN_USE,
	COOLDOWN
}	t_dongle_state;

// Dongle
typedef struct s_dongle
{
	int				id;
	t_dongle_state	state;
	long long		available_time;
}	t_dongle;

// Actor: Coder
struct s_coder
{
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
	int			(*request_dongles)(t_coder *self);
	void		(*release_dongles)(t_coder *self);
	int			(*print_status)(t_coder *self, const char *status);
};

// Heap (Queue)
struct s_heap
{
	t_coder	**data;
	int		size;
	int		capacity;
	int		scheduler_type; // 0 : FIFO, 1 : EDF
};

// System Data
struct s_data
{
	// Argument data
	int				num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				num_compiles_required;
	long long		dongle_cooldown;
	int				scheduler_type; // 0 : FIFO, 1 : EDF
	// running state
	long long		simulation_start_time;
	bool			is_simulation_running; // 0 : All thread complete
	bool			init_error;
	int				ready_count; // Ready complete thread count
	pthread_cond_t	start_cond; // issei start you no joukenhensuu
	// Array
	t_coder			*coders;
	t_dongle		*dongles;
	t_heap			*wait_queue; // Queue
	int				*arbiter_avail; // Queue check!
	t_coder			**arbiter_tmp;
	// Mutex & Condition available
	pthread_mutex_t	scheduler_mutex; // Queue waiting
	pthread_mutex_t	time_mutex; // Get time waiting
	pthread_mutex_t	print_mutex; // Print waiting
	pthread_cond_t	*dongle_conds;
	pthread_cond_t	sv_cond;
	pthread_cond_t	exit_cond;
};

// ============================== srcs/main ==============================
// ------------------------------- main.c -------------------------------
// int	main(int argc, char **argv);

// ---------------------------- simulation.c ----------------------------
bool		start_simulation(t_data *data);
void		run_supervisor_and_wait(t_data *data);
void		wait_all_threads(t_data *data, int create_count);
// static bool	handle_create_error(t_data *data, int create_count);
// static void	sync_and_start(t_data *data);

// ============================== srcs/init ==============================
// =------------------------------ parse.c -------------------------------
bool		parse_arguments(int argc, char **argv, t_data *data);
// static bool	parse_scheduler(char *arg, t_data *data);
// static bool	assign_arguments(char **argv, t_data *data);
// static bool	ft_atol(const char *str, long long *result);

// ------------------------------- init.c -------------------------------
int			init_data(t_data *data);
// static bool	init_sys_mutex_1(t_data *data);
// static bool	init_sys_mutex_2(t_data *data);
// static bool	allocate_arrays(t_data *data);
// static bool	init_coders_and_conds(t_data *data);
//
// ------------------------------ cleanup.c ------------------------------
void		cleanup_data(t_data *data);
void		free_arrays(t_data *data);
void		rollback_system_mutexes(t_data *data);
void		rollback_conds(t_data *data, int count);

// ============================== srcs/core ===============================
// ----------------------------- supervisor.c -----------------------------
void		*supervisor_routine(void *arg);
// static bool	check_all_compiled(t_data *data);
// static bool	check_if_anyone_died(t_data *data, long long now);
// static void	wake_up_all_coders(t_data *data);
// static long long	get_closest_deadline(t_data *data);

// ------------------------------ arbiter.c ------------------------------
bool		attempt_to_grab_dongles(t_coder *self, long long *cooldown_end);
// static void	init_available_array(int *abail, t_data *data)
// static void	restore_heap(t_heap *heap, t_coder **tmp, int count)
// static bool	process_poped_coder(
// 		t_coder *self, t_coder *coder, int *avail, long long *cooldown_end)
// static bool	check_my_turn(
// 		t_coder *self, t_coder *coder, int *avail, long long *cooldown_end)

// ============================== srcs/coder ===============================
// ------------------------------- coder.c --------------------------------
// Coder's Method
void		*coder_routine(void *arg);
bool		check_running(t_data *data);
// static void	perform_compile(t_coder *self);
// static void	perform_debug_and_refactor(t_coder *self);
// static void	wait_for_start_signal(t_coder *self);
//
// ---------------------------- coder_method.c -----------------------------
int			coder_request_dongles(t_coder *self);
void		coder_release_dongles(t_coder *self);
int			coder_print_status(t_coder *self, const char *status);
// static void	wait_for_dongles(t_coder *self, long long cooldown_end);

// =============================== srcs/heap ===============================
// ------------------------------ heap_pop.c ------------------------------
t_coder		*pop_heap(t_heap *heap);
// static void	heapify_down(t_heap *heap, int i);
// static int	get_smallest_child(
// 		t_heap *heap, int count, int left, int right);

// ------------------------------ heap_push.c ------------------------------
void		push_heap(t_heap *heap, t_coder *coder);
// static void	heapify_up(t_heap *heap, int i);

// ------------------------------ heap_utils.c -----------------------------
int			is_higher_priority(t_coder *a, t_coder *b, int scheduler_type);
t_heap		*init_heap(int capacity, int scheduler_type);
void		free_heap(t_heap *heap);
int			is_empty_heap(t_heap *heap);

// ============================== srcs/utils ==============================
// ------------------------------- utils.c -------------------------------
long long	get_time(void);
void		set_timespec(struct timespec *ts, long long time);
void		precise_sleep(long long sleep_time, t_data *data);

// ------------------------------- print.c -------------------------------
int			print_error(const char *msg);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 16:52:11 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 02:00:41 by nsato            ###   ########.fr       */
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
	// (Initialized 'init_coders_and_conds()')
	int				id;
	t_dongle_state	state;
	long long		available_time;
}	t_dongle;

// Actor: Coder
struct s_coder
{
	// (Initialized 'pthread_create()')
	pthread_t	thread_id;
	// (Initialized 'init_coders_and_conds()')
	int			id; // 1 ~ N
	int			left_dongle_id;
	int			right_dongle_id;
	int			compile_count;
	t_data		*data;
	void		*(*run)(void *arg);
	int			(*request_dongles)(t_coder *self);
	void		(*release_dongles)(t_coder *self);
	int			(*print_status)(t_coder *self, const char *status);
	long long	last_compile_start;
	long long	request_time; // FIFO
	long long	deadline; // EDF
	int			in_queue;
};

// Heap (Queue)
struct s_heap
{
	// (Initialized 'init_heap()')
	t_coder	**data;
	int		size;
	int		capacity;
	int		scheduler_type; // 0 : FIFO, 1 : EDF
};

// System Data
struct s_data
{
	// Argument data (Initialized arg_parse())
	int				num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				num_compiles_required;
	long long		dongle_cooldown;
	int				scheduler_type; // 0 : FIFO, 1 : EDF
	// running state
	// (Initialized 'init_data')
	bool			is_simulation_running;
	//
	long long		simulation_start_time;
	bool			init_error;
	int				ready_count; // Ready complete thread count
	// Array (Initialized 'allocate_arrays()')
	t_coder			*coders;
	t_dongle		*dongles;
	bool			*arbiter_avail; // Queue check!
	t_coder			**arbiter_tmp;
	// (Initialized 'init_data()')
	t_heap			*wait_queue; // Queue
	// Mutex & Condition available
	// (Initialized 'init_mutexes/conds/coders_and_conds()')
	// is_simulation_running, ready_count, last_compile_start, deadline
	pthread_mutex_t	time_mutex;
	// wait_queue, dongles, in_queue
	pthread_mutex_t	scheduler_mutex;
	// print_error, coder_print_status
	pthread_mutex_t	print_mutex;
	// Signal to Coder for waiting dongle
	pthread_cond_t	*dongle_conds;
	// Signal to Supervisor
	pthread_cond_t	sv_cond;
	// Signal to Coder for waiting start
	pthread_cond_t	start_cond;
	// Signal to Coder for waiting exit
	pthread_cond_t	exit_cond;
};

// ============================== srcs/main ==============================
// ------------------------------- main.c -------------------------------
// int	main(int argc, char **argv);

// ---------------------------- simulation.c ----------------------------
bool		start_simulation(t_data *data);
void		run_supervisor_and_wait(t_data *data);
void		wait_all_threads(t_data *data, int create_count);
// static void	handle_create_error(t_data *data, int create_count);
// static void	sync_and_start(t_data *data);

// ============================== srcs/init ==============================
// =------------------------------ parse.c -------------------------------
bool		parse_arguments(int argc, char **argv, t_data *data);
// static bool	parse_scheduler(char *arg, t_data *data);
// static bool	assign_arguments(char **argv, t_data *data);
// static bool	ft_atol(const char *str, long long *result);

// ------------------------------- init.c -------------------------------
int			init_data(t_data *data);
// static bool	init_mutexes(t_data *data);
// static bool	init_conds(t_data *data);
// static bool	allocate_arrays(t_data *data);
// static bool	init_coders_and_conds(t_data *data);
//
// ------------------------------ cleanup.c ------------------------------
void		cleanup_data(t_data *data);
void		free_arrays(t_data *data);
void		free_heap(t_heap *heap);
void		rollback_mutexes_and_conds(t_data *data);
void		rollback_dongle_conds(t_data *data, int count);

// ============================== srcs/core ===============================
// ----------------------------- supervisor.c -----------------------------
void		*supervisor_routine(void *arg);
void		stop_simulation(t_data *data);
// static bool	check_all_compiled(t_data *data);
// static bool	check_if_anyone_died(t_data *data, long long now);
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
bool		init_heap(t_data *data);
int			is_higher_priority(t_coder *a, t_coder *b, int scheduler_type);
int			is_empty_heap(t_heap *heap);

// ============================== srcs/utils ==============================
// ------------------------------- utils.c -------------------------------
long long	get_time(void);
void		set_timespec(struct timespec *ts, long long time);
void		precise_sleep(long long sleep_time, t_data *data);

// ------------------------------- print.c -------------------------------
int			print_error(const char *msg);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 17:51:49 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 03:58:57 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// A process that uses 'pthread_create' to create threads
/// for multiple Coders and a Supervisor, and synchronizes them so that
/// they all start at the same time.
/// *** If 'pthread_join' fails, there's nothing you can do.***
/// """
#include "../../hdrs/codexion.h"

/// """
/// Wait for successfully created Coder threads to converge.
/// By having the main thread wait here,
/// we prevent the program from terminating unexpectedly.
/// """
void	wait_all_threads(t_data *data, int create_count)
{
	int	i;

	i = 0;
	while (i < create_count)
	{
		if (pthread_join(data->coders[i].thread_id, NULL) != 0)
			print_error("Failed to join coder thread.");
		i ++;
	}
}

/// """
/// Create the Supervisor and wait for it to exit.
/// This function called affter all coders habe gathered and started.
/// When the Supervisor finishes,
/// it means either someone has died or everyone has finished compiling.
/// """
void	run_supervisor_and_wait(t_data *data)
{
	pthread_t	sv_thread;

	if (pthread_create(&sv_thread, NULL, supervisor_routine, data) != 0)
	{
		print_error("Failed to create supervisor thread.");
		stop_simulation(data);
	}
	else
		if (pthread_join(sv_thread, NULL) != 0)
			print_error("Failed to join coder thread.");
}

/// """
/// Handling for when an error occurs while creating a thread.
/// Send a termination on start signal to any threads
/// that have already been created and are waiting,
/// then terminate after safely merging them.
/// """
static void	handle_create_error(t_data *data, int create_count)
{
	print_error("Failed to create thread.");
	pthread_mutex_lock(&data->time_mutex);
	pthread_cond_broadcast(&data->start_cond);
	pthread_mutex_unlock(&data->time_mutex);
	wait_all_threads(data, create_count);
}

/// """
/// Wait until all Coders are ready, then start them all at once.
/// Save the simulation start time (0ms)
/// and synchronize the start times of the first compilation for all Coders.
/// """
static void	sync_and_start(t_data *data)
{
	int	i;

	pthread_mutex_lock(&data->time_mutex);
	while (data->ready_count < data->num_coders)
		pthread_cond_wait(&data->sv_cond, &data->time_mutex);
	data->simulation_start_time = get_time();
	i = 0;
	while (i < data->num_coders)
	{
		data->coders[i].last_compile_start = data->simulation_start_time;
		i ++;
	}
	data->is_simulation_running = true;
	pthread_cond_broadcast(&data->start_cond);
	pthread_mutex_unlock(&data->time_mutex);
}

/// """
/// Create a thread for each Coder.
/// The created Coders each enter a state
/// where they wait for a signal from 'sync_and_start'.
/// """
bool	start_simulation(t_data *data)
{
	int	i;

	data->is_simulation_running = false;
	i = 0;
	while (i < data->num_coders)
	{
		if (pthread_create(&data->coders[i].thread_id, NULL,
				data->coders[i].run, &data->coders[i]) != 0)
		{
			handle_create_error(data, i);
			return (false);
		}
		i ++;
	}
	sync_and_start(data);
	return (true);
}

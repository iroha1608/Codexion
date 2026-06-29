/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:22:44 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 02:11:33 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Allocate memory and set the initial state of mutex used by the entire
/// system and each coder.
/// """
#include "../../hdrs/codexion.h"

/// """
/// Initialize mutexes.
/// """
static bool	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->time_mutex, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&data->scheduler_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->time_mutex);
		return (false);
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->time_mutex);
		pthread_mutex_destroy(&data->scheduler_mutex);
		return (false);
	}
	return (true);
}

/// """
/// Initialize condition variables.
/// """
static bool	init_conds(t_data *data)
{
	if (pthread_cond_init(&data->sv_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->time_mutex);
		pthread_mutex_destroy(&data->scheduler_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		return (false);
	}
	if (pthread_cond_init(&data->start_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->time_mutex);
		pthread_mutex_destroy(&data->scheduler_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		pthread_cond_destroy(&data->sv_cond);
		return (false);
	}
	if (pthread_cond_init(&data->exit_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->time_mutex);
		pthread_mutex_destroy(&data->scheduler_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		pthread_cond_destroy(&data->sv_cond);
		pthread_cond_destroy(&data->start_cond);
		return (false);
	}
	return (true);
}

/// """
/// Allocate memory for the Coder, Dongle, Dongle_cond arrays,
/// available dongles array, tmp array based on the number of coders.
/// """
static bool	allocate_arrays(t_data *data)
{
	data->coders = malloc(data->num_coders * sizeof(t_coder));
	data->dongles = malloc(data->num_coders * sizeof(t_dongle));
	data->dongle_conds = malloc(data->num_coders * sizeof(pthread_cond_t));
	data->arbiter_avail = malloc(data->num_coders * sizeof(bool));
	data->arbiter_tmp = malloc(data->num_coders * sizeof(t_coder *));
	if (!data->coders || !data->dongles || !data->dongle_conds
		|| !data->arbiter_avail || !data->arbiter_tmp)
	{
		free_arrays(data);
		return (false);
	}
	return (true);
}

/// """
/// Initialize each coder and dongle.
/// 'run', 'request_dongles', 'release_dongles', 'print_status',
/// are methods of the Coder.
/// """
static bool	init_coders_and_conds(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		if (pthread_cond_init(&data->dongle_conds[i], NULL) != 0)
		{
			rollback_dongle_conds(data, i);
			return (false);
		}
		data->dongles[i].id = i;
		data->dongles[i].state = AVAILABLE;
		data->dongles[i].available_time = 0;
		data->coders[i].id = i + 1;
		data->coders[i].left_dongle_id = i;
		data->coders[i].right_dongle_id = (i + 1) % data->num_coders;
		data->coders[i].compile_count = 0;
		data->coders[i].data = data;
		data->coders[i].run = coder_routine;
		data->coders[i].request_dongles = coder_request_dongles;
		data->coders[i].release_dongles = coder_release_dongles;
		data->coders[i].print_status = coder_print_status;
		i ++;
	}
	return (true);
}

/// """
/// Initialize function called when the program starts.
/// If it fails, it rolls back all memory, mutexes, and condition variables
/// allocated so far.
/// """
int	init_data(t_data *data)
{
	if (init_mutexes(data) == false || init_conds(data) == false)
		return (false);
	if (allocate_arrays(data) == false)
	{
		rollback_mutexes_and_conds(data);
		return (false);
	}
	if (init_coders_and_conds(data) == false)
	{
		free_arrays(data);
		rollback_mutexes_and_conds(data);
		return (false);
	}
	if (init_heap(data) == false)
	{
		rollback_dongle_conds(data, data->num_coders);
		free_arrays(data);
		rollback_mutexes_and_conds(data);
		return (false);
	}
	return (true);
}

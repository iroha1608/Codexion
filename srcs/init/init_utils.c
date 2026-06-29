/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 03:12:28 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 03:45:07 by nsato            ###   ########.fr       */
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
bool	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->time_mutex, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&data->scheduler_mutex, NULL) != 0)
	{
		rollback_mutexes_and_conds(data, 1);
		return (false);
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		rollback_mutexes_and_conds(data, 2);
		return (false);
	}
	return (true);
}

/// """
/// Initialize condition variables.
/// """
bool	init_conds(t_data *data)
{
	if (pthread_cond_init(&data->sv_cond, NULL) != 0)
	{
		rollback_mutexes_and_conds(data, 3);
		return (false);
	}
	if (pthread_cond_init(&data->start_cond, NULL) != 0)
	{
		rollback_mutexes_and_conds(data, 4);
		return (false);
	}
	if (pthread_cond_init(&data->exit_cond, NULL) != 0)
	{
		rollback_mutexes_and_conds(data, 5);
		return (false);
	}
	return (true);
}

/// """
/// Allocate memory for the Coder, Dongle, Dongle_cond arrays,
/// available dongles array, tmp array based on the number of coders.
/// """
bool	allocate_arrays(t_data *data)
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
bool	init_coders_and_conds(t_data *data)
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
/// Initialize a heap (priority queue) in memory.
/// """
bool	init_heap(t_data *data)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (false);
	heap->data = malloc(data->num_coders * sizeof(t_coder *));
	if (!heap->data)
	{
		free(heap);
		return (false);
	}
	heap->size = 0;
	heap->capacity = data->num_coders;
	heap->scheduler_type = data->scheduler_type;
	data->wait_queue = heap;
	return (true);
}

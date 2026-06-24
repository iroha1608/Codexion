/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:22:44 by nsato             #+#    #+#             */
/*   Updated: 2026/06/24 16:26:27 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"

// """
//
// """
static bool	init_sys_mutex_1(t_data *data)
{
	if (pthread_mutex_init(&data->scheduler_mutex, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&data->time_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->scheduler_mutex);
		return (false);
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->scheduler_mutex);
		pthread_mutex_destroy(&data->time_mutex);
		return (false);
	}
	return (true);
}

// """
//
// """
static bool	init_sys_mutex_2(t_data *data)
{
	if (pthread_cond_init(&data->sv_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->scheduler_mutex);
		pthread_mutex_destroy(&data->time_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		return (false);
	}
	if (pthread_cond_init(&data->exit_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->scheduler_mutex);
		pthread_mutex_destroy(&data->time_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		pthread_cond_destroy(&data->sv_cond);
		return (false);
	}
	return (true);
}

// """
// Coders no kazu bun coder, dongle, dongle_cond no memory allocate.
// """
static bool allocate_arrays(t_data *data)
{
	data->coders = malloc(data->num_coders * sizeof(t_coder));
	data->dongles = malloc(data->num_coders * sizeof(t_dongle));
	data->dongle_conds = malloc(data->num_coders * sizeof(pthread_cond_t));
	if (!data->coders || !data->dongles || !data->dongle_conds)
	{
		if (data->coders) free(data->coders);
		if (data->dongles) free(data->dongles);
		if (data->dongle_conds) free(data->dongle_conds);
		return (false);
	}
	return (true);
}

// ""
//
// ""
static bool init_coders_and_conds(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		if (pthread_cond_init(&data->dongle_conds[i], NULL) != 0)
		{
			rollback_conds(data, i);
			return (false);
		}
		data->dongles[i].id = i;
		data->dongles[i].state = AVAILABLE;
		data->dongles[i].available_time = 0;

		data->coders[i].id = i + 1;
		data->coders[i].compile_count = 0;
		data->coders[i].data = data;

		data->coders[i].left_dongle_id = i;
		data->coders[i].right_dongle_id = (i + 1) % data->num_coders;
		// Actor method
		data->coders[i].print_status = coder_print_status;
		i ++;
	}
	return (true);
}

// """
// Todo: data->coders/dongles/dongle_conds のfreeは1関数にまとめて呼び出す。
// """
int	init_data(t_data *data)
{
	// Initialize queue
	if (!init_sys_mutex_1(data) || !init_sys_mutex_2(data))
		return (false);
	if (!allocate_arrays(data))
	{
		rollback_system_mutexes(data);
		return (false);
	}
	if (!init_coders_and_conds(data))
	{
		free(data->coders);
		free(data->dongles);
		free(data->dongle_conds);
		rollback_system_mutexes(data);
		return (false);
	}
	data->wait_queue = init_heap(data->num_coders, data->scheduler_type);
	if (!data->wait_queue)
	{
		free(data->coders);
		free(data->dongles);
		free(data->dongle_conds);
		rollback_conds(data, data->num_coders);
		rollback_system_mutexes(data);
		return (false);
	}
	data->is_simulation_running = true;
	return (true);
}

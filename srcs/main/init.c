/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:22:44 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 19:15:37 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../hdrs/codexion.h"

void	cleanup_data(t_data *data)
{
	int	i;

	i = 0;
	if (data->dongle_conds)
	{
		while (i < data->num_coders)
		{
			pthread_cond_destroy(&data->dongle_conds[i]);
			i ++;
		}
		free(data->dongle_conds);
	}
	if (data->coders)
		free(data->coders);
	if (data->dongles)
		free(data->dongles);

	pthread_mutex_destroy(&data->scheduler_mutex);
	pthread_mutex_destroy(&data->time_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	pthread_cond_destroy(&data->sv_cond);

	// queueの後片付け
	if (data->wait_queue)
		free_heap(data->wait_queue);
}

int	init_data(t_data *data)
{
	int	i;

	data->coders = malloc(data->num_coders * sizeof(t_coder));
	data->dongles = malloc(data->num_coders * sizeof(t_dongle));
	data->dongle_conds = malloc(data->num_coders * sizeof(pthread_cond_t));

	if (!data->coders || !data->dongles || !data->dongle_conds)
	{
		cleanup_data(data);
		return print_error("Malloc failed.");
	}
	pthread_mutex_init(&data->scheduler_mutex, NULL);
	pthread_mutex_init(&data->time_mutex, NULL);
	pthread_mutex_init(&data->print_mutex, NULL);
	pthread_cond_init(&data->sv_cond, NULL);

	i = 0;
	while (i < data->num_coders)
	{
		pthread_cond_init(&data->dongle_conds[i], NULL);
		data->dongles[i].id = i;
		data->dongles[i].state = AVAILABLE;
		data->dongles[i].available_time = 0;

		data->coders[i].id = i + 1;
		data->coders[i].compile_count = 0;
		data->coders[i].data = data;

		data->coders[i].left_dongle_id = i;
		data->coders[i].right_dongle_id = (i + 1) % data->num_coders;
		i ++;
	}

	// queueの初期化
	data->wait_queue = init_heap(data->num_coders, data->scheduler_type);
	if (!data->wait_queue)
	{
		cleanup_data(data);
		return print_error("Heap malloc failed.");
	}

	data->is_simulation_running = 1;
	return (0);
}

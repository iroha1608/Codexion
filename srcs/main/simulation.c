/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 17:51:49 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 03:31:58 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"

// """
// threadの回収。作れた分だけjoinする
// """
void	wait_all_threads(t_data *data, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(data->coders[i].thread_id, NULL);
		i ++;
	}
}

///
///
///
void	run_supervisor_and_wait(t_data *data)
{
	pthread_t	sv_thread;

	if (pthread_create(&sv_thread, NULL, supervisor_routine, data) != 0)
	{
		print_error("Failed to create supervisor thread.");
		pthread_mutex_lock(&data->time_mutex);
		data->is_simulation_running = false;
		pthread_cond_broadcast(&data->exit_cond);
		pthread_mutex_unlock(&data->time_mutex);
	}
	else
		pthread_join(sv_thread, NULL);
}

static bool	handle_create_error(t_data *data, int count)
{
	print_error("Failed to create thread.");
	pthread_mutex_lock(&data->time_mutex);
	data->is_simulation_running = false;
	pthread_cond_broadcast(&data->start_cond);
	pthread_cond_broadcast(&data->exit_cond);
	pthread_mutex_unlock(&data->time_mutex);
	wait_all_threads(data, count);
	return (false);
}

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
// 作成済みthreadの回収
// Todo: createのエラーハンドリングも別関数に分ける。
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
			return (handle_create_error(data, i));
		i ++;
	}
	sync_and_start(data);
	return (true);
}

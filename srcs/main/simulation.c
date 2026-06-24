/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 17:51:49 by nsato             #+#    #+#             */
/*   Updated: 2026/06/24 18:03:11 by nsato            ###   ########.fr       */
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

// """
// Todo: createのエラーハンドリングも別関数に分ける。
// """
bool	start_simulation(t_data *data)
{
	int	i;

	pthread_mutex_lock(&data->time_mutex);
	data->simulation_start_time = get_time();
	pthread_mutex_unlock(&data->time_mutex);
	i = 0;
	while (i < data->num_coders)
	{
		data->coders[i].last_compile_start = data->simulation_start_time;
		if (pthread_create(&data->coders[i].thread_id, NULL, data->coders[i].run, &data->coders[i]) != 0)
		{
			print_error("Failed to create thread.");
			pthread_mutex_lock(&data->time_mutex);
			data->is_simulation_running = false;
			pthread_cond_broadcast(&data->exit_cond);
			pthread_mutex_unlock(&data->time_mutex);
			wait_all_threads(data, i); // 作成済みthreadの回収
		}
		i ++;
	}
	return (true);
}

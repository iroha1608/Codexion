/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 22:46:49 by nsato             #+#    #+#             */
/*   Updated: 2026/06/25 14:45:18 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"

// """
// Initialize success system's Mutex/Cond is destroy.
// """
void	rollback_system_mutexes(t_data *data)
{
	pthread_mutex_destroy(&data->scheduler_mutex);
	pthread_mutex_destroy(&data->time_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	pthread_cond_destroy(&data->sv_cond);
	pthread_cond_destroy(&data->exit_cond);
	pthread_cond_destroy(&data->start_cond);
}

// """
//
// """
void	rollback_conds(t_data *data, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_cond_destroy(&data->dongle_conds[i]);
		i ++;
	}
}

// """
// cleanup queue
// """
void	cleanup_data(t_data *data)
{
	if (data->wait_queue)
		free_heap(data->wait_queue);
	if (data->dongle_conds)
	{
		rollback_conds(data, data->num_coders);
		free(data->dongle_conds);
	}
	if (data->coders)
		free(data->coders);
	if (data->dongles)
		free(data->dongles);
	rollback_system_mutexes(data);
}

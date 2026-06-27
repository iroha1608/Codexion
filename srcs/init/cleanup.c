/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 22:46:49 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 16:11:41 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
///
/// """
#include "../../hdrs/codexion.h"

/// """
/// Initialize success system's Mutex/Cond is destroy.
/// """
void	rollback_system_mutexes(t_data *data)
{
	pthread_mutex_destroy(&data->scheduler_mutex);
	pthread_mutex_destroy(&data->time_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	pthread_cond_destroy(&data->sv_cond);
	pthread_cond_destroy(&data->exit_cond);
	pthread_cond_destroy(&data->start_cond);
}

/// """
///
/// """
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

///
/// Memory allocate sita hennsuu nomi free.
///
void	free_arrays(t_data *data)
{
	if (data->coders)
		free(data->coders);
	if (data->dongles)
		free(data->dongles);
	if (data->dongle_conds)
		free(data->dongle_conds);
}

/// """
/// cleanup queue
/// """
void	cleanup_data(t_data *data)
{
	if (data->wait_queue)
		free_heap(data->wait_queue);
	rollback_conds(data, data->num_coders);
	free_arrays(data);
	rollback_system_mutexes(data);
}

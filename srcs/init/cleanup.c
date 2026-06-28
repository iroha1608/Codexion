/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 22:46:49 by nsato             #+#    #+#             */
/*   Updated: 2026/06/29 01:16:53 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
///
/// """
#include "../../hdrs/codexion.h"

/// """
/// Destroy the mutex and condition variables
/// that were successfully initialized.
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
/// Destroy the specified number of standby condition variables
/// created for each coder.
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

/// """
/// Free the memory allocated for the array containing
/// the Coders, Dongles, and dongle stanby condition variables.
/// """
void	free_arrays(t_data *data)
{
	if (data->coders)
		free(data->coders);
	if (data->dongles)
		free(data->dongles);
	if (data->dongle_conds)
		free(data->dongle_conds);
	if (data->arbiter_avail)
		free(data->arbiter_avail);
	if (data->arbiter_tmp)
		free(data->arbiter_tmp);
}

/// """
/// Release all memory, mutexes, and condition variables
/// when the simulation ends or an error occurs.
/// """
void	cleanup_data(t_data *data)
{
	free_heap(data->wait_queue);
	rollback_conds(data, data->num_coders);
	free_arrays(data);
	rollback_system_mutexes(data);
}

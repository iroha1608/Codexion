/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 22:46:49 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 03:30:37 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
///
/// """
#include "../../hdrs/codexion.h"

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
/// Free the heap and its internal arrays from memory.
/// """
void	free_heap(t_heap *heap)
{
	if (heap)
	{
		if (heap->data)
			free(heap->data);
		free(heap);
	}
}

/// """
/// Destroy the mutex and condition variables
/// that were successfully initialized.
/// """
void	rollback_mutexes_and_conds(t_data *data, int stage)
{
	if (stage >= 1)
		pthread_mutex_destroy(&data->time_mutex);
	if (stage >= 2)
		pthread_mutex_destroy(&data->scheduler_mutex);
	if (stage >= 3)
		pthread_mutex_destroy(&data->print_mutex);
	if (stage >= 4)
		pthread_cond_destroy(&data->sv_cond);
	if (stage >= 5)
		pthread_cond_destroy(&data->start_cond);
	if (stage >= 6)
		pthread_cond_destroy(&data->exit_cond);
}

/// """
/// Destroy the specified number of standby condition variables
/// created for each coder.
/// """
void	rollback_dongle_conds(t_data *data, int count)
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
/// Release all memory, mutexes, and condition variables
/// when the simulation ends or an error occurs.
/// """
void	cleanup_data(t_data *data)
{
	free_heap(data->wait_queue);
	rollback_dongle_conds(data, data->num_coders);
	free_arrays(data);
	rollback_mutexes_and_conds(data, 6);
}

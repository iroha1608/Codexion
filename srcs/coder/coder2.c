/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:54:22 by nsato             #+#    #+#             */
/*   Updated: 2026/06/25 17:00:07 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
///
/// """
#include "../../hdrs/codexion.h"

/// """
///
/// """
static void	wait_for_dongles(t_coder *self, long long cd_end)
{
	struct timespec	ts;

	if (cd_end > 0)
	{
		set_timespec(&ts, cd_end);
		pthread_cond_timedwait(&self->data->dongle_conds[self->id - 1],
			&self->data->scheduler_mutex, &ts);
	}
	else
		pthread_cond_wait(&self->data->dongle_conds[self->id - 1],
			&self->data->scheduler_mutex);
}

/// """
///
/// """
bool	coder_request_dongles(t_coder *self)
{
	long long		cd_end;

	pthread_mutex_lock(&self->data->scheduler_mutex);
	self->request_time = get_time();
	pthread_mutex_lock(&self->data->time_mutex);
	self->deadline = self->last_compile_start + self->data->time_to_burnout;
	pthread_mutex_unlock(&self->data->time_mutex);
	self->in_queue = 1;
	push_heap(self->data->wait_queue, self);
	while (check_running(self->data))
	{
		if (attempt_to_grab_dongles(self, &cd_end))
			break ;
		wait_for_dongles(self, cd_end);
	}
	pthread_mutex_unlock(&self->data->scheduler_mutex);
	return (check_running(self->data));
}

/// """
///
/// """
void	coder_release_dongles(t_coder *self)
{
	int			i;
	long long	now_micro;

	pthread_mutex_lock(&self->data->scheduler_mutex);
	now_micro = get_time();
	self->data->dongles[self->left_dongle_id].state = AVAILABLE;
	self->data->dongles[self->left_dongle_id].available_time = (
			now_micro + self->data->dongle_cooldown);
	self->data->dongles[self->right_dongle_id].state = AVAILABLE;
	self->data->dongles[self->right_dongle_id].available_time = (
			now_micro + self->data->dongle_cooldown);
	i = 0;
	while (i < self->data->num_coders)
	{
		pthread_cond_signal(&self->data->dongle_conds[i]);
		i++;
	}
	pthread_mutex_unlock(&self->data->scheduler_mutex);
}

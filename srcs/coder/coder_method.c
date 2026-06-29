/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_method.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:54:22 by nsato             #+#    #+#             */
/*   Updated: 2026/06/29 16:14:57 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Coder's method
/// That called in 'coder_routine'.
/// """
#include "../../hdrs/codexion.h"

/// """
/// While waiting for its turn, the process enters a 'sleep' state
/// using a Condition variable.
/// If a cooldown end time (cooldown_end) is specified,
/// the process enters sleep mode with a timer.
/// """
static void	wait_for_dongles(t_coder *self, long long cooldown_end)
{
	struct timespec	ts;

	if (cooldown_end > 0)
	{
		set_timespec(&ts, cooldown_end);
		pthread_cond_timedwait(&self->data->dongle_conds[self->id - 1],
			&self->data->scheduler_mutex, &ts);
	}
	else
		pthread_cond_wait(&self->data->dongle_conds[self->id - 1],
			&self->data->scheduler_mutex);
}

/// """
/// Request a Dongle and wait until it is granted.
/// Enroll in the Arbiter's waiting list (push_heap)
/// and wait until permission is granted.
/// """
int	coder_request_dongles(t_coder *self)
{
	long long		cooldown_end;

	pthread_mutex_lock(&self->data->scheduler_mutex);
	self->request_time = get_time();
	pthread_mutex_lock(&self->data->time_mutex);
	self->deadline = self->last_compile_start + self->data->time_to_burnout;
	pthread_mutex_unlock(&self->data->time_mutex);
	self->in_queue = 1;
	push_heap(self->data->wait_queue, self);
	while (check_running(self->data))
	{
		if (attempt_to_grab_dongles(self, &cooldown_end))
			break ;
		wait_for_dongles(self, cooldown_end);
	}
	pthread_mutex_unlock(&self->data->scheduler_mutex);
	return (check_running(self->data));
}

/// """
/// Updates the last time the dongle was used (available_time),
/// return the dongle, and send a signal to all waiting Coders.
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

/// """
/// Format and output the Coder's current stateus.
/// If the 'printf' call failsm, immediately force the simulation to teminate.
/// """
int	coder_print_status(t_coder *self, const char *status)
{
	long long	now_micro;
	long long	now_milli;

	pthread_mutex_lock(&self->data->time_mutex);
	if (!self->data->is_simulation_running)
	{
		pthread_mutex_unlock(&self->data->time_mutex);
		return (false);
	}
	pthread_mutex_lock(&self->data->print_mutex);
	now_micro = get_time() - self->data->simulation_start_time;
	now_milli = now_micro / 1000LL;
	if (printf("%lld %d %s\n", now_milli, self->id, status) < 0)
	{
		pthread_mutex_unlock(&self->data->print_mutex);
		pthread_mutex_unlock(&self->data->time_mutex);
		stop_simulation(self->data);
		return (false);
	}
	pthread_mutex_unlock(&self->data->print_mutex);
	pthread_mutex_unlock(&self->data->time_mutex);
	return (true);
}

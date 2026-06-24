/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 16:41:03 by nsato             #+#    #+#             */
/*   Updated: 2026/06/24 17:41:57 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"

// """
//
// """
static	bool check_running(t_data *data)
{
	bool	running;

	pthread_mutex_lock(&data->time_mutex);
	running = data->is_simulation_running;
	pthread_mutex_unlock(&data->time_mutex);
	return (running);
}

// """
//
// """
bool	coder_request_dongles(t_coder *self)
{
	long long	cd_end;
	struct timespec	ts;

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
			break;
		if (cd_end > 0)
		{
			set_timespec(&ts, cd_end);
			pthread_cond_timedwait(&self->data->dongle_conds[self->id - 1], &self->data->scheduler_mutex, &ts);
		}
		else
			pthread_cond_wait(&self->data->dongle_conds[self->id - 1], &self->data->scheduler_mutex);
	}
	pthread_mutex_unlock(&self->data->scheduler_mutex);
	return (check_running(self->data));
}

// """
//
// """
void	coder_release_dongles(t_coder *self)
{
	int	i;
	long long	now_micro;

	pthread_mutex_lock(&self->data->scheduler_mutex);
	now_micro = get_time();
	self->data->dongles[self->left_dongle_id].state = AVAILABLE;
	self->data->dongles[self->left_dongle_id].available_time = now_micro + self->data->dongle_cooldown;
	self->data->dongles[self->right_dongle_id].state = AVAILABLE;
	self->data->dongles[self->right_dongle_id].available_time = now_micro + self->data->dongle_cooldown;
	i = 0;
	while (i < self->data->num_coders)
	{
		pthread_cond_signal(&self->data->dongle_conds[i]);
		i++;
	}
	pthread_mutex_unlock(&self->data->scheduler_mutex);
}

// """
//
// """
static void	perform_compile(t_coder *self)
{
	self->print_status(self, "has taken a dongle");
	self->print_status(self, "has taken a dongle");
	self->print_status(self, "is compiling");
	pthread_mutex_lock(&self->data->time_mutex);
	self->last_compile_start = get_time();
	self->compile_count ++;
	pthread_cond_signal(&self->data->sv_cond);
	pthread_mutex_unlock(&self->data->time_mutex);
	precise_sleep(self->data->time_to_compile, self->data);
}

// """
// Todo: debug, refactoringの時も短くても関数を切り分けたい
// """
void	*coder_routine(void *arg)
{
	t_coder	*self;

	self = (t_coder *)arg;
	if (self->id % 2 == 0)
		precise_sleep(1000, self->data);
	while (check_running(self->data))
	{
		if (self->data->num_compiles_required != -1 && self->data->num_compiles_required <= self->compile_count)
			break;
		if (!self->request_dongles(self))
			break;
		perform_compile(self);
		self->release_dongles(self);
		self->print_status(self, "is debugging");
		precise_sleep(self->data->time_to_debug, self->data);
		self->print_status(self, "is refactoring");
		precise_sleep(self->data->time_to_refactor, self->data);
	}
	return (NULL);
}

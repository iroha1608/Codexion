/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 16:41:03 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 05:48:51 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// This is main routine that loops indefinitely until the coder
/// either 'dies' or 'everyone is finished.'
/// """
#include "../../hdrs/codexion.h"

/// """
/// Wait until all Coders are present,
/// and once they are, send signal to the Supervisor(sv_cond).
/// Wait for the 'sync_and_start()' start signal at start_cond.
/// """
static void	wait_for_start_signal(t_coder *self)
{
	pthread_mutex_lock(&self->data->time_mutex);
	self->data->ready_count ++;
	if (self->data->ready_count == self->data->num_coders)
		pthread_cond_signal(&self->data->sv_cond);
	while (self->data->is_simulation_running == false
			&& self->data->init_error == false)
		pthread_cond_wait(&self->data->start_cond, &self->data->time_mutex);
	pthread_mutex_unlock(&self->data->time_mutex);
}

/// """
/// Simulation is currently running / Check to see if anyone has died.
/// """
bool	check_running(t_data *data)
{
	bool	running;

	pthread_mutex_lock(&data->time_mutex);
	running = data->is_simulation_running;
	pthread_mutex_unlock(&data->time_mutex);
	return (running);
}

/// """
/// Perform a 'Compile' after obtaining the Dongles.
/// """
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

/// """
/// Perform 'Debugging' and 'Refactoring'.
/// """
static void	perform_debug_and_refactor(t_coder *self)
{
	self->print_status(self, "is debugging");
	precise_sleep(self->data->time_to_debug, self->data);
	self->print_status(self, "is refactoring");
	precise_sleep(self->data->time_to_refactor, self->data);
}

/// """
/// The main loop for each Coder thread.
/// It repeats the cycle of
/// Dongle request -> Compile -> Return -> Debug -> Refactor.
/// """
void	*coder_routine(void *arg)
{
	t_coder	*self;

	self = (t_coder *)arg;
	wait_for_start_signal(self);
	if (self->data->num_coders == 1)
	{
		self->print_status(self, "has taken a dongle");
		precise_sleep(self->data->time_to_burnout + 1000LL, self->data);
		return (NULL);
	}
	if (self->id % 2 == 0)
		precise_sleep(1000, self->data);
	while (check_running(self->data))
	{
		if (self->data->num_compiles_required <= self->compile_count)
			break ;
		if (!self->request_dongles(self))
			break ;
		perform_compile(self);
		self->release_dongles(self);
		perform_debug_and_refactor(self);
	}
	return (NULL);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 21:46:42 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 02:52:27 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"

// """
//
// """
int	print_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

// """
// Coder's method
// fprintf is failed, all coder destroy.
// """
bool	coder_print_status(t_coder *self, const char *status)
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
		self->data->is_simulation_running = false;
		pthread_cond_broadcast(&self->data->exit_cond);
	}
	pthread_mutex_unlock(&self->data->print_mutex);
	pthread_mutex_unlock(&self->data->time_mutex);
	return (true);
}

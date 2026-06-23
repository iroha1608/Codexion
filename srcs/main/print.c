/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 21:46:42 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 23:03:27 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../hdrs/codexion.h"

// """
// """
int	print_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

// Coder's method
// """
// """
bool	coder_print_status(t_coder *self, const char *status)
{
	long long	now_micro;
	long long	now_milli;
	bool		is_running;

	pthread_mutex_lock(&self->data->print_mutex);
	pthread_mutex_lock(&self->data->time_mutex);
	is_running = self->data->is_simulation_running;
	pthread_mutex_unlock(&self->data->time_mutex);
	// fprintf is failed, all coder destroy.
	if (is_running)
	{
		now_micro = get_time() - self->data->simulation_start_time;
		now_milli = now_micro / 1000LL;
		if (printf("%lld %d %s\n", now_milli, self->id, status) < 0)
		{
			pthread_mutex_lock(&self->data->time_mutex);
			self->data->is_simulation_running = false;
			pthread_cond_broadcast(&self->data->exit_cond);
			pthread_mutex_unlock(&self->data->time_mutex);
		}
	}
	pthread_mutex_unlock(&self->data->print_mutex);
	return (is_running);
}

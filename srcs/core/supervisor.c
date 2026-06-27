/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supervisor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 19:12:57 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 17:36:56 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
///
/// """
#include "../../hdrs/codexion.h"

/// """
///
/// """
static void	wake_up_all_coders(t_data *data)
{
	int	j;

	data->is_simulation_running = false;
	pthread_cond_broadcast(&data->exit_cond);
	j = 0;
	while (j < data->num_coders)
	{
		pthread_cond_broadcast(&data->dongle_conds[j]);
		j ++;
	}
}

/// """
///
/// """
static bool	check_if_anyone_died(t_data *data, long long now)
{
	int			i;
	long long	deadline;

	i = 0;
	while (i < data->num_coders)
	{
		deadline = data->coders[i].last_compile_start + data->time_to_burnout;
		if (now >= deadline)
		{
			pthread_mutex_lock(&data->print_mutex);
			printf("%lld %d burned out\n",
				(now - data->simulation_start_time) / 1000LL,
				data->coders[i].id);
			pthread_mutex_unlock(&data->print_mutex);
			wake_up_all_coders(data);
			return (true);
		}
		i ++;
	}
	return (false);
}

/// """
///
/// """
static bool	check_all_compiled(t_data *data)
{
	int	i;

	if (data->num_compiles_required == -1)
		return (false);
	i = 0;
	while (i < data->num_coders)
	{
		if (data->coders[i].compile_count < data->num_compiles_required)
			return (false);
		i ++;
	}
	wake_up_all_coders(data);
	return (true);
}

/// """
///
/// """
static long long	get_closest_dl(t_data *data)
{
	long long		closest;
	long long		dl;
	int				i;

	closest = -1;
	i = 0;
	while (i < data->num_coders)
	{
		dl = data->coders[i].last_compile_start + data->time_to_burnout;
		if (closest == -1 || dl < closest)
			closest = dl;
		i ++;
	}
	return (closest);
}

/// """
///
/// """
void	*supervisor_routine(void *arg)
{
	t_data			*data;
	long long		now;
	struct timespec	ts;

	data = (t_data *)arg;
	pthread_mutex_lock(&data->time_mutex);
	while (data->is_simulation_running)
	{
		now = get_time();
		if (check_all_compiled(data) || check_if_anyone_died(data, now))
			break ;
		set_timespec(&ts, get_closest_dl(data));
		pthread_cond_timedwait(&data->sv_cond, &data->time_mutex, &ts);
	}
	pthread_mutex_unlock(&data->time_mutex);
	return (NULL);
}

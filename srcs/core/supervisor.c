/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supervisor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 19:12:57 by nsato             #+#    #+#             */
/*   Updated: 2026/06/29 02:33:44 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// A Supervisor thread that monitors whether anyone has experienced
/// a burnout or whether everyone has completed the specified
/// number of compiles.
/// """
#include "../../hdrs/codexion.h"

/// """
/// When the termination conditions are met,
/// (check_if anyone_died or check_all_compiled)
/// a signal is sent to all waiting Coders.
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
/// Check the time until burnout for all Coders,
/// and if any coder has reached burnout, issue a burnout declaration.
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
/// Check whether all Coders has comiled the required number of times.
/// """
static bool	check_all_compiled(t_data *data)
{
	int	i;

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
/// Get the burnout time closest to the current time from all Coders.
/// """
static long long	get_closest_deadline(t_data *data)
{
	long long		closest;
	long long		deadline;
	int				i;

	closest = -1;
	i = 0;
	while (i < data->num_coders)
	{
		deadline = data->coders[i].last_compile_start + data->time_to_burnout;
		if (closest == -1 || deadline < closest)
			closest = deadline;
		i ++;
	}
	return (closest);
}

/// """
/// The main loop for each Supervisor thread.
/// Instead of monitoring constantly,
/// wait using 'pthread_cond_timedwait' until the next time someone burnout.
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
		set_timespec(&ts, get_closest_deadline(data));
		pthread_cond_timedwait(&data->sv_cond, &data->time_mutex, &ts);
	}
	pthread_mutex_unlock(&data->time_mutex);
	return (NULL);
}

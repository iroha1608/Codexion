/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 21:34:51 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 17:41:32 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Todo:
/// - [ ] gettimeofday()のエラーハンドリング
/// """
#include "../../hdrs/codexion.h"

/// """
/// inside time get micro second.
/// """
long long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec);
}

/// """
/// micro second -> timespec
/// """
void	set_timespec(struct timespec *ts, long long time)
{
	ts->tv_sec = time / 1000000LL;
	ts->tv_nsec = (time % 1000000LL) * 1000LL;
}

/// """
/// No huristic. use exit_cond wait timeout.
/// """
void	precise_sleep(long long sleep_time, t_data *data)
{
	struct timespec	ts;
	long long		target_time;

	target_time = get_time() + sleep_time;
	set_timespec(&ts, target_time);
	pthread_mutex_lock(&data->time_mutex);
	pthread_cond_timedwait(&data->exit_cond, &data->time_mutex, &ts);
	pthread_mutex_unlock(&data->time_mutex);
}

/// """
/// validate overflow suuti hennkann.
/// """
bool	ft_atol(const char *str, long long *result)
{
	int			i;
	long long	ret;

	i = 0;
	ret = 0;
	if (str[i] == '\0')
		return (false);
	while (str[i])
	{
		if (str[i] < '0' || '9' < str[i])
			return (false);
		if (ret > (LLONG_MAX - (str[i] - '0')) / 10)
			return (false);
		ret = ret * 10 + (str[i] - '0');
		i ++;
	}
	*result = ret;
	return (true);
}

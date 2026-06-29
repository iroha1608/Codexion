/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 21:34:51 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 05:44:10 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// """
#include "../../hdrs/codexion.h"

/// """
/// 現在の時刻をマイクロ秒で取得する
/// 'gettimeofday()'は引数が正しければ失敗しない.
/// 秒をマイクロ秒に変換し、マイクロ秒を足して返す
/// """
long long	get_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
	{
		print_error("gettimeofday failed.");
		return (-1);
	}
	return ((long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec);
}

/// """
/// time: 一番近いデッドライン
/// pthread_cond_timedwaitに渡すため,マイクロ秒を秒/ナノ秒に変換する
/// """
void	set_timespec(struct timespec *ts, long long time)
{
	ts->tv_sec = time / 1000000LL;
	ts->tv_nsec = (time % 1000000LL) * 1000LL;
}

/// """
/// 指定された時間(マイクロ秒)スレッドをスリープさせる。
/// No huristic. Use exit_cond wait timeout.
/// Catch signal of exit_cond,
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

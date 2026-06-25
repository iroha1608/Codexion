/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:06:39 by nsato             #+#    #+#             */
/*   Updated: 2026/06/25 14:48:31 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"

// """
// 全コーダーをjoinして終了を待つ。
// Todo: スーパーバイザー立ち上げの実装。
// """
int	main(int argc, char **argv)
{
	t_data		data;
	pthread_t	sv_thread;

	memset(&data, 0, sizeof(t_data));
	if (parse_arguments(argc, argv, &data) == false)
		return (1);
	if (init_data(&data) == false)
		return (1);
	if (start_simulation(&data) == false)
	{
		cleanup_data(&data);
		return (1);
	}
	if (pthread_create(&sv_thread, NULL, supervisor_routine, &data) != 0)
	{
		print_error("Failed to create supervisoe thread.");
		pthread_mutex_lock(&data.time_mutex);
		data.is_simulation_running = false;
		pthread_cond_broadcast(&data.exit_cond);
		pthread_mutex_unlock(&data.time_mutex);
	}
	else
		pthread_join(sv_thread, NULL);
	wait_all_threads(&data, data.num_coders);
	cleanup_data(&data);
	return (0);
}

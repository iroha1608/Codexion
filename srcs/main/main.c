/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:06:39 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 03:36:43 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"
// """
// 全コーダーをjoinして終了を待つ。
// Todo: - [x] スーパーバイザー立ち上げの実装。
// """
int	main(int argc, char **argv)
{
	t_data		data;

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
	run_supervisor_and_wait(&data);
	wait_all_threads(&data, data.num_coders);
	cleanup_data(&data);
	return (0);
}

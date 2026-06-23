/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:06:39 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 22:35:25 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../hdrs/codexion.h"

int	main(int argc, char **argv)
{
	t_data	data;

	memset(&data, 0, sizeof(t_data));
	if (parse_arguments(argc, argv, &data) == false)
		return (1);
	// if (!init_data(&data))
		// return (1);
	printf("Initialization success!\n");

	cleanup_data(&data);
	return (0);
}

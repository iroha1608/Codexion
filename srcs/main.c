/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:06:39 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 17:58:11 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../hdrs/codexion.h"

int	print_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

int	is_valid_number(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '\0')
		return 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i ++;
	}
	return (1);
}

int	parse_arguments(int argc, char **argv, t_data *data)
{
	int	i;
	if (argc != 9)
		return print_error("Invalid number of arguments (8 arguments required).");
	i = 1;
	while (i <= 7)
	{
		if (!is_valid_number(argv[i]))
			return print_error("Arguments 1-7 must be non-negative integers.");
		i ++;
	}

	data->num_coders = atoi(argv[1]);
	data->time_to_burnout = atoi(argv[2]);
	data->time_to_compile = atoi(argv[3]);
	data->time_to_debug = atoi(argv[4]);
	data->time_to_refactor = atoi(argv[5]);
	data->num_compiles_required = atoi(argv[6]);
	data->dongle_cooldown = atoi(argv[7]);

	// Scheduler check.
	if (strcmp(argv[8], "fifo") == 0)
		data->scheduler_type = 0;
	else if (strcmp(argv[8], "edf") == 0)
		data->scheduler_type = 1;
	else
		return print_error("Scheduler must be 'fifo' or 'edf'");

	if (data->num_coders <= 0)
		return print_error("Number of coders must be greater than 0.");

	return (0);
}

int	main(int argc, char **argv)
{
	t_data	data;

	memset(&data, 0, sizeof(t_data));

	if (parse_arguments(argc, argv, &data) != 0)
		return (1);

	if (init_data(&data) != 0)
		return (1);

	printf("Initialization success\n");

	cleanup_data(&data);
	return (0);
}

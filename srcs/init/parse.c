/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 21:54:45 by nsato             #+#    #+#             */
/*   Updated: 2026/06/29 00:50:27 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Parsing arguments.
/// """
#include "../../hdrs/codexion.h"

/// """
/// Scheduler argument character check, "fifo" = 0, "edf" = 1.
/// """
static bool	parse_scheduler(char *arg, t_data *data)
{
	if (strcmp(arg, "fifo") == 0)
		data->scheduler_type = 0;
	else if (strcmp(arg, "edf") == 0)
		data->scheduler_type = 1;
	else
		return (false);
	return (true);
}

/// """
/// Receive a string, check for integer overflow,
/// and store it as a 'long long'.
/// """
static bool	ft_atol(const char *str, long long *result)
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

/// """
/// Number arguments parsing.
/// burnout, compile, debug, refactor, coooldown save in microseconds.
/// """
static bool	assign_arguments(char **argv, t_data *data)
{
	int			i;
	long long	tmp[7];

	i = 1;
	while (i <= 7)
	{
		if (!ft_atol(argv[i], &tmp[i - 1]))
			return (false);
		i ++;
	}
	data->num_coders = (int)tmp[0];
	data->time_to_burnout = tmp[1] * 1000LL;
	data->time_to_compile = tmp[2] * 1000LL;
	data->time_to_debug = tmp[3] * 1000LL;
	data->time_to_refactor = tmp[4] * 1000LL;
	data->num_compiles_required = (int)tmp[5];
	data->dongle_cooldown = tmp[6] * 1000LL;
	return (true);
}

/// """
/// Check the number of arguments is correct, then call parding function.
/// """
bool	parse_arguments(int argc, char **argv, t_data *data)
{
	if (argc != 9)
		return (print_error("Invalid number of arguments.") == 0);
	if (assign_arguments(argv, data) == false)
		return (print_error("Args must be posotive integers only.") == 0);
	if (parse_scheduler(argv[8], data) == false)
		return (print_error("Scheduler must be 'fifo' or 'edf'.") == 0);
	if (data->num_coders <= 0 || data->num_compiles_required == 0)
		return (print_error("Invalid coder count or 0 compiles.") == 0);
	return (true);
}

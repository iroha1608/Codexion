/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 17:22:44 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 03:28:08 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Allocate memory and set the initial state of mutex used by the entire
/// system and each coder.
/// """
#include "../../hdrs/codexion.h"

/// """
///
/// """
static void	abort_init(t_data *data, const char *msg, int stage)
{
	if (stage >= 1)
		rollback_mutexes_and_conds(data, 6);
	if (stage >= 2)
		free_arrays(data);
	if (stage >= 3)
		rollback_dongle_conds(data, data->num_coders);
	print_error(msg);
}

/// """
/// Initialize function called when the program starts.
/// If it fails, it rolls back all memory, mutexes, and condition variables
/// allocated so far.
/// """
int	init_data(t_data *data)
{
	if (init_mutexes(data) == false || init_conds(data) == false)
	{
		abort_init(data, "Failed to system mutex/cond.", 0);
		return (false);
	}
	if (allocate_arrays(data) == false)
	{
		abort_init(data, "Failed to allocate arrays.", 1);
		return (false);
	}
	if (init_coders_and_conds(data) == false)
	{
		abort_init(data, "Failed to init coders.", 2);
		return (false);
	}
	if (init_heap(data) == false)
	{
		abort_init(data, "Failed to init heap.", 3);
		return (false);
	}
	return (true);
}

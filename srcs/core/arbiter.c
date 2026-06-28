/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arbiter.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 16:40:55 by nsato             #+#    #+#             */
/*   Updated: 2026/06/29 02:58:46 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// When a Coder requests Dongles,
/// arbiter decide whether to give it to them right away
/// or it on to someone else.
/// """
#include "../../hdrs/codexion.h"

/// """
/// Copy the current availabillity of the dongle into an array.
/// (e.g., AVAILABLE: true, IN_USE/COOLDOWN: false)
/// """
static void	init_available_array(int *avail, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		avail[i] = (data->dongles[i].state == AVAILABLE);
		i ++;
	}
}

/// """
/// Restore the Coder retrieved from the heap.
/// """
static void	restore_heap(t_heap *heap, t_coder **tmp, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		push_heap(heap, tmp[i]);
		i ++;
	}
}

/// """
/// If the head of the heap is self,
/// check whether it is actually accessible.
/// If both cooldowns have ended, return true.
/// If not, update 'cooldown_end'.
/// """
static bool	check_my_turn(
		t_coder *self, t_coder *coder, int *avail, long long *cooldown_end)
{
	long long	now;
	long long	cd_left;
	long long	cd_right;

	if (coder->id == self->id)
	{
		now = get_time();
		cd_left = self->data->dongles[self->left_dongle_id].available_time;
		cd_right = self->data->dongles[self->right_dongle_id].available_time;
		if (now >= cd_left && now >= cd_right)
			return (true);
		if (cd_left > cd_right)
			*cooldown_end = cd_left;
		else
			*cooldown_end = cd_right;
		return (false);
	}
	avail[coder->left_dongle_id] = 0;
	avail[coder->right_dongle_id] = 0;
	return (false);
}

/// """
/// Simulation whether a Coder retrieved from the queue
/// can retrieve a Dongles.
/// """
static bool	process_poped_coder(
		t_coder *self, t_coder *coder, int *avail, long long *cooldown_end)
{
	if (avail[coder->left_dongle_id] && avail[coder->right_dongle_id])
	{
		if (check_my_turn(self, coder, avail, cooldown_end))
		{
			self->in_queue = 0;
			self->data->dongles[self->left_dongle_id].state = IN_USE;
			self->data->dongles[self->right_dongle_id].state = IN_USE;
			return (true);
		}
	}
	else
	{
		avail[coder->left_dongle_id] = 0;
		avail[coder->right_dongle_id] = 0;
	}
	return (false);
}

/// """
/// Test whether Coder can acquire the dongle.
/// (whether Coder have the highest priority.)
/// """
bool	attempt_to_grab_dongles(t_coder *self, long long *cooldown_end)
{
	int		count;
	t_coder	*coder;

	count = 0;
	*cooldown_end = 0;
	init_available_array(self->data->arbiter_avail, self->data);
	while (!is_empty_heap(self->data->wait_queue))
	{
		coder = pop_heap(self->data->wait_queue);
		if (!coder->in_queue)
			continue ;
		self->data->arbiter_tmp[count ++] = coder;
		if (process_poped_coder(
				self, coder, self->data->arbiter_avail, cooldown_end))
		{
			restore_heap(
				self->data->wait_queue, self->data->arbiter_tmp, count);
			return (true);
		}
	}
	restore_heap(self->data->wait_queue, self->data->arbiter_tmp, count);
	return (false);
}

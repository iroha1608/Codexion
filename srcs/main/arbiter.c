/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arbiter.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 16:40:55 by nsato             #+#    #+#             */
/*   Updated: 2026/06/24 17:51:53 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
//
// """
#include "../../hdrs/codexion.h"

// """
//
// """
static void init_available_array(int *avail, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		avail[i] = (data->dongles[i].state == AVAILABLE);
		i ++;
	}
}

// """
//
// """
static void restore_heap(t_heap *heap, t_coder **tmp, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		push_heap(heap, tmp[i]);
		i ++;
	}
}

// """
//
// """
static bool check_my_turn(t_coder *self, t_coder *c, int *avail, long long *cd_end)
{
	long long	now;
	long long	cd_left;
	long long	cd_right;

	if (c->id == self->id)
	{
		now = get_time();
		cd_left = self->data->dongles[c->left_dongle_id].available_time;
		cd_right = self->data->dongles[self->right_dongle_id].available_time;
		if (now >= cd_left && now >= cd_right)
			return (true);
		if (cd_left > cd_right)
			*cd_end = cd_left;
		else
			*cd_end = cd_right;
		return (false);
	}
	avail[c->left_dongle_id] = 0;
	avail[c->right_dongle_id] = 0;
	return (false);
}

// """
//
// """
static bool process_poped_coder(t_coder *self, t_coder *c, int *avail, long long *cd_end)
{
	if (avail[c->left_dongle_id] && avail[c->right_dongle_id])
	{
		if (check_my_turn(self, c, avail, cd_end))
		{
			self->in_queue = 0;
			self->data->dongles[self->left_dongle_id].state = IN_USE;
			self->data->dongles[self->right_dongle_id].state = IN_USE;
			return (true);
		}
	}
	else
	{
		avail[c->left_dongle_id] = 0;
		avail[c->right_dongle_id] = 0;
	}
	return (false);
}

// """
//
// """
bool attempt_to_grab_dongles(t_coder *self, long long *cd_end)
{
	t_coder	*tmp[256];
	int	count;
	int	avail[256];
	t_coder	*c;

	count = 0;
	*cd_end = 0;
	init_available_array(avail, self->data);
	while (!is_empty_heap(self->data->wait_queue))
	{
		c = pop_heap(self->data->wait_queue);
		if (!c->in_queue)
			continue ;
		tmp[count ++] = c;
		if (process_poped_coder(self, c, avail, cd_end))
		{
			restore_heap(self->data->wait_queue, tmp, count);
			return (true);
		}
	}
	restore_heap(self->data->wait_queue, tmp, count);
	return (false);
}

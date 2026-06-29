/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 01:36:11 by nsato             #+#    #+#             */
/*   Updated: 2026/06/30 03:17:51 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Heap initialization, deallocation, and null checks;
/// priority determination logic for schedulers (EDF/FIFO).
/// """
#include "../../hdrs/codexion.h"

/// """
/// Compare the two elements
/// and determine which should be prioritized(placed at the root).
///
/// FIFO: In order of longest wait time.
/// In case of a tie, in order of lowest ID.
/// EDF: In order of most recent burnout time.
/// In case of a tie, in order of longest wait time;
/// if still tied, in order of lowest ID
/// """
int	is_higher_priority(t_coder *a, t_coder *b, int scheduler_type)
{
	if (scheduler_type == 0)
	{
		if (a->request_time != b->request_time)
			return (a->request_time < b->request_time);
		return (a->id < b->id);
	}
	else
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
		if (a->request_time != b->request_time)
			return (a->request_time < b->request_time);
		return (a->id < b->id);
	}
}

/// """
/// Check whether the heap is empty.
/// """
int	is_empty_heap(t_heap *heap)
{
	return (heap->size == 0);
}

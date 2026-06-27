/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 01:36:11 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 17:38:38 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
///
/// """
#include "../../hdrs/codexion.h"

/// """
/// FIFO, EDF
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
///
/// """
t_heap	*init_heap(int capacity, int scheduler_type)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->data = malloc(capacity * sizeof(t_coder *));
	if (!heap->data)
	{
		free(heap);
		return (NULL);
	}
	heap->size = 0;
	heap->capacity = capacity;
	heap->scheduler_type = scheduler_type;
	return (heap);
}

/// """
///
/// """
void	free_heap(t_heap *heap)
{
	if (heap)
	{
		if (heap->data)
			free(heap->data);
		free(heap);
	}
}

/// """
/// FIFO, EDF
/// """
int	is_empty_heap(t_heap *heap)
{
	return (heap->size == 0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_heap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 02:04:47 by nsato             #+#    #+#             */
/*   Updated: 2026/06/29 02:48:02 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Add a element to the heap.
/// After adding it to the end,
/// if there is a parent with a lower priority than the new element,
/// swap them and move up the heap.
/// """
#include "../../hdrs/codexion.h"

/// """
/// Move the added elements up to the correct posision
/// according to its priority.
/// """
static void	heapify_up(t_heap *heap, int i)
{
	int		parent;
	t_coder	*temp;

	while (i != 0)
	{
		parent = (i - 1) / 2;
		if (is_higher_priority(
				heap->data[i], heap->data[parent], heap->scheduler_type))
		{
			temp = heap->data[i];
			heap->data[i] = heap->data[parent];
			heap->data[parent] = temp;
			i = parent;
		}
		else
			break ;
	}
}

/// """
/// Add a new element to the end of the heap and sort it.
/// """
void	push_heap(t_heap *heap, t_coder *coder)
{
	int		i;

	if (heap->size >= heap->capacity)
		return ;
	i = heap->size;
	heap->data[i] = coder;
	heap->size++;
	heapify_up(heap, i);
}

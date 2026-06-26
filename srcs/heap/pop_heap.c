/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pop_heap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 18:43:07 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 02:10:54 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Todo:	関数の切り分け
/// 		行数制限の回避
/// """
#include "../../hdrs/codexion.h"

/// """
///
/// """
static int	get_smallest_child(t_heap *heap, int count, int left, int right)
{
	int	smallest;

	smallest = count;
	if (left < heap->size && is_higher_priority(
			heap->data[left], heap->data[smallest], heap->scheduler_type))
		smallest = left;
	if (right < heap->size && is_higher_priority(
			heap->data[right], heap->data[smallest], heap->scheduler_type))
		smallest = right;
	return (smallest);
}

static void	heapify_down(t_heap *heap, int i)
{
	int		left;
	int		right;
	int		smallest;
	t_coder	*temp;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = get_smallest_child(heap, i, left, right);
		if (smallest != i)
		{
			temp = heap->data[i];
			heap->data[i] = heap->data[smallest];
			heap->data[smallest] = temp;
			i = smallest;
		}
		else
			break ;
	}
}

///
/// senntou toridashi saikoutiku
///
t_coder	*pop_heap(t_heap *heap)
{
	t_coder	*root;

	if (heap->size <= 0)
		return (NULL);
	root = heap->data[0];
	heap->size --;
	if (heap->size == 0)
		return (root);
	heap->data[0] = heap->data[heap->size];
	heapify_down(heap, 0);
	return (root);
}

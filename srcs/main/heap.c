/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 18:43:07 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 22:16:13 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// """
// Todo:	関数の切り分け
// 			行数制限の回避
// """
#include "../../hdrs/codexion.h"

int	is_higher_priority(t_coder *a, t_coder *b, int scheduler_type)
{
	// FIFO
	if (scheduler_type == 0)
	{
		if (a->request_time != b->request_time)
			return (a->request_time < b->request_time);
		return (a->id < b->id);
	}
	// EDF
	else
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
		if (a->request_time != b->request_time)
			return (a->request_time < b->request_time);
		return (a->id < b->id);
	}
}

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

void	free_heap(t_heap *heap)
{
	if (heap)
	{
		if (heap->data)
			free(heap->data);
		free(heap);
	}
}

void	push_heap(t_heap *heap, t_coder *coder)
{
	int	i;
	int	parent;
	t_coder	*temp;

	if (heap->size >= heap->capacity)
		return ;
	i = heap->size;
	heap->data[i] = coder;
	heap->size++;
	while (i != 0)
	{
		parent = (i - 1) / 2;
		if (is_higher_priority(heap->data[i], heap->data[parent], heap->scheduler_type))
		{
			temp = heap->data[i];
			heap->data[i] = heap->data[parent];
			heap->data[parent] = temp;
			i = parent;
		}
		else
			break;
	}
}

t_coder	*pop_heap(t_heap *heap)
{
	t_coder	*root;
	t_coder	*temp;
	int	i;
	int	left;
	int	right;
	int smallest;

	if (heap->size <= 0)
		return (NULL);
	root = heap->data[0];
	heap->size --;
	if (heap->size == 0)
		return (root);
	heap->data[0] = heap->data[heap->size];
	i = 0;
	while (i)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size && is_higher_priority(heap->data[left], heap->data[smallest], heap->scheduler_type))
			smallest = left;
		if (right < heap->size && is_higher_priority(heap->data[right], heap->data[smallest], heap->scheduler_type))
			smallest = right;
		if (smallest != i)
		{
			temp = heap->data[i];
			heap->data[i] = heap->data[smallest];
			heap->data[smallest] = temp;
			i = smallest;
		}
		else
			break;
	}
	return (root);
}

int	is_empty_heap(t_heap *heap)
{
	return (heap->size == 0);
}

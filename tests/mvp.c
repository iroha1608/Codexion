/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mvp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 18:23:40 by nsato             #+#    #+#             */
/*   Updated: 2026/06/22 19:03:31 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int	shared_counter = 0;
pthread_mutex_t	counter_mutex;

int 	is_ready = 0;
pthread_cond_t	ready_cond;
pthread_mutex_t	ready_mutex;

// Thread A routine
void	*thread_a_routine()
{
	int	i;

	i = 0;
	printf("[Thread A] Start. Increment counter.\n");
	while (i < 10)
	{
		// 共有している変数に触る前に鍵をかける
		pthread_mutex_lock(&counter_mutex);
		shared_counter ++;
		printf("[Thread A] Start. ++: %d\n", shared_counter);
		// 終わったら鍵を開ける
		pthread_mutex_unlock(&counter_mutex);
		usleep(50000);
		i ++;
	}

	// Thread Bにシグナルを送る
	pthread_mutex_lock(&ready_mutex);
	is_ready = 1;
	// Thread Bを起こす
	pthread_cond_signal(&ready_cond);
	printf("[Thread A] Signal to [Thread B].\n");
	pthread_mutex_unlock(&ready_mutex);
	return NULL;
}

// Thread B routine
void	*thread_b_routine()
{
	printf("[Thread B] is waiting [Thread A]'s signals...\n");

	pthread_mutex_lock(&ready_mutex);
	while (is_ready == 0)
		pthread_cond_wait(&ready_cond, &ready_mutex);
	pthread_mutex_unlock(&ready_mutex);
	printf("[Thread B] is caught [Thread A]'s signals!: %d\n", shared_counter);

	return NULL;
}

int	main(void)
{
	pthread_t	thread_a;
	pthread_t	thread_b;

	pthread_mutex_init(&counter_mutex, NULL);
	pthread_mutex_init(&ready_mutex, NULL);
	pthread_cond_init(&ready_cond, NULL);

	pthread_create(&thread_a, NULL, thread_a_routine, NULL);
	pthread_create(&thread_b, NULL, thread_b_routine, NULL);

	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);

	pthread_mutex_destroy(&counter_mutex);
	pthread_mutex_destroy(&ready_mutex);
	pthread_cond_destroy(&ready_cond);

	printf("[Main] All process is complete.\n");
	return 0;
}

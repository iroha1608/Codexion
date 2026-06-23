/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mvp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 18:23:40 by nsato             #+#    #+#             */
/*   Updated: 2026/06/23 16:02:24 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 共有リソース。Codexion内のドングル
int				shared_counter = 0;
// 共有リソースの鍵。pthread_mutex_t型
pthread_mutex_t	counter_mutex;

// シグナルを送るための鍵(mutex)と条件変数(condition variable)。
// pthread_mutex_t型、pthread_cond_t型
int 			is_ready = 0;
pthread_mutex_t	ready_mutex;
pthread_cond_t	ready_cond;

// Thread A routine
// カウンター(共有リソース)を増やし最後にシグナルを送る。
void	*thread_a_routine()
{
	int	i;

	i = 0;
	printf("[Thread A] Start. Increment counter.\n");
	while (i < 10)
	{
		// リソースを取り合わないために共有している変数に触る前に鍵をかける。
		pthread_mutex_lock(&counter_mutex);
		shared_counter ++;
		printf("[Thread A] Start. count ++: %d\n", shared_counter);
		// 終わったら鍵を開ける
		pthread_mutex_unlock(&counter_mutex);
		// 0.5秒待機
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
// Aのシグナルを受け取ってから動く。
void	*thread_b_routine()
{
	printf("[Thread B] is waiting [Thread A]'s signals...\n");

	pthread_mutex_lock(&ready_mutex);
	// is_readyが1になるまで(Aが終わるまで)眠って待つ。
	// Whileループ内でビジーウェイトしない。
	while (is_ready == 0)
		// pthread_cond_waitは呼ばれると自動で鍵を開けて眠る。
		// 起こされたときに自動で鍵を締めて再開する。
		// 条件変数(ready_cond)にシグナルが送られてくるまでブロック状態
		pthread_cond_wait(&ready_cond, &ready_mutex);
	pthread_mutex_unlock(&ready_mutex);
	printf("[Thread B] is caught [Thread A]'s signals!: Final count is %d.\n", shared_counter);

	return NULL;
}

int	main(void)
{
	pthread_t	thread_a;
	pthread_t	thread_b;

	// 鍵と条件変数の初期化
	pthread_mutex_init(&counter_mutex, NULL);
	pthread_mutex_init(&ready_mutex, NULL);
	pthread_cond_init(&ready_cond, NULL);

	// スレッドの作成。A, Bが同時に動き始める。
	pthread_create(&thread_a, NULL, thread_a_routine, NULL);
	pthread_create(&thread_b, NULL, thread_b_routine, NULL);

	// スレッドの終了を待つ。
	// これがないとA, Bが終わる前にプログラムが終了する。
	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);

	// 後片付け。
	pthread_mutex_destroy(&counter_mutex);
	pthread_mutex_destroy(&ready_mutex);
	pthread_cond_destroy(&ready_cond);

	printf("[\33[32mMain\33[0m] All process is complete!\n");
	return 0;
}

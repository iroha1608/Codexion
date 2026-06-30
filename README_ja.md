*This project has been created as part of the 42 curriculum by nsato.*

<table>
	<thead>
    	<tr>
      		<th style="text-align:center"><a href="README.md">英語</a></th>
      		<th style="text-align:center">日本語</th>
    	</tr>
  	</thead>
</table>

<h1>
	Codexion
</h1> <H2>
	締め切り前に資源争奪戦を制覇せよ
	さもないと、締め切りに翻弄されてしまう
</H2>

## 📖*目次*
1. [💡概要](#1-概要)
2. [📁ファイル構成](#2-ファイル構成)
3. [✅手順](#3-手順)
4. [⛏追加要件](#4-追加要件)
	1. [対処した並行処理上の問題](#4-1.-対処した並行処理上の問題)
	2. [スレッド同期メカニズム](2.-スレッド同期メカニズム)
5. [🌈リソース](#5-リソース)
	1. [参考URL](#5-1-参考URL)
	2. [AIの使用について](#5-2-AIの使用について)

## 💡1. 概要
限られたUSBドングルをめぐって競い合いながら、燃え尽き症候群との戦いにも直面する、過酷なシミュレーションを通じて、C言語による並行プログラミングを習得しましょう。  
POSIXスレッド、ミューテックス、コンディション変数、そして高度なスケジューリングアルゴリズム（FIFO/EDF）を実装し、リソースの共有を調整し、デッドロックを防止し、公平なアクセスを確保します。  
そのすべてを、締め切りが迫る中でプログラマーの生産性を維持しながら行います。(課題PDFより)  

## 📁2. ファイル構成
```
Codexion/
├── Makefile
├── README.md
├── README_ja.md
├── .gitignore
├── hdrs/
│   └── codexion.h
└── srcs/
    ├── main/
    │   ├── main.c
    │   └── simulation.c
    ├── init/
    │   ├── parse.c
    │   ├── init.c
    │   └── init_utils.c
    ├── core/
    │   ├── arbiter.c
    │   └── supervisor.c
    ├── coder/
    ｜   ├── coder.c
    ｜   └── coder_method.c
    ├── heap/
    │   ├── pop_heap.c
    │   ├── push_heap.c
    │   └── heap_utils.c
    └── utils/
         ├── utils.c
         ├── print.c
         └── cleanup.c
```

## ✅3. 手順
1. コンパイルし、実行ファイルを作成します。  
```bash
make
# rootに'codexion'が作成されます。
```
2. 実行時、下記の引数を指定します。
```
./codexion 1.コーダーの数 2.バーンアウトまでの時間 3.コンパイル時間 4.リファクタリング時間 5.デバッグ時間 6. コンパイル回数 7. ドングルクールタイム 8. fifo/edf
```

- *実行例:*
```bash
./codexion 4 800 200 200 200 7 100 edf
```
- 作成したオブジェクトファイルを削除
```bash
make clean
```
- 作成したオブジェクトファイル、実行ファイルを削除
```bash
make clean
```

- 再コンパイル
```bash
make re
```

## ⛏4. 追加要件
### 4-1. 対処した並行処理上の問題

- コフマンの条件  

|条件名|定義・詳細|
|----|----|
|相互排除 (Mutual Exclusion)|1つの資源を同時に複数のタスクが使用できず、占有状態である。|
|保持と待機 (Hold and Wait)|少なくとも1つの資源を占有したまま、別のタスクが仕様中の資源を要求して待機している状態。|
|横取り不可 (No Preemption)|タスクが仕様中の資源を、他のタスクが強制的に奪い取ることができない。|
|循環待機 (Circular Wait)|複数のタスク間で、資源を要求する順序がループになっている。|

- デッドロックとは、上記の4条件が全て揃うことで発生する可能性があります。つまり、この条件のどれか1つを壊すことでデッドロックの防止ができます。  
- 例えばCodexionで言えば、コーダーAとコーダーBの二人がいたとして、コーダーAがドングルを1つ保持したままコーダーBもドングルを保持してしまうと、お互いに相手のドングルがないとコンパイルができず、永遠に相手のドングルが空くまで待ってしまいデッドロックが発生します。  
- 今回これを壊すために、コーダーとは別にドングルの管理者としてArbiterを用意しました。Arbiterはコーダーからドングルの要求リクエストを受け取ると、優先度付きキューで保持された順番に基づいて選ばれたコーダーに対し、そのコーダーの左右のドングルが空いているかをチェックします。もし2つとも空いていれば、2つを同時にコーダーに渡し、もし片方、または両方が空いていなければ、そのコーダーが使えるようになるまで他のコーダーにはそのドングルを渡さずに待ちます。これにうよってドングルを一つだけ持ち待機するコーダーはいなくなり、保持と待機(同時に循環待機も)壊れデッドロックは発生しなくなります。  
- スターベーションの防止について、優先度付きキューでコーダーの要求を管理することで、オプションがFIFOならリクエストが届いた順番、EDFならデッドラインが近い順版にコーダーにドングルを渡します。  
- 正確なバーンアウト検出について、各コーダーが全員準備ができたタイミングで現在時刻をマイクロ秒で取得し、一斉にコーダールーティーンが実行されます。このタイミングで中央にSupervisorを作成し、常に最もバーンアウトが近い時間までpthread_cond_timedwaitで待機します。この待機中にも常にコーダーからのシグナルを受け取れるようにし、もしバーンアウト時間が更新されれば新しく最も近いバーンアウトの時間を計算し直し、また待機を繰り返します。また、計算や時間の保持はマイクロ秒でし、出力するときにミリ秒にするため可能な限り出力の順番と実際の順番がズレないようにしました。  

### 4-2. スレッド同期メカニズム
- 今回使用しているmutex/condition varialbeは下記の通り

- pthread_mutex  

|mutex変数名|保護している変数|どの処理で使われているか|
|----|----|----|
|time_mutex|data->is_simulation_running|メインループを継続するかの確認、更新|
||data->ready_count|コーダー全員がスタートラインに立ったかの確認|
||coder->last_compile_start|コンパイル開始時間の記録|
||coder->compile_count|コンパイル完了回数のインクリメント、確認|
||coder->deadline|コーダー自身のバーンアウトの時間の保持|
|scheduler_mutex|data->dogles|ドングルの状態変更、クールダウン時間の更新|
||data->wait_queue|スケジューラの順番待ちキューへの追加|
||coder->in_queue|キューからの取り出し|
||coder->request_time|キューから取り出す際の比較材料|
|print_mutex|stdout|複数のコーダーが同時にprintfを呼び、文字が混ざるのを防ぐ|

- pthread_cond_t  

|cond変数名|待機する人|起こす人|役割|
|----|----|----|----|
|start_cond|全コーダー|メインスレッド|全コーダーが作られるまで待機させ、メインスレッドが合図を出し一斉にメインループを開始する|
|exit_cond|コンパイル、デバッグ、リファクタ中のコーダー|stop_simulation()|もし誰かがバーンアウトした時に待機しているコーダーを即座に動かしシミュレーションを終了する|
|sv_cond|Supervisor|コーダー|Supervisorが無限ループやusleepで待機しないように、次のバーンアウトの時間まで待機させる|

- 同時に複数のスレッドが同じ変数を読み書きしてデータ競合(Data Race)が起きるのを防ぐために、共有変数にはそれぞれ対応するmutexを使用。  
- これに加えて必要最低限のクリティカルセクションを保護するためにmutexを使用し、競合状態(Race Condition)を避けている。  
- これらのmutexes/condition variableを使用することで、安全に共有リソースにアクセスすることができスレッドセーフである。  

## 🌈5. リソース

### 5-1. 参考URL
- [Oracle Help Center Pthreads ライブラリ](https://docs.oracle.com/cd/E19253-01/819-0390/tlib-1/index.html)  
- [Linux Programmer's Manual (7) PTHREADS](https://linuxjm.sourceforge.io/html/LDP_man-pages/man7/pthreads.7.html)  
- [Linux Programmer's Manual (3) PTHREAD_CREATE](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/pthread_create.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_JOIN](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/pthread_join.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_MUTEX_*](https://linuxjm.sourceforge.io/html/glibc-linuxthreads/man3/pthread_mutex_init.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_COND_*](https://linuxjm.sourceforge.io/html/glibc-linuxthreads/man3/pthread_cond_init.3.html)  
- [Linux Programmer's Manual (2) GETTIMEOFDAY](https://linuxjm.sourceforge.io/html/LDP_man-pages/man2/gettimeofday.2.html)  
- [Linux Programmer's Manual (3) USLEEP](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/usleep.3.html)  
### 5-2. AIの使用について

- gemini/Antigravityを使用  
	- 要件定義、こちらの曖昧な構想に対し質問をしてもらい、設計を固めた。
	- 全体の設計後にも目標日時までのタスク分けをしてもらい、それに沿って進行。
	- 最初に作成した設計からTodoリストの作成、日報の作成。
	- 学習、復習のために、進行時の細かな疑問点をまとめてファイルに出力。
	- 翻訳。
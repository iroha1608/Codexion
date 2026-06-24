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
	タイトル
</h1> <H2>
	概要
</H2>


## 📖*目次*
1. [💡概要](#1-概要)
2. [📁File Structure](#2-ファイル構成)
3. [✅手順](#3-手順)
4. [⛏追加要件](#4-追加要件)
5. [📃入出力例](#5-入出力例)
6. [🎁ボーナス](#6-ボーナス)
7. [🌈リソース](#7-リソース)
	1. [参考URL](#7-1-参考URL)
	2. [AIの使用について](#7-2-AIの使用について)


## 💡1. 概要
## 📁2. ファイル構成
```
Codexion/
├── Makefile
├── hdrs/
│   └── codexion.h
└── srcs/
    ├── main/       (プログラムの開始と実行フロー制御)
    │   ├── main.c
    │   └── simulation.c
    ├── init/       (引数解析、初期化、後片付け)
    │   ├── parse.c
    │   ├── init.c
    │   └── cleanup.c
    ├── core/       (シミュレーションの核となる管理者)
    │   ├── arbiter.c
    │   └── supervisor.c
    ├── utils/      (ユーティリティ、データ構造、出力関数)
    │   ├── utils.c
    │   ├── print.c
    │   └── heap.c
    └── coder/      (コーダーのライフサイクルと振る舞い)
        └── coder.c
```

## ✅3. 手順
## ⛏4. 追加要件
## 📃5. 入出力例
## 🎁6. ボーナス
## 🌈7. リソース
### 7-1. 参考URL
- [Oracle Help Center Pthreads ライブラリ](https://docs.oracle.com/cd/E19253-01/819-0390/tlib-1/index.html)  
- [Linux Programmer's Manual (7) PTHREADS](https://linuxjm.sourceforge.io/html/LDP_man-pages/man7/pthreads.7.html)  
- [Linux Programmer's Manual (3) PTHREAD_CREATE](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/pthread_create.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_JOIN](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/pthread_join.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_MUTEX_*](https://linuxjm.sourceforge.io/html/glibc-linuxthreads/man3/pthread_mutex_init.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_COND_*](https://linuxjm.sourceforge.io/html/glibc-linuxthreads/man3/pthread_cond_init.3.html)  
- [Linux Programmer's Manual (2) GETTIMEOFDAY](https://linuxjm.sourceforge.io/html/LDP_man-pages/man2/gettimeofday.2.html)  
- [Linux Programmer's Manual (3) USLEEP](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/usleep.3.html)  
### 7-2. AIの使用について

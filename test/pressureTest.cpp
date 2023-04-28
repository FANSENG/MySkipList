/**
 * TODO 1. 压力测试只单独测试了 Insert 和 Find，没有测试一个进程进行 Insert，一个进程 Find。
 * TODO 2. 进行详细的压力测试，整理出一个性能表。
*/

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include "../src/include/skipList.h"

// 线程数
#define NUM_THREADS 1

// 测试数据量
#define TEST_COUNT 1000000

bool cmp(int &&a, int &&b){
    return a < b;
}

// 跳表
SkipList<int, string> skipList(18, 0.5, cmp);

void *insertElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    cout << tid << endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.Insert(rand() % TEST_COUNT, "a"); 
	}
    pthread_exit(NULL);
}

void *getElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    cout << tid << endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.Find(rand() % TEST_COUNT); 
	}
    pthread_exit(NULL);
}

int main() {
    srand (time(NULL)); 

    cout << "开始插入测试; 线程数: " << NUM_THREADS << "; 测试数据量： " << TEST_COUNT << endl; 
    {
        pthread_t threads[NUM_THREADS];
        int rc;
        size_t i;

        auto start = chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            cout << "main() : 创建线程: " << i << endl;
            rc = pthread_create(&threads[i], NULL, insertElement, (void*)i);

            if (rc) {
                cout << "Error:无法创建线程;error code:" << rc << endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("线程创建错误"); 
                exit(3);
            }
        }
        auto finish = chrono::high_resolution_clock::now(); 
        chrono::duration<double> elapsed = finish - start;
        cout << "插入耗时:" << elapsed.count() << endl;
    }

    // skipList.DisplayList();
    
    {
        pthread_t threads[NUM_THREADS];
        int rc;
        size_t i;
        auto start = chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            cout << "main() : 创建线程: " << i << endl;
            rc = pthread_create(&threads[i], NULL, getElement, (void *)i);

            if (rc) {
                cout << "Error:无法创建线程;error code:" << rc << endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("线程创建错误"); 
                exit(3);
            }
        }

        auto finish = chrono::high_resolution_clock::now(); 
        chrono::duration<double> elapsed = finish - start;
        cout << "查询耗时:" << elapsed.count() << endl;
    }

	pthread_exit(NULL);
    return 0;

}
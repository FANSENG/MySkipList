/**
 * DONE 1. 目前只有 Insert 和 Remove 加了锁，Find 没有加锁，读写冲突仍会存在，应该去加读写锁。
 * DONE 2. 要支持自定义比较函数
 * Done 3. 序列化&反序列化，将跳表以二进制形式存储到内存中，只需要存储节点中的 key & value。
 * !Q   存储和加载的数据中不能包含指针，必须为定长值，string也不能包含(底层为 char* 实现)。
*/

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstring>
#include <shared_mutex>
#include <random>
#include <functional>
#include <iostream>
#include <fstream>
#include "node.h"

using namespace std;

template<typename K, typename V>
class SkipList{
public:
    SkipList(int maxL, double p, function<bool(K&&, K&&)> compare);
    ~SkipList();

    /// @brief 查找跳表中是否存在 键K
    /// @param 待查找的键
    /// @return 若存在，返回对应节点，不存在则返回空指针
    Node<K, V>* Find(K);

    /// @brief 插入节点
    /// @param 待插入节点的键
    /// @param 待插入节点的值
    /// @return 插入是否成功
    bool Insert(K, V);

    /// @brief 移除节点
    /// @param 待移除节点的键
    /// @return 删除是否成功
    void Remove(K);

    /// @brief 打印显示跳表
    void DisplayList();
    
    /// @brief 获取跳表的节点数
    /// @return 当前跳表节点数
    int Size(){
        return nodeCount;
    }

    /// @brief 获取跳表当前level
    /// @return 跳表level
    int GetLevel(){
        return nowLevel;
    }

    /// @brief 清空跳表
    void Clear();

    /// @brief 将当前跳表的内容存储到文件中
    /// @param path 存储文件路径
    void Save(const string &path);

    /// @brief 将文件中的内容加载到跳表中
    /// @param path 存储文件路径
    /// @return 加载数据个数
    int Load(const string &path);

private:
    /// @brief 创建新节点
    /// @param  level
    /// @param  key
    /// @param  value
    /// @return new Node
    Node<K, V>* createNode(int, K, V);

    /// @brief 随机生成level
    /// @return new level
    int getRandomLevel();

private:
    /// @brief 多线程情况下保证读写安全
    shared_mutex mtx;

    /// @brief 跳表最多级别数
    int maxLevel;

    /// @brief 当前跳表级别数
    int nowLevel;

    /// @brief 跳表节点升级概率
    double probability;

    /// @brief 跳表头
    Node<K, V> *header;

    /// @brief 跳表中节点数
    size_t nodeCount;

    /// @brief 随机数引擎
    default_random_engine randomEngine;
    uniform_real_distribution<double> getRandomFloat;

    /// @brief 自定义比较函数
    function<bool(K&&, K&&)> cmp;
};

template<typename K, typename V>
SkipList<K, V>::SkipList(int maxL, double p, function<bool(K&&, K&&)> compare): 
    maxLevel(maxL), probability(p), cmp(compare), nowLevel(0), nodeCount(0),
    getRandomFloat(uniform_real_distribution<double>(0.0, 1.0)){
    K k;
    V v;
    header = new Node<K, V>(k, v, maxLevel);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList(){
    // 循环删除所有节点，防止内存泄漏
    // ?只删除了第0层的，其他层的指针仍然没有删除，存在野指针的问题
    // TODO 从高向低删除，先把上方的指针都清除了，最后删除下面的节点
    // TODO header 的 forward数组也要都置为Nullptr
    
    Node<K, V>* cur = header->forward[0];
    Node<K, V>* tmp;
    while(cur){
        tmp = cur->forward[0];
        delete cur;
        cur = tmp;  
    }
    delete header; 
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::Find(K key){
    shared_lock<shared_mutex> locker(mtx);

    Node<K, V>* current = header;

    for(int i = nowLevel; i >= 0; --i){
        // while(current->forward[i] && current->forward[i]->Key() < key) current = current->forward[i];
        while(current->forward[i] && cmp(current->forward[i]->Key(), move(key))) current = current->forward[i];
    }
    current = current->forward[0];
    if(current && current->Key() == key) return current;
    
    return nullptr;
}

template<typename K, typename V>
bool SkipList<K, V>::Insert(K key, V value){
    unique_lock<shared_mutex> locker(mtx);

    Node<K, V>* current = header;

    Node<K, V>* update[maxLevel + 1];
    memset(update, 0, sizeof(void*) * (maxLevel + 1));

    // 记录插入节点在第i层插入位置的上一个节点
    for(int i = nowLevel; i >= 0; --i){
        // while(current->forward[i] && current->forward[i]->Key() < key) current = current->forward[i];
        while(current->forward[i] && cmp(current->forward[i]->Key(), move(key))) current = current->forward[i];
        update[i] = current;
    }

    current = current->forward[0];

    if(current != nullptr && current->Key() == key){
        // 插入键值已存在
        return false;
    }
    if(current == nullptr || current->Key() != key){
        int randomLevel = getRandomLevel();
        for(int i = nowLevel + 1; i <= randomLevel; ++i){
            update[i] = header;
        }
        nowLevel = max(randomLevel, nowLevel);
        Node<K, V>* insertNode = createNode(randomLevel, key, value);
        
        // 在每一层都插入 insertNode
        for(int i = 0; i <= randomLevel; ++i){
            insertNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = insertNode;
        }
        ++nodeCount;
    }
    return true;
}

template<typename K, typename V>
void SkipList<K, V>::Remove(K key){
    unique_lock<shared_mutex> locker(mtx);

    Node<K, V>* current = header;
    Node<K, V>* update[maxLevel + 1];
    memset(update, 0, sizeof(void*) * (maxLevel + 1));

    for(int i = nowLevel; i >= 0; --i){
        // while(current->forward[i] && current->forward[i]->Key() < key) current = current->forward[i];
        while(current->forward[i] && cmp(current->forward[i]->Key(), key)) current = current->forward[i];
        update[i] = current;
    }

    current = current->forward[0];
    if(current && current->Key() == key){
        for(int i = current->level; i >= 0; --i){
            update[i]->forward[i] = current->forward[i];
        }

        while(nowLevel > 0 && header->forward[nowLevel] == nullptr) --nowLevel;
        // 要注意 delete 防止内存泄露
        delete current;
        --nodeCount;
    }
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::createNode(int level, K k, V v){
    return new Node<K, V>(k, v, level);
}

template<typename K, typename V>
int SkipList<K, V>::getRandomLevel(){
    int res = 0;
    // p^(i - 1) 的概率构建第i级索引
    while(getRandomFloat(randomEngine) < probability) res++;
    return (res < maxLevel) ? res : maxLevel;
}

template<typename K, typename V>
void SkipList<K, V>::DisplayList(){
    cout << "====================display list BEGIN====================" << endl << endl;
    for(int i = nowLevel; i >= 0; --i){
        Node<K, V>* ptr = header->forward[i];
        cout << "Level: " << i << endl;
        while(ptr){
            cout << ptr->Key() << ":" << ptr->Value() << ";  ";
            ptr = ptr->forward[i];
        }
        cout << endl;
    }
    cout << "=====================display list END=====================" << endl << endl;
}

template<typename K, typename V>
void SkipList<K, V>::Clear(){
    // 循环删除所有节点，防止内存泄漏
    Node<K, V>* cur = header->forward[0];
    Node<K, V>* tmp;
    while(cur){
        tmp = cur->forward[0];
        delete cur;
        cur = tmp;  
    }
}

template<typename K, typename V>
void SkipList<K, V>::Save(const string &path){
    ofstream outFile(path, ios::out|ios::binary);

    Node<K, V>* cur = header->forward[0];
    while(cur){
        outFile.write(reinterpret_cast<char*>(&cur->data), sizeof(cur->data));
        cur = cur->forward[0];
    }
    outFile.close();
}

template<typename K, typename V>
int SkipList<K, V>::Load(const string &path){
    nodeData<K, V> *tmp = new nodeData<K, V>();
    this->Clear();
    ifstream inFile(path, ios::in|ios::binary);
    if(!inFile) {
        cout << "Load File Open Error" <<endl;
        return 0;
    }
    while(inFile.read(reinterpret_cast<char*>(tmp), sizeof(*tmp))){
        this->Insert(tmp->key, tmp->value);
    }
    inFile.close();
    return this->nodeCount;
}

#endif  // SKIPLIST_H
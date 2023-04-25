#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include "node.h"

using namespace std;

template<typename K, typename V>
class SkipList{
public:
    SkipList(int maxL, int p);
    ~SkipList();

    /// @brief 查找跳表中是否存在 键K
    /// @param 待查找的键
    /// @return 若存在，返回对应节点，不存在则返回空指针
    Node<K, V>* Find(K) const;

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
    mutex mtx;

    /// @brief 跳表最多级别数
    int maxLevel;

    /// @brief 当前跳表级别数
    int nowLevel;

    /// @brief 2 ~ 5
    // 2 : 50% 概率建立下一级索引
    // 3 : 33% 概率建立下一级索引
    // 4 : 25% 概率建立下一级索引
    // 5 : 20% 概率建立下一级索引
    int probability;

    /// @brief 跳表头
    Node<K, V> *header;

    /// @brief 跳表中节点数
    size_t nodeCount;
};

template<typename K, typename V>
SkipList<K, V>::SkipList(int maxL, int p): maxLevel(maxL), probability(p), nowLevel(0), nodeCount(0){
    K k;
    V v;
    header = new Node<K, V>(k, v, maxLevel);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList(){ delete header; }

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::Find(K key) const{
    Node<K, V>* current = header;

    for(int i = nowLevel; i >= 0; --i){
        while(current->forward[i] && current->forward[i]->Key() < key) current = current->forward[i];
    }
    current = current->forward[0];
    if(current && current->Key() == key) return current;
    
    return nullptr;
}

template<typename K, typename V>
bool SkipList<K, V>::Insert(K key, V value){
    lock_guard<mutex> logker(mtx);

    Node<K, V>* current = header;

    Node<K, V>* update[maxLevel + 1];
    memset(update, 0, sizeof(void*) * (maxLevel + 1));

    // 记录插入节点在第i层插入位置的上一个节点
    for(int i = nowLevel; i >= 0; --i){
        while(current->forward[i] && current->forward[i]->Key() < key) current = current->forward[i];
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
    lock_guard<mutex> locker(mtx);
    Node<K, V>* current = header;
    Node<K, V>* update[maxLevel + 1];
    memset(update, 0, sizeof(void*) * (maxLevel + 1));

    for(int i = nowLevel; i >= 0; --i){
        while(current->forward[i] && current->forward[i]->Key() < key) current = current->forward[i];
        update[i] = current;
    }

    current = current->forward[0];
    if(current && current->Key() == key){
        for(int i = 0; i <= nowLevel; ++i){
            if(update[i]->forward[i] != current) break;
            update[i]->forward[i] = current->forward[i];
        }

        while(nowLevel > 0 && header->forward[nowLevel] == nullptr) --nowLevel;
        --nodeCount;
    }
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::createNode(int level, K k, V v){
    return new Node<K, V>(k, v, level);
}

template<typename K, typename V>
int SkipList<K, V>::getRandomLevel(){
    int res = 1;
    // 1 / p 的概率构建二级索引 (1 / p)^2 构建三级....
    while (rand() % probability) res++; 
    return (res < maxLevel) ? res : maxLevel;
}

template<typename K, typename V>
void SkipList<K, V>::DisplayList(){
    cout << "====================display list BEGIN====================" << endl;
    for(int i = nowLevel; i >= 0; --i){
        Node<K, V>* ptr = header->forward[i];
        cout << "Level: " << i << endl;
        while(ptr){
            cout << ptr->Key() << " : " << ptr->Value() << ";";
            ptr = ptr->forward[i];
        }
        cout << endl;
    }
    cout << "=====================display list END=====================" << endl;
}

#endif  // SKIPLIST_H
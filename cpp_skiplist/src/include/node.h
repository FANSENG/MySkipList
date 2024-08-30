#ifndef NODE_H
#define NODE_H

template<typename K, typename V>
class SkipList;

template<typename K, typename V>
struct nodeData{
    K key;
    V value;
};

template<typename K, typename V>
class Node{
    friend class SkipList<K, V>;

public:
    Node() { }
    Node(K, V, int);
    ~Node();

    K Key() const;
    V Value() const;
    void SetValue(V);
private:
    // K key;
    // V value;
    nodeData<K, V> data;
    Node<K, V> **forward;
    int level;
};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, const int l): data({k, v}), level(l){
    forward = new Node<K, V>*[level + 1];
    memset(forward, 0, sizeof(void*) * (level + 1));
}

template<typename K, typename V>
Node<K, V>::~Node(){
    delete [] forward;
}

template<typename K, typename V>
K Node<K, V>::Key() const{ return data.key; }

template<typename K, typename V>
V Node<K, V>::Value() const{ return data.value; }

template<typename K, typename V>
void Node<K, V>::SetValue(V v){ data.value = v; }

#endif  //NODE_H
#include <iostream>
#include "node.h"
#include "skipList.h"

bool cmp(int &&a, int &&b){
    return a < b;
}

void baseTest(){
    SkipList<int, int> skiplist(6, 0.5, cmp);
    skiplist.Insert(1, 10);
    skiplist.Insert(2, 20);
    skiplist.Insert(3, 30);
    skiplist.Insert(4, 40);
    skiplist.Insert(5, 50);
    skiplist.Insert(6, 60);
    skiplist.Insert(7, 70);
    skiplist.Insert(8, 80);
    skiplist.Insert(9, 90);
    skiplist.Insert(12345, 100);
    Node<int, int>* tmp = skiplist.Find(7);
    if(tmp != nullptr)
        cout << "Find: [" << to_string(tmp->Key()) << ", " << tmp->Value() << "]" << endl;

    tmp = skiplist.Find(124);
    if(tmp != nullptr)
        cout << "Find: [" << to_string(tmp->Key()) << ", " << tmp->Value() << "]" << endl;

    skiplist.DisplayList();
}

void loadTest(){
    SkipList<int, int> skiplist(6, 0.5, cmp);
    skiplist.Load("../data/0.dat");
    skiplist.DisplayList();
}

void saveTest(){
    SkipList<int, int> skiplist(6, 0.5, cmp);
    skiplist.Insert(1, 10);
    skiplist.Insert(2, 20);
    skiplist.Insert(3, 30);
    skiplist.Insert(4, 40);
    skiplist.Insert(5, 50);
    skiplist.Insert(6, 60);
    skiplist.Insert(7, 70);
    skiplist.Insert(8, 80);
    skiplist.Insert(9, 90);
    skiplist.Insert(12345, 100);
    skiplist.DisplayList();

    skiplist.Save("../data/0.dat");
}

int main(){
    // loadTest();

    // saveTest();

    baseTest();

    return 0;
}
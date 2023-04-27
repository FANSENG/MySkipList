#include <iostream>
#include "node.h"
#include "skipList.h"

bool cmp(int &&a, int &&b){
    return a < b;
}

int main(){
    SkipList<int, string> skiplist(6, 0.5, cmp);
    skiplist.Insert(1, "测试1");
    skiplist.Insert(2, "测试2");
    skiplist.Insert(3, "测试3");
    skiplist.Insert(4, "测试4");
    skiplist.Insert(5, "测试5");
    skiplist.Insert(6, "测试6");
    skiplist.Insert(7, "测试7");
    skiplist.Insert(8, "测试8");
    skiplist.Insert(9, "测试9");
    skiplist.Insert(12345, "测试10");
    Node<int, string>* tmp = skiplist.Find(7);
    if(tmp != nullptr)
        cout << "Find: [" << to_string(tmp->Key()) << ", " << tmp->Value() << "]" << endl;

    tmp = skiplist.Find(124);
    if(tmp != nullptr)
        cout << "Find: [" << to_string(tmp->Key()) << ", " << tmp->Value() << "]" << endl;

    skiplist.DisplayList();
    return 0;
}
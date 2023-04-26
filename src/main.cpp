#include <iostream>
#include "node.h"
#include "skipList.h"

int main(){
    SkipList<string, int> skiplist(6, 0.5);
    skiplist.Insert("测试1", 1);
    skiplist.Insert("测试2", 2);
    skiplist.Insert("测试3", 3);
    skiplist.Insert("测试4", 4);
    skiplist.Insert("测试5", 5);
    skiplist.Insert("测试6", 6);
    skiplist.Insert("测试7", 7);
    skiplist.Insert("测试8", 8);
    skiplist.Insert("测试9", 9);
    skiplist.Insert("测试10", 1213);
    Node<string, int>* tmp = skiplist.Find("测试7");
    if(tmp != nullptr)
        cout << "Find: [" << tmp->Key() << ", " << to_string(tmp->Value()) << "]" << endl;

    tmp = skiplist.Find("测试");
    if(tmp != nullptr)
        cout << "Find: [" << tmp->Key() << ", " << to_string(tmp->Value()) << "]" << endl;

    skiplist.DisplayList();

    return 0;
}
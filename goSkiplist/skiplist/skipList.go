package skiplist

import (
	"encoding/binary"
	"fmt"
	"io"
	"math/rand"
	"os"
	"sync"
)

// SkipList 表示跳表结构
type SkipList[K comparable, V any] struct {
	maxLevel    int
	nowLevel    int
	probability float64
	header      *Node[K, V]
	nodeCount   int
	cmp         func(K, K) bool
	mtx         sync.RWMutex
}

// NewSkipList 创建一个新的跳表
func NewSkipList[K comparable, V any](maxLevel int, probability float64, compare func(K, K) bool) *SkipList[K, V] {
	var k K
	var v V
	return &SkipList[K, V]{
		maxLevel:    maxLevel,
		nowLevel:    0,
		probability: probability,
		header:      NewNode(k, v, maxLevel),
		nodeCount:   0,
		cmp:         compare,
	}
}

// Find 在跳表中查找键对应的节点
func (sl *SkipList[K, V]) Find(key K) *Node[K, V] {
	sl.mtx.RLock()
	defer sl.mtx.RUnlock()

	current := sl.header
	for i := sl.nowLevel; i >= 0; i-- {
		for current.forward[i] != nil && sl.cmp(current.forward[i].Key(), key) {
			current = current.forward[i]
		}
	}
	current = current.forward[0]
	if current != nil && current.Key() == key {
		return current
	}
	return nil
}

// Insert 向跳表中插入新的键值对
func (sl *SkipList[K, V]) Insert(key K, value V) bool {
	sl.mtx.Lock()
	defer sl.mtx.Unlock()

	current := sl.header
	update := make([]*Node[K, V], sl.maxLevel+1)

	for i := sl.nowLevel; i >= 0; i-- {
		for current.forward[i] != nil && sl.cmp(current.forward[i].Key(), key) {
			current = current.forward[i]
		}
		update[i] = current
	}

	current = current.forward[0]

	if current != nil && current.Key() == key {
		return false
	}

	randomLevel := sl.getRandomLevel()
	if randomLevel > sl.nowLevel {
		for i := sl.nowLevel + 1; i <= randomLevel; i++ {
			update[i] = sl.header
		}
		sl.nowLevel = randomLevel
	}

	newNode := NewNode(key, value, randomLevel)

	for i := 0; i <= randomLevel; i++ {
		newNode.forward[i] = update[i].forward[i]
		update[i].forward[i] = newNode
	}

	sl.nodeCount++
	return true
}

// Remove 从跳表中移除指定键的节点
func (sl *SkipList[K, V]) Remove(key K) {
	sl.mtx.Lock()
	defer sl.mtx.Unlock()

	current := sl.header
	update := make([]*Node[K, V], sl.maxLevel+1)

	for i := sl.nowLevel; i >= 0; i-- {
		for current.forward[i] != nil && sl.cmp(current.forward[i].Key(), key) {
			current = current.forward[i]
		}
		update[i] = current
	}

	current = current.forward[0]
	if current != nil && current.Key() == key {
		for i := 0; i <= sl.nowLevel; i++ {
			if update[i].forward[i] != current {
				break
			}
			update[i].forward[i] = current.forward[i]
		}

		for sl.nowLevel > 0 && sl.header.forward[sl.nowLevel] == nil {
			sl.nowLevel--
		}
		sl.nodeCount--
	}
}

// DisplayList 打印显示跳表
func (sl *SkipList[K, V]) DisplayList() {
	fmt.Println("====================display list BEGIN====================")
	for i := sl.nowLevel; i >= 0; i-- {
		fmt.Printf("Level: %d\n", i)
		node := sl.header.forward[i]
		for node != nil {
			fmt.Printf("%v:%v;  ", node.Key(), node.Value())
			node = node.forward[i]
		}
		fmt.Println()
	}
	fmt.Println("=====================display list END=====================")
}

// Size 返回跳表中的节点数
func (sl *SkipList[K, V]) Size() int {
	return sl.nodeCount
}

// GetLevel 返回跳表的当前级别
func (sl *SkipList[K, V]) GetLevel() int {
	return sl.nowLevel
}

// Clear 清空跳表
func (sl *SkipList[K, V]) Clear() {
	sl.header.forward = make([]*Node[K, V], sl.maxLevel+1)
	sl.nowLevel = 0
	sl.nodeCount = 0
}

// Save 将跳表内容保存到文件
func (sl *SkipList[K, V]) Save(path string) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()

	node := sl.header.forward[0]
	for node != nil {
		err = binary.Write(file, binary.LittleEndian, node.data)
		if err != nil {
			return err
		}
		node = node.forward[0]
	}
	return nil
}

// Load 从文件加载内容到跳表
func (sl *SkipList[K, V]) Load(path string) (int, error) {
	file, err := os.Open(path)
	if err != nil {
		return 0, err
	}
	defer file.Close()

	sl.Clear()
	var data NodeData[K, V]
	for {
		err = binary.Read(file, binary.LittleEndian, &data)
		if err == io.EOF {
			break
		}
		if err != nil {
			return sl.nodeCount, err
		}
		sl.Insert(data.Key, data.Value)
	}
	return sl.nodeCount, nil
}

func (sl *SkipList[K, V]) getRandomLevel() int {
	level := 0
	for rand.Float64() < sl.probability && level < sl.maxLevel {
		level++
	}
	return level
}

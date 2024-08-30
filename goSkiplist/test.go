package main

import (
	"fmt"
	"fs1n/my_skiplist/skiplist"
	"math/rand"
)

func main() {
	// 创建一个新的跳表实例
	sl := skiplist.NewSkipList[int, string](16, 0.5, func(a, b int) bool {
		return a < b
	})

	// 插入一些数据
	fmt.Println("插入数据:")
	sl.Insert(30, "value-30")
	sl.Insert(70, "value-70")
	for i := 0; i < 10; i++ {
		key := rand.Intn(100)
		value := fmt.Sprintf("value-%d", key)
		if sl.Insert(key, value) {
			fmt.Printf("插入成功: %d -> %s\n", key, value)
		} else {
			fmt.Printf("插入失败: %d\n", key)
		}
	}

	// 显示跳表内容
	fmt.Println("\n跳表内容:")
	sl.DisplayList()

	// 查找操作
	fmt.Println("\n查找操作:")
	keysToFind := []int{30, 50, 70}
	for _, key := range keysToFind {
		if node := sl.Find(key); node != nil {
			fmt.Printf("找到键 %d: 值为 %s\n", key, node.Value())
		} else {
			fmt.Printf("未找到键 %d\n", key)
		}
	}

	// 删除操作
	fmt.Println("\n删除操作:")
	keysToRemove := []int{30, 50}
	for _, key := range keysToRemove {
		sl.Remove(key)
		fmt.Printf("删除键 %d\n", key)
	}

	// 再次显示跳表内容
	fmt.Println("\n删除后的跳表内容:")
	sl.DisplayList()

	// 输出跳表的一些统计信息
	fmt.Printf("\n跳表统计:\n")
	fmt.Printf("节点数量: %d\n", sl.Size())
	fmt.Printf("当前级别: %d\n", sl.GetLevel())
}

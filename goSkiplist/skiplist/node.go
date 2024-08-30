package skiplist

// NodeData 存储节点的键值对
type NodeData[K comparable, V any] struct {
	Key   K
	Value V
}

// Node 表示跳表中的一个节点
type Node[K comparable, V any] struct {
	data    NodeData[K, V]
	forward []*Node[K, V]
	level   int
}

// NewNode 创建一个新的节点
func NewNode[K comparable, V any](key K, value V, level int) *Node[K, V] {
	return &Node[K, V]{
		data:    NodeData[K, V]{Key: key, Value: value},
		forward: make([]*Node[K, V], level+1),
		level:   level,
	}
}

// Key 返回节点的键
func (n *Node[K, V]) Key() K {
	return n.data.Key
}

// Value 返回节点的值
func (n *Node[K, V]) Value() V {
	return n.data.Value
}

// SetValue 设置节点的值
func (n *Node[K, V]) SetValue(value V) {
	n.data.Value = value
}

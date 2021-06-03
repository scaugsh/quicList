# quicList
基于std::map实现的链表，支持快速查找值是否存在，快速删除值，需要保证链表不会出现重复值。建议直接使用quicKVList

# cache
基于quicList实现的LRU cache，支持过期lazy淘汰

# quicKVList 
和quicList功能类似，把key和value分开了，quicList的value理论上不支持修改，因为会用value去做map的key。如果value较大或者有改value需求则使用quicKVList

# FileStruct 
模拟文件目录的数据结构，支持文件、目录、软链接、硬链接

#include "ipv4.h"
#include <sched.h>              /* CPU_ZERO, CPU_SET */
#include <pthread.h>            /* pthread_* */
// #include <vector> 
#include <string.h>             /* memset */
#include <stdio.h>              /* printf */
#include <stdlib.h>             /* memalign */
#include <sys/time.h>           /* gettimeofday */
#include <stdbool.h>
#include <stddef.h>
// #include <linux/slab.h>  // 确保包含正确的头文件
// #include <linux/kernel.h>
//比如需要存储Ipv4
//定义一个存储Ipv4 的节点结构体
struct SNodeIpv4
{
    struct hlist_node hnode;
    uint64_t ipv4; // 改为 64 位
};
 
#define HASH_COUNT 1321 //指定hash bucket的数量
static struct hlist_head *ipv4_hash = NULL; //head

//计算hash值 
struct hlist_head *call_hash_ipv4(const uint64_t *ipv4) // 改为 64 位
{
    uint64_t val = 0;
    if (NULL == ipv4_hash || NULL == ipv4)
    {
        return NULL;
    }
    val = *ipv4 % HASH_COUNT;
    return &ipv4_hash[val];
}
  
//相等判断
int eq_ipv4(const uint64_t *_left, const uint64_t *_right) // 改为 64 位
{
    if (NULL == _left || NULL == _right)
    {
        return 0;
    }
 
    if (*_left ^ *_right)
    {
        return 0;
    }
    return 1;
}

int InitHashIpv4(void) {
    int i;
    int retCode = 0;

    // 分配内存
    ipv4_hash = (struct hlist_head *)malloc(sizeof(struct hlist_head) * HASH_COUNT);
    if (ipv4_hash == NULL) {
        // fprintf(stderr, "ipv4_hash alloc error\n");
        retCode = -1;
        return retCode;
    }

    // 初始化哈希表头
    for (i = 0; i < HASH_COUNT; i++) {
        ipv4_hash[i].first = NULL; // 假设的初始化
    }

    // printf("InitHashIpv4 success.\n");
    return retCode;
}

int AddIpv4(const uint64_t *_ipv4, uint32_t count) // 改为 64 位
{
    int i = 0;
    const uint64_t *node_ipv4 = _ipv4; // 改为 64 位
    struct hlist_head *hash_bucket = NULL;
    struct hlist_head **hash_buckets = (struct hlist_head **)malloc(sizeof (struct hlist_head *) * count);
    struct SNodeIpv4 *pNode = NULL;
    struct hlist_node *next = NULL;
    int retCode = 0;
    if (NULL == node_ipv4 || count < 1)
    {
        return -1;
    }
    for(int i=0; i<count; i++)
    {
        node_ipv4 = _ipv4 + i;
        hash_buckets[i] = call_hash_ipv4(node_ipv4);
    }
    for(i = 0; i < count; i++)
    {
        //找到bucket
        hash_bucket = hash_buckets[i];
        node_ipv4 = _ipv4 + i;
        if (NULL == hash_bucket)
        {
            retCode = -1;
            break; 
        }
 
        hlist_for_each_entry_safe(pNode, next, hash_bucket, hnode)
        {
            if (eq_ipv4(&pNode->ipv4, node_ipv4))
            {
                //已经存在
                continue;
            }
        }
        
        //分配一个node
        pNode = (struct SNodeIpv4 *)malloc(sizeof(struct SNodeIpv4));
        if (pNode == NULL)
        {
            // fprintf(stderr, "SNodeIpv4 alloc error at AddIpv4 function\n");
            retCode = -1;
            break;
        }
        memset(pNode, 0, sizeof(struct SNodeIpv4));
        INIT_HLIST_NODE(&pNode->hnode);
        memcpy(&pNode->ipv4, node_ipv4, sizeof(pNode->ipv4));
        hlist_add_head(&pNode->hnode, hash_bucket);
        // printf("AddIpv4 [%lu]\n", *node_ipv4); // 输出64位数值
    }
    free(hash_buckets);
    return retCode;
}

int FindIpv4(const uint64_t *_ipv4) // 改为 64 位
{
    struct hlist_head *hash_bucket = NULL;
    struct SNodeIpv4 *pNode = NULL;
    struct hlist_node *next = NULL;
    int retCode = -1;
 
    if (NULL == _ipv4)
    {
        return -1;
    }
 
    do
    {
        hash_bucket = call_hash_ipv4(_ipv4);
        if (NULL == hash_bucket)
        {
            retCode = -1;
            break;
        }
 
        hlist_for_each_entry_safe(pNode, next, hash_bucket, hnode)
        {
            if (eq_ipv4(&pNode->ipv4, _ipv4))
            {
                retCode = 0;
                break;
            }
        }
    } while(false);
 
    return retCode;
}

int DelIpv4(const uint64_t *_ipv4, uint32_t count) // 改为 64 位
{
    int i = 0;
    const uint64_t *node_ipv4 = _ipv4; // 改为 64 位
    struct hlist_head *hash_bucket = NULL;
    struct SNodeIpv4 *pNode = NULL;
    struct hlist_node *next = NULL;
 
    int retCode = 0;
 
    if (NULL == _ipv4 || count < 1)
    {
        return -1;
    }
 
    for(i = 0; i < count; ++i)
    {
        node_ipv4 = _ipv4 + i;
        hash_bucket = call_hash_ipv4(node_ipv4);
        if (NULL == hash_bucket)
        {
            retCode = -1;
            break;
        }
 
        hlist_for_each_entry_safe(pNode, next, hash_bucket, hnode)
        {
            if (eq_ipv4(&pNode->ipv4, node_ipv4))
            {
                hlist_del(&pNode->hnode);
                free(pNode);
            }
        }
    }
    return retCode;
}

// int DestoryHashIpv4(void)
// {
//     struct SNodeIpv4 *pNode = NULL;
//     struct hlist_node *next = NULL;
//     int i = 0;
//     int retCode = 0;
    
//     do
//     { 
//         if (NULL == ipv4_hash)
//         {
//             // fprintf(stderr, "ipv4_hash is NULL at AddIpv4 function\n");
//             retCode = -1; 
//             break;
//         }
 
//         for (i = 0; i < HASH_COUNT; i++)
//         {
//             hlist_for_each_entry_safe(pNode, next, &ipv4_hash[i], hnode)
//             {
//                 hlist_del(&pNode->hnode);  
//                 free(pNode);
//             }
//         }
 
//         free(ipv4_hash);
//         ipv4_hash = NULL;
//     } while (false);  
 
//     return retCode;
// }
int DestoryHashIpv4(void)
{
    struct SNodeIpv4 *pNode = NULL;
    int i = 0;
    int retCode = 0;
    
    do
    { 
        if (NULL == ipv4_hash)
        {
            // fprintf(stderr, "ipv4_hash is NULL at AddIpv4 function\n");
            retCode = -1; 
            break;
        }

        for (i = 0; i < HASH_COUNT; i++)
        {
            struct hlist_node *current = ipv4_hash[i].first;  // 当前节点
            struct hlist_node *next = NULL;                  // 下一个节点
            struct hlist_node *third = NULL;                 // 第三个节点
            next=current?current->next:NULL;
            while (next) 
            {
                third=next->next;
                current = (struct hlist_node *)((char *)next->pprev - offsetof(struct hlist_node, next));
                pNode = (struct SNodeIpv4 *)((char *)current - offsetof(struct SNodeIpv4, hnode));
                hlist_del(&pNode->hnode);
                free(pNode);
                if(third==NULL)
                {
                    current=next;
                }
                next=third;
            }
            if(current)
            {
                pNode = (struct SNodeIpv4 *)((char *)current - offsetof(struct SNodeIpv4, hnode));
                hlist_del(&pNode->hnode);  
                free(pNode);
            }
        }

        // 释放哈希表本身
        free(ipv4_hash);
        ipv4_hash = NULL;
    } while (false);

    return retCode;
}

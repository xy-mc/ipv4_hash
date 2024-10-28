#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ipv4.h"
#include "../gem5_dda/include/gem5/m5ops.h"
#define BATCH_SIZE 100 // 一次添加的IP地址数量

// 生成随机的32位IPv4地址并存储在64位变量中
uint64_t generate_random_ipv4() {
    return (uint64_t)((rand() % 256 << 24) | (rand() % 256 << 16) | (rand() % 256 << 8) | (rand() % 256));
}

int main(int argc, char *argv[]) {
    // 检查是否提供了数量参数
    if (argc != 2) {
        // fprintf(stderr, "Usage: %s <number_of_ips>\n", argv[0]);
        return 1;
    }

    int num_ips = atoi(argv[1]);
    if (num_ips <= 0) {
        // fprintf(stderr, "Please enter a positive number for the number of IPs.\n");
        return 1;
    }

    // 初始化随机数生成器
    srand(time(NULL));

    // 初始化哈希表
    if (InitHashIpv4() != 0) {
        // fprintf(stderr, "Failed to initialize IPv4 hash table.\n");
        return 1;
    }

    // 分配内存用于存储随机生成的IPv4地址（64位数据结构）
    uint64_t *random_ips = malloc(num_ips * sizeof(uint64_t));
    if (random_ips == NULL) {
        // fprintf(stderr, "Memory allocation for random IPs failed.\n");
        DestoryHashIpv4();
        return 1;
    }

    // 生成随机IPv4地址并添加到哈希表
    for (int i = 0; i < num_ips; i++) {
        random_ips[i] = generate_random_ipv4(); // 使用64位存储32位的IPv4地址
    }

    // GEM5 操作
    // m5_checkpoint(0, 0);
    // m5_reset_stats(0, 0);
    
    // 一次性添加多个IP地址
    for (int i = 0; i < num_ips; i += BATCH_SIZE) {
        int count = (i + BATCH_SIZE > num_ips) ? (num_ips - i) : BATCH_SIZE;
        if (AddIpv4(&random_ips[i], count) != 0) {
            // fprintf(stderr, "Failed to add IPv4 addresses from %d to %d\n", i, i + count);
        }
    }

    // m5_dump_stats(0, 0);

    // 查找并验证所有添加的IPv4地址
    for (int i = 0; i < num_ips; i++) {
        if (FindIpv4(&random_ips[i]) == 0) {
            // printf("Found IPv4 address: %u\n", (uint32_t)random_ips[i]); // 打印32位部分
        } else {
            // printf("IPv4 address not found: %u\n", (uint32_t)random_ips[i]);
        }
    }

    // 清理内存
    free(random_ips);

    // 清理哈希表
    m5_checkpoint(0, 0);
    m5_reset_stats(0, 0);
    if (DestoryHashIpv4() != 0) {
        fprintf(stderr, "Failed to destroy IPv4 hash table.\n");
        return 1;
    }
    m5_dump_stats(0, 0);
    return 0;
}

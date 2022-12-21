#pragma once

#include "sui_types.h"

struct sui_arena;
struct sui_htable;

struct sui_arena {
        void* start;
        void* pointer;
        void* end;
        void* prev;
        void* root_prev;
};

struct sui_arena sui_arena_init(void* mem, u64 n);
void*            sui_arena_push(struct sui_arena* arena, u64 n);
void*            sui_arena_pusha(struct sui_arena* arena, u64 n, u64 a);
i32              sui_arena_pushup(struct sui_arena* arena, void* ptr, u64 n);
void*            sui_arena_repush(struct sui_arena* arena, void* ptr, u64 o, u64 n);
void*            sui_arena_repusha(struct sui_arena* arena, void* ptr, u64 o, u64 n, u64 a);
struct sui_arena sui_arena_alloc(struct sui_arena* arena, u64 n);
i32              sui_arena_free(struct sui_arena* root, struct sui_arena* node);

// help from: https://nullprogram.com/blog/2022/08/08/
//            https://benhoyt.com/writings/hash-table-in-c/

struct sui_ht_item {
        u64   hash;
        char* key;
        void* ptr;
};

struct sui_ht {
        struct sui_ht_item* items;
        i32                 len;
};

u64           sui_hash(char* c);
u64           sui_hti_mem(i32 n);
struct sui_ht sui_ht_init(void* mem, i32 n);
i32           sui_ht_insert(struct sui_ht* ht, char* key, void* data);
i32           sui_ht_delete(struct sui_ht* ht, char* key);
void*         sui_ht_find(struct sui_ht* ht, char* key);
void          sui_ht_rehash(struct sui_ht* ht, void* mem, i32 n);

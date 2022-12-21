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
struct sui_htable {
        void** table;
        i32    len;
};

u64               sui_hash(char* c);
struct sui_htable sui_htable_init(void* mem, i32 len);
i32               sui_htable_lookup(struct sui_htable* ht, char* key);
void*             sui_htable_find(struct sui_htable* ht, char* key);
i32               sui_htable_insert(struct sui_htable* ht, char* key, void* item);
void              sui_htable_delete(struct sui_htable* ht, char* key);
// rehashing is impossible while we have no table items
// void              sui_htable_rehash(struct sui_htable* ht, void* mem, i32 n);

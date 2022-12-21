#pragma once

#include "sui_types.h"

struct sui_arena;
struct sui_htable;
// struct sui_recti;
// struct sui_rectf;

struct sui_arena {
        void* start;
        void* pointer;
        void* end;
        void* prev;
};

struct sui_arena sui_arena_init(void* mem, u64 n);
void*            sui_arena_push(struct sui_arena* arena, u64 n, u64 a);
i32              sui_arena_pushup(struct sui_arena* arena, void* ptr, u64 n);
void*            sui_arena_repush(struct sui_arena* arena, void* ptr, u64 o, u64 n, u64 a);
struct sui_arena sui_arena_alloc(struct sui_arena* arena, u64 a);
i32              sui_arena_free(struct sui_arena* root, struct sui_arena* node);

struct sui_htable {
        i32    len;
        void** table;
};

u64               sui_hash(char* c, i32 len);
struct sui_htable sui_htable_init(void* mem, i32 len);
i32               sui_htable_lookup(u64 hash, i32 exp, i32 idx);
void*             sui_htable_insert(struct sui_htable* ht, char* key);
void*             sui_htable_delete(struct sui_htable* ht, char* key);
// rehash

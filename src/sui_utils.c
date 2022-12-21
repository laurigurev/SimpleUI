#include "sui_utils.h"
#include "sui_defines.h"
#include <string.h>

struct sui_arena sui_arena_init(void* mem, u64 n)
{
        return (struct sui_arena){mem, mem, mem + n, NULL, NULL};
}

void* sui_arena_push(struct sui_arena* arena, u64 n)
{
        sui_assert(arena);
        if (arena->end < arena->pointer + n) return NULL;
        void* ret = arena->pointer;
        arena->pointer += n;
        arena->prev = ret;
        return ret;
}

void* sui_arena_pusha(struct sui_arena* arena, u64 n, u64 a)
{
        sui_assert(arena);
        sui_assert(a);
        u64 align = a;
        // TODO: find a quick modulo, because alignment can only be a
        //       multiple of two
        if (a != 1) align = (u64)arena->pointer % a;
        if (arena->end < arena->pointer + align + n) return NULL;
        void* ret = arena->pointer + align;
        arena->pointer += align + n;
        arena->prev = ret;
        return ret;
}

i32 sui_arena_pushup(struct sui_arena* arena, void* ptr, u64 n)
{
        sui_assert(arena);
        sui_assert(ptr);
        if (arena->prev != ptr) return 0;
        if (arena->end < arena->pointer + n) return 0;
        arena->pointer += n;
        return 1;
}

void* sui_arena_repush(struct sui_arena* arena, void* ptr, u64 o, u64 n)
{
        sui_assert(arena);
        sui_assert(ptr);
        if (sui_arena_pushup(arena, ptr, n - o)) return ptr;
        return sui_arena_push(arena, n);
}

void* sui_arena_repusha(struct sui_arena* arena, void* ptr, u64 o, u64 n, u64 a)
{
        sui_assert(arena);
        sui_assert(ptr);
        sui_assert(a);
        if (sui_arena_pushup(arena, ptr, n - o)) return ptr;
        return sui_arena_pusha(arena, n, a);
}

struct sui_arena sui_arena_alloc(struct sui_arena* arena, u64 n)
{
        sui_assert(arena);
        void*            prev = arena->prev;
        struct sui_arena node = sui_arena_init(sui_arena_push(arena, n), n);
        node.root_prev = prev;
        return node;
}

i32 sui_arena_free(struct sui_arena* root, struct sui_arena* node)
{
        sui_assert(root);
        sui_assert(node);
        if (root->prev != (void*)node) return 0;
        root->pointer = node->start;
        root->prev = node->root_prev;
        return 1;
}

u64 sui_hash(char* c)
{
        // djb2 - hash function
        u64 hash = 5381;
        while (*c) hash = ((hash << 5) + hash) + *c++;
        return hash;
}

struct sui_htable sui_htable_init(void* mem, i32 len)
{
        struct sui_htable ht = (struct sui_htable){(void**)mem, len};
        memset(ht.table, 0, len * sizeof(void*));
        return ht;
}

i32 sui_htable_lookup(struct sui_htable* ht, char* key)
{
        i32 i = sui_hash(key) % (u64)ht->len;
        return i;
}

void* sui_htable_find(struct sui_htable* ht, char* key)
{
        i32 i = sui_htable_lookup(ht, key);
        i32 len = ht->len;
        while (len--) {
                if (!ht->table[i % ht->len]) {
                        return ht->table[i];
                }
                i++;
        }
        return NULL;
}

i32 sui_htable_insert(struct sui_htable* ht, char* key, void* item)
{
        i32 i = sui_htable_lookup(ht, key);
        i32 len = ht->len;
        while (len--) {
                if (!ht->table[i % ht->len]) {
                        ht->table[i % ht->len] = item;
                        return 1;
                }
                i++;
        }
        return 0;
}

void sui_htable_delete(struct sui_htable* ht, char* key)
{
        i32 i = sui_htable_lookup(ht, key);
        i32 len = ht->len;
        while (len--) {
                if (!ht->table[i % ht->len]) {
                        ht->table[i % ht->len] = NULL;
                }
                i++;
        }
}
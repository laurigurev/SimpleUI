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

u64 sui_hti_mem(i32 n)
{
        return n * sizeof(struct sui_ht_item);
}

struct sui_ht sui_ht_init(void* mem, i32 n)
{
        sui_assert(mem);
        struct sui_ht ht = (struct sui_ht){(struct sui_ht_item*)mem, n};
        memset(ht.items, 0, n * sizeof(struct sui_ht_item));
        return ht;
}

i32 sui_ht_insert(struct sui_ht* ht, char* key, void* data)
{
        sui_assert(ht);
        sui_assert(data);
        u64 hash = sui_hash(key);
        i32 i = hash % ht->len;
        if (!ht->items[i].ptr) {
                ht->items[i] = (struct sui_ht_item){hash, key, data};
                return 1;
        }
        // collision handling uses linear probing
        i32 len = ht->len;
        while (len--) {
                i++;
                i %= ht->len;
                if (!ht->items[i].ptr) {
                        ht->items[i] = (struct sui_ht_item){hash, key, data};
                        return 1;
                }
        }
        // failed to find empty spots
        return 0;
}

i32 sui_ht_delete(struct sui_ht* ht, char* key)
{
        sui_assert(ht);
        u64 hash = sui_hash(key);
        i32 i = hash % ht->len;
        // could also compare key and string saved in item
        if (ht->items[i].hash == hash) {
                memset(ht->items + i, 0, sizeof(struct sui_ht_item));
                return 1;
        }
        i32 len = ht->len;
        while (len--) {
                i++;
                i %= ht->len;
                // could also compare key and string saved in item
                if (ht->items[i].hash == hash) {
                        memset(ht->items + i, 0, sizeof(struct sui_ht_item));
                        return 1;
                }
        }
        // failed to find item
        return 0;
}

void* sui_ht_find(struct sui_ht* ht, char* key)
{
        sui_assert(ht);
        u64 hash = sui_hash(key);
        i32 i = hash % ht->len;
        // could also compare key and string saved in item
        if (ht->items[i].hash == hash) return ht->items[i].ptr;
        i32 len = ht->len;
        while (len--) {
                i++;
                i %= ht->len;
                // could also compare key and string saved in item
                if (ht->items[i].hash == hash) return ht->items[i].ptr;
        }
        // failed to find item
        return NULL;
}

void sui_ht_rehash(struct sui_ht* ht, void* mem, i32 n)
{
        sui_assert(ht);
        sui_assert(mem);
        struct sui_ht_item* old_items = ht->items;
        i32                 old_len = ht->len;
        ht->items = (struct sui_ht_item*)mem;
        ht->len = n;
        memset(ht->items, 0, n * sizeof(struct sui_ht_item));
        for (struct sui_ht_item* item = old_items; item < old_items + old_len; item++) {
                if (item->hash) {
                        sui_ht_insert(ht, item->key, item->ptr);
                }
        }
}

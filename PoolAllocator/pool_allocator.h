//
// Created by dante on 21.10.25.
//

#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>

// Выравнивание на 8 байт для большинства архитектур
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct Chunk{
    struct Chunk* next_adress;
    bool used;

} Chunk;

typedef struct {
    void* start_pool;        // Начало всего выделенного блока памяти
    Chunk* free_list;        // Список свободных блоков или переменная в которой хранится первый свободный чанк
    size_t chunk_size;       // Размер одного блока (включая заголовок)
    size_t chunk_count;      // Количество блоков
    size_t pool_size;        // Общий размер пула

} __attribute__((aligned(ALIGNMENT))) PoolAllocator;

// Публичный интерфейс
PoolAllocator* allocator_init(size_t chunk_data_size, size_t chunk_count);
void* pool_alloc(PoolAllocator* allocator);
void chunk_free(PoolAllocator* allocator, void* ptr);
void pool_destroy(PoolAllocator* allocator);

// Вспомогательные функции (опционально)
size_t get_available_chunks(PoolAllocator* allocator);
bool is_pointer_valid(PoolAllocator* allocator, void* ptr);

#endif //POOL_ALLOCATOR_H

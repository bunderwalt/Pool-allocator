#include "pool_allocator.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

PoolAllocator* allocator_init(size_t chunk_data_size, size_t chunk_count) {
    // Рассчитываем полный размер чанка (заголовок + данные)
    size_t chunk_full_size = ALIGN(sizeof(Chunk) + chunk_data_size);

    // Рассчитываем общий размер пула
    size_t total_data_size = chunk_full_size * chunk_count;
    size_t total_pool_size = sizeof(PoolAllocator) + total_data_size;

    // Выравниваем на границу страницы
    long page_size = sysconf(_SC_PAGESIZE);
    size_t aligned_pool_size = (total_pool_size + page_size - 1) & ~(page_size - 1);

    // Выделяем память
    void* memory = mmap(NULL, aligned_pool_size,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) return NULL;

    PoolAllocator* allocator = (PoolAllocator*)memory;
    allocator->start_pool = (char*)memory + sizeof(PoolAllocator);
    allocator->pool_size = aligned_pool_size;
    allocator->chunk_count = chunk_count;
    allocator->chunk_size = chunk_full_size;
    allocator->free_list = NULL;

    // Инициализируем список свободных чанков
    char* chunk_start = (char*)allocator->start_pool;

    // Идем с КОНЦА чтобы next указывал вперед
    for (size_t i = chunk_count; i > 0; i--) {
        Chunk* current_chunk = (Chunk*)(chunk_start + (i - 1) * chunk_full_size);
        current_chunk->used = false;
        current_chunk->next_adress = allocator->free_list;  // Теперь next указывает вперед!
        allocator->free_list = current_chunk;
    }

    return allocator;

}

void* pool_alloc(PoolAllocator* allocator) {
    if (!allocator || !allocator->free_list) return NULL;

    Chunk* current_chunk = (Chunk*)allocator->free_list;
    current_chunk->used;
    current_chunk->used = true;
    allocator->free_list = current_chunk->next_adress;

    return (void*)((char*)current_chunk + sizeof(Chunk));

}

void chunk_free(PoolAllocator* allocator, void* chunk) {
    if (!allocator || !chunk) return;


    Chunk* chunk_current = (Chunk*)((char*)chunk - sizeof(Chunk));
    chunk_current->used = false;
    chunk_current->next_adress = allocator->free_list;
    allocator->free_list = chunk_current;
}

void pool_destroy(PoolAllocator* allocator) {
    if (!allocator) return;

    size_t total_size = allocator->pool_size;
    munmap(allocator, total_size);
}

// Вспомогательные функции

size_t get_available_chunks(PoolAllocator* allocator) {
    if (!allocator) return 0;

    size_t count = 0;
    Chunk* current = allocator->free_list;
    while (current) {
        count++;
        current = current->next_adress;
    }
    return count;
}

bool is_pointer_valid(PoolAllocator* allocator, void* ptr) {
    if (!allocator || !ptr) return false;

    // Проверяем, что указатель находится в пределах нашего пула
    Chunk* chunk = (Chunk*)((char*)ptr - sizeof(Chunk));
    void* pool_start = allocator->start_pool;
    void* pool_end = (char*)pool_start + allocator->chunk_size * allocator->chunk_count;

    return (chunk >= pool_start && chunk < pool_end);
}
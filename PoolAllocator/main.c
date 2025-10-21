#include "pool_allocator.h"
#include <stdio.h>
#include <string.h>

void print_pool_status(PoolAllocator* pool, const char* message) {
    printf("%s\n", message);
    printf("  Всего чанков: %zu\n", pool->chunk_count);
    printf("  Доступно чанков: %zu\n", get_available_chunks(pool));
    printf("  Использовано чанков: %zu\n", pool->chunk_count - get_available_chunks(pool));
    printf("---\n");
}

int main() {
    printf("=== Демонстрация пул-аллокатора ===\n");

    // Создаем аллокатор с чанками по 64 байта данных, 10 чанков
    PoolAllocator* pool = allocator_init(64, 10);
    if (!pool) {
        fprintf(stderr, "Ошибка: не удалось создать пул-аллокатор\n");
        return 1;
    }

    print_pool_status(pool, "После инициализации:");

    printf("=== Тестирование выделения памяти ===\n");

    // Выделяем несколько блоков
    char* str1 = (char*)pool_alloc(pool);
    char* str2 = (char*)pool_alloc(pool);
    char* str3 = (char*)pool_alloc(pool);

    if (str1 && str2 && str3) {
        // Используем память
        strcpy(str1, "Привет из чанка 1");
        strcpy(str2, "Привет из чанка 2");
        strcpy(str3, "Привет из чанка 3");

        printf("Выделено и использовано 3 чанка:\n");
        printf("  Чанк 1: %s\n", str1);
        printf("  Чанк 2: %s\n", str2);
        printf("  Чанк 3: %s\n", str3);

        // Проверяем валидность указателей
        printf("Проверка валидности указателей:\n");
        printf("  str1 валиден: %s\n", is_pointer_valid(pool, str1) ? "да" : "нет");
        printf("  str2 валиден: %s\n", is_pointer_valid(pool, str2) ? "да" : "нет");

        print_pool_status(pool, "После выделения 3 чанков:");
    }

    printf("=== Тестирование освобождения памяти ===\n");

    // Освобождаем память
    if (str1) chunk_free(pool, str1);
    if (str2) chunk_free(pool, str2);

    printf("Освобождено 2 чанка\n");
    print_pool_status(pool, "После освобождения 2 чанков:");

    // Выделяем снова
    char* str4 = (char*)pool_alloc(pool);
    if (str4) {
        strcpy(str4, "Переиспользованный чанк");
        printf("Повторно выделен: %s\n", str4);
        print_pool_status(pool, "После повторного выделения:");
    }

    // Тестирование с невалидным указателем
    printf("=== Тестирование с невалидным указателем ===\n");
    int invalid_data = 42;
    printf("Проверка невалидного указателя: %s\n",
           is_pointer_valid(pool, &invalid_data) ? "валиден (ОШИБКА!)" : "невалиден (правильно)");

    printf("\n=== Очистка ===\n");
    pool_destroy(pool);
    printf("Пул-аллокатор уничтожен успешно\n");

    return 0;
}
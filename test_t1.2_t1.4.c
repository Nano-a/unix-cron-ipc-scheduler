#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "task_tree.h"
#include "serialization.h"

// Test T1.2 - Module de gestion de l'arborescence
int test_t1_2(void) {
    printf("=== Test T1.2 - Module de gestion de l'arborescence ===\n");
    
    char path[512];
    const char *test_dir = "/tmp/test_erraid";
    
    // Test 1: init_task_directory
    printf("\n1. Test init_task_directory...\n");
    if (init_task_directory(test_dir) == 0) {
        printf("   ✓ Répertoire créé avec succès\n");
    } else {
        printf("   ✗ Erreur lors de la création du répertoire\n");
        return 1;
    }
    
    // Vérifier que le répertoire existe
    struct stat st;
    if (stat(test_dir, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("   ✓ Répertoire existe bien\n");
    } else {
        printf("   ✗ Répertoire n'existe pas\n");
        return 1;
    }
    
    // Test 2: build_task_dir_path
    printf("\n2. Test build_task_dir_path...\n");
    if (build_task_dir_path(path, sizeof(path), test_dir, 5) == 0) {
        printf("   ✓ Chemin construit: %s\n", path);
        if (strcmp(path, "/tmp/test_erraid/erraid/tasks/5") == 0) {
            printf("   ✓ Chemin correct\n");
        } else {
            printf("   ✗ Chemin incorrect (attendu: /tmp/test_erraid/erraid/tasks/5)\n");
            return 1;
        }
    } else {
        printf("   ✗ Erreur lors de la construction du chemin\n");
        return 1;
    }
    
    // Test 3: build_task_path
    printf("\n3. Test build_task_path...\n");
    if (build_task_path(path, sizeof(path), test_dir, 5, "timing") == 0) {
        printf("   ✓ Chemin construit: %s\n", path);
        if (strcmp(path, "/tmp/test_erraid/erraid/tasks/5/timing") == 0) {
            printf("   ✓ Chemin correct\n");
        } else {
            printf("   ✗ Chemin incorrect (attendu: /tmp/test_erraid/erraid/tasks/5/timing)\n");
            return 1;
        }
    } else {
        printf("   ✗ Erreur lors de la construction du chemin\n");
        return 1;
    }
    
    printf("\n✅ Tous les tests T1.2 sont passés !\n");
    return 0;
}

// Test T1.4 - Module de sérialisation
int test_t1_4(void) {
    printf("\n=== Test T1.4 - Module de sérialisation ===\n");
    
    const char *test_file = "/tmp/test_serialization.bin";
    int fd;
    
    // Test 1: Écriture
    printf("\n1. Test d'écriture...\n");
    fd = open(test_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    uint8_t u8_write = 0x42;
    uint16_t u16_write = 0x1234;
    uint32_t u32_write = 0x12345678;
    uint64_t u64_write = 0x123456789ABCDEF0ULL;
    int64_t i64_write = -1234567890123456789LL;
    
    if (write_uint8(fd, u8_write) < 0) {
        printf("   ✗ Erreur write_uint8\n");
        close(fd);
        return 1;
    }
    printf("   ✓ write_uint8 OK\n");
    
    if (write_uint16(fd, u16_write) < 0) {
        printf("   ✗ Erreur write_uint16\n");
        close(fd);
        return 1;
    }
    printf("   ✓ write_uint16 OK\n");
    
    if (write_uint32(fd, u32_write) < 0) {
        printf("   ✗ Erreur write_uint32\n");
        close(fd);
        return 1;
    }
    printf("   ✓ write_uint32 OK\n");
    
    if (write_uint64(fd, u64_write) < 0) {
        printf("   ✗ Erreur write_uint64\n");
        close(fd);
        return 1;
    }
    printf("   ✓ write_uint64 OK\n");
    
    if (write_int64(fd, i64_write) < 0) {
        printf("   ✗ Erreur write_int64\n");
        close(fd);
        return 1;
    }
    printf("   ✓ write_int64 OK\n");
    
    close(fd);
    
    // Test 2: Lecture
    printf("\n2. Test de lecture...\n");
    fd = open(test_file, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    uint8_t u8_read;
    uint16_t u16_read;
    uint32_t u32_read;
    uint64_t u64_read;
    int64_t i64_read;
    
    if (read_uint8(fd, &u8_read) < 0) {
        printf("   ✗ Erreur read_uint8\n");
        close(fd);
        return 1;
    }
    if (u8_read != u8_write) {
        printf("   ✗ read_uint8: valeur incorrecte (0x%02X != 0x%02X)\n", u8_read, u8_write);
        close(fd);
        return 1;
    }
    printf("   ✓ read_uint8 OK (0x%02X)\n", u8_read);
    
    if (read_uint16(fd, &u16_read) < 0) {
        printf("   ✗ Erreur read_uint16\n");
        close(fd);
        return 1;
    }
    if (u16_read != u16_write) {
        printf("   ✗ read_uint16: valeur incorrecte (0x%04X != 0x%04X)\n", u16_read, u16_write);
        close(fd);
        return 1;
    }
    printf("   ✓ read_uint16 OK (0x%04X)\n", u16_read);
    
    if (read_uint32(fd, &u32_read) < 0) {
        printf("   ✗ Erreur read_uint32\n");
        close(fd);
        return 1;
    }
    if (u32_read != u32_write) {
        printf("   ✗ read_uint32: valeur incorrecte (0x%08X != 0x%08X)\n", u32_read, u32_write);
        close(fd);
        return 1;
    }
    printf("   ✓ read_uint32 OK (0x%08X)\n", u32_read);
    
    if (read_uint64(fd, &u64_read) < 0) {
        printf("   ✗ Erreur read_uint64\n");
        close(fd);
        return 1;
    }
    if (u64_read != u64_write) {
        printf("   ✗ read_uint64: valeur incorrecte (0x%016lX != 0x%016lX)\n", u64_read, u64_write);
        close(fd);
        return 1;
    }
    printf("   ✓ read_uint64 OK (0x%016lX)\n", u64_read);
    
    if (read_int64(fd, &i64_read) < 0) {
        printf("   ✗ Erreur read_int64\n");
        close(fd);
        return 1;
    }
    if (i64_read != i64_write) {
        printf("   ✗ read_int64: valeur incorrecte (%ld != %ld)\n", i64_read, i64_write);
        close(fd);
        return 1;
    }
    printf("   ✓ read_int64 OK (%ld)\n", i64_read);
    
    close(fd);
    unlink(test_file);
    
    printf("\n✅ Tous les tests T1.4 sont passés !\n");
    return 0;
}

int main(void) {
    int result = 0;
    
    printf("========================================\n");
    printf("Tests des tâches T1.2 et T1.4\n");
    printf("========================================\n");
    
    result |= test_t1_2();
    result |= test_t1_4();
    
    printf("\n========================================\n");
    if (result == 0) {
        printf("✅ TOUS LES TESTS SONT PASSÉS !\n");
    } else {
        printf("✗ CERTAINS TESTS ONT ÉCHOUÉ\n");
    }
    printf("========================================\n");
    
    return result;
}


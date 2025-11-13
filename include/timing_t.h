#include <stdint.h>

typedef struct {
    uint64_t minutes;      // Bits 0–59 = minutes 0–59
    uint32_t hours;        // Bits 0–23 = heures 0–23
    uint8_t days_of_week;  // Bits 0–6 = dimanche–samedi
} timing_t;


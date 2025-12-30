// effects/static_color.cpp
// Простейший эффект: статичный цвет
// Использует только: brightness (обязательный)

#include "../effectBase.h"

void effect_static_color(LedMatrix& matrix, EffectContext& ctx, uint32_t dt) {
    // Заполнить всю матрицу белым цветом
    // Яркость будет применена автоматически в EffectManager
    matrix.fill(255, 255, 255);
    
    // Если нужен другой цвет, можно добавить логику:
    // matrix.fill(255, 0, 0);  // Красный
    // matrix.fill(0, 255, 0);  // Зеленый
    // matrix.fill(0, 0, 255);  // Синий
}
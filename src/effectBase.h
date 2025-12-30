#ifndef EFFECT_BASE_H
#define EFFECT_BASE_H

#include <stdint.h>
#include <cstring>

// Forward declarations
class LedMatrix;
struct EffectContext;

// Битовые флаги для опциональных параметров
enum EffectFlags : uint8_t {
    EFFECT_NONE = 0,
    EFFECT_HAS_SPEED = (1 << 0),    // Эффект использует speed
    EFFECT_HAS_KFACTOR = (1 << 1),  // Эффект использует k_factor
};

// Сигнатура функции эффекта
// matrix - LED матрица для рисования
// ctx - контекст с параметрами и состоянием
// dt - время с последнего кадра в миллисекундах
typedef void (*EffectFunction)(LedMatrix& matrix, EffectContext& ctx, uint32_t dt);

// Метаданные эффекта (статичны, хранятся в Flash)
struct EffectDescriptor {
    const char* name;           // Кодовое имя: "fire", "rainbow", "static"
    EffectFunction function;    // Указатель на функцию
    uint8_t flags;             // Битовая маска EffectFlags
    uint8_t target_fps;        // Целевой FPS (60, 30, 24)
};

// Параметры эффекта (изменяемые)
struct EffectParams {
    uint8_t brightness;  // 0-255, обязательный
    uint8_t speed;       // 0-255, опциональный
    int8_t k_factor;     // -128..127, опциональный
    
    EffectParams() : brightness(255), speed(128), k_factor(0) {}
};

// Контекст выполнения эффекта
struct EffectContext {
    EffectParams params;        // Текущие параметры
    uint32_t frame_count;       // Счетчик кадров
    uint32_t elapsed_ms;        // Время работы эффекта в мс
    void* state;                // Указатель на внутреннее состояние (опционально)
    
    EffectContext() : frame_count(0), elapsed_ms(0), state(nullptr) {}
    
    // Сброс контекста при переключении эффекта
    void reset() {
        frame_count = 0;
        elapsed_ms = 0;
        // state очищается вручную эффектом если нужно
    }
};

// Абстракция LED матрицы
class LedMatrix {
private:
    uint8_t* buffer;  // RGB буфер: [R0,G0,B0, R1,G1,B1, ...]
    uint16_t width;
    uint16_t height;
    
public:
    LedMatrix(uint16_t w, uint16_t h) : width(w), height(h) {
        buffer = new uint8_t[w * h * 3];
        clear();
    }
    
    ~LedMatrix() {
        delete[] buffer;
    }
    
    uint16_t getWidth() const { return width; }
    uint16_t getHeight() const { return height; }
    uint16_t getPixelCount() const { return width * height; }
    
    // Установить цвет пикселя (x, y)
    void setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
        if (x >= width || y >= height) return;
        uint32_t idx = (y * width + x) * 3;
        buffer[idx] = r;
        buffer[idx + 1] = g;
        buffer[idx + 2] = b;
    }
    
    // Получить цвет пикселя
    void getPixel(uint16_t x, uint16_t y, uint8_t& r, uint8_t& g, uint8_t& b) {
        if (x >= width || y >= height) {
            r = g = b = 0;
            return;
        }
        uint32_t idx = (y * width + x) * 3;
        r = buffer[idx];
        g = buffer[idx + 1];
        b = buffer[idx + 2];
    }
    
    // Очистить матрицу
    void clear() {
        memset(buffer, 0, width * height * 3);
    }
    
    // Заполнить всю матрицу одним цветом
    void fill(uint8_t r, uint8_t g, uint8_t b) {
        for (uint16_t i = 0; i < width * height; i++) {
            buffer[i * 3] = r;
            buffer[i * 3 + 1] = g;
            buffer[i * 3 + 2] = b;
        }
    }
    
    // Прямой доступ к буферу для продвинутых эффектов
    uint8_t* getBuffer() { return buffer; }
    
    // Применить глобальную яркость (0-255)
    void applyBrightness(uint8_t brightness) {
        if (brightness == 255) return;
        uint32_t size = width * height * 3;
        for (uint32_t i = 0; i < size; i++) {
            buffer[i] = (buffer[i] * brightness) >> 8;
        }
    }
};

#endif // EFFECT_BASE_H
#pragma once

#define LED_PIN         0   // пин ленты
#define BTN_PIN         1   // пин кнопки
#define ADC_PIN         3   // пин акб через делитель

#define WIDTH           ( 16U )  // ширина матрицы
#define HEIGHT          ( 16U )  // высота матрицы
#define COLOR_ORDER     ( RGB )  // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

#define NTP_ADDRESS     ("ru.pool.ntp.org")     // сервер времени. если не работает, пробуйте "time.windows.com"
#define NTP_INTERVAL    (90 * 60UL * 1000UL)    // интервал синхронизации времени (59 минут)

// ================== devoloper  settings ================== 

#define NUM_LEDS (uint16_t)(WIDTH * HEIGHT)

#define ADC_MAX 4095.0
#define ADC_REF 3.00
#define DIVIDER_RATIO 2.0

#define FASTLED_USE_PROGMEM 1 // просим библиотеку FASTLED экономить память контроллера на свои палитры
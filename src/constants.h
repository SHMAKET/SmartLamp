#pragma once

#define LED_PIN             19           // пин ленты
#define BTN_PIN             20           // пин кнопки
#define ADC_PIN             3            // пин акб через делитель
#define SCL_PIN             2
#define SDA_PIN             1

#define WIDTH               ( 16U )     // ширина матрицы
#define HEIGHT              ( 16U )     // высота матрицы
#define COLOR_ORDER         ( GRB )     // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB
#define LED_TYPE            WS2812
#define MAX_POWER_MW        10000       // 10W power limit

#define MATRIX_TYPE         0           // Matrix type: 0 = zigzag (serpentine) - most common; 1 = parallel (all strips go same direction)
#define CONNECTION_ANGLE    1           // Connection angle (where first LED is located): 0 = bottom-left; 1 = top-left; 2 = top-right; 3 = bottom-right
#define STRIP_DIRECTION     3           // Strip direction from connection angle: 0 = right; 1 = up; 2 = left; 3 = down

#define NTP_ADDRESS         ("ru.pool.ntp.org")     // сервер времени. если не работает, пробуйте "time.windows.com"
#define NTP_INTERVAL        (90 * 60UL * 1000UL)    // интервал синхронизации времени

#define UB_DEB_TIME         20          // (мс) дебаунс
#define UB_HOLD_TIME        500         // (мс) время до перехода в состояние "удержание"
#define UB_CLICK_TIME       300         // (мс) ожидание кликов

#define AP_SSID             "SmartLamp"             // SSID wifi AP
#define AP_PASS             "31415926"              // password wifi AP


// ================== devoloper  settings ================== 

#define ADC_MAX             4095.0
#define ADC_REF             4.1
#define DIVIDER_RATIO       2.0

#define NUM_LEDS (uint16_t)(WIDTH * HEIGHT)
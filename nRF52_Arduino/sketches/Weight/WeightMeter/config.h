#pragma once

#define PIN_GREEN_LED     LED_BLUE
#define PIN_RED_LED       LED_RED

//constexpr uint32_t MEASUREMENT_INTERVAL    = 10 * 60 * 1000;
constexpr uint32_t MEASUREMENT_INTERVAL   = 3 * 1000;

// 分解能を10gとする。（10g単位で測定できるようにする）
static float offset = 0.0;

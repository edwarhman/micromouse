/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <esp_timer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "controlMovimiento.h"
#include "Sensores.h"
#include "navegacion.h"

#define MOTOR_A_OUTPUT_1 GPIO_NUM_2  // GPIO para el motor A
#define MOTOR_A_OUTPUT_2 GPIO_NUM_4  // GPIO para el motor A
#define MOTOR_A_ENABLE GPIO_NUM_5    // GPIO para el control de velocidad del motor A
#define MOTOR_B_OUTPUT_1 GPIO_NUM_18 // GPIO para el motor B
#define MOTOR_B_OUTPUT_2 GPIO_NUM_19 // GPIO para el motor B
#define MOTOR_B_ENABLE GPIO_NUM_21   // GPIO para el control de velocidad del motor B
#define BUTTON GPIO_NUM_0
#define SENSOR_IZQUIERDO GPIO_NUM_15
#define SENSOR_DERECHO GPIO_NUM_17
#define SENSOR_FRONTAL GPIO_NUM_16
#define SENSOR_RUEDA_A GPIO_NUM_22 // GPIO para el sensor de rueda A

void app_main(void)
{
    ControlMovimientoConfig configControlMomiviento = {
        .timerNumber = LEDC_TIMER_0,
        .rightMotorGpio1 = MOTOR_A_OUTPUT_1,
        .rightMotorGpio2 = MOTOR_A_OUTPUT_2,
        .rightMotorGpioPwm = MOTOR_A_ENABLE,
        .leftMotorGpio1 = MOTOR_B_OUTPUT_1,
        .leftMotorGpio2 = MOTOR_B_OUTPUT_2,
        .leftMotorGpioPwm = MOTOR_B_ENABLE,
        .minVelocity = 100,
        .maxVelocity = 1023,
    };

    inicializarControlMovimiento(configControlMomiviento);
    setVelocidadMotorDerecho(500);
    setVelocidadMotorIzquierdo(500);

    SensoresConfig configSensores = {
        .sensorFrontal = SENSOR_FRONTAL,
        .sensorDerecho = SENSOR_DERECHO,
        .sensorIzquierdo = SENSOR_IZQUIERDO,
        .sensorRuedaA = SENSOR_RUEDA_A,
    };
    inicializarSensores(configSensores);

    vTaskDelay(pdMS_TO_TICKS(5000)); // Wait for 5 seconds before starting
    avanzar();

    // Botón para simular avance de una casilla del micromouse
    esp_rom_gpio_pad_select_gpio(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY); // Set button with pull-up resistor

    while (1)
    {
        if (gpio_get_level(BUTTON) == 0) // Button pressed
        {
            vTaskDelay(pdMS_TO_TICKS(150)); // Debounce delay
            // simularAvance();
            reiniciarCuentaSensorRueda();
            printf("SIMULAR AVANCE!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }
        maquinaDeEstados();
        int cuentaRueda = obtenerCuentaSensorRueda();
        printf("Cuenta del sensor de rueda A: %d\n", cuentaRueda);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Polling delay
    }
}

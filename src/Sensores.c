#include <driver/gpio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Sensores.h"

gpio_num_t sensorFrontal;
gpio_num_t sensorDerecho;
gpio_num_t sensorIzquierdo;
gpio_num_t triggerSensorSonico;
gpio_num_t echoSensorSonico;

void inicializarSensores(SensoresConfig sensoresConfig)
{
    sensorFrontal = sensoresConfig.sensorFrontal;
    sensorDerecho = sensoresConfig.sensorDerecho;
    sensorIzquierdo = sensoresConfig.sensorIzquierdo;
    triggerSensorSonico = sensoresConfig.sensorSonicoTrigger;
    echoSensorSonico = sensoresConfig.sensorSonicoEcho;

    gpio_set_direction(sensorFrontal, GPIO_MODE_INPUT);
    gpio_set_direction(sensorDerecho, GPIO_MODE_INPUT);
    gpio_set_direction(sensorIzquierdo, GPIO_MODE_INPUT);

    printf("Sensores inicializados.\n");
}

int leerSensorFrontal()
{
    return gpio_get_level(sensorFrontal);
}

int leerSensorDerecho()
{
    return gpio_get_level(sensorDerecho);
}

int leerSensorIzquierdo()
{
    return gpio_get_level(sensorIzquierdo);
}

int medirDistanciaSensorSonico()
{
    int start_t = 0, end_t = 0;
    const int CONSTANTE_DISTANCIA = 58; // 1 cm = 58 ms

    gpio_set_level(triggerSensorSonico, 1);
    vTaskDelay(10);
    gpio_set_level(triggerSensorSonico, 0);

    while (gpio_get_level(echoSensorSonico) == 0)
        start_t = esp_timer_get_time();
    while (gpio_get_level(echoSensorSonico) == 1)
        end_t = esp_timer_get_time();
    int duration_in_ms = end_t - start_t;

    int distance = duration_in_ms / CONSTANTE_DISTANCIA;

    return distance;
}

SensoresData obtenerDatosSensores()
{
    SensoresData datos;
    datos.sensorFrontal = leerSensorFrontal();
    datos.sensorDerecho = leerSensorDerecho();
    datos.sensorIzquierdo = leerSensorIzquierdo();
    datos.nivelSensorSonico = medirDistanciaSensorSonico();

    return datos;
}
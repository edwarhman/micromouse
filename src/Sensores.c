#include <driver/gpio.h>
#include <driver/pulse_cnt.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Sensores.h"

// ENTRADAS SENSORES
gpio_num_t sensorFrontal;
gpio_num_t sensorDerecho;
gpio_num_t sensorIzquierdo;
gpio_num_t triggerSensorSonico;
gpio_num_t echoSensorSonico;
const gpio_num_t sensorRuedaA = GPIO_NUM_19; // GPIO for wheel A sensor
gpio_num_t sensorRuedaB;

// Configuración de los sensores

/*
INICIA CONFIGURACIÓN DEL SENSOR DE RUEDAS
*/
pcnt_unit_config_t pcnt_config = {
    .low_limit = -1000,
    .high_limit = 1000,
};
pcnt_unit_handle_t pulse_cnt_unit = NULL;

const int CHANNEL_GPIO_A = GPIO_NUM_19;

pcnt_chan_config_t chan_config = {
    .edge_gpio_num = sensorRuedaA,
    .level_gpio_num = -1, // Not used
};
pcnt_channel_handle_t channel_handle = NULL;

pcnt_glitch_filter_config_t filter_config = {
    .max_glitch_ns = 10000, // Set the glitch filter pulse width
};
/*
FIN CONFIGURACIÓN DEL SENSOR DE RUEDAS
*/

void inicializarSensorRuedas()
{
    ESP_ERROR_CHECK(pcnt_new_unit(
        &pcnt_config,
        &pulse_cnt_unit // Pass the handle here
        ));
    ESP_ERROR_CHECK(pcnt_new_channel(pulse_cnt_unit, &chan_config, &channel_handle));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(channel_handle, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pulse_cnt_unit, &filter_config));
    ESP_ERROR_CHECK(pcnt_unit_enable(pulse_cnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pulse_cnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pulse_cnt_unit));
}

void inicializarSensores(SensoresConfig sensoresConfig)
{
    sensorFrontal = sensoresConfig.sensorFrontal;
    sensorDerecho = sensoresConfig.sensorDerecho;
    sensorIzquierdo = sensoresConfig.sensorIzquierdo;
    triggerSensorSonico = sensoresConfig.sensorSonicoTrigger;
    echoSensorSonico = sensoresConfig.sensorSonicoEcho;
    chan_config.edge_gpio_num = sensoresConfig.sensorRuedaA;

    gpio_set_direction(sensorFrontal, GPIO_MODE_INPUT);
    gpio_set_direction(sensorDerecho, GPIO_MODE_INPUT);
    gpio_set_direction(sensorIzquierdo, GPIO_MODE_INPUT);
    inicializarSensorRuedas();

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

int obtenerCuentaSensorRueda()
{
    int count = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pulse_cnt_unit, &count));
    return count;
}

int reiniciarCuentaSensorRueda()
{
    return pcnt_unit_clear_count(pulse_cnt_unit);
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
    datos.cuentaSensorRueda = obtenerCuentaSensorRueda();

    return datos;
}

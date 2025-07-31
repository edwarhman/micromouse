#include <driver/gpio.h>
#include <driver/ledc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ControlMovimiento.h"

// Variables globales para los pines de control de los motores
gpio_num_t motorIzquierdoEnable = 0;
gpio_num_t motorDerechoEnable = 0;
gpio_num_t motorIzquierdoInput1 = 0;
gpio_num_t motorIzquierdoInput2 = 0;
gpio_num_t motorDerechoInput1 = 0;
gpio_num_t motorDerechoInput2 = 0;
int detenido = 0;

// Configuración Canal A (Motor Izquierdo)
ledc_channel_config_t ledc_channel_a_config = {
    .gpio_num = 0, // Se asigna en la inicialización
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0, // Inicia con ciclo de trabajo 0
};

// Configuración Canal B (Motor Derecho)
ledc_channel_config_t ledc_channel_b_config = {
    .gpio_num = 0, // Se asigna en la inicialización
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_1,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0, // Inicia con ciclo de trabajo 0
};

/**
 * @brief Establece los pines de control del motor izquierdo.
 * @param input1 GPIO para IN1
 * @param input2 GPIO para IN2
 * @param inputEnable GPIO para Enable/PWM
 */
void setMotorIzquierdo(gpio_num_t input1, gpio_num_t input2, gpio_num_t inputEnable)
{
    motorIzquierdoInput1 = input1;
    motorIzquierdoInput2 = input2;
    motorIzquierdoEnable = inputEnable;
}

/**
 * @brief Establece los pines de control del motor derecho.
 * @param input1 GPIO para IN1
 * @param input2 GPIO para IN2
 * @param inputEnable GPIO para Enable/PWM
 */
void setMotorDerecho(gpio_num_t input1, gpio_num_t input2, gpio_num_t inputEnable)
{
    motorDerechoInput1 = input1;
    motorDerechoInput2 = input2;
    motorDerechoEnable = inputEnable;
}

/**
 * @brief Detiene ambos motores y marca el estado como detenido.
 */
void detener()
{
    gpio_set_level(motorIzquierdoInput1, 0);
    gpio_set_level(motorIzquierdoInput2, 0);
    gpio_set_level(motorDerechoInput1, 0);
    gpio_set_level(motorDerechoInput2, 0);

    vTaskDelay(pdMS_TO_TICKS(500)); // Espera 500 ms antes de continuar
    detenido = 1;
    printf("Motores detenidos\n");
}

/**
 * @brief Hace avanzar ambos motores hacia adelante.
 */
void avanzar()
{
    gpio_set_level(motorIzquierdoInput1, 1);
    gpio_set_level(motorIzquierdoInput2, 0);
    gpio_set_level(motorDerechoInput1, 1);
    gpio_set_level(motorDerechoInput2, 0);
    printf("Motores avanzando\n");
    detenido = 0;
}

/**
 * @brief Hace retroceder ambos motores.
 */
void retroceder()
{
    gpio_set_level(motorIzquierdoInput1, 0);
    gpio_set_level(motorIzquierdoInput2, 1);
    gpio_set_level(motorDerechoInput1, 0);
    gpio_set_level(motorDerechoInput2, 1);
    printf("Motores retrocediendo\n");
    detenido = 0;
}

/**
 * @brief Gira el robot a la derecha (motor izquierdo avanza, derecho retrocede).
 */
void girarDerecha()
{
    gpio_set_level(motorIzquierdoInput1, 1);
    gpio_set_level(motorIzquierdoInput2, 0);
    gpio_set_level(motorDerechoInput1, 0);
    gpio_set_level(motorDerechoInput2, 1);
    printf("Motores girando a la derecha\n");
    detenido = 0;
}

/**
 * @brief Gira el robot a la izquierda (motor derecho avanza, izquierdo retrocede).
 */
void girarIzquierda()
{
    gpio_set_level(motorIzquierdoInput1, 0);
    gpio_set_level(motorIzquierdoInput2, 1);
    gpio_set_level(motorDerechoInput1, 1);
    gpio_set_level(motorDerechoInput2, 0);
    printf("Motores girando a la izquierda\n");
    detenido = 0;
}

/**
 * @brief Ajusta la velocidad del motor izquierdo usando PWM.
 * @param velocidad Valor de ciclo de trabajo (0-1023)
 */
void setVelocidadMotorIzquierdo(int velocidad)
{
    if (velocidad < 0)
    {
        velocidad = 0; // No permitir valores negativos
    }
    if (velocidad > 1023)
    {
        velocidad = 1023; // Límite para resolución de 10 bits
    }

    ledc_set_duty(ledc_channel_a_config.speed_mode, ledc_channel_a_config.channel, velocidad);
    ledc_update_duty(ledc_channel_a_config.speed_mode, ledc_channel_a_config.channel);
    printf("Velocidad motor izquierdo: %d\n", velocidad);
}

/**
 * @brief Ajusta la velocidad del motor derecho usando PWM.
 * @param velocidad Valor de ciclo de trabajo (0-1023)
 */
void setVelocidadMotorDerecho(int velocidad)
{
    if (velocidad < 0)
    {
        velocidad = 0; // No permitir valores negativos
    }
    if (velocidad > 1023)
    {
        velocidad = 1023; // Límite para resolución de 10 bits
    }

    ledc_set_duty(ledc_channel_b_config.speed_mode, ledc_channel_b_config.channel, velocidad);
    ledc_update_duty(ledc_channel_b_config.speed_mode, ledc_channel_b_config.channel);
    printf("Velocidad motor derecho: %d\n", velocidad);
}

/**
 * @brief Indica si el robot está detenido.
 * @return 1 si está detenido, 0 en caso contrario.
 */
int estaDetenido()
{
    return detenido;
}

/**
 * @brief Inicializa los pines y PWM para el control de los motores.
 * @param controlMovParams Estructura con los pines y timer a usar.
 */
void inicializarControlMovimiento(ControlMovimientoConfig controlMovParams)
{
    // Asigna los GPIO para el motor derecho
    motorDerechoInput1 = controlMovParams.rightMotorGpio1;
    motorDerechoInput2 = controlMovParams.rightMotorGpio2;
    motorDerechoEnable = controlMovParams.rightMotorGpioPwm;

    // Asigna los GPIO para el motor izquierdo
    motorIzquierdoInput1 = controlMovParams.leftMotorGpio1;
    motorIzquierdoInput2 = controlMovParams.leftMotorGpio2;
    motorIzquierdoEnable = controlMovParams.leftMotorGpioPwm;

    // Configura los GPIO como salida
    gpio_set_direction(motorIzquierdoInput1, GPIO_MODE_OUTPUT);
    gpio_set_direction(motorIzquierdoInput2, GPIO_MODE_OUTPUT);
    gpio_set_direction(motorDerechoInput1, GPIO_MODE_OUTPUT);
    gpio_set_direction(motorDerechoInput2, GPIO_MODE_OUTPUT);

    // Inicializa los canales LEDC para PWM
    ledc_channel_a_config.gpio_num = motorIzquierdoEnable;
    ledc_channel_a_config.timer_sel = controlMovParams.timerNumber;
    ledc_channel_b_config.gpio_num = motorDerechoEnable;
    ledc_channel_b_config.timer_sel = controlMovParams.timerNumber;

    printf("%d\n", ledc_channel_config(&ledc_channel_a_config));
    printf("%d\n", ledc_channel_config(&ledc_channel_b_config));

    // Detiene el robot al iniciar para evitar movimientos inesperados
    detener();
}
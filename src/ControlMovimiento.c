#include <driver/gpio.h>
#include <driver/ledc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ControlMovimiento.h"

gpio_num_t motorIzquierdoEnable = 0;
gpio_num_t motorDerechoEnable = 0;
gpio_num_t motorIzquierdoInput1 = 0;
gpio_num_t motorIzquierdoInput2 = 0;
gpio_num_t motorDerechoInput1 = 0;
gpio_num_t motorDerechoInput2 = 0;
int detenido = 0;

// Configuración Canal A (Motor Izquierdo)
ledc_channel_config_t ledc_channel_a_config = {
    .gpio_num = 0, // Placeholder, will be set in initialization
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0, // Start with duty cycle 0
};

// Configuración Canal B (Motor Derecho)
ledc_channel_config_t ledc_channel_b_config = {
    .gpio_num = 0, // Placeholder, will be set in initialization
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_1,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0, // Start with duty cycle 0
};

// Establece los pines del motor izquierdo
void setMotorIzquierdo(gpio_num_t input1, gpio_num_t input2, gpio_num_t inputEnable)
{
    motorIzquierdoInput1 = input1;
    motorIzquierdoInput2 = input2;
    motorIzquierdoEnable = inputEnable;
}

// Establece los pines del motor derecho
void setMotorDerecho(gpio_num_t input1, gpio_num_t input2, gpio_num_t inputEnable)
{
    motorDerechoInput1 = input1;
    motorDerechoInput2 = input2;
    motorDerechoEnable = inputEnable;
}

void detener()
{
    gpio_set_level(motorIzquierdoInput1, 0);
    gpio_set_level(motorIzquierdoInput2, 0);
    gpio_set_level(motorDerechoInput1, 0);
    gpio_set_level(motorDerechoInput2, 0);

    vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 5 seconds before starting
    detenido = 1;
    printf("Motores detenidos\n");
}

void avanzar()
{
    gpio_set_level(motorIzquierdoInput1, 1);
    gpio_set_level(motorIzquierdoInput2, 0);
    gpio_set_level(motorDerechoInput1, 1);
    gpio_set_level(motorDerechoInput2, 0);
    printf("Motores avanzando\n");
    detenido = 0;
}

void retroceder()
{
    gpio_set_level(motorIzquierdoInput1, 0);
    gpio_set_level(motorIzquierdoInput2, 1);
    gpio_set_level(motorDerechoInput1, 0);
    gpio_set_level(motorDerechoInput2, 1);
    printf("Motores retrocediendo\n");
    detenido = 0;
}

void girarDerecha()
{
    gpio_set_level(motorIzquierdoInput1, 1);
    gpio_set_level(motorIzquierdoInput2, 0);
    gpio_set_level(motorDerechoInput1, 0);
    gpio_set_level(motorDerechoInput2, 1);
    printf("Motores girando a la derecha\n");
    detenido = 0;
}

void girarIzquierda()
{
    gpio_set_level(motorIzquierdoInput1, 0);
    gpio_set_level(motorIzquierdoInput2, 1);
    gpio_set_level(motorDerechoInput1, 1);
    gpio_set_level(motorDerechoInput2, 0);
    printf("Motores girando a la izquierda\n");
    detenido = 0;
}

void setVelocidadMotorIzquierdo(int velocidad)
{
    if (velocidad < 0)
    {
        velocidad = 0; // Ensure speed is not negative
    }
    if (velocidad > 1023)
    {
        velocidad = 1023; // Ensure speed does not exceed maximum for 10-bit resolution
    }

    ledc_set_duty(ledc_channel_a_config.speed_mode, ledc_channel_a_config.channel, velocidad);
    ledc_update_duty(ledc_channel_a_config.speed_mode, ledc_channel_a_config.channel);
    printf("Velocidad motor izquierdo: %d\n", velocidad);
}

void setVelocidadMotorDerecho(int velocidad)
{
    if (velocidad < 0)
    {
        velocidad = 0; // Ensure speed is not negative
    }
    if (velocidad > 1023)
    {
        velocidad = 1023; // Ensure speed does not exceed maximum for 10-bit resolution
    }

    ledc_set_duty(ledc_channel_b_config.speed_mode, ledc_channel_b_config.channel, velocidad);
    ledc_update_duty(ledc_channel_b_config.speed_mode, ledc_channel_b_config.channel);
    printf("Velocidad motor derecho: %d\n", velocidad);
}

int estaDetenido()
{
    return detenido;
}

void inicializarControlMovimiento(ControlMovimientoConfig controlMovParams)
{
    // Set GPIO for right Motor
    motorDerechoInput1 = controlMovParams.rightMotorGpio1;
    motorDerechoInput2 = controlMovParams.rightMotorGpio2;
    motorDerechoEnable = controlMovParams.rightMotorGpioPwm;

    // Set GPIO for left Motor
    motorIzquierdoInput1 = controlMovParams.leftMotorGpio1;
    motorIzquierdoInput2 = controlMovParams.leftMotorGpio2;
    motorIzquierdoEnable = controlMovParams.leftMotorGpioPwm;

    // Set GPIO directions to be output
    gpio_set_direction(motorIzquierdoInput1, GPIO_MODE_OUTPUT);
    gpio_set_direction(motorIzquierdoInput2, GPIO_MODE_OUTPUT);
    gpio_set_direction(motorDerechoInput1, GPIO_MODE_OUTPUT);
    gpio_set_direction(motorDerechoInput2, GPIO_MODE_OUTPUT);

    // Initialize LEDC channels for PWM control
    // ledc_timer_config_data.timer_num = controlMovParams.timerNumber;
    ledc_channel_a_config.gpio_num = motorIzquierdoEnable;
    ledc_channel_a_config.timer_sel = controlMovParams.timerNumber;
    ledc_channel_b_config.gpio_num = motorDerechoEnable;
    ledc_channel_b_config.timer_sel = controlMovParams.timerNumber;

    // printf("%d\n", ledc_timer_config(&ledc_timer_config_data));
    printf("%d\n", ledc_channel_config(&ledc_channel_a_config));
    printf("%d\n", ledc_channel_config(&ledc_channel_b_config));

    // prevent the car from move headless
    detener();
}
#ifndef CONTROL_MOVIMIENTO_H
#define CONTROL_MOVIMIENTO_H
#include <driver/gpio.h>

typedef struct
{
    int timerNumber;
    int pwmChannelMotorRight;
    int pwmChannelMotorTwo;
    int leftMotorGpio1;
    int leftMotorGpio2;
    int leftMotorGpioPwm;
    int rightMotorGpio1;
    int rightMotorGpio2;
    int rightMotorGpioPwm;
    int maxVelocity;
    int minVelocity;
} ControlMovimientoConfig;

void inicializarControlMovimiento(ControlMovimientoConfig);
void setMotorIzquierdo(gpio_num_t input1, gpio_num_t input2, gpio_num_t inputEnable);
void setMotorDerecho(gpio_num_t input1, gpio_num_t input2, gpio_num_t inputEnable);
void avanzar();
void retroceder();
void detener(void);
int estaDetenido();
void setVelocidadMotorIzquierdo(int velocidad);
void setVelocidadMotorDerecho(int velocidad);
void girarDerecha();
void girarIzquierda();

#endif // CONTROL_MOVIMIENTO_H

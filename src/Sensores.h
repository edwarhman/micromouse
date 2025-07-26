#ifndef SENSORES_H
#define SENSORES_H

typedef struct
{
    int sensorFrontal;
    int sensorIzquierdo;
    int sensorDerecho;
    int sensorSonicoTrigger; // GPIO for ultrasonic sensor trigger
    int sensorSonicoEcho;    // GPIO for ultrasonic sensor echo
    int sensorRuedaA;        // GPIO for wheel A sensor
    int sensorRuedaB;        // GPIO for wheel B sensor
} SensoresConfig;

typedef struct
{
    /* data */
    int sensorFrontal;
    int sensorDerecho;
    int sensorIzquierdo;
    int nivelSensorSonico;
} SensoresData;

void inicializarSensores(SensoresConfig);
int leerSensorFrontal();
int leerSensorDerecho();
int leerSensorIzquierdo();
int medirDistanciaSensorSonico();
int obtenerCuentaSensorRueda();
int reiniciarCuentaSensorRueda();

#endif
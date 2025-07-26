#include "Sensores.h"
#include "ControlMovimiento.h"
#include "Navegacion.h"

enum EstadoCarro
{
    AVANZANDO,
    FRENANDO,
    DETENIDO,
    GIRANDO
};

typedef struct
{
    int sensorFrontal;
    int sensorDerecho;
    int sensorIzquierdo;
    int nivelSensorSonico;
} MockupSensores;

MockupSensores mockupSensores[] = {
    {1, 0, 1, 10}, // avance 1
    {0, 0, 1, 10}, // frenada 2
    {1, 0, 1, 20}, // avance 2
    {0, 0, 1, 20}, // frenada 4
    {1, 1, 0, 30}, // avance 3
    {0, 1, 0, 30}, // frenada 6
    {1, 1, 0, 40}, // avance 4
    {0, 1, 0, 40}, // frenada 8
    {0, 1, 1, 40}, // avance 5
    {0, 1, 0, 40}, // avance 6
    {1, 1, 1, 50}, // avance 7
    {1, 0, 1, 50}, // frenada 10

    {0, 0, 1, 60}, // avance 8
    {0, 0, 0, 60},
    {0, 1, 1, 60}, // avance 9
    {1, 0, 1, 10}, // avance 10
    {1, 0, 1, 20}, // Mockup data for frontal sensor
};
int pasosMockup = 0;

enum EstadoCarro estado = AVANZANDO;
int laberinto[4][3];
int posicionActual[2] = {0, 0};   // {fila, columna}
int posicionObjetivo[2] = {1, 0}; // {fila, columna}
int sentidoActual = 0;            // 0: adelante, 1: derecha, 2: atras, 3: izquierda
int sentidoObjetivo = 0;          // 0: adelante, 1: derecha, 2: atras, 3: izquierda
SensoresData datosSensores;

int estaEnPosicionObjetivo()
{
    return (posicionActual[0] == posicionObjetivo[0] && posicionActual[1] == posicionObjetivo[1]);
}

void incrementarSentido(int *sentido)
{
    (*sentido)++;
    if (*sentido > 3)
    {
        *sentido = 0;
    }
}

void decrementarSentido(int *sentido)
{
    (*sentido)--;
    if (*sentido < 0)
    {
        *sentido = 3;
    }
}

// TODO: Implementar lógica de resolución del laberinto
void actualizarSentidoObjetivo()
{
    if (datosSensores.sensorFrontal == 1 || estaEnPosicionObjetivo())
    {
        if (datosSensores.sensorDerecho == 0)
        {
            incrementarSentido(&sentidoObjetivo); // derecha
        }
        else if (datosSensores.sensorIzquierdo == 0)
        {
            decrementarSentido(&sentidoObjetivo); // izquierda
        }
        else if (datosSensores.sensorFrontal == 1 && datosSensores.sensorDerecho == 1 && datosSensores.sensorIzquierdo == 1)
        {
            incrementarSentido(&sentidoObjetivo); // adelante
        }
    }
}

void actualizarPosicionActual()
{
    if (sentidoActual == 0)
    {
        posicionActual[0] += 1; // adelante
    }
    else if (sentidoActual == 1)
    {
        posicionActual[1] += 1; // derecha
    }
    else if (sentidoActual == 2)
    {
        posicionActual[0] -= 1; // atras
    }
    else if (sentidoActual == 3)
    {
        posicionActual[1] -= 1; // izquierda
    }
}

void actualizarPosicionObjetivo()
{
    if (!estaEnPosicionObjetivo() || datosSensores.sensorFrontal == 1)
    {
        printf("Ya estamos en la posición objetivo o hay pared al frente.\n");
        return;
    }
    if (sentidoObjetivo == 0)
    {
        posicionObjetivo[0] += 1; // adelante
    }
    else if (sentidoObjetivo == 1)
    {
        posicionObjetivo[1] += 1; // derecha
    }
    else if (sentidoObjetivo == 2)
    {
        posicionObjetivo[0] -= 1; // atras
    }
    else if (sentidoObjetivo == 3)
    {
        posicionObjetivo[1] -= 1; // izquierda
    }
}

int estaEnSentidoObjetivo()
{
    return sentidoActual == sentidoObjetivo;
}

void girarSentidoDeseado()
{
    if (!estaEnSentidoObjetivo())
    {
        int diferencia = sentidoObjetivo - sentidoActual;
        if (diferencia == 1 || diferencia == -3)
        {
            girarDerecha();
            incrementarSentido(&sentidoActual);
        }
        else if (diferencia == -1 || diferencia == 3)
        {
            girarIzquierda();
            decrementarSentido(&sentidoActual);
        }
        else if (diferencia == 2 || diferencia == -2)
        {
            girarDerecha();
            incrementarSentido(&sentidoActual);
        }
    }
}

void simularAvance()
{
    if (pasosMockup >= 16)
    {
        return;
    }
    if (datosSensores.sensorFrontal != 1)
    {

        actualizarPosicionActual();
    }
    datosSensores.sensorFrontal = mockupSensores[pasosMockup].sensorFrontal;
    datosSensores.sensorDerecho = mockupSensores[pasosMockup].sensorDerecho;
    datosSensores.sensorIzquierdo = mockupSensores[pasosMockup].sensorIzquierdo;
    datosSensores.nivelSensorSonico = mockupSensores[pasosMockup].nivelSensorSonico;
    pasosMockup++;
}

void maquinaDeEstados()
{
    switch (estado)
    {
    case AVANZANDO:
        printf("Estado AVANZANDO\n");
        if (datosSensores.sensorFrontal == 1 || estaEnPosicionObjetivo())
        {
            detener();
            estado = FRENANDO;
        }
        break;
    case FRENANDO:
        printf("Estado FRENANDO\n");
        if (estaDetenido())
        {
            estado = DETENIDO;
            actualizarSentidoObjetivo();
            printf("Cambiando a estado DETENIDO en la maquina de estados.\n");
        }

        break;
    case DETENIDO:
        printf("Estado DETENIDO\n");
        if (estaEnSentidoObjetivo())
        {
            if (!estaEnPosicionObjetivo())
            {
                avanzar();
                estado = AVANZANDO;
                printf("Cambiando a estado AVANZANDO en la maquina de estados.\n");
            }
        }
        else
        {
            girarSentidoDeseado();
            estado = GIRANDO;
            printf("Cambiando a estado GIRANDO en la maquina de estados.\n");
        }
        printf("Estamos en estado detenido \n");
        break;
    case GIRANDO:
        printf("Estado GIRANDO");
        if (estaEnSentidoObjetivo()) // sustituir cuando se pueda obtener la posición de la rueda
        {
            estado = FRENANDO;
            simularAvance();
            printf("SIMULANDO GIRO!!!!.\n");
            detener();
            printf("Cambiando a estado FRENANDO en la maquina de estados.\n");
        }
        else
        {
            printf("Girando hacia el sentido deseado.\n");
        }

        break;

    default:
        break;
    }
    actualizarPosicionObjetivo();
    printf("Estado actual: %d\n", estado);
    printf("Posición actual: [%d, %d]\n", posicionActual[0], posicionActual[1]);
    printf("Sentido actual: %d\n", sentidoActual);
    printf("Sentido objetivo: %d\n", sentidoObjetivo);
    printf("Posición objetivo: [%d, %d]\n", posicionObjetivo[0], posicionObjetivo[1]);
    printf("Datos sensores: Frontal: %d, Derecho: %d, Izquierdo: %d, Sonico: %d\n",
           datosSensores.sensorFrontal, datosSensores.sensorDerecho, datosSensores.sensorIzquierdo, datosSensores.nivelSensorSonico);
}
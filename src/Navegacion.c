#include "Sensores.h"
#include "ControlMovimiento.h"
#include "Navegacion.h"

#define PASOS_AVANCE_DE_CASILLA 7
#define PASOS_GIRO_90_GRADOS 4

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

// frontal, derecho, izquierdo, nivelSonico
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
    {0, 1, 1, 60}, // avance 8
    {0, 0, 1, 60},
    {0, 0, 0, 60}, // avance 9
    {0, 1, 1, 10}, // avance 10
    {1, 0, 1, 20}, // Mockup data for frontal sensor
    {1, 0, 1, 20}, // Mockup data for frontal sensor
    {0, 0, 1, 20}, // Mockup data for frontal sensor
    {1, 0, 1, 20}, // Mockup data for frontal sensor
    {0, 0, 1, 20}, // Mockup data for frontal sensor
    {1, 1, 1, 20}, // Mockup data for frontal sensor
};
int pasosMockup = 0;

enum EstadoCarro estado = AVANZANDO;
int laberinto[4][3];
int posicionActual[2] = {0, 0};   // {fila, columna}
int posicionObjetivo[2] = {1, 0}; // {fila, columna}
int sentidoActual = 0;            // 0: adelante, 1: derecha, 2: atras, 3: izquierda
int sentidoObjetivo = 0;          // 0: adelante, 1: derecha, 2: atras, 3: izquierda
int proximoSentido = 0;           // Variable auxiliar para almacenar el sentido al que gira después de girar
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

int obtenerCasillaSegura(int fila, int columna)
{
    if (fila < 0 || fila >= 4 || columna < 0 || columna >= 3)
    {
        return -1; // Fuera de rango
    }
    return laberinto[fila][columna]; // Retorna el valor de la casilla
}

int obtenerCasillaSiguiente()
{
    int fila = posicionActual[0];
    int columna = posicionActual[1];
    int sentido = sentidoActual;
    switch (sentido)
    {
    case 0: // adelante
        return obtenerCasillaSegura(fila + 1, columna);
    case 1: // derecha
        return obtenerCasillaSegura(fila, columna + 1);
    case 2: // atras
        return obtenerCasillaSegura(fila - 1, columna);
    case 3: // izquierda
        return obtenerCasillaSegura(fila, columna - 1);
    default:
        return 0; // Sentido no válido
    }
}

// TODO: Implementar lógica de resolución del laberinto
void actualizarSentidoObjetivo()
{
    int casillaFrontal = obtenerCasillaSiguiente();
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
    if (
        !estaEnSentidoObjetivo() ||
        !estaEnPosicionObjetivo() ||
        datosSensores.sensorFrontal == 1 ||
        ((datosSensores.sensorDerecho == 0 || datosSensores.sensorIzquierdo == 0) && obtenerCasillaSiguiente() > 0))
    {
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
            incrementarSentido(&proximoSentido);
        }
        else if (diferencia == -1 || diferencia == 3)
        {
            girarIzquierda();
            decrementarSentido(&proximoSentido);
        }
        else if (diferencia == 2 || diferencia == -2)
        {
            girarDerecha();
            incrementarSentido(&proximoSentido);
        }
    }
}

void actualizarLaberinto(int fila, int columna, int valor)
{
    if (fila < 0 || fila >= 4 || columna < 0 || columna >= 3)
    {
        printf("Índice fuera de rango al actualizar el laberinto.\n");
        return;
    }
    laberinto[fila][columna] = valor;
}

void manejarCambioDeCasilla()
{
    if (datosSensores.cuentaSensorRueda >= PASOS_AVANCE_DE_CASILLA)
    {
        reiniciarCuentaSensorRueda();
        actualizarLaberinto(posicionActual[0], posicionActual[1], 2); // Marcar casilla como visitada
        actualizarPosicionActual();

        printf("Casilla visitada: [%d, %d]\n", posicionActual[0], posicionActual[1]);
        simularAvance();
    }
}

void manejarGiroDe90Grados()
{
    if (datosSensores.cuentaSensorRueda >= PASOS_GIRO_90_GRADOS)
    {
        reiniciarCuentaSensorRueda();
        printf("Giro de 90 grados completado.\n");
        sentidoActual = proximoSentido; // Actualizar sentido actual al sentido al que se ha girado
        printf("Sentido actual actualizado a: %d\n", sentidoActual);
        estado = FRENANDO; // Cambiar estado a FRENANDO después del giro
        detener();         // Detener el carro después del giro
        simularAvance();   // Simular avance después del giro
    }
}

void simularAvance()
{
    printf("SIMULAR AVANCE!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    if (pasosMockup >= 22)
    {
        return;
    }
    datosSensores.sensorFrontal = mockupSensores[pasosMockup].sensorFrontal;
    datosSensores.sensorDerecho = mockupSensores[pasosMockup].sensorDerecho;
    datosSensores.sensorIzquierdo = mockupSensores[pasosMockup].sensorIzquierdo;
    datosSensores.nivelSensorSonico = mockupSensores[pasosMockup].nivelSensorSonico;
    pasosMockup++;
}

void imprimirSentido(int sentido)
{
    switch (sentido)
    {
    case 0:
        printf("NORTE\n");
        break;
    case 1:
        printf("DERECHA\n");
        break;
    case 2:
        printf("SUR\n");
        break;
    case 3:
        printf("IZQUIERDA\n");
        break;
    default:
        printf("Sentido no válido\n");
        break;
    }
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
        manejarCambioDeCasilla();
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
        manejarGiroDe90Grados();
        break;

    default:
        break;
    }
    actualizarPosicionObjetivo();
    printf("Estado actual: %d\n", estado);
    printf("Posición actual: [%d, %d]\n", posicionActual[0], posicionActual[1]);
    printf("Posición objetivo: [%d, %d]\n", posicionObjetivo[0], posicionObjetivo[1]);
    imprimirSentido(sentidoActual);
    imprimirSentido(sentidoObjetivo);
    printf("Datos sensores: Frontal: %d, Derecho: %d, Izquierdo: %d, Sonico: %d\n",
           datosSensores.sensorFrontal, datosSensores.sensorDerecho, datosSensores.sensorIzquierdo, datosSensores.nivelSensorSonico);

    datosSensores.cuentaSensorRueda = obtenerCuentaSensorRueda(); // TODO: sustituir por la función real de obtención de datos sensores
    // datosSensores = obtenerDatosSensores(); // Actualizar datosSensores con la función real
}
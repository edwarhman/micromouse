#include "Sensores.h"
#include "ControlMovimiento.h"
#include "Navegacion.h"

#define PASOS_AVANCE_DE_CASILLA 4
#define PASOS_GIRO_90_GRADOS 2
#define LABERINTO_FILAS 4
#define LABERINTO_COLUMNAS 3

typedef enum
{
    NORTE = 0,
    ESTE = 1,
    SUR = 2,
    OESTE = 3
} SentidoCarro;

typedef enum
{
    AVANZANDO = 0,
    FRENANDO = 1,
    DETENIDO = 2,
    GIRANDO = 3
} EstadoCarro;

typedef struct
{
    int sensorFrontal;
    int sensorDerecho;
    int sensorIzquierdo;
    int nivelSensorSonico;
} MockupSensores;

// frontal, derecho, izquierdo, nivelSonico
MockupSensores mockupSensores[] = {
    {1, 0, 1, 10}, // paso 0
    {0, 0, 1, 10}, // paso 1
    {1, 0, 1, 20}, // paso 2
    {0, 0, 1, 20}, // paso 3
    {1, 1, 0, 30}, // paso 4
    {0, 1, 0, 30}, // paso 5
    {1, 1, 0, 40}, // paso 6
    {0, 1, 0, 40}, // paso 7
    {0, 1, 1, 40}, // paso 8
    {0, 1, 0, 40}, // paso 9
    {1, 1, 1, 50}, // paso 10
    {1, 0, 1, 50}, // paso 11
    {0, 1, 1, 60}, // paso 12
    {0, 0, 1, 60}, // paso 13
    {0, 0, 0, 60}, // paso 14
    {0, 1, 1, 10}, // paso 15
    {1, 0, 1, 20}, // paso 16
    {0, 0, 1, 20}, // paso 17
    {1, 0, 1, 20}, // paso 18
    {0, 0, 1, 20}, // paso 19
    {1, 1, 1, 20}, // paso 20
};
int pasosMockup = 0;

EstadoCarro estado = AVANZANDO;
int laberinto[LABERINTO_FILAS][LABERINTO_COLUMNAS];
int posicionActual[2] = {0, 0};   // {fila, columna}
int posicionObjetivo[2] = {1, 0}; // {fila, columna}
SentidoCarro sentidoActual = NORTE;
SentidoCarro sentidoObjetivo = NORTE;
SentidoCarro proximoSentido = NORTE;
SensoresData datosSensores;

int estaEnPosicionObjetivo()
{
    return (posicionActual[0] == posicionObjetivo[0] && posicionActual[1] == posicionObjetivo[1]);
}

int estaEnSentidoObjetivo()
{
    return sentidoActual == sentidoObjetivo;
}

void incrementarSentido(SentidoCarro *sentido)
{
    (*sentido)++;
    if (*sentido > OESTE)
    {
        *sentido = NORTE;
    }
}

void decrementarSentido(SentidoCarro *sentido)
{
    (*sentido)--;
    if (*sentido < NORTE)
    {
        *sentido = OESTE;
    }
}

int obtenerCasillaSegura(int fila, int columna)
{
    if (fila < 0 || fila >= LABERINTO_FILAS || columna < 0 || columna >= LABERINTO_COLUMNAS)
    {
        return -1; // Fuera de rango
    }
    return laberinto[fila][columna]; // Retorna el valor de la casilla
}

int obtenerCasillaSiguiente()
{
    int fila = posicionActual[0];
    int columna = posicionActual[1];
    switch (sentidoActual)
    {
    case NORTE:
        return obtenerCasillaSegura(fila + 1, columna);
    case ESTE:
        return obtenerCasillaSegura(fila, columna + 1);
    case SUR:
        return obtenerCasillaSegura(fila - 1, columna);
    case OESTE:
        return obtenerCasillaSegura(fila, columna - 1);
    default:
        return 0; // Sentido no válido
    }
}

int obtenerCasillaActual()
{
    return laberinto[posicionActual[0]][posicionActual[1]];
}

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

void moverPosicion(int *pos, SentidoCarro sentido)
{
    switch (sentido)
    {
    case NORTE:
        pos[0] += 1; // adelante
        break;
    case ESTE:
        pos[1] += 1; // derecha
        break;
    case SUR:
        pos[0] -= 1; // atras
        break;
    case OESTE:
        pos[1] -= 1; // izquierda
        break;
    }
}

void actualizarPosicionActual()
{
    moverPosicion(posicionActual, sentidoActual);
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
    moverPosicion(posicionObjetivo, sentidoObjetivo);
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

void imprimirSentido(SentidoCarro sentido)
{
    switch (sentido)
    {
    case NORTE:
        printf("NORTE\n");
        break;
    case ESTE:
        printf("ESTE\n");
        break;
    case SUR:
        printf("SUR\n");
        break;
    case OESTE:
        printf("OESTE\n");
        break;
    default:
        printf("Sentido no válido\n");
        break;
    }
}

char *obtenerCadenaEstado(EstadoCarro estado)
{
    switch (estado)
    {
    case AVANZANDO:
        return "AVANZANDO";
    case FRENANDO:
        return "FRENANDO";
    case DETENIDO:
        return "DETENIDO";
    case GIRANDO:
        return "GIRANDO";
    default:
        return "ESTADO DESCONOCIDO";
    }
}

void imprimirEstadoRobot()
{
    printf("Estado actual: %s\n", obtenerCadenaEstado(estado));
    printf("Posición actual: [%d, %d]\n", posicionActual[0], posicionActual[1]);
    printf("Posición objetivo: [%d, %d]\n", posicionObjetivo[0], posicionObjetivo[1]);
    printf("Sentido actual: ");
    imprimirSentido(sentidoActual);
    printf("Sentido objetivo: ");
    imprimirSentido(sentidoObjetivo);
    printf("Datos sensores: Frontal: %d, Derecho: %d, Izquierdo: %d, Sonico: %d\n",
           datosSensores.sensorFrontal, datosSensores.sensorDerecho, datosSensores.sensorIzquierdo, datosSensores.nivelSensorSonico);
    printf("Valor casilla siguiente: %d\n", obtenerCasillaSiguiente());
    printf("Valor casilla actual: %d\n", obtenerCasillaActual());
    printf("Paso simulación: %d\n", pasosMockup);
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
        manejarGiroDe90Grados();
        break;

    default:
        break;
    }
    actualizarPosicionObjetivo();
    imprimirEstadoRobot();
    datosSensores.cuentaSensorRueda = obtenerCuentaSensorRueda(); // TODO: sustituir por la función real de obtención de datos sensores
    // datosSensores = obtenerDatosSensores(); // Actualizar datosSensores con la función real
}
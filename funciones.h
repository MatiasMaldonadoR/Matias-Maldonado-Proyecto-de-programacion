#ifndef FUNCIONES_H
#define FUNCIONES_H

#define NUM_ESTACIONES 5
#define DIAS_REGISTRO 30
#define LARGO_NOMBRE 50
#define LARGO_TEXTO 300

#define ARCHIVO_REGISTROS "registro_estaciones.dat"
#define ARCHIVO_INFORME "informe_calidad_aire.txt"

#define TOPE_CO2 1000.0f
#define TOPE_SO2 40.0f
#define TOPE_NO2 25.0f
#define TOPE_PM25 15.0f

typedef struct {
    float carbono;
    float azufre;
    float nitrogeno;
    float particulas;
} Medicion;

typedef struct {
    float gradoCalor;
    float vientoKmh;
    float humedadRel;
} Ambiente;

typedef struct {
    char nombre[LARGO_NOMBRE];
    Medicion historial[DIAS_REGISTRO];
    Medicion medicionActual;
    Medicion pronostico;
    Ambiente ambiente;
} Estacion;

void vaciarBuffer();
int pedirEntero(char mensaje[], int min, int max);
float pedirDecimal(char mensaje[], float min, float max);
void pedirTexto(char mensaje[], char destino[], int tam);

void prepararEstaciones(Estacion estaciones[]);
int recuperarRegistros(Estacion estaciones[]);
void almacenarRegistros(Estacion estaciones[]);

void desplegarMenu();
void capturarEstacion(Estacion *estacion);
void capturarDatos(Estacion estaciones[]);

void definirActual(Estacion *estacion);
float mediaPonderada(float valores[]);
void pronosticarEstacion(Estacion *estacion);
void obtenerMediaHistorica(Estacion *estacion, Medicion *media);

int excedeTopes(Medicion datos);
void construirAlerta(Estacion estacion, char alerta[]);
void construirRecomendacion(Estacion estacion, char recomendacion[]);

void verEstadoActual(Estacion estaciones[]);
void verPronosticos(Estacion estaciones[]);
void verAlertas(Estacion estaciones[]);
void verMedias(Estacion estaciones[]);
void verRecomendaciones(Estacion estaciones[]);

void exportarInforme(Estacion estaciones[]);

#endif
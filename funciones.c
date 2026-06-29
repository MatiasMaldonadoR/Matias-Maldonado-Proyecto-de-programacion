#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

void vaciarBuffer() {
    int caracter;
    while ((caracter = getchar()) != '\n' && caracter != EOF);
}

int pedirEntero(char mensaje[], int min, int max) {
    int numero, lectura;
    do {
        printf("%s", mensaje);
        lectura = scanf("%d", &numero);
        if (lectura == EOF) {
            printf("\nEntrada finalizada. Cerrando el programa.\n");
            exit(0);
        }
        if (lectura != 1 || numero < min || numero > max) {
            printf("Entrada invalida. Escriba un numero entre %d y %d.\n", min, max);
            vaciarBuffer();
        }
    } while (lectura != 1 || numero < min || numero > max);
    vaciarBuffer();
    return numero;
}

float pedirDecimal(char mensaje[], float min, float max) {
    float numero;
    int lectura;
    do {
        printf("%s", mensaje);
        lectura = scanf("%f", &numero);
        if (lectura == EOF) {
            printf("\nEntrada finalizada. Cerrando el programa.\n");
            exit(0);
        }
        if (lectura != 1 || numero < min || numero > max) {
            printf("Entrada invalida. Escriba un valor entre %.2f y %.2f.\n", min, max);
            vaciarBuffer();
        }
    } while (lectura != 1 || numero < min || numero > max);
    vaciarBuffer();
    return numero;
}

void pedirTexto(char mensaje[], char destino[], int tam) {
    int valido, k;
    do {
        valido = 1;
        printf("%s", mensaje);
        if (fgets(destino, tam, stdin) == NULL) {
            printf("\nEntrada finalizada. Cerrando el programa.\n");
            exit(0);
        }
        if (destino[0] == '\n' || destino[0] == '\0') {
            printf("Entrada invalida. El texto no puede quedar vacio.\n");
            valido = 0;
        }
    } while (valido == 0);

    for (k = 0; destino[k] != '\0'; k++) {
        if (destino[k] == '\n') {
            destino[k] = '\0';
            break;
        }
    }
}

void prepararEstaciones(Estacion estaciones[]) {
    int e, d;
    for (e = 0; e < NUM_ESTACIONES; e++) {
        sprintf(estaciones[e].nombre, "Estacion_%d", e + 1);
        estaciones[e].ambiente.gradoCalor = 0;
        estaciones[e].ambiente.vientoKmh = 0;
        estaciones[e].ambiente.humedadRel = 0;
        estaciones[e].medicionActual.carbono = 0;
        estaciones[e].medicionActual.azufre = 0;
        estaciones[e].medicionActual.nitrogeno = 0;
        estaciones[e].medicionActual.particulas = 0;
        estaciones[e].pronostico.carbono = 0;
        estaciones[e].pronostico.azufre = 0;
        estaciones[e].pronostico.nitrogeno = 0;
        estaciones[e].pronostico.particulas = 0;
        for (d = 0; d < DIAS_REGISTRO; d++) {
            estaciones[e].historial[d].carbono = 0;
            estaciones[e].historial[d].azufre = 0;
            estaciones[e].historial[d].nitrogeno = 0;
            estaciones[e].historial[d].particulas = 0;
        }
    }
}

int recuperarRegistros(Estacion estaciones[]) {
    FILE *flujo = fopen(ARCHIVO_REGISTROS, "rb");
    if (flujo == NULL) return 0;
    fread(estaciones, sizeof(Estacion), NUM_ESTACIONES, flujo);
    fclose(flujo);
    return 1;
}

void almacenarRegistros(Estacion estaciones[]) {
    FILE *flujo = fopen(ARCHIVO_REGISTROS, "wb");
    if (flujo == NULL) {
        printf("No fue posible guardar el archivo de registros.\n");
        return;
    }
    fwrite(estaciones, sizeof(Estacion), NUM_ESTACIONES, flujo);
    fclose(flujo);
}

void desplegarMenu() {
    printf("\n********** VIGILANCIA DE LA CALIDAD DEL AIRE **********\n");
    printf("1) Consultar estado actual\n");
    printf("2) Consultar pronostico a 24 horas\n");
    printf("3) Consultar alertas preventivas\n");
    printf("4) Consultar media historica de 30 dias\n");
    printf("5) Consultar medidas de mitigacion\n");
    printf("6) Registrar o actualizar informacion\n");
    printf("0) Finalizar\n");
    printf("******************************************************\n");
}

void capturarEstacion(Estacion *estacion) {
    int d;
    pedirTexto("Nombre de la estacion: ", estacion->nombre, LARGO_NOMBRE);

    printf("\nRegistre las condiciones ambientales actuales:\n");
    estacion->ambiente.gradoCalor = pedirDecimal("Temperatura (C): ", 0, 60);
    estacion->ambiente.vientoKmh = pedirDecimal("Velocidad del viento (km/h): ", 0, 200);
    estacion->ambiente.humedadRel = pedirDecimal("Humedad relativa (%): ", 0, 100);

    printf("\nRegistre el historial de los ultimos 30 dias:\n");
    for (d = 0; d < DIAS_REGISTRO; d++) {
        printf("\n-- Dia %d --\n", d + 1);
        estacion->historial[d].carbono = pedirDecimal("CO2: ", 0, 5000);
        estacion->historial[d].azufre = pedirDecimal("SO2: ", 0, 1000);
        estacion->historial[d].nitrogeno = pedirDecimal("NO2: ", 0, 1000);
        estacion->historial[d].particulas = pedirDecimal("PM2.5: ", 0, 1000);
    }

    definirActual(estacion);
    pronosticarEstacion(estacion);
}

void capturarDatos(Estacion estaciones[]) {
    int eleccion, indice, e;
    printf("\n1) Registrar una sola estacion\n");
    printf("2) Registrar las 5 estaciones\n");
    eleccion = pedirEntero("Indique su eleccion: ", 1, 2);

    if (eleccion == 1) {
        indice = pedirEntero("Numero de estacion (1-5): ", 1, NUM_ESTACIONES);
        capturarEstacion(&estaciones[indice - 1]);
    } else {
        for (e = 0; e < NUM_ESTACIONES; e++) {
            printf("\n>>>>> ESTACION %d <<<<<\n", e + 1);
            capturarEstacion(&estaciones[e]);
        }
    }

    almacenarRegistros(estaciones);
    exportarInforme(estaciones);
    printf("\nInformacion registrada con exito.\n");
    printf("Informe creado automaticamente en %s\n", ARCHIVO_INFORME);
}

void definirActual(Estacion *estacion) {
    estacion->medicionActual = estacion->historial[DIAS_REGISTRO - 1];
}

float mediaPonderada(float valores[]) {
    float acumulado = 0, totalPesos = 0;
    int d;
    for (d = 0; d < DIAS_REGISTRO; d++) {
        int peso = d + 1;
        acumulado += valores[d] * peso;
        totalPesos += peso;
    }
    return acumulado / totalPesos;
}

void pronosticarEstacion(Estacion *estacion) {
    float carbono[DIAS_REGISTRO], azufre[DIAS_REGISTRO];
    float nitrogeno[DIAS_REGISTRO], particulas[DIAS_REGISTRO];
    float ajuste = 1.0f;
    int d;

    for (d = 0; d < DIAS_REGISTRO; d++) {
        carbono[d] = estacion->historial[d].carbono;
        azufre[d] = estacion->historial[d].azufre;
        nitrogeno[d] = estacion->historial[d].nitrogeno;
        particulas[d] = estacion->historial[d].particulas;
    }

    if (estacion->ambiente.gradoCalor > 30) ajuste += 0.10f;
    if (estacion->ambiente.humedadRel > 70) ajuste += 0.05f;
    if (estacion->ambiente.vientoKmh > 20) ajuste -= 0.10f;
    if (ajuste < 0.50f) ajuste = 0.50f;

    estacion->pronostico.carbono = mediaPonderada(carbono) * ajuste;
    estacion->pronostico.azufre = mediaPonderada(azufre) * ajuste;
    estacion->pronostico.nitrogeno = mediaPonderada(nitrogeno) * ajuste;
    estacion->pronostico.particulas = mediaPonderada(particulas) * ajuste;
}

void obtenerMediaHistorica(Estacion *estacion, Medicion *media) {
    int d;
    float carbono = 0, azufre = 0, nitrogeno = 0, particulas = 0;
    for (d = 0; d < DIAS_REGISTRO; d++) {
        carbono += estacion->historial[d].carbono;
        azufre += estacion->historial[d].azufre;
        nitrogeno += estacion->historial[d].nitrogeno;
        particulas += estacion->historial[d].particulas;
    }
    media->carbono = carbono / DIAS_REGISTRO;
    media->azufre = azufre / DIAS_REGISTRO;
    media->nitrogeno = nitrogeno / DIAS_REGISTRO;
    media->particulas = particulas / DIAS_REGISTRO;
}

int excedeTopes(Medicion datos) {
    if (datos.carbono > TOPE_CO2) return 1;
    if (datos.azufre > TOPE_SO2) return 1;
    if (datos.nitrogeno > TOPE_NO2) return 1;
    if (datos.particulas > TOPE_PM25) return 1;
    return 0;
}

void construirAlerta(Estacion estacion, char alerta[]) {
    if (excedeTopes(estacion.medicionActual) || excedeTopes(estacion.pronostico))
        strcpy(alerta, "ALERTA: la contaminacion supera los topes permitidos.");
    else
        strcpy(alerta, "Normal: los niveles se mantienen dentro del rango.");
}

void construirRecomendacion(Estacion estacion, char recomendacion[]) {
    if (excedeTopes(estacion.medicionActual) || excedeTopes(estacion.pronostico))
        strcpy(recomendacion, "Limitar el trafico, regular industrias y suspender actividades al aire libre.");
    else
        strcpy(recomendacion, "Conservar el monitoreo y las medidas vigentes.");
}

void verEstadoActual(Estacion estaciones[]) {
    int e;
    printf("\n======================= ESTADO ACTUAL =======================\n");
    printf(" #  Estacion            CO2       SO2       NO2       PM2.5\n");
    printf("-------------------------------------------------------------\n");
    for (e = 0; e < NUM_ESTACIONES; e++) {
        definirActual(&estaciones[e]);
        printf(" %-2d %-18s %-9.2f %-9.2f %-9.2f %-9.2f\n",
               e + 1, estaciones[e].nombre,
               estaciones[e].medicionActual.carbono, estaciones[e].medicionActual.azufre,
               estaciones[e].medicionActual.nitrogeno, estaciones[e].medicionActual.particulas);
    }
    printf("=============================================================\n");
}

void verPronosticos(Estacion estaciones[]) {
    int e;
    printf("\n=================== PRONOSTICO A 24 HORAS ===================\n");
    printf(" #  Estacion            CO2       SO2       NO2       PM2.5\n");
    printf("-------------------------------------------------------------\n");
    for (e = 0; e < NUM_ESTACIONES; e++) {
        pronosticarEstacion(&estaciones[e]);
        printf(" %-2d %-18s %-9.2f %-9.2f %-9.2f %-9.2f\n",
               e + 1, estaciones[e].nombre,
               estaciones[e].pronostico.carbono, estaciones[e].pronostico.azufre,
               estaciones[e].pronostico.nitrogeno, estaciones[e].pronostico.particulas);
    }
    printf("=============================================================\n");
}

void verAlertas(Estacion estaciones[]) {
    int e;
    char alerta[LARGO_TEXTO];
    printf("\n==================== ALERTAS PREVENTIVAS ====================\n");
    for (e = 0; e < NUM_ESTACIONES; e++) {
        construirAlerta(estaciones[e], alerta);
        printf(" [%d] %-18s -> %s\n", e + 1, estaciones[e].nombre, alerta);
    }
    printf("=============================================================\n");
}

void verMedias(Estacion estaciones[]) {
    int e;
    Medicion media;
    printf("\n================ MEDIA HISTORICA (30 DIAS) ==================\n");
    printf(" #  Estacion            CO2       SO2       NO2       PM2.5\n");
    printf("-------------------------------------------------------------\n");
    for (e = 0; e < NUM_ESTACIONES; e++) {
        obtenerMediaHistorica(&estaciones[e], &media);
        printf(" %-2d %-18s %-9.2f %-9.2f %-9.2f %-9.2f\n",
               e + 1, estaciones[e].nombre,
               media.carbono, media.azufre, media.nitrogeno, media.particulas);
    }
    printf("=============================================================\n");
}

void verRecomendaciones(Estacion estaciones[]) {
    int e;
    char recomendacion[LARGO_TEXTO];
    printf("\n=================== MEDIDAS DE MITIGACION ===================\n");
    for (e = 0; e < NUM_ESTACIONES; e++) {
        construirRecomendacion(estaciones[e], recomendacion);
        printf(" [%d] %-18s -> %s\n", e + 1, estaciones[e].nombre, recomendacion);
    }
    printf("=============================================================\n");
}

void exportarInforme(Estacion estaciones[]) {
    FILE *flujo = fopen(ARCHIVO_INFORME, "w");
    int e;
    char alerta[LARGO_TEXTO], recomendacion[LARGO_TEXTO];
    Medicion media;

    if (flujo == NULL) {
        printf("No fue posible crear el informe.\n");
        return;
    }

    fprintf(flujo, "############################################################\n");
    fprintf(flujo, "        INFORME DE VIGILANCIA DE LA CALIDAD DEL AIRE\n");
    fprintf(flujo, "############################################################\n\n");

    for (e = 0; e < NUM_ESTACIONES; e++) {
        definirActual(&estaciones[e]);
        pronosticarEstacion(&estaciones[e]);
        obtenerMediaHistorica(&estaciones[e], &media);
        construirAlerta(estaciones[e], alerta);
        construirRecomendacion(estaciones[e], recomendacion);

        fprintf(flujo, "ESTACION %d: %s\n", e + 1, estaciones[e].nombre);
        fprintf(flujo, "   Contaminante |   Actual   | Pronostico |    Media\n");
        fprintf(flujo, "   -------------+------------+------------+------------\n");
        fprintf(flujo, "   CO2          | %10.2f | %10.2f | %10.2f\n",
                estaciones[e].medicionActual.carbono, estaciones[e].pronostico.carbono, media.carbono);
        fprintf(flujo, "   SO2          | %10.2f | %10.2f | %10.2f\n",
                estaciones[e].medicionActual.azufre, estaciones[e].pronostico.azufre, media.azufre);
        fprintf(flujo, "   NO2          | %10.2f | %10.2f | %10.2f\n",
                estaciones[e].medicionActual.nitrogeno, estaciones[e].pronostico.nitrogeno, media.nitrogeno);
        fprintf(flujo, "   PM2.5        | %10.2f | %10.2f | %10.2f\n",
                estaciones[e].medicionActual.particulas, estaciones[e].pronostico.particulas, media.particulas);
        fprintf(flujo, "   Estado: %s\n", alerta);
        fprintf(flujo, "   Medida: %s\n", recomendacion);
        fprintf(flujo, "------------------------------------------------------------\n\n");
    }
    fclose(flujo);
}
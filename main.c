#include <stdio.h>
#include "funciones.h"

int main() {
    Estacion estaciones[NUM_ESTACIONES];
    int opcion;

    prepararEstaciones(estaciones);

    if (recuperarRegistros(estaciones)) {
        printf("Registros recuperados correctamente desde el archivo.\n");
    } else {
        printf("Aun no existen registros. Use la opcion 6 para ingresarlos.\n");
    }

    do {
        desplegarMenu();
        opcion = pedirEntero("Indique su eleccion: ", 0, 6);

        switch (opcion) {
            case 1:
                verEstadoActual(estaciones);
                break;
            case 2:
                verPronosticos(estaciones);
                break;
            case 3:
                verAlertas(estaciones);
                break;
            case 4:
                verMedias(estaciones);
                break;
            case 5:
                verRecomendaciones(estaciones);
                break;
            case 6:
                capturarDatos(estaciones);
                break;
            case 0:
                almacenarRegistros(estaciones);
                exportarInforme(estaciones);
                printf("Informacion almacenada. Cerrando el programa...\n");
                break;
            default:
                printf("La eleccion no es valida.\n");
                break;
        }
    } while (opcion != 0);

    return 0;
}
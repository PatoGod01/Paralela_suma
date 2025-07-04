#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

long long suma_secuencial(int *array, int n) {
    long long resultado = 0;
    for (int i = 0; i < n; i++) {
        resultado += array[i];
    }
    return resultado;
}

long long suma_paralela(int *array, int n) {
    long long resultado = 0;
    #pragma omp parallel for reduction(+:resultado)
    for (int i = 0; i < n; i++) {
        resultado += array[i];
    }
    return resultado;
}

void inicializar_array(int *array, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 100 + 1; // Valores entre 1 y 100
    }
}

void probar_tamaño(int n) {
    printf("\n=== PRUEBA CON N = %d ===\n", n);
    
    // Intentar asignar memoria
    int *array = (int*)malloc(n * sizeof(int));
    if (array == NULL) {
        printf("ERROR: No se pudo asignar memoria para %d elementos\n", n);
        printf("Memoria requerida: %.2f MB\n", (n * sizeof(int)) / (1024.0 * 1024.0));
        return;
    }
    
    printf("Memoria asignada exitosamente: %.2f MB\n", (n * sizeof(int)) / (1024.0 * 1024.0));
    
    // Inicializar array
    printf("Inicializando array...\n");
    inicializar_array(array, n);
    
    // Variables para resultados y tiempos
    long long suma_seq, suma_par;
    double tiempo_seq, tiempo_par;
    double inicio, fin;
    
    // Suma secuencial
    printf("Ejecutando suma secuencial...\n");
    inicio = omp_get_wtime();
    suma_seq = suma_secuencial(array, n);
    fin = omp_get_wtime();
    tiempo_seq = fin - inicio;
    
    // Suma paralela
    printf("Ejecutando suma paralela...\n");
    inicio = omp_get_wtime();
    suma_par = suma_paralela(array, n);
    fin = omp_get_wtime();
    tiempo_par = fin - inicio;
    
    // Mostrar resultados
    printf("Resultados:\n");
    printf("  Suma secuencial: %lld (Tiempo: %.6f s)\n", suma_seq, tiempo_seq);
    printf("  Suma paralela:   %lld (Tiempo: %.6f s)\n", suma_par, tiempo_par);
    printf("  Verificación: %s\n", (suma_seq == suma_par) ? "CORRECTA" : "ERROR");
    
    if (tiempo_seq > 0) {
        double speedup = tiempo_seq / tiempo_par;
        printf("  Speedup: %.2fx\n", speedup);
        if (speedup < 1.0) {
            printf("  ⚠️  PROBLEMA: La versión paralela es MÁS LENTA\n");
        }
    }
    
    free(array);
}

int main() {
    printf("ANÁLISIS: ¿Por qué falla la suma paralela con arrays grandes?\n");
    printf("=========================================================\n");
    
    // Mostrar información del sistema
    printf("Información del sistema:\n");
    printf("  Número de hilos disponibles: %d\n", omp_get_max_threads());
    printf("  Tamaño de int: %zu bytes\n", sizeof(int));
    
    // Probar diferentes tamaños
    int tamaños[] = {1000, 10000, 50000, 100000, 500000, 1000000, 5000000};
    int num_pruebas = sizeof(tamaños) / sizeof(tamaños[0]);
    
    for (int i = 0; i < num_pruebas; i++) {
        probar_tamaño(tamaños[i]);
        
        // Pausa para observar el comportamiento
        if (tamaños[i] >= 100000) {
            printf("Presiona Enter para continuar...\n");
            getchar();
        }
    }
    
    printf("\n=== ANÁLISIS DE PROBLEMAS POTENCIALES ===\n");
    printf("1. MEMORIA INSUFICIENTE:\n");
    printf("   - Arrays grandes requieren mucha RAM\n");
    printf("   - malloc() puede fallar si no hay memoria suficiente\n");
    printf("   - 100,000 enteros = ~400 KB\n");
    printf("   - 1,000,000 enteros = ~4 MB\n");
    printf("   - 100,000,000 enteros = ~400 MB\n\n");
    
    printf("2. OVERHEAD DE PARALELIZACIÓN:\n");
    printf("   - Crear y sincronizar hilos tiene costo\n");
    printf("   - Para problemas pequeños, el overhead > beneficio\n");
    printf("   - La reducción requiere sincronización adicional\n\n");
    
    printf("3. CACHE Y LOCALIDAD DE DATOS:\n");
    printf("   - Arrays grandes no caben en cache L1/L2/L3\n");
    printf("   - Múltiples hilos compiten por el ancho de banda de memoria\n");
    printf("   - False sharing en variables compartidas\n\n");
    
    printf("4. BALANCEADO DE CARGA:\n");
    printf("   - Distribución desigual del trabajo entre hilos\n");
    printf("   - Algunos hilos terminan antes que otros\n\n");
    
    printf("5. LIMITACIONES DEL HARDWARE:\n");
    printf("   - Ancho de banda de memoria limitado\n");
    printf("   - Número limitado de núcleos físicos\n");
    printf("   - Contención en el bus de memoria\n");
    
    return 0;
}

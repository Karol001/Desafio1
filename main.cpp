#include <iostream>

#define NUM_MUESTRAS 100  // Número de muestras de la señal a capturar

// Variables globales para almacenar las características de la señal
float frecuencia = 0.0;
float amplitud = 0.0;
int forma_onda = 0;  // 0: desconocida, 1: sinusoidal, 2: cuadrada, 3: triangular

// Arreglo para almacenar las muestras capturadas
float muestras[NUM_MUESTRAS];

// Simulamos el valor de la señal capturada
void capturarSenal() {
    float A = 5.0;  // Amplitud máxima (en voltios)
    int mitad = NUM_MUESTRAS / 2;  // Punto medio de la señal

    // Subida de la onda triangular
    for (int i = 0; i < mitad; i++) {
        muestras[i] = (A / mitad) * i;  // Valores suben linealmente desde 0 hasta 5 V
    }

    // Bajada de la onda triangular
    for (int i = mitad; i < NUM_MUESTRAS; i++) {
        muestras[i] = A - (A / mitad) * (i - mitad);  // Valores bajan linealmente desde 5 V hasta 0 V
    }
}

// Función para medir la frecuencia de la señal en Hertz
float medirFrecuencia() {
    int transiciones = 0;
    float valor_menor = 1.0;  // Valor mínimo para considerar la transición
    float valor_mayor = 4.0;  // Valor máximo para considerar la transición

    for (int i = 1; i < NUM_MUESTRAS; i++) {
        // Detectamos un cambio brusco de estado
        if ((muestras[i] >= valor_mayor && muestras[i - 1] <= valor_menor) ||
            (muestras[i] <= valor_menor && muestras[i - 1] >= valor_mayor)) {
            transiciones++;
        }
    }

    // En este caso, asumimos que 2 transiciones (subida y bajada) corresponden a 1 ciclo completo
    int ciclos_completos = transiciones / 2;

    // La frecuencia en Hz se calcula en base a la duración del arreglo
    float tiempo_total = NUM_MUESTRAS;  // Asumimos que la duración total es igual al número de muestras
    return ciclos_completos / tiempo_total;
}

// Función para medir la amplitud de la señal en Voltios
float medirAmplitud() {
    float max_valor = 0.0;
    float min_valor = 5.0;

    for (int i = 0; i < NUM_MUESTRAS; i++) {
        if (muestras[i] > max_valor) {
            max_valor = muestras[i];
        }
        if (muestras[i] < min_valor) {
            min_valor = muestras[i];
        }
    }

    // Amplitud es la diferencia entre el valor máximo y mínimo
    return max_valor - min_valor;
}

// Función para identificar la forma de onda de la señal
int identificarFormaOnda() {
    bool es_cuadrada = true;
    bool es_triangular = true;
    bool es_sinusoidal = true;

    // Verificar si la onda es cuadrada
    bool estado_prev = muestras[0] > 2.5;  // Consideramos la onda cuadrada si supera la mitad de la amplitud
    for (int i = 1; i < NUM_MUESTRAS; i++) {
        bool estado_actual = muestras[i] > 2.5;
        if (estado_actual != estado_prev) {
            estado_prev = estado_actual;
        } else {
            es_cuadrada = false;
        }
    }

    // Verificar si la onda es triangular
    int mitad = NUM_MUESTRAS / 2;
    for (int i = 1; i < mitad; i++) {
        if (muestras[i] <= muestras[i - 1]) {
            es_triangular = false;
            break;
        }
    }
    for (int i = mitad; i < NUM_MUESTRAS; i++) {
        if (muestras[i] >= muestras[i - 1]) {
            es_triangular = false;
            break;
        }
    }

    // Verificar si la onda es sinusoidal
    float prev_slope = muestras[1] - muestras[0];
    for (int i = 2; i < NUM_MUESTRAS; i++) {
        float curr_slope = muestras[i] - muestras[i - 1];
        if (std::abs(curr_slope - prev_slope) > 0.5) {
            es_sinusoidal = false;
            break;
        }
        prev_slope = curr_slope;
    }

    if (es_cuadrada) {
        return 2;  // Cuadrada
    }

    if (es_triangular) {
        return 3;  // Triangular
    }

    if (es_sinusoidal) {
        return 1;  // Sinusoidal
    }

    return 0;  // Desconocida
}

// Función para procesar la señal capturada y mostrar los resultados
void procesarSenal() {
    // Medir frecuencia y amplitud
    frecuencia = medirFrecuencia();
    amplitud = medirAmplitud();

    // Identificar la forma de onda
    forma_onda = identificarFormaOnda();

    // Visualizar en pantalla (simulación)
    std::cout << "Resultados:" << std::endl;
    std::cout << "Frecuencia: " << frecuencia << " Hz" << std::endl;
    std::cout << "Amplitud: " << amplitud << " V" << std::endl;

    switch (forma_onda) {
    case 1:
        std::cout << "Forma de onda: Sinusoidal" << std::endl;
        break;
    case 2:
        std::cout << "Forma de onda: Cuadrada" << std::endl;
        break;
    case 3:
        std::cout << "Forma de onda: Triangular" << std::endl;
        break;
    default:
        std::cout << "Forma de onda: Desconocida" << std::endl;
        break;
    }
}

int main() {
    bool adquisicion_activa = true;
    bool boton_presionado = false;

    // Simulamos el inicio del proceso al presionar un botón
    while (adquisicion_activa) {
        std::cout << "Presione el boton para capturar y procesar la señal (1 para continuar, 0 para salir): ";
        std::cin >> boton_presionado;

        if (boton_presionado) {
            // Capturar las muestras de la señal
            capturarSenal();

            // Procesar la señal capturada
            procesarSenal();
        } else {
            adquisicion_activa = false;
            std::cout << "Adquisición detenida." << std::endl;
        }
    }

    return 0;
}

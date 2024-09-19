#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

int pinAnalogico = 0;                
int valorLeido = 0;                  
const int botonInicio = 2;           
const int botonInfo = 4;             
const int numMuestras = 100;         
int *datosSenal = new int[numMuestras];

void setup() {
    pinMode(botonInicio, INPUT);
    pinMode(botonInfo, INPUT);
    lcd.begin(16, 2);
    lcd.print("Iniciando...");
    Serial.begin(9600);
}

void loop() {
    valorLeido = analogRead(pinAnalogico);
    Serial.println(valorLeido);
    
    if (digitalRead(botonInicio) == HIGH) {
        lcd.clear();
        lcd.print("Adquiriendo...");
        capturarDatosSenal(datosSenal, numMuestras);
    }
  
    if (digitalRead(botonInfo) == HIGH) {
        lcd.clear();
        lcd.print("Procesando...");
        
        float frecuencia = calcularFrecuencia(datosSenal, numMuestras);
        float amplitud = calcularAmplitud(datosSenal, numMuestras);
        identificarOnda(datosSenal, numMuestras);

        lcd.setCursor(0, 0);
        lcd.print("Freq: ");
        lcd.print(frecuencia);
        lcd.print(" Hz");

        lcd.setCursor(0, 1);
        lcd.print("Amp: ");
        lcd.print(amplitud);
        lcd.print(" V");
        
        delay(2000);  
    }
}

void capturarDatosSenal(int *datos, int tamano) {
    for (int i = 0; i < tamano; i++) {
        datos[i] = analogRead(pinAnalogico);
        delay(5);
    }
}

float calcularFrecuencia(int *datos, int tamano) {
    int crucesCero = 0;
    int ultimoEstado = (datos[0] >= 0) ? 1 : -1;

    for (int i = 1; i < tamano; i++) {
        int estadoActual = (datos[i] >= 0) ? 1 : -1;

        if (estadoActual != ultimoEstado) {
            crucesCero++;
            ultimoEstado = estadoActual;
        }
    }

    if (crucesCero == 0) {
        Serial.println("No se detectaron cruces por cero.");
        return 0.0;
    }

    int ciclosCompletos = crucesCero / 2;
    float periodo = (tamano * 5.0) / ciclosCompletos;
    float frecuencia = 1000.0 / periodo;

    return frecuencia;
}

float calcularAmplitud(int *datos, int tamano) {
    int valorMax = 0;
    int valorMin = 1023;
    
    for (int i = 0; i < tamano; i++) {
        if (datos[i] > valorMax) valorMax = datos[i];
        if (datos[i] < valorMin) valorMin = datos[i];
    }
    
    return (valorMax - valorMin) * (5.0 / 1023.0);
}

void identificarOnda(int *datos, int tamano) {
    int valorMax = 0, valorMin = 1023;
    int cuentaTransiciones = 0;
    int deltaMax = 0;
    int deltasConsistentes = 0;
    int cambiosDireccion = 0;
    bool esSenoidal = true;
    bool esCuadrada = true;
    bool esTriangular = true;

    for (int i = 0; i < tamano; i++) {
        if (datos[i] > valorMax) valorMax = datos[i];
        if (datos[i] < valorMin) valorMin = datos[i];
    }

    int umbral = (valorMax + valorMin) / 2;
    int histeresis = (valorMax - valorMin) * 0.1;

    int deltaAnterior = datos[1] - datos[0];

    for (int i = 1; i < tamano; i++) {
        int delta = datos[i] - datos[i - 1];
        bool estadoActual = (datos[i] > umbral + histeresis) || 
                            (datos[i] > umbral - histeresis && (datos[i - 1] > umbral + histeresis));
        
        if (estadoActual != (datos[i - 1] > umbral)) {
            cuentaTransiciones++;
        }

        if (delta * deltaAnterior > 0) {
            deltasConsistentes++;
        } else {
            cambiosDireccion++;
            deltaAnterior = delta;
        }

        if (delta < 0) delta = -delta;
        if (delta > deltaMax) deltaMax = delta;

        if (delta > (valorMax - valorMin) * 0.2) {
            esSenoidal = false;
        }
    }

    if (cambiosDireccion > 2 || deltasConsistentes < tamano * 0.7) {
        esTriangular = false;
    }

    if (cuentaTransiciones < 2 || deltaMax < (valorMax - valorMin) * 0.3) {
        esCuadrada = false;
    }

    lcd.clear();
    lcd.setCursor(0, 0);

    if (esCuadrada) {
        lcd.print("Onda cuadrada");
        Serial.println("Onda cuadrada detectada.");
    } else if (esTriangular) {
        lcd.print("Onda triangular");
        Serial.println("Onda triangular detectada.");
    } else if (esSenoidal) {
        lcd.print("Onda senoidal");
        Serial.println("Onda senoidal detectada.");
    } else {
        lcd.print("Onda desconocida");
        Serial.println("Onda desconocida.");
    }
    delay(2000);
}

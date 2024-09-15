#include <Adafruit_LiquidCrystal.h>

int seconds = 0;
int analogPin = A0;
int val = 0;
const int signalPin = 2; // Pin donde se conecta la señal
volatile int peakCount = 0;
volatile int* peakCountPtr = &peakCount; // Puntero al contador de picos
unsigned long startTime;
const int botonPin = 3; // Pin del botón para iniciar la captura
bool isCapturing = false; // Variable para controlar si se está capturando
int minVal = 1023; // Valor mínimo para calcular la amplitud
int maxVal = 0;    // Valor máximo para calcular la amplitud

Adafruit_LiquidCrystal lcd_1(0);

void setup() {
    Serial.begin(9600);

    pinMode(botonPin, INPUT);

    lcd_1.begin(16, 2);
    lcd_1.print("Presiona el boton");

    pinMode(signalPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(signalPin), detectPeak, RISING);
    startTime = millis();
}

void loop() {
    // Leer el estado del botón
    if (digitalRead(botonPin) == HIGH) {
        isCapturing = !isCapturing; // Alterna la captura de datos
        delay(200); // Pequeño retardo para evitar múltiples lecturas del botón
    }

    if (isCapturing) {
        val = analogRead(analogPin);
        if (val < minVal) minVal = val;
        if (val > maxVal) maxVal = val;

        Serial.println(val);
        lcd_1.setCursor(0, 0);
        lcd_1.print("Capturando... ");

        lcd_1.setCursor(0, 1);
        lcd_1.print("Segundos: ");
        lcd_1.print(seconds);

        lcd_1.setBacklight(1);
        delay(500);
        lcd_1.setBacklight(0);
        delay(500);
        seconds += 1;

        if (millis() - startTime >= 1000) { // Cada segundo
            Serial.print("Frecuencia: ");
            Serial.print(*peakCountPtr); // Usar el puntero para acceder al valor
            Serial.println(" Hz");

            int amplitude = maxVal - minVal; // Calcular la amplitud
            Serial.print("Amplitud: ");
            Serial.print(amplitude);
            Serial.println(" V");

            // Restablecer valores
            *peakCountPtr = 0;
            minVal = 1023;
            maxVal = 0;
            startTime = millis();
        }
    } else {
        lcd_1.setCursor(0, 0);
        lcd_1.print("Presiona el boton");
        delay(500);
    }
}

void detectPeak() {
    (*peakCountPtr)++; // Incrementa el contador usando el puntero
}

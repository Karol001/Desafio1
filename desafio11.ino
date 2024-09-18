#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

int analogPin = 0;
int val = 0;
const int buttonStart = 2;
const int buttonInfo = 4;
const int numSamples = 100;
int *signalData = new int[numSamples];

void setup() {
    pinMode(buttonStart, INPUT);
    pinMode(buttonInfo, INPUT);
    lcd.begin(16, 2);
    lcd.print("Iniciando...");
    Serial.begin(9600);
}

void loop() {
    val = analogRead(analogPin);
    Serial.println(val);
    
    if (digitalRead(buttonStart) == HIGH) {
        lcd.clear();
        lcd.print("Adquiriendo...");
        captureSignalData(signalData, numSamples);
    }
  
    if (digitalRead(buttonInfo) == HIGH) {
        lcd.clear();
        lcd.print("Procesando...");
        
        float frecuencia = calcularFrecuencia(signalData, numSamples);
        float amplitud = calcularAmplitud(signalData, numSamples);
        identificarOnda(signalData, numSamples);

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

void captureSignalData(int *data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] = analogRead(analogPin);
        delay(5);
    }
}

float calcularFrecuencia(int *data, int size) {
    int zeroCrossings = 0;
    int lastState = (data[0] >= 0) ? 1 : -1;

    for (int i = 1; i < size; i++) {
        int currentState = (data[i] >= 0) ? 1 : -1;
        if (currentState != lastState) {
            zeroCrossings++;
            lastState = currentState;
            Serial.print("Cruce por cero en la muestra ");
            Serial.println(i);
        }
    }

    if (zeroCrossings == 0) {
        Serial.println("No se detectaron cruces por cero.");
        return 0.0;
    }

    int fullCycles = zeroCrossings / 2;
    float period = (size * 5.0) / fullCycles;
    float frequency = 1000.0 / period;

    Serial.print("Frecuencia calculada: ");
    Serial.println(frequency);

    return frequency;
}

float calcularAmplitud(int *data, int size) {
    int maxVal = 0;
    int minVal = 1023;
    
    for (int i = 0; i < size; i++) {
        if (data[i] > maxVal) maxVal = data[i];
        if (data[i] < minVal) minVal = data[i];
    }
    
    return (maxVal - minVal) * (5.0 / 1023.0);
}

void identificarOnda(int *data, int size) {
    int highCount = 0, lowCount = 0;
    bool isSquare = true;
    bool isSine = true;

    for (int i = 1; i < size; i++) {
        if (data[i] > 800) {
            highCount++;
            if (data[i - 1] <= 800 && highCount > 1) {
                lowCount = 0;  
            }
        } else if (data[i] < 200) {
            lowCount++;
            if (data[i - 1] >= 200 && lowCount > 1) {
                highCount = 0;  
            }
        } else {
            isSquare = false;
            break;
        }
    }

    for (int i = 1; i < size; i++) {
        if (abs(data[i] - data[i-1]) > 200) {
            isSine = false;
            break;
        }
    }

    lcd.clear();
    lcd.setCursor(0, 0);

    if (isSquare) {
        lcd.print("Onda cuadrada");
        Serial.println("Onda cuadrada detectada.");
    } else if (isSine) {
        lcd.print("Onda senoidal");
        Serial.println("Onda senoidal detectada.");
    } else {
        lcd.print("Desconocida");
        Serial.println("Desconocida");
    }

    delay(2000);  
}

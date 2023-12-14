#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

// Configura tus credenciales de WiFi
const char *ssid = "iPhone de: Gisela";
const char *password = "holita02";

// Configura los pines de los sensores
#define DHT_PIN 5      // Pin de datos del sensor DHT11
#define OLED_RESET 4   // Pin de datos de la pantalla
const int ldrPin1 = 32; // Pin del sensor LDR1
const int ldrPin2 = 34; // Pin del sensor LDR2
const float limitTemperatura = 24;
const float limitHumitat = 70;
const float limitLluminositat = 700;

DHT dht(DHT_PIN, DHT11);
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conéctate a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a la red WiFi");

  // Inicializa el sensor DHT11
  dht.begin();
  delay(5000);

  // Inicializa el display OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Lee la temperatura y la humedad
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Lee el valor del sensor LDR
  int ldrValue1 = analogRead(ldrPin1);
  int ldrValue2 = analogRead(ldrPin2);

  // Limpia el display
  display.clearDisplay();

  if (temperature > limitTemperatura) {
    display.setCursor(0, 40);
    display.print("¡Alerta! Alta temperatura");
  }

  if (temperature > limitHumitat) {
    display.setCursor(0, 50);
    display.print("¡Alerta! Alta temperatura");
  }

  if (ldrValue1 < limitLluminositat || ldrValue2 < limitLluminositat) {
    display.setCursor(0, 60);
    display.print("¡Alerta! Baja luminosidad");
  }

  else {
  display.setCursor(0, 40);
  display.print("Todo en orden");
  }

  display.clearDisplay();
  delay(5000);

  // Imprime los valores en el display
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temperature);
  display.print(" C");

  //Valor Humedad
  display.setCursor(0, 10);
  display.print("Humedad: ");
  display.print(humidity);
  display.print(" %");

  //Valor Luz
  display.setCursor(0, 20);
  display.print("LDR1: ");
  display.print(ldrValue1);

  display.setCursor(0, 30);
  display.print("LDR2: ");
  display.print(ldrValue2);

  // Muestra los datos en el display
  display.display();

  // Imprime los valores
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Valor del sensor LDR1: ");
  Serial.println(ldrValue1);

  Serial.print("Valor del sensor LDR2: ");
  Serial.println(ldrValue2);

  // Espera 5 segundos antes de realizar la próxima lectura
  delay(5000);
}
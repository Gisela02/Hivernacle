#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

// Configura tus credenciales de WiFi
const char *ssid = "iPhone de Paula";
const char *password = "f472547F";

// Direccion del servidor Sentilo
const char* host = "147.83.83.21";
const char* token = "f3ac6fb7b1657a691e28dec382a41e0182f2289bd13f120e399f008bb03ed2f9";
const char* provider = "grup_3-101@grup3_provider/";
const char* sensor1 = "humitat/";
const char* sensor2 = "temperatura/";
const char* sensor3 = "ldr1/";
const char* sensor4 = "ldr2/";

// Configura los pines de los sensores
#define DHT_PIN 5      // Pin de datos del sensor DHT11
#define OLED_RESET 22   // Pin de datos de la pantalla
const int ldrPin1 = 32; // Pin del sensor LDR1
const int ldrPin2 = 34; // Pin del sensor LDR2
const int buzzerPin = 27; // Pin del buzzer AFEGIIIIIT
const float limitSuperiorTemperatura = 30;
const float limitSuperiorHumitat = 60;
const float limitSuperiorLluminositat = 700;
const float limitInferiorTemperatura = 10;
const float limitInferiorHumitat = 45;
const float limitInferiorLluminositat = 600;

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

  pinMode(buzzerPin, OUTPUT); // Afegit buzzer

  display.display();
  delay(2000);
  display.clearDisplay();
}

// Función para calibrar la temperatura
float calibrateTemperature(float currentTemperature) {
  // Aplica la corrección
  float calibratedTemperature = currentTemperature - 3.0;
  return calibratedTemperature;
}

void putdata(const char* sensor, float value) {
  WiFiClient client;
  const int httpPort = 8081;

  if (client.connect(host, httpPort)) {
    client.print("PUT /data/");
    client.print(provider);
    client.print(sensor);
    client.print(value);
    client.print(" HTTP/1.1\r\nIDENTITY_KEY: ");
    client.print(token);
    client.print("\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Superado el tiempo de espera !");
        client.stop();
        return;
      }
    }

    client.stop();
  } else {
    Serial.println("Ha fallado la conexión");
  }
}

// Metode Get per obtenir les 8 ultimes dades del sensor així treu les dades d'un dia sencer 24h
void analisidades(const char* sensor) {
  WiFiClient client;
  const int httpPort = 8081;

  if (client.connect(host, httpPort)) {
    client.print("GET /data/");
    client.print(provider);
    client.print(sensor);
    client.print("/?limit=8 HTTP/1.1\r\n");
    client.print("IDENTITY_KEY: ");
    client.print(token);
    client.print("\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Superado el tiempo de espera !");
        client.stop();
        return;
      }
    }

    const int capacity = 2048;
    char buffer[capacity];
    size_t bytesRead = client.readBytes(buffer, capacity);

    Serial.write(buffer, bytesRead);

    client.stop();
  } else {
    Serial.println("Ha fallado la conexión");
  }
}

// Función para detener el tono en el Buzzer Pasivo
void noTone(int pin) {
  if (pin < 0 || pin >= NUM_DIGITAL_PINS) {
    return;  // Salir si el pin no es válido
  }

  ledcDetachPin(pin);
}

void tone(int pin, int frequency, int duration) {
  if (pin < 0 || pin >= NUM_DIGITAL_PINS || frequency == 0) {
    return;  // Salir si el pin no es válido o la frecuencia es cero
  }

  ledcAttachPin(pin, 0);
  ledcWriteTone(0, frequency);
  delay(duration);
  noTone(pin);
}

void alertaSonido() {
  // Genera un tono de alerta
  tone(buzzerPin, 900, 3000);  // Cambia la frecuencia según tus preferencias
  delay(6000);  // Puedes ajustar la duración del tono
  noTone(buzzerPin);  // Detiene el sonido
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature1 = dht.readTemperature();
  float temperature = calibrateTemperature(temperature1);
  // Lee el valor del sensor LDR
  int ldrValue1 = analogRead(ldrPin1);
  int ldrValue2 = analogRead(ldrPin2);

  putdata(sensor1, humidity);
  putdata(sensor2, temperature);
  putdata(sensor3, ldrValue1);
  putdata(sensor4, ldrValue2);

  analisidades(sensor1);
  analisidades(sensor2);
  analisidades(sensor3);
  analisidades(sensor4);

  // Limpia el display
  display.clearDisplay();
  delay(1000);

  // Imprime los valores en el display
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temperatura: ");
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

  display.setCursor(70, 20);
  display.print("LDR2: ");
  display.print(ldrValue2);

  if (temperature > limitSuperiorTemperatura || temperature < limitInferiorTemperatura) {
    display.setCursor(0, 30);
    alertaSonido();
    if(temperature > limitSuperiorTemperatura){
      display.print("¡Alta temperatura!");
    }
    else{
      display.print("¡Baja temperatura!");
    }
  } 
  if (humidity > limitSuperiorHumitat || humidity < limitInferiorHumitat) {
    display.setCursor(0, 40);
    alertaSonido();
    if(humidity > limitSuperiorHumitat){
      display.print("¡Alta Humedad!");
    }
    else{
      display.print("¡Baja Humedad!");
    }
  } 
  if (ldrValue1 > limitSuperiorLluminositat || ldrValue2 > limitSuperiorLluminositat || ldrValue1 < limitInferiorLluminositat || ldrValue2 < limitInferiorLluminositat ) {
    display.setCursor(0, 50);
    alertaSonido();
    if(ldrValue1 > limitSuperiorLluminositat || ldrValue2 > limitSuperiorLluminositat){
      display.print("¡Alta luminosidad!");
    }
    else{
      display.print("¡Baja luminosidad!");
    }
  }
  else {
    display.setCursor(0, 40);
    display.print("Todo en orden");
  }

  // Muestra los datos en el display
  display.display();

  // Imprime los valores por pantalla ordenador
  Serial.println(" ");

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
  delay(10800000); //CADA 3 HORAS
}


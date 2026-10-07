#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_system.h>

//CONIF
const char* WIFI_SSID = "ELSA_PC_2.4G";
const char* WIFI_PASSWORD = "BADADITA";

// IP de la computadora donde funciona Mosquitto.
const char* MQTT_HOST = "192.168.0.206";
const uint16_t MQTT_PORT = 1883;
const char* TOPIC = "laboratorio/mq135";

//SENSOR
const uint8_t PIN_MQ135 = 34;

const float R1 = 10000.0f;
const float R2 = 10000.0f;

const unsigned long INTERVALO_MS = 5000;

//MQTT
WiFiClient conexionWiFi;
PubSubClient mqtt(conexionWiFi);

char sesion[16];
uint32_t idMuestra = 0;

unsigned long ultimoEnvio = 0;
unsigned long ultimoIntentoMQTT = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_MQ135, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_MQ135, ADC_11db);

  // Identifica cada arranque para no confundir muestras.
  snprintf(
    sesion,
    sizeof(sesion),
    "%08lx",
    (unsigned long)esp_random()
  );

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Conectando al Wi-Fi...");

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setBufferSize(512);
  mqtt.setSocketTimeout(3);

  ultimoIntentoMQTT = millis() - 3000;
}

void loop() {
  // Esperar conexión Wi-Fi.
  if (WiFi.status() != WL_CONNECTED) {
    delay(100);
    return;
  }

  // Conectar o reconectar al broker.
  if (!mqtt.connected()) {
    if (millis() - ultimoIntentoMQTT >= 3000) {
      ultimoIntentoMQTT = millis();

      String clienteID = String("mq135-") + sesion;

      Serial.print("IP del ESP32: ");
      Serial.println(WiFi.localIP());

      Serial.println("Conectando a Mosquitto...");

      if (mqtt.connect(clienteID.c_str())) {
        Serial.println("MQTT conectado");
      } else {
        Serial.print("Error MQTT: ");
        Serial.println(mqtt.state());
      }
    }
    return;
  }

  mqtt.loop();

  if (millis() - ultimoEnvio < INTERVALO_MS) {
    return;
  }

  ultimoEnvio = millis();

  //PROMDIO
  const int NUM_LECTURAS = 20;

  uint32_t sumaADC = 0;
  uint32_t sumaMilivoltios = 0;

  for (int i = 0; i < NUM_LECTURAS; i++) {
    sumaADC += analogRead(PIN_MQ135);
    sumaMilivoltios += analogReadMilliVolts(PIN_MQ135);
    delay(5);
  }

  // ADC entero compatible con el receptor Python.
  int adcPromedio =
      (sumaADC + NUM_LECTURAS / 2) / NUM_LECTURAS;

  float voltajeGPIO =
      sumaMilivoltios / float(NUM_LECTURAS) / 1000.0f;

  float voltajeAO =
      voltajeGPIO * (R1 + R2) / R2;

  idMuestra++;

  //JSON
  char mensaje[384];

  snprintf(
    mensaje,
    sizeof(mensaje),
    "{\"dispositivo\":\"esp32_01\","
    "\"sesion\":\"%s\","
    "\"id_muestra\":%lu,"
    "\"sensor\":\"MQ135\","
    "\"origen\":\"sensor\","
    "\"adc\":%d,"
    "\"voltaje_gpio\":%.3f,"
    "\"voltaje_ao_estimado\":%.3f,"
    "\"tiempo_ms\":%lu}",
    sesion,
    (unsigned long)idMuestra,
    adcPromedio,
    voltajeGPIO,
    voltajeAO,
    millis()
  );

  //ENVIAR MQTT
  bool enviado = mqtt.publish(TOPIC, mensaje, false);

  Serial.println(mensaje);

  if (enviado) {
    Serial.println("Publicado por MQTT");
  } else {
    Serial.println("No se pudo publicar la muestra");
  }
}
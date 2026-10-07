#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_system.h>

const char* WIFI_SSID = "S24 de Pedro";
const char* WIFI_PASSWORD = "diqueerespobre";
const char* URL_HTTP = "http://10.177.136.156:5000/muestras";

const int PIN_MQ135 = 34;
const int LECTURAS = 20;
const unsigned long INTERVALO = 5000; // Espera tras confirmar antes de otra lectura
const unsigned long INTERVALO_ENVIO = 10000; // Entre inicios de envios, incluidos reintentos
const int TIMEOUT_HTTP_MS = 5000; // Espera de respuesta, independiente del intervalo de envio
unsigned long ultimaConfirmacion = 0;
unsigned long ultimoInicioEnvio = 0;
unsigned long idMuestra = 0;
bool pendiente = false;
bool wifiAvisado = false;
char sesion[9];
char jsonPendiente[384];

void setup() {
  Serial.begin(115200);
  pinMode(PIN_MQ135, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_MQ135, ADC_11db);
  snprintf(sesion, sizeof(sesion), "%08lx", (unsigned long)esp_random());
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (!WiFi.setSleep(false)) Serial.println("No se pudo desactivar el ahorro WiFi.");
  Serial.println("HTTP: envios separados por 10 s, incluidos reintentos. Nueva lectura 5 s tras confirmar.");
}

void tomarMuestra() {
  uint32_t sumaADC = 0;
  uint32_t sumaMV = 0;
  for (int i = 0; i < LECTURAS; i++) {
    sumaADC += analogRead(PIN_MQ135);
    sumaMV += analogReadMilliVolts(PIN_MQ135);
    delay(5);
  }
  int adc = (sumaADC + LECTURAS / 2) / LECTURAS;
  float voltajeGPIO = sumaMV / (LECTURAS * 1000.0f);
  float voltajeAO = voltajeGPIO * 2.0f; // R1 = R2 = 10 kohm
  idMuestra++;
  snprintf(jsonPendiente, sizeof(jsonPendiente),
    "{\"dispositivo\":\"esp32_01\",\"sesion\":\"%s\","
    "\"id_muestra\":%lu,\"sensor\":\"MQ135\",\"origen\":\"sensor\","
    "\"adc\":%d,\"voltaje_gpio\":%.3f,\"voltaje_ao_estimado\":%.3f,"
    "\"tiempo_ms\":%lu}",
    sesion, idMuestra, adc, voltajeGPIO, voltajeAO, millis());
  pendiente = true;
  Serial.println(jsonPendiente);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (!wifiAvisado) {
      Serial.println("Sin WiFi: esperando reconexion; pendiente conservada.");
      wifiAvisado = true;
    }
    delay(100);
    return;
  }
  if (wifiAvisado) {
    Serial.print("WiFi conectado. IP ESP32: ");
    Serial.println(WiFi.localIP());
    wifiAvisado = false;
  }
  if (!pendiente) {
    if (millis() - ultimaConfirmacion < INTERVALO) {
      delay(1);
      return;
    }
    tomarMuestra();
  }
  if (millis() - ultimoInicioEnvio < INTERVALO_ENVIO) {
    delay(1);
    return;
  }
  ultimoInicioEnvio = millis();
  WiFiClient cliente;
  HTTPClient http;
  if (!http.begin(cliente, URL_HTTP)) {
    Serial.println("URL invalida: revisar URL_HTTP.");
    return;
  }
  http.setReuse(false);
  http.setConnectTimeout(6000);
  http.setTimeout(TIMEOUT_HTTP_MS);
  http.addHeader("Content-Type", "application/json");
  Serial.printf("Enviando muestra %lu, sesion %s | RSSI=%ld dBm\n", idMuestra, sesion, (long)WiFi.RSSI());
  unsigned long inicioPeticion = millis();
  int codigo = http.POST(String(jsonPendiente));
  if (codigo > 0) {
    Serial.printf("HTTP: %d\n", codigo);
    String respuesta = http.getString();
    Serial.println(respuesta);
  } else {
    Serial.print("Error HTTP: ");
    Serial.println(HTTPClient::errorToString(codigo));
  }
  Serial.printf("Duracion peticion y respuesta: %lu ms\n", millis() - inicioPeticion);
  http.end();
  // Este receptor devuelve 201 tras guardar o 200 para un duplicado guardado.
  if (codigo == 201 || codigo == 200) {
    Serial.printf("CONFIRMADA muestra %lu.\n", idMuestra);
    pendiente = false;
    ultimaConfirmacion = millis();
  } else {
    Serial.printf("Pendiente %lu: lecturas pausadas; se reenviara el MISMO JSON en el siguiente turno de envio.\n", idMuestra);
    if (codigo == 400 || codigo == 404 || codigo == 415) {
      Serial.println("Revisar respuesta del servidor, ruta y formato JSON.");
    }
  }
}

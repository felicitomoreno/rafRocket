#include <Arduino.h>
#line 1 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
//----------------------------------------------------General---------------------------------------------------------------------
// #include <ArduinoJSON.h>
int ciclotrabajo = 50, TimeWithoutConnectionPermited = 15000; // en milisegundos.
bool resetSensors;                                            // Indica que deben volver a cero los valores de Rotación y aceleración.

//----------------------------------------------------Connection------------------------------------------------------------------
/* connect to red wifi */
void espInit();
/* Realiza la petición a servidor */
String fetch();

#include <HTTPClient.h>
#include <WiFi.h>
const char *ssid = "ELIZABETH";
const char *password = "sergio30900410";
const char *ssid2 = "Canchon";
const char *password2 = "12345678";
#define url "https://rocketust.000webhostapp.com/"
WiFiClient client;
HTTPClient http;
String json = "";
String dataToSend = "";
int TimeWithoutConnection = 50;

//----------------------------------------------------Sensor MPU6050---------------------------------------------------------------
void MPU6050init();
void MPU6050Run();

#include <MPU6050.h>
#include "Wire.h"
MPU6050 sensor;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int seconds;

#line 36 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void setup();
#line 46 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void loop();
#line 89 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
String fetch(String module, String accion, String parameters);
#line 125 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void writePins(String json);
#line 36 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void setup()
{
  Serial.begin(115200);
  espInit();
  MPU6050init();

  // connect to API
  http.begin(url);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    MPU6050Run();
    String sensorsStatus = fetch("rocket", "getSystemVar", "{}"); // Recibir datos
    if (sensorsStatus != "")
    {
      writePins(sensorsStatus);
    }

    // fetch("rocket", "getValues", "{}"); // Enviar datos
  }
  else
  {
    espInit();
    // Guardar los datos que recibe del sensor localmente para luego transmitirlos.
  }

  delay(ciclotrabajo);
}

//-------------------------------------------- Definición de funciones -------------------------------------------------

//----------------------------------------------------Connection------------------------------------------------------------------
void espInit()
{
  delay(TimeWithoutConnection);
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) // Check for the connection
  {
    TimeWithoutConnection += 500;
    delay(500);
    Serial.print(".");
    if (TimeWithoutConnection > TimeWithoutConnectionPermited) // Si tarda más de 15 segundos en conectarse a la primera red WiFi que se cambie a la segunda definida.
    {
      WiFi.begin(ssid2, password2);
    }
  }
  Serial.println("Conectado con éxito a una red wifi.");
}

String fetch(String module, String accion, String parameters)
{
  if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
  {
    json = "{\"module\":\"" + module + "\", \"accion\": \"" + accion + "\", \"parameters\": " + parameters + "}";
    dataToSend = "json=" + String(json);
    Serial.println("\nDatos a enviar: " + String(dataToSend));
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int responseCode = http.POST(dataToSend); // Enviamos el post pasándole los datos que queremos enviar. retorna un código http

    if (responseCode > 0) // no hubo errores al hacer la petición
    {
      Serial.println("Código HTTP ► " + String(responseCode));
      if (responseCode == 200)
      { // La API envió una respuesta
        Serial.println("El servidor respondió ▼ ");
        String response = http.getString();
        Serial.println(response + "▼");
        http.end();
        Serial.println("------------------");
        return response;
      }
    }
    else
    {
      Serial.print("Error enviando POST, código: ");
      String response = http.getString();
      Serial.println(responseCode + ' - ' + response);
    }
  }
  else
  {
    Serial.println("\nError en la conexión WIFI");
  }
  return "";
}
void writePins(String json)
{
  // JsonDocument doc;
  // deserializeJson(doc, json);

  // if (doc.containsKey("ciclotrabajo"))
  // {
  //   ciclotrabajo = atoi(doc["ciclotrabajo"]);
  //   Serial.println("\n\nCiclo de trabajo cambiado a " + String(ciclotrabajo) + " milisegundos\n");
  // }

  // if (doc.containsKey("TimeWithoutConnectionPermited"))
  // {
  //   TimeWithoutConnectionPermited = atoi(doc["TimeWithoutConnectionPermited"]);
  //   Serial.println("\n\nTiempo sin conexión cambiado a < " + String(TimeWithoutConnectionPermited) + "°c\n");
  // }

  // if (doc.containsKey("resetSensors"))
  // {
  //   resetSensors = atoi(doc["resetSensors"]);
  //   Serial.println("\n\nActivar lanzamiento cambiado a > " + String(resetSensors) + "%\n");
  // }

  // Buscar las claves y extraer los valores
  int index = json.indexOf("\"ciclotrabajo\":") + strlen("ciclotrabajo") + 4;
  //Serial.println(index + json);
  if (index != -1)
  {
    ciclotrabajo = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
    Serial.println("\n\nCiclo de trabajo cambiado a " + String(ciclotrabajo) + " milisegundos\n");
  }

  index = json.indexOf("\"timeWithoutConnectionPermited\":");
  //Serial.println(index + json);
  if (index != -1)
  {
    TimeWithoutConnectionPermited = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
    Serial.println("\n\nTiempo sin conexión cambiado a < " + String(TimeWithoutConnectionPermited) + " milisegundos\n");
  }

  index = json.indexOf("\"resetSensors\":");
  //Serial.println(index + json);
  if (index != -1)
  {
    resetSensors = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
    Serial.println("\n\nActivar lanzamiento cambiado a > " + String(resetSensors) + ".\n");
  }
}

//----------------------------------------------------Sensor MPU6050---------------------------------------------------------------
void MPU6050init()
{
  Wire.begin();
  sensor.initialize();

  if (sensor.testConnection())
    Serial.println("Sensor iniciado correctamente");
  else
    Serial.println("Errror al inciar el sensor");
}

void MPU6050Run()
{
  sensor.getAcceleration(&ax, &ay, &az);
  float acc_and_x = atan(ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.1416);
  float acc_and_y = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / 3.1416);

  seconds += 1;
  Serial.print(seconds);
  Serial.print("      ");
  Serial.print("Valor ax: ");
  Serial.print(ax);
  Serial.print(", Valor ay: ");
  Serial.print(ay);
  Serial.print(", Valor az: ");
  Serial.print(az);
  Serial.print(", Inclinación en x: ");
  Serial.print(acc_and_x);
  Serial.print(", Inclinación en y: ");
  Serial.print(acc_and_y);

  sensor.getRotation(&gx, &gy, &gz);
  Serial.print("            Rotación x: ");
  Serial.print(gx);
  Serial.print(", Rotación y: ");
  Serial.print(gy);
  Serial.print(", Rotación z: ");
  Serial.println(gz);
}

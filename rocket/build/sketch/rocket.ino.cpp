#include <Arduino.h>
#line 1 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
//-----------------------------------------Connection------------------------------------------------------------------
/* Conecta a internet la placa */
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


//--------------------------------------------------Sensor MPU6050---------------------------------------------------------------
void MPU6050init();
void MPU6050Run();

#include <MPU6050.h>
#include "Wire.h"
MPU6050 sensor;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int seconds, milis = 50;

#line 31 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void setup();
#line 38 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void loop();
#line 67 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
String fetch(String module, String accion, String parameters);
#line 31 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void setup()
{
  Serial.begin(115200);
  espInit();
  MPU6050init();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    MPU6050Run();
    fetch("rocket", "getValues", "{}");
  }
}

//-------------------------------------------- Definición de funciones -------------------------------------------------

//-----------------------------------------Connection------------------------------------------------------------------
void espInit()
{
  // connect to red wifi
  delay(50);
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED)
  { // Check for the connection
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado con éxito a la red wifi " + String(ssid));

  // connect to API
  http.begin(url);
}

String fetch(String module, String accion, String parameters)
{
  delay(4000);
  if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
  {
    json = "{\"module\":\"" + module + "\", \"accion\": \"" + accion + "\", \"parameters\": " + parameters + "}";
    dataToSend = "json=" + String(json);
    Serial.println("\nDatos a enviar: " + String(dataToSend));
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int responseCode = http.POST(dataToSend); // Enviamos el post pasándole los datos que queremos enviar. retorna un código http

    if (responseCode > 0)
    { // no hubo errores al hacer la petición
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
      Serial.println(responseCode);
    }
  }
  else
  {
    Serial.println("\nError en la conexión WIFI");
  }
  return "";
}

//--------------------------------------------------Sensor MPU6050---------------------------------------------------------------
void MPU6050init()
{
  Wire.begin();
  sensor.initialize();
  // espInit();

  Serial.println("Hola Mundo.");
  Serial.println();
  Serial.println();
  Serial.println("Seconds:");
  Serial.println();

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

  delay(milis);
}

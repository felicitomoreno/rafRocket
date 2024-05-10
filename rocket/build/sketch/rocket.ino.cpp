#include <Arduino.h>
#line 1 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
//----------------------------------------------------General---------------------------------------------------------------------
// #include <ArduinoJSON.h>
int ciclotrabajo = 50, TimeWithoutConnectionPermited = 180000, TimeForRecolectData = 20000; // en milisegundos.
bool resetSensors, printMessages = true, connecToBegin = false;                             // Indica que deben volver a cero los valores de Rotación y aceleración.

// const int sizeData = 100;
int tiempotranscurrido, millisInt;
String dataJson;

// struct data // Estructura para guardar los datos
// {
//   String acc_x;
//   String acc_y;
//   String acc_z;
//   String vel_x;
//   String vel_y;
//   String vel_z;
//   String pos_x;
//   String pos_y;
//   String pos_z;
//   String rot_x;
//   String rot_y;
//   String rot_z;
// };
// data datos[sizeData];

//----------------------------------------------------Connection------------------------------------------------------------------
/* connect to red wifi - Para la operativa porque usa un while*/
void espInit();
/* Realiza la petición a servidor */
String fetch();

#include <HTTPClient.h>
#include <WiFi.h>
const char *ssid = "ELIZABETH";
const char *password = "sergio30900410";
// const char *ssid2 = "CANCHON";
// const char *password2 = "12345678";
const char *ssid2 = "USTA_Estudiantes";
const char *password2 = "#soytomasino";
#define url "https://rocketust2.000webhostapp.com/"
WiFiClient client;
HTTPClient http;
String json = "";
String dataToSend = "";
int TimeWithoutConnection = 50;

//----------------------------------------------------Sensor MPU6050---------------------------------------------------------------
/* Inicia el sensor */
void MPU6050init();
/* Recolecta información */
void MPU6050Run();

#include <MPU6050.h>
#include "Wire.h"
MPU6050 sensor;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float vx, vy, vz;
float dx, dy, dz;

int seconds;

#line 64 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void setup();
#line 75 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void loop();
#line 145 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
String fetch(String module, String accion, String parameters);
#line 181 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void writePins(String json);
#line 232 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void sentData();
#line 64 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
void setup()
{
  Serial.begin(115200);
  if (connecToBegin)
    espInit();
  MPU6050init();

  // connect to API
  http.begin(url);
}

void loop()
{
  millisInt = millis();
  if (WiFi.status() == WL_CONNECTED)
  {
    MPU6050Run();
    String sensorsStatus = fetch("rocket", "getSystemVar", "{}"); // Recibir datos
    if (sensorsStatus != "")
      writePins(sensorsStatus);

    if ((TimeForRecolectData + tiempotranscurrido - millisInt) < 0) // La idea es que haya un tiempo prudente para recolectar datos antes de iniciar una conexión a internet.
    {
      tiempotranscurrido = millis();
      sentData();
    }
  }
  else
  {
    if ((TimeForRecolectData + tiempotranscurrido - millisInt) < 0) // La idea es que haya un tiempo prudente para recolectar datos antes de iniciar una conexión a internet.
    {
      tiempotranscurrido = millis();
      espInit();
      sentData();
    }
    MPU6050Run();
    Serial.println(TimeForRecolectData);
    Serial.println(tiempotranscurrido);
    Serial.println(millis());
    Serial.println(TimeForRecolectData + tiempotranscurrido - millisInt);
  }

  delay(ciclotrabajo);
}

//-------------------------------------------- Definición de funciones -------------------------------------------------

//----------------------------------------------------Connection------------------------------------------------------------------
void espInit()
{
  delay(TimeWithoutConnection);
  WiFi.begin(ssid2, password2);
  Serial.print("Conectando...");
  bool cambioRed = true;
  while (WiFi.status() != WL_CONNECTED) // Check for the connection
  {
    TimeWithoutConnection += 500;
    delay(500);
    Serial.print(".");
    if (TimeWithoutConnection > TimeWithoutConnectionPermited) // Si tarda más de 15 segundos en conectarse a la primera red WiFi que se cambie a la segunda definida.
    {
      WiFi.disconnect(true);
      delay(50);
      if (cambioRed)
      {
        WiFi.begin(ssid, password);
        Serial.println('Intentando conectar a ' + String(ssid));
        cambioRed = !cambioRed;
      }
      else
      {
        WiFi.begin(ssid2, password2);
        Serial.println('Intentando conectar a ' + String(ssid2));
        cambioRed = !cambioRed;
      }
      TimeWithoutConnection = 50;
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
  // Serial.println(index + json);
  if (index != -1)
  {
    ciclotrabajo = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
    Serial.println("\n\nCiclo de trabajo cambiado a " + String(ciclotrabajo) + " milisegundos\n");
  }

  index = json.indexOf("\"timeWithoutConnectionPermited\":") + strlen("timeWithoutConnectionPermited") + 4;
  // Serial.println(index + json);
  if (index != -1)
  {
    TimeWithoutConnectionPermited = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
    Serial.println("\n\nTiempo sin conexión cambiado a < " + String(TimeWithoutConnectionPermited) + " milisegundos\n");
  }

  index = json.indexOf("\"resetSensors\":") + strlen("resetSensors") + 4;
  // Serial.println(index + json);
  if (index != -1)
  {
    resetSensors = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
    Serial.println("\n\nActivar lanzamiento cambiado a > " + String(resetSensors) + ".\n");
    // sentData();
  }
}

/* Emitir datos al servidor */
void sentData()
{
  String Response;
  if (dataJson != "")
    Response = fetch("rocket", "addData", "[" + dataJson.substring(1) + "]");

  if (Response = "1")
  {
    dataJson = "";
    Serial.println("DataJson reseteado");
  }
}

//----------------------------------------------------Sensor MPU6050---------------------------------------------------------------
void MPU6050init()
{
  Wire.begin();
  sensor.initialize();

  if (sensor.testConnection())
  {
    if (printMessages)
      Serial.println("Sensor iniciado correctamente");
  }
  else
  {
    if (printMessages)
      Serial.println("Errror al inciar el sensor");
  }
}

void MPU6050Run()
{
  // sensor.getAcceleration(&ax, &ay, &az);
  // float acc_and_x = atan(ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.1416);
  // float acc_and_y = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / 3.1416);

  // seconds += 1;
  // Serial.print(seconds);
  // Serial.print("      ");
  // Serial.print("Valor ax: ");
  // Serial.print(ax);
  // Serial.print(", Valor ay: ");
  // Serial.print(ay);
  // Serial.print(", Valor az: ");
  // Serial.print(az);
  // Serial.print(", Inclinación en x: ");
  // Serial.print(acc_and_x);
  // Serial.print(", Inclinación en y: ");
  // Serial.print(acc_and_y);

  // sensor.getRotation(&gx, &gy, &gz);
  // Serial.print("            Rotación x: ");
  // Serial.print(gx);
  // Serial.print(", Rotación y: ");
  // Serial.print(gy);
  // Serial.print(", Rotación z: ");
  // Serial.println(gz);

  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  float ax_m_s2 = ax * (9.81 / 32768.0);
  float ay_m_s2 = ay * (9.81 / 32768.0);
  float az_m_s2 = az * (9.81 / (32768.0 * 2));
  float dt = 0.5;

  vx = +ax_m_s2 * dt;
  vy = +ay_m_s2 * dt;
  vz = +az_m_s2 * dt;

  dx = +vx * dt;
  dy = +vy * dt;
  dz = +vz * dt;

  sensor.getAcceleration(&ax, &ay, &az); //primer código, muestra hasta los 90 grados estos ángulos
  gx = atan(ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.1416);
  gy = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / 3.1416);
  gz = sqrt(pow(gx, 2) + pow(gy, 2));

  dataJson += ",{\"acc_x\":\"" + String(ax_m_s2) + "\",\"acc_y\":\"" + String(ay_m_s2) + "\",\"acc_z\":\"" + String(az_m_s2) + "\",\"vel_x\":\"" + String(vx) + "\",\"vel_y\":\"" + String(vy) + "\",\"vel_z\":\"" + String(vz) + "\",\"pos_x\":\"" + String(dx) + "\",\"pos_y\":\"" + String(dy) + "\",\"pos_z\":\"" + String(dz) + "\",\"rot_x\":\"" + String(gx) + "\",\"rot_y\":\"" + String(gy) + "\",\"rot_z\":\"" + String(gz) + "\"}";
  // Serial.println(dataJson);

  if (printMessages)
  {
    Serial.println("ax: " + String(ax_m_s2));
    Serial.println("ay: " + String(ay_m_s2));
    Serial.println("az: " + String(az_m_s2));
    Serial.println("gx: " + gx);
    Serial.println("gy: " + gy);
    Serial.println("gz: " + gz);
    Serial.println("vx: " + String(vx));
    Serial.println("vy: " + String(vy));
    Serial.println("vz: " + String(vz));
    Serial.println("dx: " + String(dx));
    Serial.println("dy: " + String(dy));
    Serial.println("dz: " + String(dz));
  }
}

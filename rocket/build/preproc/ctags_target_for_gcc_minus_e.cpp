# 1 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
//----------------------------------------------------General---------------------------------------------------------------------
// #include <ArduinoJSON.h>
void (*resetFunc)(void) = 0;
void RecolectAndSentData();

int ciclotrabajo = 50, TimeWithoutConnectionPermited = 180000, TimeForRecolectData = 25000, timeForUpgradeVar = 5000, Retrazo_sentData = 5000; // en milisegundos.
bool resetSensors, printMessages = true, connecToBegin = true, DisableSerial, DataJsonContruido; // Indica que deben volver a cero los valores de Rotación y aceleración.

// const int sizeData = 100;
int tiempotranscurrido, millisInt, tiempotranscurrido_UpgradeVar;
String dataJson;

//----------------------------------------------------Connection------------------------------------------------------------------
/* connect to red wifi - Para la operativa porque usa un while*/
void espInit();
/* Realiza la petición a servidor */
String fetch();

# 20 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino" 2
# 21 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino" 2
const char *ssid2 = "RAFAEL";
const char *password2 = "12345678A";
// const char *ssid = "CANCHON";
// const char *password = "12345678";
const char *ssid = "USTA_Estudiantes";
const char *password = "#soytomasino";

WiFiClient client;
HTTPClient http;
HTTPClient httpData;
String dataToSend = "";
int TimeWithoutConnection = 50;

//----------------------------------------------------Sensor MPU6050---------------------------------------------------------------
/* Inicia el sensor */
void MPU6050init();
/* Recolecta información */
void MPU6050Run();

# 41 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino" 2
# 42 "M:\\U\\U 2024-1\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino" 2
MPU6050 sensor;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float vx, vy, vz;
float dx, dy, dz;

int seconds;

void setup()
{
 // if (Serial.available() > 0)
 Serial.begin(115200);
 if (connecToBegin)
  espInit();
 MPU6050init();
 // connect to API
 http.begin("https://rafrocketust.000webhostapp.com/");
 httpData.begin("https://rafrocketust.000webhostapp.com/");

 dataJson.reserve(600000);
}

void loop()
{
 if (true) // printMessages
 {
  // Serial.println("TimeForRecolectData: " + String(TimeForRecolectData));
  // Serial.println("tiempotranscurrido: " + String(tiempotranscurrido));
  Serial.println("Tiempo transcurrido: " + String(millisInt / 1000) + "seg.");
  // Serial.println("TimeForRecolectData + tiempotranscurrido - millisInt: " + String(TimeForRecolectData + tiempotranscurrido - millisInt) + "\"");
 }

 millisInt = millis();

 if (WiFi.status() == WL_CONNECTED)
 {
  if ((timeForUpgradeVar + tiempotranscurrido_UpgradeVar - millisInt) < 0)
  {
   delay(500);
   String response = fetch("rocket", "setlastConnection", "{}"); // Comunicar la última conexión de la placa
   delay(2000);
   String sensorsStatus = fetch("rocket", "getSystemVar", "{}"); // Recibir datos

   if (sensorsStatus != "")
    writePins(sensorsStatus);

   tiempotranscurrido_UpgradeVar = millis();
  }
 }
 else
 {
  espInit();
 }

 delay(ciclotrabajo);
}

//-------------------------------------------- Definición de funciones -------------------------------------------------

//----------------------------------------------------Connection------------------------------------------------------------------
void espInit()
{
 delay(TimeWithoutConnection);
 WiFi.begin(ssid2, password2);
 if (printMessages)
  Serial.print("Conectando a " + String(ssid2) + " ...");
 bool cambioRed = true;
 while (WiFi.status() != WL_CONNECTED) // Check for the connection
 {
  TimeWithoutConnection += 500;
  delay(500);
  if (printMessages)
   Serial.print(".");
  if (TimeWithoutConnection > TimeWithoutConnectionPermited) // Si tarda más de 15 segundos en conectarse a la primera red WiFi que se cambie a la segunda definida.
  {
   WiFi.disconnect(true);
   delay(50);
   if (cambioRed)
   {
    WiFi.begin(ssid, password);
    if (printMessages)
     Serial.println('Intentando conectar a ' + String(ssid));
    cambioRed = !cambioRed;
   }
   else
   {
    WiFi.begin(ssid2, password2);
    if (printMessages)
     Serial.println('Intentando conectar a ' + String(ssid2));
    cambioRed = !cambioRed;
   }
   TimeWithoutConnection = 50;
  }
 }
 if (printMessages)
  Serial.println("Conectado con éxito a una red wifi.");
}

String fetch(String module, String accion, String parameters)
{
 if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
 {
  dataToSend = "json={\"module\":\"" + module + "\", \"accion\": \"" + accion + "\", \"parameters\": " + parameters + "}";
  if (parameters.length() > 200)
   delay(Retrazo_sentData);

  if (printMessages)
   Serial.println("\nDatos a enviar: " + String(dataToSend));

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int responseCode = http.POST(dataToSend); // Enviamos el post pasándole los datos que queremos enviar. retorna un código http
  if (parameters.length() > 200)
   delay(Retrazo_sentData);

  if (responseCode > 0) // no hubo errores al hacer la petición
  {
   if (printMessages)
    Serial.println("Código HTTP ► " + String(responseCode));

   if (responseCode == 200) // La API envió una respuesta
   {
    String response = http.getString();
    if (printMessages)
    {
     Serial.println("El servidor respondió ▼ ");
     Serial.println(response + "▼");
     Serial.println("------------------");
    }
    http.end();
    return response;
   }
  }
  else
  {
   String response = http.getString();
   http.end();

   if (printMessages)
   {
    Serial.print("Error enviando POST, código: ");
    Serial.println(String(responseCode));
    Serial.println(response);
   }
  }
 }
 else
 {
  if (printMessages)
   Serial.println("\nError en la conexión WIFI");
 }
 return "";
}

void writePins(String json)
{
 // Buscar las claves y extraer los valores
 int index = json.indexOf("\"ciclotrabajo\":") + strlen("ciclotrabajo") + 4;
 // if (printMessages)
 //  Serial.println(index + json);
 if (index != -1)
 {
  int ciclotrabajo_Response = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
  if (ciclotrabajo < ciclotrabajo_Response)
  {
   if (printMessages)
    Serial.println("Reseteando sistema....");
   resetFunc();

   // if (printMessages)
   // 	Serial.println("\nCiclo de trabajo cambiado a " + String(ciclotrabajo) + " milisegundos");
  }
 }

 index = json.indexOf("\"timeWithoutConnectionPermited\":") + strlen("timeWithoutConnectionPermited") + 4;
 // if (printMessages)
 // Serial.println(index + json);
 if (index != -1)
 {
  int TimeWithoutConnectionPermited_Response = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
  if (TimeWithoutConnectionPermited_Response != TimeWithoutConnectionPermited)
  {
   TimeWithoutConnectionPermited = TimeWithoutConnectionPermited_Response;

   if (printMessages)
    Serial.println("Tiempo sin conexión cambiado a < " + String(TimeWithoutConnectionPermited) + " milisegundos");
  }
 }

 index = json.indexOf("\"resetSensors\":") + strlen("resetSensors") + 4;
 // if (printMessages)
 // Serial.println(index + json);
 if (index != -1)
 {
  bool resetSensors_Response = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
  if (resetSensors_Response != resetSensors)
  {
   resetSensors = resetSensors_Response;

   if (printMessages)
    Serial.println("Activar lanzamiento cambiado a > " + String(resetSensors) + ".");

   if (resetSensors)
   {
    RecolectAndSentData();
   }
  }
 }

 // Controlar la activación sel Serial (Falta implementarlo completamente)
 // index = json.indexOf("\"DisableSerial\":") + strlen("DisableSerial") + 4;
 // // if (printMessages)
 // // Serial.println(index + json);
 // if (index != -1)
 // {
 //   DisableSerial = json.substring(index, json.substring(index).indexOf("\"") + index).toInt();
 //   if (printMessages)
 //     Serial.println("\n\nDisableSerial cambiado a < " + String(DisableSerial) + ".\n");

 //   if (DisableSerial)
 //   {
 //     Serial.end();
 //     printMessages = false;
 //   }
 // }
}

/* Emitir datos al servidor */
String sentData()
{
 // if (printMessages)
 // 		Serial.println("Ingreso a sentData");

 if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
 {
  httpData.end();

  // if (printMessages)
  // {
  // 	Serial.println("\nIngreso al sentData.");
  // 	Serial.println(dataJson);
  // }

  if (not DataJsonContruido)
  {
   if (dataJson != "")
   {
    delay(2000);
    dataJson += "]}";
    delay(2000);
   }
   DataJsonContruido = true;
  }
  delay(3000);

  if (printMessages)
  {
   Serial.print("\nDataJsonContruido -> Datos a enviar: ");
   Serial.println(dataJson);
  }

  delay(5000);

  httpData.addHeader("Content-Type", "application/x-www-form-urlencoded");
  httpData.setTimeout(20000);
  int responseCode = httpData.POST(dataJson); // Enviamos el post pasándole los datos que queremos enviar. retorna un código http
  // int responseCode = httpData.POST("json={\"module\": \"rocket\", \"accion\": \"getLastData\", \"parameters\": {}}"); // Enviamos el post pasándole los datos que queremos enviar. retorna un código http
  delay(5000);

  if (responseCode > 0) // no hubo errores al hacer la petición
  {
   if (printMessages)
    Serial.println("Código HTTP ► " + String(responseCode));

   if (responseCode == 200) // La API envió una respuesta
   {
    String response = httpData.getString();
    if (printMessages)
    {
     Serial.println("El servidor respondió ▼ ");
     Serial.println(response + "▼");
     Serial.println("------------------");
    }
    httpData.end();
    return response;
   }
  }
  else
  {
   String response = httpData.getString();
   httpData.end();

   if (printMessages)
   {
    Serial.print("Error enviando POST, código: ");
    Serial.println(String(responseCode));
    Serial.println(response);
   }
   return "";
  }
 }
 else
 {
  if (printMessages)
   Serial.println("\nError en la conexión WIFI");
 }
 return "";
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
   Serial.println("Error al inciar el sensor");
 }
}

void MPU6050Run()
{
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

 sensor.getAcceleration(&ax, &ay, &az); // primer código, muestra hasta los 90 grados estos ángulos
 gx = atan(ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.1416);
 gy = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / 3.1416);
 gz = sqrt(pow(gx, 2) + pow(gy, 2));

 if (dataJson.length() < 500000)
 {
  // Serial.println("menor al sice-------------------------------------------------");
  if (dataJson == "")
   dataJson += "json={\"module\":\"rocket\", \"accion\": \"addData\", \"parameters\": [{\"acc_x\":\"" + String(ax_m_s2) + "\",\"acc_y\":\"" + String(ay_m_s2) + "\",\"acc_z\":\"" + String(az_m_s2) + "\",\"vel_x\":\"" + String(vx) + "\",\"vel_y\":\"" + String(vy) + "\",\"vel_z\":\"" + String(vz) + "\",\"pos_x\":\"" + String(dx) + "\",\"pos_y\":\"" + String(dy) + "\",\"pos_z\":\"" + String(dz) + "\",\"rot_x\":\"" + String(gx) + "\",\"rot_y\":\"" + String(gy) + "\",\"rot_z\":\"" + String(gz) + "\"}";
  else
   dataJson += ",{\"acc_x\":\"" + String(ax_m_s2) + "\",\"acc_y\":\"" + String(ay_m_s2) + "\",\"acc_z\":\"" + String(az_m_s2) + "\",\"vel_x\":\"" + String(vx) + "\",\"vel_y\":\"" + String(vy) + "\",\"vel_z\":\"" + String(vz) + "\",\"pos_x\":\"" + String(dx) + "\",\"pos_y\":\"" + String(dy) + "\",\"pos_z\":\"" + String(dz) + "\",\"rot_x\":\"" + String(gx) + "\",\"rot_y\":\"" + String(gy) + "\",\"rot_z\":\"" + String(gz) + "\"}";
 }
 // else
 // {
 // 	if (WiFi.status() == WL_CONNECTED)
 // 		sentData();
 // 	else
 // 	{
 // 		espInit();
 // 		sentData();
 // 	}
 // }
 // if(printMessages)
 // Serial.println(dataJson);

 if (printMessages)
 {
  Serial.println("\ngx: " + String(gx));
  Serial.println("gy: " + String(gy));
  Serial.println("gz: " + String(gz));
  Serial.println("ax: " + String(ax_m_s2));
  Serial.println("ay: " + String(ay_m_s2));
  Serial.println("az: " + String(az_m_s2));
  Serial.println("vx: " + String(vx));
  Serial.println("vy: " + String(vy));
  Serial.println("vz: " + String(vz));
  Serial.println("dx: " + String(dx));
  Serial.println("dy: " + String(dy));
  Serial.println("dz: " + String(dz));
 }
}

void RecolectAndSentData() // La conexión a internet debe estar bien.
{
 if (printMessages)
  Serial.println("Ingreso a RecolectAndSentData " + String(TimeForRecolectData + tiempotranscurrido - millisInt));

 tiempotranscurrido = millisInt;
 DataJsonContruido = false;

 while ((TimeForRecolectData + tiempotranscurrido - millisInt) > 0) // La idea es que haya un tiempo prudente para recolectar datos antes de enviar datos.
 {
  if (printMessages)
   Serial.println("Recolectando data. " + String(TimeForRecolectData + tiempotranscurrido - millisInt));

  millisInt = millis();
  MPU6050Run();
  delay(150);
 }

 String Response;
 while (Response != "1") // Intenta enviar los datos al servidor hasta que se envíen
 {
  if (WiFi.status() == WL_CONNECTED)
  {
   Response = sentData();
   delay(3000);

   String responsesetlastConnection_ = fetch("rocket", "setlastConnection", "{}"); // Comunicar la última conexión de la placa
   Serial.println("While -> Response != 1");

   // if (printMessages)
   // 	Serial.println(Response);

   if (Response == "1")
   {
    dataJson = "";

    tiempotranscurrido = millis();
    if (printMessages)
     Serial.println("DataJson reseteado");
   }
  }
  else
   espInit();
 }

 String response_resetSensors;
 while (response_resetSensors != "1") // Intenta enviar los datos al servidor hasta que se envíen
 {
  if (WiFi.status() == WL_CONNECTED)
  {
   response_resetSensors = fetch("rocket", "setSystemvar", "{\"field\": \"resetSensors\", \"value\": \"0\"}"); // Resetear el lanzamiento - No enviar lanzamiento.
   delay(3000);

   String responsesetlastConnection_ = fetch("rocket", "setlastConnection", "{}"); // Comunicar la última conexión de la placa
   Serial.println("While -> response_resetSensors != 1");
  }
  else
   espInit();
 }
}

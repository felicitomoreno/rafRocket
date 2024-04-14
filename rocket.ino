// #include <MPU6050_tockn.h>
#include <MPU6050.h>
#include "Wire.h"

MPU6050 sensor;

int16_t ax, ay, az;
int16_t gx, gy, gz;

int seconds, milis = 50;

void setup()
{

    Wire.begin();
    sensor.initialize();

    Serial.begin(115200);
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

void loop()
{
    sensor.getAcceleration(&ax , &ay, &az);
    float acc_and_x = atan(ax/sqrt(pow(ay,2) + pow(az,2))) * (180.0 / 3.1416);
    float acc_and_y = atan(ay/sqrt(pow(ax,2) + pow(az,2))) * (180.0 / 3.1416);

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
    
    sensor.getRotation(&gx , &gy, &gz);
    Serial.print("            Rotación x: ");
    Serial.print(gx);
    Serial.print(", Rotación y: ");
    Serial.print(gy);
    Serial.print(", Rotación z: ");
    Serial.println(gz);

    delay(milis);
}
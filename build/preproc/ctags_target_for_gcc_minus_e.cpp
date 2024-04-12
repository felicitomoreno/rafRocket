# 1 "C:\\Users\\Admin\\Downloads\\PI 2024-1\\Arduino Code\\rocket\\rocket.ino"
int seconds;

void setup() {

  Serial.begin(115200);
  Serial.println("Hola Mundo.");
  Serial.println();
  Serial.println();
  Serial.println("Seconds:");
  Serial.println();
}

void loop() {

  seconds += 1;
  Serial.println(seconds);
  delay(1000);
}

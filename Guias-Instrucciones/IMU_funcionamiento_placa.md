# IMU

Con estos codigos funcioina la **IMU** de manera independiente, resta hacerlo compatible con el firmware de andino

## Liberia

MPU6050_light



## Código

```c
\#include <Wire.h>

\#include <MPU6050_light.h>

MPU6050 mpu(Wire);

void setup() {

  Serial.begin(115200);

  Wire.begin();

  Serial.println("Inicializando...");

  byte status = mpu.begin();

  Serial.print("Status: ");

  Serial.println(status);

  if (status != 0) {

​    Serial.println("Error MPU6050");

​    while (1);

  }

  Serial.println("Calibrando...");

  mpu.calcOffsets(true, true);

  Serial.println("Listo");

}

void loop() {

  mpu.update();

  Serial.print("Acc X: ");

  Serial.print(mpu.getAccX());

  Serial.print(" Y: ");

  Serial.print(mpu.getAccY());

  Serial.print(" Z: ");

  Serial.println(mpu.getAccZ());

  delay(200);

}
```









## Código con giroscopio



```c
#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("Inicializando MPU6050...");

  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("Error de conexión MPU6050");
    while (1);
  }

  Serial.println("Calibrando... no muevas el sensor");
  delay(1000);
  mpu.calcOffsets(true, true); // accel + gyro
  Serial.println("Listo.");
}

void loop() {
  mpu.update();

  if (millis() - timer > 200) {
    timer = millis();

    // Aceleración (g)
    Serial.print("Acc [g] X: ");
    Serial.print(mpu.getAccX());
    Serial.print(" Y: ");
    Serial.print(mpu.getAccY());
    Serial.print(" Z: ");
    Serial.println(mpu.getAccZ());

    // Giróscopo (°/s)
    Serial.print("Gyro [°/s] X: ");
    Serial.print(mpu.getGyroX());
    Serial.print(" Y: ");
    Serial.print(mpu.getGyroY());
    Serial.print(" Z: ");
    Serial.println(mpu.getGyroZ());

    // Ángulos estimados (roll, pitch)
    Serial.print("Angle X (roll): ");
    Serial.print(mpu.getAngleX());
    Serial.print("  Y (pitch): ");
    Serial.println(mpu.getAngleY());

    Serial.println("----------------------");
  }
}
```


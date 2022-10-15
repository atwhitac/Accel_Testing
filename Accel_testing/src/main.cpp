#include <Arduino.h>
#include <MPU6050.h>
#include <SdFat.h>

#ifndef UTIL_H
#define UTIL_H
MPU6050 mpu;
SdFs sd;
FsFile file;
unsigned long t_start = 0;
char filename[32];
bool is_logging;
// void intervalFunc(void);
// IntervalTimer timer;
// int ticks = 0;

/**
 * Generates and selects an unused filename from the SD card. Currently implemented in an inefficient way that
 * doesn't matter because it runs in setup and nowhere else. It works by incrementally mutating the passed buffer
 * with filenames ranging from "D0.BIN" to "D999.BIN", then checking if they already exist on the SD card.
 * If not, it returns. The program is expected to use the mutated char buffer to get the filename. It should
 * only be called AFTER SD has successfully been initialized.
 * */
// CREDIT FOR THIS CODE GOES TO RAHUL
void select_next_filename(char *buffer, SdFs *sd)
{ // Passed buff should be of size FILENAME_SIZE
  for (int fileNum = 0; fileNum < 1000; fileNum++)
  {
    char fileNumber[5]; // 4-character number + null
    sprintf(fileNumber, "%d", fileNum);
    strcpy(buffer, "Accel_Data_");
    strcat(buffer, fileNumber);
    strcat(buffer, ".csv");
    // debugl(buffer);
    if (!sd->exists(buffer))
    {
      return;
    }
  }
}

#endif

void setup()
{

  Serial.begin(115200);
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  mpu.calibrateGyro();
  // timer.begin(intervalFunc, 100000);
  Serial.println("Initialize MPU6050");
}

void init_log()
{

  select_next_filename(filename, &sd);
  if (!file.open(filename, O_RDWR | O_CREAT))
  {
    return;
  }

  file.printf(
      "Linear_X (m/s^2), Linear_Y (m/s^2), Linear_Z (m/s^2), Gyro_X (rad/s), Gyro_Y (rad/s), Gyro_Z (rad/s)\n");
  t_start = millis();
  is_logging = true;
}

void checkSettings()
{
  Serial.println();

  Serial.print(" * Sleep Mode:        ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Clock Source:      ");
  switch (mpu.getClockSource())
  {
  case MPU6050_CLOCK_KEEP_RESET:
    Serial.println("Stops the clock and keeps the timing generator in reset");
    break;
  case MPU6050_CLOCK_EXTERNAL_19MHZ:
    Serial.println("PLL with external 19.2MHz reference");
    break;
  case MPU6050_CLOCK_EXTERNAL_32KHZ:
    Serial.println("PLL with external 32.768kHz reference");
    break;
  case MPU6050_CLOCK_PLL_ZGYRO:
    Serial.println("PLL with Z axis gyroscope reference");
    break;
  case MPU6050_CLOCK_PLL_YGYRO:
    Serial.println("PLL with Y axis gyroscope reference");
    break;
  case MPU6050_CLOCK_PLL_XGYRO:
    Serial.println("PLL with X axis gyroscope reference");
    break;
  case MPU6050_CLOCK_INTERNAL_8MHZ:
    Serial.println("Internal 8MHz oscillator");
    break;
  }

  Serial.print(" * Gyroscope:         ");
  switch (mpu.getScale())
  {
  case MPU6050_SCALE_2000DPS:
    Serial.println("2000 dps");
    break;
  case MPU6050_SCALE_1000DPS:
    Serial.println("1000 dps");
    break;
  case MPU6050_SCALE_500DPS:
    Serial.println("500 dps");
    break;
  case MPU6050_SCALE_250DPS:
    Serial.println("250 dps");
    break;
  }

  Serial.print(" * Gyroscope offsets: ");
  Serial.print(mpu.getGyroOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getGyroOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getGyroOffsetZ());

  Serial.println();
}

void loop()
{
  if (digitalRead(20) == 1)
  {
    // stop the csv file here
    file.close();
  }

  // Prints Linear data
  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();

  // Uncomment if you want to run raw data
  // Serial.print(rawAccel.XAxis);
  // Serial.print(" ");
  // Serial.print(rawAccel.YAxis);
  // Serial.print(" ");
  // Serial.println(rawAccel.ZAxis);

  // Comment if you want to run normalized acceleration
  Serial.print(normAccel.XAxis);
  Serial.print(" ");
  Serial.print(normAccel.YAxis);
  Serial.print(" ");
  Serial.println(normAccel.ZAxis);

  // Prints Gyro data
  Vector rawGyro = mpu.readRawGyro();
  Vector normGyro = mpu.readNormalizeGyro();

  // Uncomment if you want to run raw data
  // Serial.print(rawGyro.XAxis);
  // Serial.print(" ");
  // Serial.print(rawGyro.YAxis);
  // Serial.print(" ");
  // Serial.println(rawGyro.ZAxis);

  Serial.print(normGyro.XAxis);
  Serial.print(" ");
  Serial.print(normGyro.YAxis);
  Serial.print(" ");
  Serial.println(normGyro.ZAxis);
  // ticks++;

  file.printf("%f, %f, %f, %f, %f, %f\n", normAccel.XAxis, normAccel.YAxis, normAccel.ZAxis, normGyro.XAxis, normGyro.YAxis, normGyro.ZAxis);
  // file.printf("%f, %f, %f, %f, %f, %f\n", rawAccel.XAxis, rawAccel.YAxis, rawAccel.ZAxis, rawGyro.XAxis, rawGyro.YAxis, rawGyro.ZAxis);
}

// void intervalFunc(void)
// {
//   Serial.println("One second");
//   Serial.println(ticks);
//   ticks = 0;
// }
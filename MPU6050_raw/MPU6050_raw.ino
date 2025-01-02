/*
  MPU6050 Raw

  A code for obtaining raw data from the MPU6050 module with the option to
  modify the data output format.

  Find the full MPU6050 library documentation here:
  https://github.com/ElectronicCats/mpu6050/wiki
*/
#include "I2Cdev.h"
#include "MPU6050.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 
// MQTT Broker 初始化
const char* mqttServer = "192.168.50.209"; // 你的本地 IP 就是本地伺服器接在哪邊 目前是我電腦
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Network credentials (lab207 wifi can not work!!)
const char* ssid     = "OPPO Reno7 5G";
const char* password = "k24g3k9k";


MPU6050 mpu;
#define OUTPUT_READABLE_ACCELGYRO
#define BUTTON_PIN 23
int16_t ax, ay, az;
int16_t gx, gy, gz;
struct MyData{
  byte X;
  byte Y;
};
bool flag = 0; // flag to record the status(gesture) change of mpu6050
int gesture_num = 0;
int gesture_x = 2;
int gesture_y = 2;
int pregesture = 0;
MyData data;
char msg[7] = "";
char state = 'N';

void setup() {
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin(); 
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  Serial.begin(115200); //Initializate Serial wo work well at 8MHz/16MHz
  /*Initialize device and check connection*/ 
  Serial.println("Initializing MPU...");
  mpu.initialize();
  Serial.println("Testing MPU6050 connection...");
  if(mpu.testConnection() ==  false){
    Serial.println("MPU6050 connection failed");
    while(true);
  }
  else{
    Serial.println("MPU6050 connection successful");
  }

  /* Use the code below to change accel/gyro offset values. Use MPU6050_Zero to obtain the recommended offsets */ 
  Serial.println("Updating internal sensor offsets...\n");
  mpu.setXAccelOffset(0); //Set your accelerometer offset for axis X
  mpu.setYAccelOffset(0); //Set your accelerometer offset for axis Y
  mpu.setZAccelOffset(0); //Set your accelerometer offset for axis Z
  mpu.setXGyroOffset(0);  //Set your gyro offset for axis X
  mpu.setYGyroOffset(0);  //Set your gyro offset for axis Y
  mpu.setZGyroOffset(0);  //Set your gyro offset for axis Z
  /*Print the defined offsets*/
  Serial.print("\t");
  Serial.print(mpu.getXAccelOffset());
  Serial.print("\t");
  Serial.print(mpu.getYAccelOffset()); 
  Serial.print("\t");
  Serial.print(mpu.getZAccelOffset());
  Serial.print("\t");
  Serial.print(mpu.getXGyroOffset()); 
  Serial.print("\t");
  Serial.print(mpu.getYGyroOffset());
  Serial.print("\t");
  Serial.print(mpu.getZGyroOffset());
  Serial.print("\n");

  /*Configure board LED pin for output*/ 
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

// 連線到mqtt
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.publish("home/ir/connected/esp2", "esp2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  // Serial.println(state);
  static int count = 0;
  if (digitalRead(BUTTON_PIN) == LOW) {
    state = 'Y';
  }
  // Serial.println(state);
  /* Read raw accel/gyro data from the module. Other methods commented*/
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);
  data.X = map(az, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255);  // Y axis data
  //delay(500);
  // Serial.print("Axis X = ");
  // Serial.print(data.X);
  // Serial.print("  ");
  // Serial.print("Axis Y = ");
  // Serial.println(data.Y);
//  if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();
  String output = " ";

  if (data.Y < 80) {
    gesture_y = 0;
  }
  else if (data.Y > 145) {
    gesture_y = 1;
  }else {
    gesture_y = 2;
  }
  if (data.X > 155) {
    gesture_x = 3;
  }
  else if (data.X < 80) {
    gesture_x = 4;
  }else {
    gesture_x = 2;
  }
  gesture_num = (abs(data.X) > abs(data.Y))?gesture_x:gesture_y;
  if(state == 'Y') {
    // gesture_num = gesture_x+gesture_y*2;
    msg[count] = (gesture_num+'0');
    Serial.print("gesture: ");
    Serial.println(gesture_num);
    Serial.print("count: ");
    Serial.println(count);
    if(gesture_num!=pregesture) {
      count++;
      pregesture = gesture_num;
    }
    if(count >= 6) {
        state = 'N';
        msg[6] = '\0';
        Serial.println(msg);
        count = 0;
    }
  }
  if (flag == 1 && output != " ") {
    // client.publish("home/ir/gesture/esp2", output.c_str()); // send message to server
  }
  
  delay(500);
  /*Blink LED to indicate activity*/
  // blinkState = !blinkState;
  // digitalWrite(LED_BUILTIN, blinkState);
  
}

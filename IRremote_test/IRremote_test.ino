#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

const uint16_t kRecvPin = 2;  // 根據你的連接修改引腳
const uint16_t kIrLedPin = 4;  // 根據你的實際連接修改引腳

IRsend irsend(kIrLedPin);
IRrecv irrecv(kRecvPin);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // 啟用紅外線接收
  Serial.println("IR Receiver Initialized");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println("Received IR signal");
    Serial.print("Raw Data: ");
    Serial.println(results.value, HEX);  // 打印原始數據
    Serial.print("Bits: ");
    Serial.println(results.bits);
    Serial.print("Protocol: ");
    Serial.println(typeToString(results.decode_type));
    irrecv.resume();  // 接收下一個信號
    Serial.println(results.value, HEX);
  }
  // uint64_t rawData = 0xD81; 

  // Serial.println("Sending Panasonic IR signal");
  uint64_t rawData = 0x344A9054C4;
  // Serial.println("Sending Panasonic IR signal");
  irsend.sendPanasonic(0x4004, rawData);
  delay(5000); 


  // irsend.sendSymphony(rawData, 12);  
  // delay(1000);
}


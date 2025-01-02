

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 2;
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
IRrecv irrecv(kRecvPin);
decode_results results;

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData[67] = {9000, 4500, 650, 550, 650, 1650, 600, 550, 650, 550,
                        600, 1650, 650, 550, 600, 1650, 650, 1650, 650, 1650,
                        600, 550, 650, 1650, 650, 1650, 650, 550, 600, 1650,
                        650, 1650, 650, 550, 650, 550, 650, 1650, 650, 550,
                        650, 550, 650, 550, 600, 550, 650, 550, 650, 550,
                        650, 1650, 600, 550, 650, 1650, 650, 1650, 650, 1650,
                        650, 1650, 650, 1650, 650, 1650, 600};
// Example Samsung A/C state captured from IRrecvDumpV2.ino
uint8_t samsungState[kSamsungAcStateLength] = {
    0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0,
    0x01, 0xE2, 0xFE, 0x71, 0x40, 0x11, 0xF0};

void setup() {
  irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
  irrecv.enableIRIn();  // 啟用紅外線接收
  Serial.println("IR Receiver Initialized");
}

void loop() {
  // Serial.println("NEC");
  // irsend.sendNEC(0x00FFE01FUL);

  // Serial.println("Sony");
  // irsend.sendSony(0xa90, 12, 2);  // 12 bits & 2 repeats

  // irsend.sendPanasonic(0x4004, 0x344A9054C4);
  // delay(1000); 

  irsend.sendSymphony(0xD82, 12);  
  delay(1000);

  if (irrecv.decode(&results)) {
    Serial.println("Received IR signal");
    Serial.print("Raw Data: ");
    Serial.println(results.value, HEX);  // 打印原始數據
    Serial.print("Bits: ");
    Serial.println(results.bits);
    Serial.print("Protocol: ");
    Serial.println(typeToString(results.decode_type));
    Serial.print("Adress: ");
    Serial.println(results.address);
    irrecv.resume();  // 接收下一個信號
    Serial.println(results.value, HEX);
    Serial.println();
  }

  // Serial.println("a rawData capture from IRrecvDumpV2");
  // irsend.sendRaw(rawData, 67, 38);  // Send a raw data capture at 38kHz.
  // delay(2000);
  // Serial.println("a Samsung A/C state from IRrecvDumpV2");
  // irsend.sendSamsungAC(samsungState);
  // delay(2000);
}

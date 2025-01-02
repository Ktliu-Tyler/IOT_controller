#include <ESP8266WiFi.h> // 使用 ESP8266WiFi 库
// 下面這一個庫是mqtt
#include <PubSubClient.h>
#include <IRremoteESP8266.h> // 使用适用于 ESP8266 的 IRremote 库
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

// WiFi 配置
// const char* ssid = "APRDC-Public";   
// const char* password = "23456789";  

// const char* ssid = "AndroidAPFF94";     
// const char* password = "lank5516"; 

const char* ssid = "OPPO Reno7 5G";   
const char* password = "k24g3k9k"; 

// const char* ssid = "R207_5G";   
// const char* password = "33665107";

// MQTT Broker 初始化
const char* mqttServer = "192.168.50.209"; // 你的本地 IP 就是本地伺服器接在哪邊 目前是我電腦
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t recvPin = 2; // D2
const uint16_t sendPin = 4; // D1
IRrecv irrecv(recvPin);
IRsend irsend(sendPin);
decode_results results;

void setup() {
  Serial.begin(115200);

  // 這裡設定 WiFi 你用自己原本連wifi方式就好
  setup_wifi();

  // 這裡設定 MQTT 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 // 到這

#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
  irrecv.enableIRIn(); 
  irsend.begin();  
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char key = (char)payload[0];
  char msg[length-1];
  for (unsigned int i = 1; i < length; i++) {
    msg[i-1] = (char)payload[i];
  }
  msg[length-1] = '\0';
  const unsigned long long value = strtoull(msg, NULL, 0);
  switch(key) {
      case 's':
        Serial.println("SYMPHONY");
        irsend.sendSymphony(value, 12, 5);
        // irsend.sendSymphony(value, 12, 10);
        // irsend.sendSymphony(value, 12, 10);
        Serial.println(value);
        break;
      case'N':
        Serial.println("NEC");
        irsend.sendNEC(value, 12, 3);
        irsend.sendNEC(value, 12, 3);
        irsend.sendNEC(value, 12, 3);
        Serial.println(msg);
        break;
      case 'S':
        Serial.println("SONY");
        irsend.sendSony(value, 12, 3);
        irsend.sendSony(value, 12, 3);
        irsend.sendSony(value, 12, 3);
        Serial.println(msg);
        break;
      case 'G':
        Serial.println("SAMSUNG");
        irsend.sendSamsung36(value, 12, 3);
        irsend.sendSamsung36(value, 12, 3);
        irsend.sendSamsung36(value, 12, 3);
        Serial.println(msg);
        break;
      case 'P':
        Serial.print("PANASONIC");
        irsend.sendPanasonic(value, 12, 3);
        irsend.sendPanasonic(value, 12, 3);
        irsend.sendPanasonic(value, 12, 3);
        Serial.println(msg);
        break;
    }
  
  

  // if (strcmp(topic, "home/ir/send/esp1") == 0) {
  //   unsigned long irCode = strtoul(msg, NULL, 10);
  //   irsend.sendNEC(irCode, 32);
  //   Serial.println("IR code sent");
  // }
}


// 這個也要 這是用來連線到mqtt
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe("home/ir/send/esp1");
      client.publish("home/ir/connected/esp1", "esp1"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
//到這

void loop() {
  // 這裡要
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // 到這邊


  // Serial.printf("Running..\n");
  if (irrecv.decode(&results)) {
    Serial.println("Received IR signal");
    Serial.print("Raw Data: ");
    Serial.println(results.value, DEC);  // 打印原始數據
    Serial.print("Bits: ");
    Serial.println(results.bits);
    Serial.print("Protocol: ");
    Serial.println(typeToString(results.decode_type));
    Serial.print("Adress: ");
    Serial.println(results.address);
    char msg[32];
    char key = 'n';
    switch(results.decode_type) {
      case SYMPHONY:
        key = 's';
        Serial.println("Symphony");
        break;
      case NEC:
        key = 'N';
        Serial.println("NEC");
        break;
      case SONY:
        key = 'S';
        Serial.println("SONY");
      case SAMSUNG:
        key = 'G';
        Serial.println("SAMSUNG");
        break;
      case PANASONIC:
        key = 'P';
        Serial.println("PANASONIC");
        break;
    }
    
    if(key != 'n') {
      sprintf(msg, "%c%llu", key, results.value);
      client.publish("home/ir/receive/esp1", msg);// 這一行發送訊息 目前都打這樣 然後你可以把receive改成其他的名稱 像是temper 類的 我就可以判斷
    }else {
      Serial.println("Unknown protocol");
    }
    irrecv.resume();  
    Serial.println(results.value, HEX);
    Serial.println();
  }
  // if (Serial.available() > 0) {
  //   String message = Serial.readStringUntil('\n'); 
  //   Serial.print("Publishing message: ");
  //   Serial.println(message);
  //   client.publish("home/ir/connected/esp1", message.c_str()); 
  // }
}

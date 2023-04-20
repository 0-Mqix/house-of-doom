#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <WiFiEsp.h>
#include <ArduinoSTL.h>

#include "led.h"

#define DEBOUNCE_DELAY 300
#define AT_BAUD_RATE 115200

const int di_red_button = 2;
const int di_blue_button = 3;
const int di_dht = 4;

volatile RGB status_led(24, 23, 22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

char ssid[] = "Arduino";          // your network SSID (name)
char pass[] = "Zp3Y8FKt";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char host[] = "10.11.16.91";

std::vector<WiFiEspClient*> clients(0);
int last_id = -1;

DHT dht;
WiFiEspClient client;
WiFiEspServer server(6000);

volatile int screen = 0;

void setup() {
  //blue button
  pinMode(di_red_button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(di_red_button), red_button_handler, FALLING);

  //blue button
  pinMode(di_blue_button, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(di_blue_button), blue_button_handler, FALLING);

  //screen
  lcd.init();
  lcd.backlight();
  
  Serial.begin(115200);
  //start up message
  Serial.println("----------------");
  Serial.println("house-of-doom");
  Serial.println("");
  Serial.println("----------------");

  //status rgb led
  status_led.init();
  status_led.set_color(green);
  status_led.show();
  
  Serial1.begin(115200);
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  server.begin();

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // if (client->connect(host, 4001)) {
  //   client->println("GET / HTTP/1.1");
  //   client->println("Host: 10.11.16.91");
  //   client->println("Content-Type: text/plain");
  //   client->println("Content-Length: 4");
  //   client->println();
  //   client->println("test");
  // }
}

void loop() {
  // while (client->available()) {
  //   char c = client->read();
  //   Serial.write(c);
  // }

  // if (status_led.is_active()) {
  //   dht_print_lcd();
  //   delay(10000);
  // }

  // listen for incoming clients
  WiFiEspClient new_client = server.available();

  if (new_client && last_id != new_client.id()) {
    clients.push_back(&new_client);
    last_id = new_client.id();
  }

  size_t size = clients.size();

  for (size_t i = 0; i < size; i++) {
    handle_client(i, clients[i]);
  } 
}


void red_button_handler() {
  status_led.toggle();
  status_led.show();
}

void dht_print_lcd() {
  dht.read11(di_dht);
  
  lcd.clear();

  lcd.setCursor(0, 0); 
  lcd.print("Humidity:");
  lcd.setCursor(12, 0);     
  lcd.print(dht.humidity, 0);
  lcd.print(" %");
  
  lcd.setCursor(0, 1); 
  lcd.print("Temprature:");
  lcd.setCursor(12, 1);     
  lcd.print(dht.temperature, 0);
  lcd.print(" C");
}

void printMacAddress() {
  // get your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  
  // print MAC address
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);
}

String msg = "";

void handle_client(size_t i, WiFiEspClient* client) {  
  while (client->available()) {
      char c = client->read();
      if (c == "\n") {
        msg = "";
      } else {
        msg.concat(c);
      }  
  }

  client->write(msg.c_str());

  // if (client->getWriteError()) {
  //   Serial.print(client->remoteIP());
  //   Serial.print("-");
  //   Serial.print(client->id());
  //   Serial.print("-");
  //   Serial.print(i);
  //   Serial.println(" [X]");

  //   std::vector<WiFiEspClient*>::iterator it = clients.begin();
  //   std::advance(it, i);
  //   clients.erase(it);
  //   client->stop();
  // }
}

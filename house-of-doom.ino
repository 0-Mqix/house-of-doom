//Made By 0-Mqix! 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <WiFiEspAT.h>

#include "Rgb.h"

#define DEBOUNCE_DELAY 300
#define AT_BAUD_RATE 115200

const int di_red_button = 2;
const int di_dht = 3;

RGB status_led(24, 23, 22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int status = WL_IDLE_STATUS;

// char ssid[] = "arduino";
// char pass[] = "zp3y8fkt";

// char host[] = "0.0.0.0";

DHT dht;

WiFiClient client;

//string to store the incomming message in;
String message;

//long to store time of the last cooldowns of ...
long send_cooldown = 0;
long rgb_shift_cooldown = 0;
long update_display_cooldown = 0;

//rgb if this is true the led will do color shifting each 1s
bool rgb = false;

//for vertical scroll on the lcd
int display_state = 0;

//the red button clicks comming from the database
int clicks = 0; 

/*
volatile is used to indicate to the compiler that a variable's value may change unexpectedly,
and therefore it should not make any assumptions about its value or optimize any code based on that assumption.
so you can modifiy its value inside a interupt
*/

//volitile bool to store if the button is clicked
volatile bool red_button_click = false;

void displayHumidity(int y) {
  lcd.setCursor(0, y); 
  lcd.print("Humidity:");
  lcd.setCursor(12, y);     
  lcd.print(dht.humidity, 0);
  lcd.print(" %");
}

void displayTemprature(int y) {
  lcd.setCursor(0, y); 
  lcd.print("Temprature:");
  lcd.setCursor(12, y);     
  lcd.print(dht.temperature, 0);
  lcd.print(" C");
}

void displayClicks(int y) {
  lcd.setCursor(0, y);    
  lcd.print("Clicks:");
  lcd.setCursor(12, y);     
  
  String number = String(clicks);
  String string = String("    ").substring(number.length(), 4) + number;

  lcd.print(string);
}

void updateDisplay() {
  lcd.clear();

    switch (display_state) {
    case 0:
      displayClicks(0);
      displayTemprature(1);
      break;

    case 1:
      displayHumidity(0);
      displayClicks(1);
      break;

    case 2:
      displayTemprature(0);
      displayHumidity(1);
      break;
    }
}

//function that processes the finished message string so i can
//control the arduino based the message content
void handleMessage(String msg) {
  String cmd = msg;

  if (cmd.indexOf("status_led:") == 0) {
    cmd.replace("status_led:", "");

    int value = cmd.toInt();

    if (cmd.equals("toggle")) {
      status_led.toggle();
    } else if (cmd.equals("rgb")) {
      rgb = true;
    } else if (value > -1 && value < 7) {
      rgb = false;
      status_led.set_color((Color)value);
    }
  }
  
  status_led.update();
  
  if (cmd.indexOf("set_clicks:") == 0) {
    cmd.replace("set_clicks:", "");
    
    int value = cmd.toInt();
    
    if (value >= 0) {
      clicks = value;
    
      if (display_state != 2) {
       updateDisplay();
      }
    } 
  }

  Serial.println("[SERVER] -> " + msg);
}

void redButtonHandler() {
  red_button_click = true;
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

//the '!' tells the server that it needs to be tcp only
void connectToServer() {
  if (client.connect(host, 80)) {
      client.write('!');
      client.flush();
  }
}

void setup() {
  //blue button
  pinMode(di_red_button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(di_red_button), redButtonHandler, FALLING);
  
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
  status_led.set_active(false);
  status_led.set_color(Color::WHITE);
  status_led.update();
  
  Serial1.begin(115200);
  WiFi.init(&Serial1);


  //this sets that the connection cant timeout so the connection can stay open as long as possible
  client.setTimeout(0);

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

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  printMacAddress();
  connectToServer();
  updateDisplay();
}

void loop() {
  //if connection is lost i reconnect
  if (!client.connected()) {
    connectToServer();
  }

  //while there is data avalible it appends each byte to message until '\n' -> handleMessage -> reapeat until no more bytes left
  //i had a serialEvent for this but it gave alot of weird behavior so i dont use dalays and use cooldowns to execute and wait for things
   while (client.available()) {
    char c = client.read();
    if (c == '\n') {
      handleMessage(message);
      message = "";
    } else {
      message += c;
    }
  }

  if (red_button_click == true) {
    client.print("click\n");
    red_button_click = false;
    

  }


  if (rgb && (rgb_shift_cooldown == 0 || rgb_shift_cooldown <= millis() - 1000)) {
      status_led.shift();
      status_led.update();
      
      rgb_shift_cooldown = millis();
  }

  if (send_cooldown == 0 || send_cooldown <= millis() - 5000) {
    dht.read11(di_dht);

    client.print("data:");
    client.print(dht.temperature, 0);
    client.write(",");
    client.print(dht.humidity, 0);
    client.write("\n");

    send_cooldown = millis();
  }
  
  //make sure there is room for data
  client.flush();

  //update the lcd every 5 secs to show scrolling effect
  if (update_display_cooldown == 0 || update_display_cooldown <= millis() - 2500) {

    display_state = (display_state + 1) % 3;
    updateDisplay();

    update_display_cooldown = millis();
  }
}


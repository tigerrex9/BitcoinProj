#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>


const char* ssid     = "2020internet";
const char* password = "TaylorSt";

const char* host = "api.coindesk.com";

double priceHour;
double priceNow;
String line;

int leds = 12;
int ledPin = 14;
int rgb[3] = {0,0,0};

// Initiate NeoPixel as "pixels"
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(leds, ledPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Setting up pins
  Serial.println("Setting up RGB");
  pinMode(ledPin, OUTPUT);
  pixels.begin();
  pixels.setBrightness(25); //Brightness: 0 (off) - 255
  
  // Connecting to a WiFi network

  Serial.println();
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

int value = 0;

void loop() {
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/v2/bpi/currentprice.json";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  
  // Parse the json in the reply from the server
  StaticJsonDocument<1024> doc;

  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  double rate = doc["bpi"]["USD"]["rate_float"];

  Serial.print("Bitcoin price(USD):");
  Serial.print(rate);
  

  // Check loop value and store
  if(value == 0){
    priceNow = rate;
    priceHour = rate;
    rgb[0] = 100;
    rgb[1] = 100;
    rgb[2] = 100;
  }
  else if (value%10 != 0){
      // Store bitcoun price to priceNow
      priceNow = rate;
      
      // Check if priceNow > priceHour
      if (priceNow >= priceHour){
        rgb[0] = 0;
        rgb[1] = 255;
        rgb[2] = 0;
      }
      else{
        rgb[0] = 255;
        rgb[1] = 0;
        rgb[2] = 0;
      }
  }
  else{
    // Store bitcoin price to priceHour
    priceHour = rate;
    
    // Check if priceHour > priceNow

    // Here I want to put a function that takes the difference between the bitcoin values
    // Then take that value and if its positive shift a yellow in the green direction
    // and if its negative shift a yellow in the red direction porporionally.
    if (priceHour >= priceNow){
      rgb[0] = 0;
      rgb[1] = 255;
      rgb[2] = 0;
    }
    else{
      rgb[0] = 255;
      rgb[1] = 0;
      rgb[2] = 0;
    }
  }

  // sets neopixels
  Serial.println();
  Serial.println("setting neopixels");
  Serial.println("\n\n\n");
  for (int i = 0; i < 12; i++) {
    pixels.setPixelColor(i, pixels.Color(rgb[0], rgb[1], rgb[2]));
    pixels.show();
    delay(100);
  }
  ++value;
  delay(10000);
}

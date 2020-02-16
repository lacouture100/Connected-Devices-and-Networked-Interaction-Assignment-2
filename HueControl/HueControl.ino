/* HueBlink example for ArduinoHttpClient library
   uses Arduino_JSON library as well
   Uses ArduinoHttpClient library to control Philips Hue
   For more on Hue developer API see http://developer.meethue.com
  To control a light, the Hue expects a HTTP PUT request to:
  http://hue.hub.address/api/hueUserName/lights/lightNumber/state
  The body of the PUT request looks like this:
  {"on": true} or {"on":false}
  This example  shows how to concatenate Strings to assemble the
  PUT request and the body of the request, and how to use the
  Arduino_JSON library to assemble JSON to send the request.
  A typical light state JSON for the Hue system looks like this:
  {"on":true,"bri":254,"hue":0,"sat":0,"effect":"none",
  "xy":[0.4584,0.4100],"ct":156,"alert":"none",
  "colormode":"ct","reachable":true}
   modified 2 Feb 2020
   by Tom Igoe (tigoe)
*/
#include <Wire.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Arduino_JSON.h>
#include "arduino_secrets.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// make a wifi instance and a HttpClient instance:
WiFiClient wifi;
HttpClient httpClient = HttpClient(wifi, SECRET_HUBADDR);
// a JSON object to hold the light state:
JSONVar lightState;
int difference = 25;
int brightness = 0;
int hue = 0;
int sensorPin = A7;    // select the input pin for the potentiometer     // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
int mappedValue = 0;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
    while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println("SSD1306 setup...");
    delay(100);
  }
  
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SECRET_SSID);
    // Connect to WPA/WPA2 network:
    displayWrite("connecting:" + String(SECRET_SSID),0,0);
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  displayWrite("connected",0,0);
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network IP = ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}
void loop() {

  
  sensorValue = analogRead(sensorPin);
  mappedValue = map(sensorValue,0,1023,0,60000);
  Serial.println(mappedValue);
  lightState["hue"] = mappedValue;
  

  sendRequest(6, lightState);   // turn light on
    displayLightData("Light" + String(SECRET_HUBADDR),0,0,
                 "Hue:" + String(mappedValue),0,10,
                 "Bright:" + String(mappedValue),0,20);
            

}
void sendRequest(int light, JSONVar myState) {
  // make a String for the HTTP request path:
  String request = "/api/" + String(SECRET_HUBUSER);
  request += "/lights/";
  request += light;
  request += "/state/";
  String contentType = "application/json";
  // make a string for the JSON command:
  String body  = JSON.stringify(lightState);
  // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(request);
  Serial.print("JSON command to server: ");
  Serial.println(body);
  // make the PUT request to the hub:
  httpClient.put(request, contentType, body);
  // read the status code and body of the response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();
  Serial.print("Status code from server: ");
  Serial.println(statusCode);
  Serial.print("Server response: ");
  Serial.println(response);
  Serial.println();
}
void displayWrite(String message, int x, int y) {
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(message);
  display.display();
}
void displayLightData(String message1, int x1, int y1,String message2, int x2, int y2,String message3, int x3, int y3) {
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(x1, y1);
  display.println(message1);
    display.setCursor(x2, y2);
  display.println(message2);
    display.setCursor(x3, y3);
  display.println(message3);
  display.display();
}

/* Sketch made by Alvaro Lacouture based on the
   HueBlink example for ArduinoHttpClient library made by Tom Igoe 2020
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

// Wifi instance and an HttpClient instance:
WiFiClient wifi;
HttpClient httpClient = HttpClient(wifi, SECRET_HUBADDR);

// a JSON object to hold the light state:
JSONVar lightState;

int sensorHuePin = A3;    // Pin for the potentiometer that's controls Hue
int sensorHueValue = 0;  // variable to store the value coming from the sensor
int mappedHueValue = 0;

int sensorBriPin = A6;    // Pin for the potentiometer that's controls Brightness
int sensorBriValue = 0;  // variable to store the value coming from the sensor
int mappedBriValue = 0;

int sensorSatPin = A7;    // Pin for the potentiometer that's controls Brightness
int sensorSatValue = 0;  // variable to store the value coming from the sensor
int mappedSatValue = 0;

int ledPin = 2;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println("SSD1306 setup...");
    delay(100);

  }
  //Indicate to the user the device is on
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SECRET_SSID);

    // Connect to WPA/WPA2 network and display status to user:
    displayConnecting();
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  //Indicate the user is connnected
  displayWrite("WOOO! Connected!", 0, 0);

  IPAddress ip = WiFi.localIP();
  //Serial.println(ip);
}

void loop() {

  //Sensor value for the Hue
  sensorHueValue = analogRead(sensorHuePin);
  mappedHueValue = map(sensorHueValue, 0, 1023, 0, 60000);
  lightState["hue"] = mappedHueValue;

  //Sensor value for the Brightness
  sensorBriValue = analogRead(sensorBriPin);
  mappedBriValue = map(sensorBriValue, 0, 1023, 0, 255);
  lightState["bri"] =   mappedBriValue;

  //Sensor value for the Brightness
  sensorSatValue = analogRead(sensorSatPin);
  mappedSatValue = map(sensorSatValue, 0, 1023, 0, 255);
  lightState["sat"] =   mappedSatValue;

  // Send HTTP Request to the Hue Hub
  sendRequest(6, lightState);

  //Display the Light's data in the OLED screen.
  displayLightData("Light" + String(SECRET_HUBADDR),
                   "Hue:" + String(mappedHueValue),
                   "Bright:" + String(mappedBriValue),
                   "Saturation:" + String(mappedSatValue));
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

void displayConnecting() {
  display.clearDisplay();
  display.setTextSize(1); // Draw 1X-scale text
  display.setTextColor(WHITE);
  int posY = 0;
  int posX = 0;

  display.setCursor(posX, posY);
  display.println("Connecting...");
}

void displayLightData(String message1, String message2,  String message3, String message4 ) {
  display.clearDisplay();
  display.setTextSize(1); // Draw 1X-scale text
  display.setTextColor(WHITE);
  int posY = 0;
  int posX = 0;

  display.setCursor(posX, posY);
  display.println(message1);
  posX += 10;

  display.setCursor(posX, posY);
  display.println(message2);
  posX += 10;

  display.setCursor(posX, posY);
  display.println(message3);
  posX += 10;

  display.setCursor(posX, posY);
  display.println(message4);
  posX += 10;
  display.display();
}

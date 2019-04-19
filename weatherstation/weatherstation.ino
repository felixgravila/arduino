#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "";
const char* password = "";

Adafruit_BMP085 bmp;

ESP8266WebServer server(80);

void handleRoot() {
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonBuffer<capacity> jb;

  JsonObject& obj = jb.createObject();
  obj["temperature"] = bmp.readTemperature();
  obj["pressure"] = bmp.readPressure();
  obj["light"] = analogRead(A0);

  char output[128];
  obj.printTo(output);

  server.send(200, "text/plain", output);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/json", message);
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  IPAddress ip(192, 168, 1, 200);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1) {}
  }

  server.on("/", handleRoot);

  server.on("/weather", []() {
    String str = "";
    str = str + "Temperature = " + bmp.readTemperature() + " *C";
    str = str + "\nPressure = " + bmp.readPressure() + " Pa";
    str = str + "\nLight = " + analogRead(A0);
    server.send(200, "text/plain", str);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

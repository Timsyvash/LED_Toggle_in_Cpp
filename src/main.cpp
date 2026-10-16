#include <Arduino.h>
#include <ESP8266WebServer.h>

const char *ssid = "";
const char *password = "";

unsigned long previousMillis = 0;
const long interval = 500; // Інтервал для крапок (мс)

ESP8266WebServer server(80);

short LED_toggle_pin = D4;

void OnLED()
{
  digitalWrite(LED_toggle_pin, LOW);
  server.send(200, "text/plain", "The LED is on.");
}

void OffLED()
{
  digitalWrite(LED_toggle_pin, HIGH);
  server.send(200, "text/plain", "The LED is not lit.");
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_toggle_pin, OUTPUT);
  digitalWrite(LED_toggle_pin, LOW);

  Serial.println();
  Serial.printf("Connecting to %s ", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Неблокуюче очікування підключення через millis()
  while (WiFi.status() != WL_CONNECTED)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      Serial.print(".");
    }
    yield(); // Обов'язково для запобігання WDT reset в ESP8266
  }

  // Очікування дійсної IP-адреси через millis()
  previousMillis = millis();
  while (WiFi.localIP().toString() == "0.0.0.0" || WiFi.localIP().toString() == "192.168.0.0")
  {
    if (millis() - previousMillis >= 100)
    {
      previousMillis = millis();
    }
    yield();
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/on", HTTP_GET, OnLED);
  server.on("/off", HTTP_GET, OffLED);

  server.begin();
  Serial.println("Web server started!");
}

void loop()
{
  server.handleClient();
}
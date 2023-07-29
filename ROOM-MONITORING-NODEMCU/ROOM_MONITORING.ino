#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11
#include <ESP8266WiFi.h>

const char *ssid = "YOUR_SSID_HERE";
const char *password = "YOUR_PASSWROD_HERE";
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

int sensorVal = 0;

const int analogReadSensor = A0;






WiFiServer server(8080);
String header;

#define dht_dpin D1
DHT dht(dht_dpin, DHTTYPE);

void setup(void)
{
  dht.begin();

  Serial.begin(9600);
  Serial.print("Conecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  delay(700);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println(".");
  Serial.println("Connected To the Wifi");

  Serial.println("IP ADDRESS");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  float h = 0.0;   // Humidity level
  float t = 0.0;   // Temperature in Celsius
  float f = 0.0;
  int value = LOW;
  sensorVal = analogRead(analogReadSensor);

  int brightPrecent = map(sensorVal, 0, 1024, 0, 100);
  

  
  Serial.println(brightPrecent);

  WiFiClient client = server.available();

  if (client)
  {
    Serial.println("New Client."); // print a message out in the serial port

    String currentLine = "";

    currentTime = millis();
    previousTime = currentTime;

    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {
      currentTime = millis();

      if (client.available())
      {
        char c = client.read(); // read a byte then
        Serial.write(c);
        header += c;

        if (c == '\n')
        { // If a newline is received, process the request
          // Send the HTTP response headers
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          if (header.indexOf("/Up=ON") != -1)  {
              h = dht.readHumidity();    //Read humidity level
              t = dht.readTemperature(); //Read temperature in celcius
              f = (h * 1.8) + 32;        //Temperature converted to Fahrenheit
              value = HIGH;
            }
          if (currentLine.length() == 0)
          {
            // Send   the HTML content for the main page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<body><h1>ESP8266 Web Server</h1>");
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); 
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<h1 align=center>Temperature & Humidity Monitoring with DHT11</h1><br><br>");

            client.println("Brightness in precent = ");
            client.println(brightPrecent);
            client.println("<br>");
            client.print("Temperature in Celsius =");
            client.println(t);
            client.println("<br>");
            client.print("Temperature in Fahrenheit =");
            client.println(f);
            client.println("<br>");
            client.print("Humidity =");
            client.println(h);
            client.print(" %");
            client.println("<br>");
            client.println();

            client.println("<a href=\"/Up=ON\"\"><button>Update Temperature & Humidity</button></a><br />");  

            client.println("</body></html>");
          }
          else
          {
            // Clear the currentLine if it's not empty
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          // If the received character is not a carriage return, add it to the currentLine
          currentLine += c;
        }
      }
    } // end of the while loop while the client is connected

    // You have reached the end of the client connection

    header = "";
    // F the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

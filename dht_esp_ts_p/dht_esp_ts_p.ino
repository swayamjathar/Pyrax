#include <DHT.h>

#include <ESP8266WiFi.h>


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif



String apiKey = "AR287VRH36HPRP5J;

const char *ssid =  "Kashish";
const char *pass =  "06112001";
const char* server = "api.thingspeak.com";

#define DHTPIN D3
#define BUZZERPIN D4


#define LED_PIN    D5

#define LED_COUNT 60

DHT dht(DHTPIN, DHT11);

WiFiClient client;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(115200);
  delay(10);
  dht.begin();

  strip.begin();
  strip.show();
  strip.setBrightness(50);

  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop()
{

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (client.connect(server, 80))  //184.106.153.149
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");

    if (dht.readTemperature() > 20 )
    {
      digitalWrite(BUZZERPIN, HIGH);
      theaterChase(strip.Color(127,   0,   0), 50);
    }
    else {
      digitalWrite(BUZZERPIN, LOW);
    }

  }
  client.stop();

  Serial.println("Waiting...");
  delay(1000);
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {
    for(int b=0; b<3; b++) {
      strip.clear();
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color);
      }
      strip.show();
      delay(wait);
    }
  }
}

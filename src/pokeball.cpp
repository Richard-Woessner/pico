#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Use the symbolic names for the XIAO ESP32-C6
const int SDA_PIN = SDA;
const int SCL_PIN = SCL;

const char *WIFI_SSID = "##########";     // Example SSID for WiFi Direct
const char *WIFI_PASSWORD = "############"; // No password for WiFi Direct

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
RTC_DS3231 rtc;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -18000;   // EST is UTC-5 hours = -5 * 3600 = -18000 seconds
const int daylightOffset_sec = 3600; // EDT adds 1 hour during daylight saving time

bool wifiConnected = false;

int x = SCREEN_WIDTH / 2;
int y = SCREEN_HEIGHT / 2;
int dx = 1;
int dy = 1;
int radius = 15; // try 1 for a single pixel

const uint8_t PROGMEM ball_bitmap[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x07, 0x9e, 0x00, 0x0c, 0x03, 0x00, 0x18,
    0x01, 0x80, 0x30, 0x00, 0xc0, 0x30, 0xf0, 0xc0, 0x31, 0x98, 0xc0, 0x3f, 0x0f, 0xc0, 0x3f, 0x0f,
    0xc0, 0x31, 0x98, 0xc0, 0x30, 0xf0, 0xc0, 0x30, 0x00, 0xc0, 0x18, 0x01, 0x80, 0x0c, 0x03, 0x00,
    0x07, 0x9e, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN); // SDA, SCL, frequency
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 allocation failed");
    while (true)
    {
      delay(100);
    }
  }

  display.clearDisplay();
  display.display();

  // Initialize RTC
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
  }
  else
  {
    Serial.println("RTC found!");
  }

  // Connect to WiFi Direct network (no password)
  WiFi.mode(WIFI_STA);

  // Scan for networks starting with "DIRECT-21-SM-G991"
  Serial.println("Scanning for WiFi Direct network...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  delay(1000);

  // Try to connect for 10 seconds
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected!");
    wifiConnected = true;

    // Configure time with NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(2000); // Wait for time sync

    // Set RTC from NTP time
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
      rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
      Serial.println("RTC set from NTP!");
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi connected!");
    display.display();
    delay(1000);
  }
  else
  {
    Serial.println("\nWiFi connection failed!");
    wifiConnected = false;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi failed!");
    display.display();
    delay(1000);
  }
}

void loop()
{
  // Update position
  x += dx;
  y += dy;

  // Bounce on edges
  if (x - radius <= 0 || x + radius >= SCREEN_WIDTH)
  {
    dx = -dx;
  }
  if (y - radius <= 0 || y + radius >= SCREEN_HEIGHT)
  {
    dy = -dy;
  }

  display.clearDisplay();

  // Draw the ball
  display.drawBitmap(x, y, ball_bitmap, 20, 20, SSD1306_WHITE);

  // Display time or error message
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, SCREEN_HEIGHT / 2 - 20);

  // Get time from RTC
  DateTime now = rtc.now();
  if (now.year() >= 2020) // Valid time check
  {
    char timeStr[11];
    sprintf(timeStr, "%02d:%02d", now.twelveHour(), now.minute());
    display.println(timeStr);
  }
  else
  {
    display.println("ERR");
  }

  display.display();
  delay(10); // smaller = faster animation
}

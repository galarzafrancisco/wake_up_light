// Includes
#include <WiFi.h>
#include "FastLED.h"

// Defines
#define LED_PIN 12
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    60

CRGB leds[NUM_LEDS];
// green - red - blue

// WiFi parameters
const char* ssid     = "imnotgonnatellyou";
const char* password = "seriously";

// API parameters
const char* api_host = "10.0.0.99";
const char* api_url = "/api/wake_up_light";
const int httpPort = 3333;

// Colours
CRGB dark = CRGB(0, 0, 0);
CRGB light = CRGB(255, 156, 60);


// Setup
void setup()
{
  // Init the LED strip
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  apply_colour(dark);

  // Init the serial port
  Serial.begin(115200);
  delay(10000);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Connect to a WiFi network
  connect_to_wifi();
}



// Main loop
void loop()
{

    Serial.print("connecting to ");
    Serial.println(api_host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    if (!client.connect(api_host, httpPort)) {
        Serial.println("connection failed");
        delay(2000);
        return;
    }

    // We now create a URI for the request
    String url = api_url;

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + api_url + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 15000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Identify the line of the HTTP response where the value is
    bool is_empty_line = false;
    while(client.available()) {
        String line = client.readStringUntil('\r');
        if (is_empty_line) {
          int value = line.toInt();
          Serial.println(value);
          analyse_response(value);
        }
        if (line == "\n") {
          is_empty_line = true;
        } else {
          is_empty_line = false;
        }
    }
}


// Analyses the response from the API
void analyse_response(int response) {
  // if the event is more than 10 minutes away, go to sleep for 80% of the duration
  if (response > 600000) {
    int sleep_time = response * 0.8;
    Serial.print("Going to sleep for ");
    Serial.print(sleep_time);
    Serial.println(" milliseconds.");
    take_a_nap(sleep_time * 1000);
  } else {
    if (response > 0) {
      unsigned long countdown = millis() + response;
      while (countdown > millis()) {
        int wait_time = _max((countdown - millis()) * 0.7, 500);
        Serial.print("Waiting ");
        Serial.print(wait_time);
        Serial.println(" milliseconds...");
        delay(wait_time);
      }
      Serial.println("Event triggered!");
      handle_event();
    } else {
      Serial.println("Got a negative time. I'm gonna take a 30 minutes nap.");
      take_a_nap(30 * 60 * 1000 * 1000);
    }
  }
}


// This will get executed when the calendar says so
void handle_event() {
  Serial.println("Starting sunrise simulation...");
  sunrise(1.0);
  Serial.println("Holding max brightness for 30 minutes...");
  for (int minutes = 0; minutes < 30; minutes++) {
    delay(60000);
  }
  Serial.println("Dimming off the light...");
  ramp_colour(light, dark, 10);
}


// Animates the sunrise
void sunrise(float speed_factor) {
  ramp_colour(dark, light, speed_factor * 30);
}


// Animation to shift from one colour to another in X amount of time
void ramp_colour(CRGB colour_0, CRGB colour_1, float duration_minutes) {
  // Define the duration of the period in milliseconds (waiting time before updating the colour)
  int period_duration = 500;

  // Convert the ramp duration from minutes to milliseconds
  int duration = (int)(duration_minutes * 60 * 1000);

  // Set time variables
  int t = 0; // time relative to the start
  unsigned long t_0 = millis(); // absolute time at the start

  // Set the initial colour
  apply_colour(colour_0);

  // Loop until the animation is complete
  while (t<duration) {
    // Compute the new colour
    CRGB new_colour = interpolate_colours(colour_0, colour_1, duration, t);

    // Wait until it's time to update the colour again
    while (t > (millis() - t_0)) {
      delay(10);
    }
    t += period_duration;
    apply_colour(new_colour);

  }
  apply_colour(colour_1);
 }


// This function returns a colour computed as an interpolation between 2 other colours
CRGB interpolate_colours(CRGB colour_0, CRGB colour_1, int duration, int t) {
  CRGB colour_output;
  if (t > duration) {
    colour_output = colour_1;
  } else {
    float time_coefficient = float(t) / float(duration);
    for (int rgb_index = 0; rgb_index < 3; rgb_index++) {
      float colour_difference = (colour_1[rgb_index] - colour_0[rgb_index]);
      float colour_change = colour_difference * time_coefficient;
      colour_output[rgb_index] = colour_0[rgb_index] + (int)colour_change;
      colour_output[rgb_index] = _max(_min(colour_output[rgb_index], 255), 0);
    }
  }
  return colour_output;
}


// This function applies a colour to every single led in the strip
void apply_colour(CRGB colour) {
  for(int i=0; i < NUM_LEDS; i++) {
    leds[i] = colour;
  }
  FastLED.show();
}


// Simple function to connect to a WiFi network.
void connect_to_wifi() {
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


// Deep sleep for a period of time (in milliseconds)
void take_a_nap(unsigned long sleep_time) {
  Serial.println("Going to sleep.");
  esp_sleep_enable_timer_wakeup(sleep_time);
  delay(2000); // give time for the serial port to flush all the data.
  esp_deep_sleep_start();
}


// Prints the reason why the microcontroller woke up from deep sleep
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}
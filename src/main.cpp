/*
  This project is a simple example of how to use the ESP32-CAM with the ESPAsyncWebServer library to create a simple web server that serves a still image and an MJPEG video stream. The project uses the esp32cam library to interface with the camera and the Network class to handle WiFi and mDNS setup.
  The hardware used is the M5Stack Unit CamS3, which is an ESP32-CAM module with a camera. The hardware documentation can be found at https://docs.m5stack.com/en/unit/Unit-CamS3.

  To do next: add saving to micro-SD card, add microphone support,
  Onboard peripherals not currently used: 
    - micro-SD, microphone, blue indicator LED.
*/

//hardware used: M5-Stack Unit CamS3
//hardware documentation: https://docs.m5stack.com/en/unit/Unit-CamS3
//library used:  https://github.com/yoursunny/esp32cam
//project setup board: AI Thinker ESP32-CAM

#include <Arduino.h>
#include <esp32cam.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "config.h"
#include "Network.h"
#include "esp32cam-asyncweb.h"

Network* network;
AsyncWebServer server(80);

void info(){
  Serial.println("HTTP server started");
  Serial.println("Available endpoints:");

  const char* paths[] = {"/image", "/stream"};
  const char* labels[] = {"Still Image", "Video Stream"};

  for (int i = 0; i < 2; i++) {
    Serial.printf("http://%s%s \t---\t %s\n", network->getIP().c_str(), paths[i], labels[i]);
    Serial.printf("http://%s%s \t\t---\t %s\n", MDNS_URL, paths[i], labels[i]);
  }
  Serial.println("--------------------");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Setting up ...");

  // --- WIFI CONNECTION ---
  network = new Network(WIFI_SSID, WIFI_PASS, MDNS_URL);
  network->connect();

  // --- MDNS SETUP ---
  try {
    Serial.println("Setting up mDNS");
    network->setupMDNS();
  } catch (const char* error) {
    Serial.println(error);
  }
  Serial.printf("Done setting up mDNS: %s \n", MDNS_URL);
  Serial.println("--------------------");

  // --- CAMERA INITIALIZATION ---
  auto res = esp32cam::Resolution::find(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  esp32cam::Config cfg;
  cfg.setPins(esp32cam::pins::UNIT_CAM_S3);
  cfg.setResolution(res);
  cfg.setJpeg(80); // Set JPEG quality (adjust if needed)
  esp32cam::Camera.begin(cfg);

  // --- ENDPOINT: /image (Static Still Image) ---
  server.on("/image", HTTP_GET, [](AsyncWebServerRequest *request) {
    auto img = esp32cam::capture();
    if (img == nullptr) {
      request->send(500, "text/plain", "Capture failed");
      return;
    }
    AsyncResponseStream *response = request->beginResponseStream("image/jpeg");
    img->writeTo(*response);
    request->send(response);
  });

  // --- ENDPOINT: /stream (MJPEG Video Stream) ---
  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *request) {
    // This helper creates an MJPEG response using a FreeRTOS task
    esp32cam::asyncweb::handleMjpeg(request);
  });

  // --- START THE SERVER ---
  server.begin();
  info();
}

void loop() {
}

/*
  The esp32cam does not have configuration for M5_CamS3 (so I created one myslef from schematics and reference configuration) - lib/esp32cam/internal/pins.hpp
  constexpr Pins UNIT_CAM_S3{
    D0: 6,
    D1: 15,
    D2: 16,
    D3: 7,
    D4: 5,
    D5: 10,
    D6: 4,
    D7: 13,
    XCLK: 11,
    PCLK: 12,
    VSYNC: 42,
    HREF: 18,
    SDA: 17,
    SCL: 41,
    RESET: 21,
    PWDN: -1,
  };
*/
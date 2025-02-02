#ifndef NETWORK_H
#define NETWORK_H

#include <ESPmDNS.h>
#include <WiFi.h>

struct NetworkStatus {
  int id;
  String text;
  uint32_t color;
};

const NetworkStatus NET_CONNECTED = {0, "CONNECTED", 0x07E0};        // GREEN
const NetworkStatus NET_DISCONNECTED = {1, "DISCONNECTED", 0xF800};  // RED
const NetworkStatus NET_CONNECTING = {2, "CONNECTING", 0xFDA0};      // ORANGE

class Network {
 private:
  String ssid;
  String pass;
  String mdns_url = "esp32";
  NetworkStatus status;

 public:
  Network(String ssid, String pass);
  Network(String ssid, String pass, String mdns_url);
  void connect();
  void setupMDNS();
  void setupMDNS(String mdns_url);
  void setupEvents();

  String getSSID();
  String getIP();
  String getMacAddress();
  bool isConnected();
  NetworkStatus getStatus();
  void setStatus(NetworkStatus status);

  void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
  void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
  void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

  ~Network();
};

#endif  // NETWORK_H
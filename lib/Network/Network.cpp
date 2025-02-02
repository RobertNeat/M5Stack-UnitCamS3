#include <Network.h>

Network::Network(String ssid, String pass, String mdns_url) {
  this->ssid = ssid;
  this->pass = pass;
  this->mdns_url = mdns_url;
}

Network::Network(String ssid, String pass) {
  this->ssid = ssid;
  this->pass = pass;
}

void Network::connect() {
  Serial.print("Connecting to ");
  Serial.print(ssid);

  this->setupEvents();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
}

String Network::getIP() {
  return WiFi.localIP().toString();
}

String Network::getSSID() {
  return WiFi.SSID();
}

String Network::getMacAddress() {
  return WiFi.macAddress();
}

bool Network::isConnected() {
  return WiFi.isConnected();
}

NetworkStatus Network::getStatus() {
  return this->status;
}

void Network::setStatus(NetworkStatus status) {
  this->status = status;
}

void Network::setupMDNS() {
  const int MAX_MDNS_ATTEMPTS = 5;
  for (int attempt = 1; attempt <= MAX_MDNS_ATTEMPTS; attempt++) {
    if (MDNS.begin(this->mdns_url)) {
      return;
    }
    delay(1000);
  }

  throw "Failed to set up MDNS after 5 attempts";
}

void Network::setupMDNS(String mdns_url) {
  this->mdns_url = mdns_url;
  setupMDNS();
}

void Network::setupEvents() {
  WiFi.disconnect(true);
  delay(1000);
  WiFi.onEvent(
    [this](WiFiEvent_t event, WiFiEventInfo_t info) { this->WiFiStationConnected(event, info); },
    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) { this->WiFiGotIP(event, info); },
               WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.onEvent(
    [this](WiFiEvent_t event, WiFiEventInfo_t info) { this->WiFiStationDisconnected(event, info); },
    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

void Network::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  this->setStatus(NET_CONNECTED);
  Serial.println("\nConnected to AP successfully!");
}

void Network::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  this->setStatus(NET_CONNECTED);
  Serial.println("WiFi connected. Obtained IP.");
  Serial.println("Local IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("--------------------");
}

void Network::WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  this->setStatus(NET_DISCONNECTED);
  Serial.println("\nDisconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.print("Trying to Reconnect");
  WiFi.begin(ssid, pass);
  this->setStatus(NET_CONNECTING);
  Serial.println("--------------------");
}

Network::~Network() {}
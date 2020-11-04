#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#define LED_BUILTIN 2

const byte PuertoDNS = 53;
const byte PuertoHTTP = 80;

struct ArchivoConfiguracion {
  char* WIFISSID = "PS4-672_by_zerofo";
  char* WIFIPass = "";
  IPAddress IP = IPAddress(9, 9, 9, 9);
  IPAddress Subnet = IPAddress(255, 255, 255, 0);
};

ArchivoConfiguracion Configuracion;
ESP8266WebServer WebServer(PuertoHTTP);
DNSServer DNS;

void ConfigurarWIFIy() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(Configuracion.IP, Configuracion.IP, Configuracion.Subnet);
  WiFi.softAP(Configuracion.WIFISSID);
}

void setup() {
  SPIFFS.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  ConfigurarWIFIy();
  DNS.setTTL(300);
  DNS.setErrorReplyCode(DNSReplyCode::ServerFailure);
  DNS.start(PuertoDNS, "*", Configuracion.IP);
  WebServer.onNotFound([]() {
    if (!ManejarArchivo(WebServer.uri()))
      WebServer.sendHeader("Location", String("/"), true);
    WebServer.send ( 302, "text/plain", "");
  });
  WebServer.begin();
}

String obtenerTipo(String filename) {
  if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".bin")) return "application/octet-stream";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  else if (filename.endsWith(".manifest")) return "text/cache-manifest";
  return "text/plain";
}

bool ManejarArchivo(String path) {
  if (path.endsWith("/")) path += "index.html";
  String mimeType = obtenerTipo(path);
  String pathComprimido = path + ".gz";
  if (SPIFFS.exists(pathComprimido) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathComprimido)) path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = WebServer.streamFile(file, mimeType);
    file.close();
    return true;
  }
  return false;
}
int led=0;
int detect;
void loop() {
  detect = WebServer.client();
  if (detect!=led){digitalWrite(LED_BUILTIN, led);led = detect;}
  
  WebServer.handleClient();
  DNS.processNextRequest();
}

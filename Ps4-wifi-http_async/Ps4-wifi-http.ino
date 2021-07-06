#include <SD.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#define LED_BUILTIN 2
const byte PuertoDNS = 53;
const byte PuertoHTTP = 80;

bool hasSD=false;
const int CS = D8; //SD Pinout:  D5 = CLK , D6 = MISO , D7 = MOSI , D8 = CS


struct ArchivoConfiguracion {
  char* WIFISSID = "demo";
  char* WIFIPass = "";
  IPAddress IP = IPAddress(9, 9, 9, 9);
  IPAddress Subnet = IPAddress(255, 0, 0, 0);
};

ArchivoConfiguracion Configuracion;
AsyncWebServer WebServer(PuertoHTTP);
DNSServer DNS;

void ConfigurarWIFIy() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(Configuracion.IP, Configuracion.IP, Configuracion.Subnet);
  if(Configuracion.WIFIPass == "")
  WiFi.softAP(Configuracion.WIFISSID);
  else
  WiFi.softAP(Configuracion.WIFISSID,Configuracion.WIFIPass);
}
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(100);
  if (SD.begin(CS))
  hasSD = true;
  else{
    hasSD = false;
    LittleFS.begin();
  }

  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);

  ConfigurarWIFIy();
  delay(100);

  DNS.setTTL(300);
  DNS.setErrorReplyCode(DNSReplyCode::ServerFailure);
  DNS.start(PuertoDNS, "*", Configuracion.IP);
  //Serial.println(" init. ");
  delay(100);


/*  WebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, path, mimeType);
    request->send(response);
    });*/
  WebServer.onNotFound([](AsyncWebServerRequest *request){
    //Serial.println(" in? ");
    if (!ManejarArchivo(request)){
    //Serial.println(" not ");
      //request->
    request->send(204);
    }
    });
  WebServer.begin();
}

String obtenerTipo(String filename) {
  if (filename.endsWith(".html")||filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith("cache")||filename.endsWith(".manifest")) return "text/cache-manifest";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".bin")) return "application/octet-stream";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  else if (filename.endsWith(".svg")) return "image/svg+xml"; 
  else if (filename.endsWith(".ttf")) return "application/x-font-ttf";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  return "text/plain";
}


bool ManejarArchivo(AsyncWebServerRequest *request) {
  String path = request->url();
  if ((path.indexOf('/ps4/')>1) && (path.indexOf("/document/")>1))
     path = path.substring(16,-1);  
  if (path.endsWith("/") ) path += "index.html";
  String mimeType = obtenerTipo(path);
  String pathComprimido = path + ".gz";

  //Serial.println(path+ " setge 1 ");
  if(hasSD){
    //Serial.println(path+ " setge 2 ");
    if (SD.exists(pathComprimido) || SD.exists(path)) {
    if (SD.exists(pathComprimido)) path += ".gz";
    fs::File rdfile = SD.open(path,"r");
    AsyncWebServerResponse* response = request->beginResponse(rdfile, path, mimeType);
    if (SD.exists(pathComprimido)) 
     response->addHeader("Content-Encoding", "gzip"); // --> uncomment if your file is GZIPPED 
    request->send(response);
    return true;
    }
  }
   else{
    if (LittleFS.exists(pathComprimido) || LittleFS.exists(path)) {
    //Serial.println(path+ " setge 2 ");

    AsyncWebServerResponse* response = request->beginResponse(LittleFS, path, mimeType);
    if (LittleFS.exists(pathComprimido)) 
     response->addHeader("Content-Encoding", "gzip"); // --> uncomment if your file is GZIPPED 
    request->send(response);
    return true;
    }
  }
  return false;
}

void loop() {
  delay(0);
  
  DNS.processNextRequest();
}

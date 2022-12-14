#include <SD.h>
#include <WiFi.h>
#include "exfathax.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
// #include <LittleFS.h>
#include <FS.h>
#include "USB.h"
#include <USBMSC.h>
#define LED_BUILTIN 2
const byte PuertoDNS = 53;
const byte PuertoHTTP = 80;
USBMSC dev;

bool hasSD=false;
String inPage = "/index.html";


struct ArchivoConfiguracion {
  char* WIFISSID = "Ps4_zerofo_esp32";
  char* WIFIPass = "";
  IPAddress IP = IPAddress(9, 9, 9, 9);
  IPAddress Subnet = IPAddress(255, 255, 255, 0);
};
long enTime = 0;
boolean hasEnabled = false;
ArchivoConfiguracion Configuracion;
AsyncWebServer WebServer(PuertoHTTP);
DNSServer DNS;

void ConfigurarWIFIy() {
  WiFi.mode(WIFI_AP);
  if(Configuracion.WIFIPass == "")
  WiFi.softAP(Configuracion.WIFISSID);
  else
  WiFi.softAP(Configuracion.WIFISSID,Configuracion.WIFIPass);
  WiFi.softAPConfig(Configuracion.IP, Configuracion.IP, Configuracion.Subnet);
  delay(200);
}

void turnoff_usb(){
  enTime = 0;
  hasEnabled = false;
  dev.end();
}
void restart_esp(){
  enTime = 0;
  hasEnabled = false;
  dev.end();
  ESP.restart();
}
static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  return true;
}
static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
  if (lba > 4){lba = 4;}
  memcpy(buffer, exfathax[lba] + offset, bufsize);
  return bufsize;
}
// static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
//   return bufsize;
// }

void turnon_usb(){
  dev.vendorID("USB");
  dev.productID("ESP32 Server");
  dev.productRevision("3.00");
  dev.onStartStop(onStartStop);
  dev.onRead(onRead);
  // dev.onWrite(onWrite);
  dev.mediaPresent(true);
  dev.begin(8192, 512);
  USB.begin();
  enTime = millis();
  hasEnabled = true;
}
void setup() {

  if (SD.begin(SS))
  hasSD = true;
  else{
    hasSD = false;
    LittleFS.begin();
  }
  ConfigurarWIFIy();
  WebServer.on("/", HTTP_ANY, [](AsyncWebServerRequest *request){
  String HostIP = request->host();
      if (!ManejarArchivo(request)){
          request->redirect("http://"+HostIP+inPage);
      }
      });
  WebServer.onNotFound([](AsyncWebServerRequest *request){
  String HostIP = request->host();
      if (!ManejarArchivo(request)){
          request->redirect("http://"+HostIP+inPage);
      }
      });

  WebServer.begin();
  DNS.setTTL(300);
  DNS.setErrorReplyCode(DNSReplyCode::ServerFailure);
  DNS.start(PuertoDNS, "*", Configuracion.IP);

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
  String HostIP = request->host();

  if(path.endsWith("turnoff_usb")){
    turnoff_usb();
    return true;
  }
  if(path.endsWith("turnon_usb")){
    turnon_usb();
    return true;
  }
  if(path.endsWith("restart_esp")){
    restart_esp();
    return true;
  }
    if (path.indexOf("/document/")>1 && path.indexOf("/ps4/")> 1){
            request->redirect("http://"+HostIP+inPage);
        return true;
    }
    if (path.indexOf("?smcid=")>0){
            request->redirect("http://"+HostIP+inPage);
        return true;
    }
    if (path.endsWith("/") ) path += "index.html";

    String mimeType = obtenerTipo(path);
    String pathComprimido = path + ".gz";
  if(hasSD){
    request->send(200, "text/html", " log SD\n" );

    if (SD.exists(pathComprimido) || SD.exists(path)) {
    if (SD.exists(pathComprimido)) path += ".gz";
    fs::File rdfile = SD.open(path,"r");
    AsyncWebServerResponse* response = request->beginResponse(rdfile, path, mimeType);
    if (SD.exists(pathComprimido)) 
     response->addHeader("Content-Encoding", "gzip"); // --> uncomment if your file is GZIPPED 
    request->send(response);
    rdfile.close();
    return true;
    }
  }
   else{
    if (LittleFS.exists(pathComprimido) || LittleFS.exists(path)) {
    if (LittleFS.exists(pathComprimido)) path += ".gz";
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, path, mimeType);
      if (LittleFS.exists(pathComprimido)) {
       response->addHeader("Content-Encoding", "gzip"); // --> uncomment if your file is GZIPPED 

      }
    request->send(response);
    return true;}
  }
  return true;
}

void loop() {
  if (hasEnabled && millis() >= (enTime + 18000))
   {
    turnoff_usb();
   } 
  delay(10);
  DNS.processNextRequest();
}

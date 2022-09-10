#include <SD.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#define LED_BUILTIN 2
#define D7pin 13
const byte PuertoDNS = 53;
const byte PuertoHTTP = 80;

bool hasSD=false;
const int CS = D8; //SD Pinout:    D5 = CLK , D6 = MISO , D7 = MOSI , D8 = CS
String inPage = "/index.html";

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
    WiFi.softAP(Configuracion.WIFISSID);
}
void turnoff_usb(){

  // digitalWrite(D6pin, LOW);
  // delay(500);
  digitalWrite(D7pin, LOW);
}
void turnon_usb(){
  digitalWrite(D7pin, HIGH);
  // delay(500);
  // digitalWrite(D6pin, HIGH);
}
void setup() {
    if (SD.begin(CS))
    hasSD = true;
    else{
        hasSD = false;
        LittleFS.begin();
    }
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 1);
    pinMode(D7pin, OUTPUT);
    // pinMode(D6pin, OUTPUT);
    digitalWrite(D7pin, LOW);
    // turnoff_usb();
    ConfigurarWIFIy();

    DNS.setTTL(300);
    DNS.setErrorReplyCode(DNSReplyCode::ServerFailure);
    DNS.start(PuertoDNS, "*", Configuracion.IP);


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
}

String obtenerTipo(String filename) {
    if (filename.endsWith(".html")||filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".cache")||filename.endsWith(".manifest")) return "text/cache-manifest";
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
    if (path.indexOf("/document/")>1 && path.indexOf("/ps4/")> 1){
            request->redirect("http://"+HostIP+inPage);
        return false;
    }
    if (path.indexOf("?smcid=")>0){
            request->redirect("http://"+HostIP+inPage);
        return false;
    }
    if (path.endsWith("/") ) path += "index.html";
    String mimeType = obtenerTipo(path);
    String pathComprimido = path + ".gz";
    if(hasSD){
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
        if (LittleFS.exists(pathComprimido)) 
         response->addHeader("Content-Encoding", "gzip"); // --> uncomment if your file is GZIPPED 
        request->send(response);
        return true;
        }
    }
    return false;
}

void loop() {
    delay(10);
    
    DNS.processNextRequest();
}

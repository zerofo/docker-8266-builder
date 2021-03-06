#include <SD.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#define LED_BUILTIN 2
const byte PuertoDNS = 53;
const byte PuertoHTTP = 80;
bool hasSD=0;


//#define SD_CS_PIN D8
const int CS = D8; //SD Pinout:  D5 = CLK , D6 = MISO , D7 = MOSI , D8 = CS
//sdfat::SdExFat SD_tf;
struct ArchivoConfiguracion {
  char* WIFISSID = "demo";
  char* WIFIPass = "";
  IPAddress IP = IPAddress(9, 9, 9, 9);
  IPAddress Subnet = IPAddress(255, 255, 0, 0);
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
  ConfigurarWIFIy();
  ///if (SD_tf.begin(SD_CS_PIN))
  if (SD.begin(CS))
  {hasSD = true;
  }else{
    hasSD = false;
    LittleFS.begin();
  }

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  DNS.setTTL(300);
  //DNS.setErrorReplyCode(DNSReplyCode::ServerFailure);
  DNS.start(PuertoDNS, "*", Configuracion.IP);
  WebServer.begin();
  WebServer.onNotFound([]() {
    if (!ManejarArchivo(WebServer.uri())){WebServer.sendHeader("Location", String("/"), true);
    WebServer.send(302, "text/plain","");}
  });
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
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  return "text/plain";
}

bool ManejarArchivo(String path) {
  if ((path.length()>16) && (path.substring(0,10)=="/document/"))
      path = path.substring(16,-1);
  
  if (path.endsWith("/") ) path += "index.html";
  String mimeType = obtenerTipo(path);
  String pathComprimido = path + ".gz";

  if(hasSD){
    Serial.println(path+ "  sd ");
    if (SD.exists(pathComprimido) || SD.exists(path)) {
    if (SD.exists(pathComprimido)) path += ".gz";
    fs::File rdfile = SD.open(path,"r");
    WebServer.streamFile(rdfile, mimeType);
  
    rdfile.close();
    return true;}else return false;
  }else{

    if (LittleFS.exists(pathComprimido) || LittleFS.exists(path)) {
    if (LittleFS.exists(pathComprimido)) path += ".gz";
    fs::File file = LittleFS.open(path, "r");
    WebServer.streamFile(file, mimeType);
    file.close();
    return true;
    }else return false;
  }
  return false;
}
int led=0;
int detect;
void loop() {
  DNS.processNextRequest();
  detect = WebServer.client();

  if (detect!=led){digitalWrite(LED_BUILTIN, led);led = detect;}
  
  WebServer.handleClient();
}

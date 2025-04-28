/*
  Irrigação Automática com ESP32, ThingsBoard e CallMeBot (WhatsApp)
  [Versão otimizada para enviar todos dados juntos no ThingsBoard]
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ==== CONFIGURAÇÃO Wi-Fi ====
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ==== CONFIGURAÇÃO ThingsBoard ====
const char* tbServer = "demo.thingsboard.io";
const char* tbToken  = "RUMrTQfRUIi8bDxcgqjb";  // seu Access Token

// ==== CONFIGURAÇÃO CallMeBot WhatsApp ====
const char* waApiUrl   = "https://api.callmebot.com/whatsapp.php";
const char* WA_PHONE   = "556194547890";
const char* WA_API_KEY = "4825915";

// ==== PINOS ESP32 ====
#define PINO_BOTAO       13

#define PINO_TEMP_GRUPO1 36
#define PINO_UMID_GRUPO1 39

#define PINO_TEMP_GRUPO2 34
#define PINO_UMID_GRUPO2 35

#define PINO_TEMP_GRUPO3 32
#define PINO_UMID_GRUPO3 33

#define PINO_IRRIG1      14
#define PINO_IRRIG2      27
#define PINO_IRRIG3      26

#define TEMPO_IRRIGACAO_MANUAL 10000  // 10 segundos

struct Grupo {
  const char* nome;
  int pinoIrrig, pinoTemp, pinoUmid;
  float tempMin, tempMax, umidMin;
};

Grupo grupos[] = {
  {"Grupo 1", PINO_IRRIG1, PINO_TEMP_GRUPO1, PINO_UMID_GRUPO1, 25, 30, 52},
  {"Grupo 2", PINO_IRRIG2, PINO_TEMP_GRUPO2, PINO_UMID_GRUPO2, 20, 28, 48},
  {"Grupo 3", PINO_IRRIG3, PINO_TEMP_GRUPO3, PINO_UMID_GRUPO3, 15, 22, 60}
};
const int numGrupos = sizeof(grupos) / sizeof(grupos[0]);

void conectarWiFi();
void reconectarWiFi();
void sendAllToThingsBoard();
void sendWhatsApp(const String& msg);
String urlencode(const String& str);
bool botaoPressionado();
float lerTemperatura(int pino);
float lerUmidade(int pino);
void controlarIrrigacao(Grupo& g, float t, float u);
void irrigacaoManual();

void setup() {
  Serial.begin(9600);
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  for (int i = 0; i < numGrupos; i++) {
    pinMode(grupos[i].pinoIrrig, OUTPUT);
    digitalWrite(grupos[i].pinoIrrig, LOW);
  }
  conectarWiFi();
}

void loop() {
  reconectarWiFi();

  if (botaoPressionado()) {
    Serial.println(">> Botao manual: irrigacao ON");
    irrigacaoManual();
  } else {
    StaticJsonDocument<512> doc;
    
    for (int i = 0; i < numGrupos; i++) {
      Grupo& g = grupos[i];
      float t = lerTemperatura(g.pinoTemp);
      float u = lerUmidade(g.pinoUmid);

      Serial.printf("\n[%s] T=%.1f°C U=%.1f%%\n", g.nome, t, u);
      controlarIrrigacao(g, t, u);

      String kT = String("temp_") + (i+1);
      String kU = String("umid_") + (i+1);
      doc[kT] = t;
      doc[kU] = u;
    }
    
    sendAllToThingsBoard(doc);
  }
  delay(10000);  // a cada 10 segundos
}

void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Wi-Fi conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConectado! IP: %s\n", WiFi.localIP().toString().c_str());
}

void reconectarWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWi-Fi perdido! Reconectando...");
    conectarWiFi();
  }
}

void sendAllToThingsBoard(const StaticJsonDocument<512>& doc) {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  String url = String("http://") + tbServer + "/api/v1/" + tbToken + "/telemetry";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  String body;
  serializeJson(doc, body);
  
  int code = http.POST(body);
  Serial.printf("TB envio → codigo %d\n", code);
  http.end();
}

void sendWhatsApp(const String& msg) {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  String fullUrl = String(waApiUrl)
    + "?phone=" + WA_PHONE
    + "&apikey=" + WA_API_KEY
    + "&text="   + urlencode(msg);
  http.begin(fullUrl);
  int code = http.GET();
  Serial.printf("WhatsApp API → codigo %d\n", code);
  http.end();
}

String urlencode(const String& str) {
  String enc;
  char buf[5];
  for (auto c: str) {
    if (isalnum(c) || c=='-'||c=='_'||c=='.'||c=='~') {
      enc += c;
    } else {
      sprintf(buf, "%%%02X", (uint8_t)c);
      enc += buf;
    }
  }
  return enc;
}

bool botaoPressionado() {
  return digitalRead(PINO_BOTAO) == LOW;
}

float lerTemperatura(int p) {
  int v = analogRead(p);
  float vol = v * (3.3f / 4095.0f);
  return (vol - 0.5f) * 100.0f;
}

float lerUmidade(int p) {
  int v = analogRead(p);
  return map(v, 0, 4095, 0, 100);
}

void controlarIrrigacao(Grupo& g, float t, float u) {
  bool irrig = (t > g.tempMin) || (u < g.umidMin);
  digitalWrite(g.pinoIrrig, irrig ? HIGH : LOW);
  if (irrig) {
    Serial.println("! Irrigando (condicoes fora do ideal)");
    String m = String(g.nome) + ": IRRIGACAO ON (T=" + String(t,1)
               + "°C U=" + String(u,1) + "%)";
    sendWhatsApp(m);
  } else {
    Serial.println("OK: sem irrigacao");
  }
}

void irrigacaoManual() {
  for (auto& g: grupos) digitalWrite(g.pinoIrrig, HIGH);
  delay(TEMPO_IRRIGACAO_MANUAL);
  for (auto& g: grupos) digitalWrite(g.pinoIrrig, LOW);
  Serial.println(">> Irrigação manual finalizada");
}

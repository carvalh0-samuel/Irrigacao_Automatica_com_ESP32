// ==== DEFINIÇÕES ====
#define PINO_BOTAO 7
#define TEMPO_IRRIGACAO_MANUAL 15000  // 15 segundos

// Pinos dos sensores de temperatura e umidade
#define PINO_TEMP_GRUPO1 A0
#define PINO_UMID_GRUPO1 A1

#define PINO_TEMP_GRUPO2 A2
#define PINO_UMID_GRUPO2 A3

#define PINO_TEMP_GRUPO3 A4
#define PINO_UMID_GRUPO3 A5

// ==== ESTRUTURA DE DADOS ====
struct Grupo {
  const char* nome;
  int pinoIrrigacao;
  int pinoTemp;
  int pinoUmid;
  float tempMin;
  float tempMax;
  float umidadeMin;
};

// ==== CONFIGURAÇÃO DOS GRUPOS ====
Grupo grupos[] = {
  {"Grupo 1 (Capim-acu + Amendoim)", 9, PINO_TEMP_GRUPO1, PINO_UMID_GRUPO1, 25, 30, 52},
  {"Grupo 2 (Mandioca + Feijao)",    10, PINO_TEMP_GRUPO2, PINO_UMID_GRUPO2, 20, 28, 48},
  {"Grupo 3 (Batata)",               11, PINO_TEMP_GRUPO3, PINO_UMID_GRUPO3, 15, 22, 60}
};

const int numGrupos = sizeof(grupos) / sizeof(grupos[0]);

// ==== FUNÇÕES ====
void controlarIrrigacao(Grupo grupo, float temp, float umid);
bool botaoPressionado();
float lerTemperatura(int pinoTemp);
float lerUmidade(int pinoUmid);
void irrigacaoManual();

void setup() {
  Serial.begin(9600);

  // Configura os pinos de irrigação
  for (int i = 0; i < numGrupos; i++) {
    pinMode(grupos[i].pinoIrrigacao, OUTPUT);
    digitalWrite(grupos[i].pinoIrrigacao, LOW);
  }

  // Configura o botão com resistor pull-up interno
  pinMode(PINO_BOTAO, INPUT_PULLUP);

  Serial.println(" Sistema de irrigacao iniciado. Aguardando leitura de sensores...");
}

void loop() {
  if (botaoPressionado()) {
    Serial.println(" Botao pressionado! Iniciando irrigacao manual de todos os grupos...");
    irrigacaoManual();
  } else {
    for (int i = 0; i < numGrupos; i++) {
      float temperatura = lerTemperatura(grupos[i].pinoTemp);
      float umidade = lerUmidade(grupos[i].pinoUmid);

      Serial.println("========================================");
      Serial.println(grupos[i].nome);
      Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");
      Serial.print("Umidade: "); Serial.print(umidade); Serial.println(" %");

      controlarIrrigacao(grupos[i], temperatura, umidade);
    }
  }

  delay(13000); // Aguarda 13 segundos para próxima leitura
}

// ==== DETECÇÃO DO BOTÃO ====
bool botaoPressionado() {
  // Com INPUT_PULLUP, botão pressionado = LOW
  return digitalRead(PINO_BOTAO) == LOW;
}

// ==== LEITURA DE SENSORES ====
float lerTemperatura(int pinoTemp) {
  int leitura = analogRead(pinoTemp);
  float tensao = leitura * (5.0 / 1023.0);
  return (tensao - 0.5) * 100.0;  // TMP36
}

float lerUmidade(int pinoUmid) {
  int leitura = analogRead(pinoUmid);
  return map(leitura, 0, 1023, 0, 100);  // 0% seco, 100% molhado
}

// ==== LÓGICA DE IRRIGAÇÃO AUTOMÁTICA ====
void controlarIrrigacao(Grupo grupo, float temp, float umid) {
  bool precisaIrrigar = (temp > grupo.tempMin) || (umid < grupo.umidadeMin);

  digitalWrite(grupo.pinoIrrigacao, precisaIrrigar ? HIGH : LOW);

  if (precisaIrrigar) {
    Serial.println(" Condicoes fora do ideal. Iniciando irrigacao.");
    Serial.print("Criterio: ");
    if (temp > grupo.tempMin) Serial.print("Temperatura acima do minimo. ");
    if (umid < grupo.umidadeMin) Serial.print("Umidade abaixo do ideal.");
    Serial.println();
  } else {
    Serial.println(" Condicoes adequadas. Irrigacao nao necessaria.");
  }
}

// ==== IRRIGAÇÃO MANUAL ====
void irrigacaoManual() {
  for (int i = 0; i < numGrupos; i++) {
    digitalWrite(grupos[i].pinoIrrigacao, HIGH);
  }

  delay(TEMPO_IRRIGACAO_MANUAL);

  for (int i = 0; i < numGrupos; i++) {
    digitalWrite(grupos[i].pinoIrrigacao, LOW);
  }

  Serial.println("Irrigacao manual concluida. Todos os sistemas desligados.");
}

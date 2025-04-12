// ==== DEFINIÇÕES ====
#define PINO_BOTAO 7
#define TEMPO_IRRIGACAO_MANUAL 10000  // 10 segundos

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
  {"Grupo 1 (Capim-açu + Amendoim)", 9, PINO_TEMP_GRUPO1, PINO_UMID_GRUPO1, 25, 32, 52},
  {"Grupo 2 (Mandioca + Feijão)",    10, PINO_TEMP_GRUPO2, PINO_UMID_GRUPO2, 18, 30, 48},
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

  Serial.println("Sistema iniciado. Aguardando leitura de sensores...");
}

void loop() {
  if (botaoPressionado()) {
    Serial.println(">> Botão pressionado - Iniciando irrigação manual");
    irrigacaoManual();
  } else {
    for (int i = 0; i < numGrupos; i++) {
      float temperatura = lerTemperatura(grupos[i].pinoTemp);
      float umidade = lerUmidade(grupos[i].pinoUmid);

      Serial.print(grupos[i].nome); 
      Serial.print(" - Temp: "); Serial.print(temperatura); Serial.print(" °C | ");
      Serial.print("Umid: "); Serial.print(umidade); Serial.println(" %");

      controlarIrrigacao(grupos[i], temperatura, umidade);
    }
  }

  delay(5000); // Aguarda 5 segundos para próxima leitura
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
  return map(leitura, 1023, 0, 0, 100); // 0% (molhado) a 100% (seco)
}

// ==== LÓGICA DE IRRIGAÇÃO AUTOMÁTICA ====
void controlarIrrigacao(Grupo grupo, float temp, float umid) {
  // A irrigação será acionada se a temperatura for maior que o mínimo ou a umidade for menor que o mínimo
  bool precisaIrrigar = (temp > grupo.tempMin) || (umid < grupo.umidadeMin);

  digitalWrite(grupo.pinoIrrigacao, precisaIrrigar ? HIGH : LOW);

  Serial.print(grupo.nome);
  if (precisaIrrigar) {
    Serial.println(" - Temp > mínima ou umidade < mínima → Irrigando...");
  } else {
    Serial.println(" - Não precisa irrigar.");
  }
}

// ==== IRRIGAÇÃO MANUAL ====
void irrigacaoManual() {
  // Aciona a irrigação de todos os grupos
  for (int i = 0; i < numGrupos; i++) {
    digitalWrite(grupos[i].pinoIrrigacao, HIGH);
  }

  delay(TEMPO_IRRIGACAO_MANUAL);  // Irriga por 10 segundos

  // Desliga a irrigação de todos os grupos
  for (int i = 0; i < numGrupos; i++) {
    digitalWrite(grupos[i].pinoIrrigacao, LOW);
  }

  Serial.println(">> Irrigação manual finalizada");
}

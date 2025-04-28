💧 Irrigação Automática com ESP32, ThingsBoard e WhatsApp (Simulação com Potenciômetros)

- Este projeto apresenta um sistema de irrigação automática inteligente utilizando um ESP32, com simulação de sensores via potenciômetros, integração com o ThingsBoard para monitoramento remoto e envio de alertas via WhatsApp, utilizando a API do CallMeBot.

🧪 Simulação com Potenciômetros

Cada grupo de irrigação possui dois potenciômetros:

- Um simula a temperatura

- Outro simula a umidade do solo

- Isso permite testar a lógica de controle sem a necessidade de sensores reais.

🔧 Funcionalidades:

📶 Conexão automática à rede Wi-Fi e reconexão automática;

🌡️ Leitura de temperatura e umidade simuladas para até 3 grupos;

🤖 Controle automático da irrigação com base em limites configuráveis;

🔘 Botão físico para ativar irrigação manual por 10 segundos;

📈 Envio de dados para o ThingsBoard (plataforma IoT);

📲 Notificações no WhatsApp sempre que a irrigação automática é ativada.

⚙️ Componentes Utilizados:

- ESP32
- 6 potenciômetros (3 para temperatura, 3 para umidade)
- 3 saídas de controle (simulando bombas/relés)
- 1 botão físico

Integração com:

- ThingsBoard
- CallMeBot WhatsApp API

📤 Dados Enviados ao ThingsBoard:

- temp_1, temp_2, temp_3 → temperaturas simuladas (via potenciômetro)
- umid_1, umid_2, umid_3 → umidades simuladas (via potenciômetro)

🚨 Exemplo de Notificação WhatsApp:

- Grupo 2: IRRIGACAO ON (T=29.3°C U=45.2%)

link do Projeto no Wokwi: https://wokwi.com/projects/429318669164524545
* Utilizando potenciometro no lugar dos sensores para exemplo.
![plot](./Imagem/ESP32.png)

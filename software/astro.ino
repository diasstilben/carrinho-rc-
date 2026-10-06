#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// ==========================================
// 1. CONFIGURAÇÃO DA REDE WI-FI (Access Point)
// ==========================================
const char* ssid = "Astro_Rc";
const char* password = "dias";

// Inicia o servidor web na porta padrão HTTP (80)
WebServer server(80);

// ==========================================
// 2. CONFIGURAÇÃO DOS PINOS E MOTORES
// ==========================================
// Motores Lado Esquerdo
const int ENA_ESQ = 13;
const int IN1_ESQ = 14;
const int IN2_ESQ = 27;
const int IN3_ESQ = 26;
const int IN4_ESQ = 25;
const int ENB_ESQ = 33;

// Motores Lado Direito
const int ENA_DIR = 32;
const int IN1_DIR = 22;
const int IN2_DIR = 19;
const int IN3_DIR = 18;
const int IN4_DIR = 16;
const int ENB_DIR = 17;

// Pino da Buzina
const int BUZINA_PIN = 23;

// Pinos do Sensor
const int TRIG_PIN = 4;
const int ECHO_PIN = 35; 
// Variáveis para o controle do sensor de ré 
bool emRe = false;
bool buzinaReAtiva = false;
unsigned long ultimoTempoBip = 0;
int estadoBuzinaRe = LOW;

// Declaração antecipada da função parar para o configurarPinos não dar erro
void parar(); 

void configurarPinos() {
    // Configura os pinos de direção como SAÍDA
    pinMode(IN1_ESQ, OUTPUT); pinMode(IN2_ESQ, OUTPUT);
    pinMode(IN3_ESQ, OUTPUT); pinMode(IN4_ESQ, OUTPUT);
    pinMode(IN1_DIR, OUTPUT); pinMode(IN2_DIR, OUTPUT);
    pinMode(IN3_DIR, OUTPUT); pinMode(IN4_DIR, OUTPUT);
    
    // Configura buzina e garante que comece desligada
    pinMode(BUZINA_PIN, OUTPUT);
    digitalWrite(BUZINA_PIN, LOW);

    // Configura os pinos do Sensor Ultrassônico (CORRIGIDO: Faltava configurar)
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Configura o PWM para o controle de velocidade (1000 Hz, 10 bits = 0 a 1023)
    ledcAttach(ENA_ESQ, 1000, 10);
    ledcAttach(ENB_ESQ, 1000, 10);
    ledcAttach(ENA_DIR, 1000, 10);
    ledcAttach(ENB_DIR, 1000, 10);
    
    parar(); // Garante que o robô inicie parado
}

// Função base para acionar as pontes H
void moverMotores(int v1_esq, int v2_esq, int v3_esq, int v4_esq, 
                  int v1_dir, int v2_dir, int v3_dir, int v4_dir, int val_pwm) {
    digitalWrite(IN1_ESQ, v1_esq); digitalWrite(IN2_ESQ, v2_esq);
    digitalWrite(IN3_ESQ, v3_esq); digitalWrite(IN4_ESQ, v4_esq);
    
    digitalWrite(IN1_DIR, v1_dir); digitalWrite(IN2_DIR, v2_dir);
    digitalWrite(IN3_DIR, v3_dir); digitalWrite(IN4_DIR, v4_dir);

    // Aplica a velocidade (PWM)
    ledcWrite(ENA_ESQ, val_pwm); ledcWrite(ENB_ESQ, val_pwm);
    ledcWrite(ENA_DIR, val_pwm); ledcWrite(ENB_DIR, val_pwm);
}

// Comandos de Movimento
void parar()    { moverMotores(0,0,0,0, 0,0,0,0, 0); }
void frente()   { moverMotores(1,0,1,0, 1,0,1,0, 1023); }
void tras()     { moverMotores(0,1,0,1, 0,1,0,1, 1023); }
void esquerda() { moverMotores(0,1,0,1, 1,0,1,0, 1023); } 
void direita()  { moverMotores(1,0,1,0, 0,1,0,1, 1023); }

// ==========================================
// 3. SERVIDOR DE ARQUIVOS (LittleFS)
// ==========================================
void enviarArquivo(String caminho, String tipoConteudo) {
    if (LittleFS.exists(caminho)) {
        File arquivo = LittleFS.open(caminho, "r");
        server.streamFile(arquivo, tipoConteudo);
        arquivo.close();
    } else {
        server.send(404, "text/plain", "Erro 404: Arquivo nao encontrado no ESP32");
    }
}

long medirDistancia() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Timeout de 30ms (~5 metros no máximo) para não travar o código
    long duracao = pulseIn(ECHO_PIN, HIGH, 30000); 
    
    if (duracao == 0) return 999; // Se não ler nada, assume que tá livre
    return (duracao / 2) / 29.1;  // Converte o tempo do eco para centímetros
}

void desativarAlarmeRe() {
    emRe = false;
    if (buzinaReAtiva) { // Desliga a buzina de ré só se ela estiver apitando
        digitalWrite(BUZINA_PIN, LOW);
        buzinaReAtiva = false;
    }
}

// ==========================================
// 4. SETUP INICIAL DO SISTEMA
// ==========================================
void setup() {
    Serial.begin(115200);
    configurarPinos();

    // Inicia a memória Flash (LittleFS)
    if (!LittleFS.begin(true)) {
        Serial.println("Erro ao montar o LittleFS. Verifique se fez o upload da pasta 'data'.");
        return;
    }
    Serial.println("LittleFS montado com sucesso!");

    // Cria a Rede Wi-Fi
    Serial.println("Iniciando Wi-Fi do Astro...");
    WiFi.softAP(ssid, password);
    Serial.print("Rede criada! IP de acesso: ");
    Serial.println(WiFi.softAPIP()); // O IP será 192.168.4.1

    // ------------------------------------------
    // ROTAS DO SERVIDOR WEB
    // ------------------------------------------
    
    // Entrega o HTML, CSS e JS quando o navegador pedir
    server.on("/", HTTP_GET, []() { enviarArquivo("/index.html", "text/html"); });
    server.on("/index.html", HTTP_GET, []() { enviarArquivo("/index.html", "text/html"); });
    server.on("/style.css", HTTP_GET, []() { enviarArquivo("/style.css", "text/css"); });
    server.on("/script.js", HTTP_GET, []() { enviarArquivo("/script.js", "application/javascript"); });

    // Rotas de Comandos Motores e Buzina 
    server.on("/B", []() { emRe = true; tras(); server.send(200); }); // Ativa a ré
    server.on("/F", []() { desativarAlarmeRe(); frente(); server.send(200); });
    server.on("/L", []() { desativarAlarmeRe(); esquerda(); server.send(200); });
    server.on("/R", []() { desativarAlarmeRe(); direita(); server.send(200); });
    server.on("/S", []() { desativarAlarmeRe(); parar(); server.send(200); });
    server.on("/H", []() { digitalWrite(BUZINA_PIN, HIGH); server.send(200); });
    server.on("/Q", []() { digitalWrite(BUZINA_PIN, LOW); server.send(200); });
    server.on("/ping", []() { server.send(200); });
    
    // Inicia o servidor
    server.begin();
    Serial.println("Servidor pronto! Astro aguardando ordens.");
}

// ==========================================
// 5. LOOP PRINCIPAL
// ==========================================
void loop() { 
    server.handleClient(); // Continua ouvindo o navegador rapidamente

    // Lógica do Sensor de Ré
    if (emRe) {
        long distancia = medirDistancia();
        int intervaloBip = 0;

        if (distancia < 15) {
            intervaloBip = 100; // Menos de 15cm: Bipe muito rápido (cuidado!)
        } else if (distancia < 30) {
            intervaloBip = 300; // Menos de 30cm: Bipe médio
        } else if (distancia < 60) {
            intervaloBip = 600; // Menos de 60cm: Bipe lento
        } else {
            intervaloBip = 0;   // Mais de 60cm: Muito longe, não apita
            if (buzinaReAtiva) {
                digitalWrite(BUZINA_PIN, LOW);
                buzinaReAtiva = false;
            }
        }

        // Toca o bipe no ritmo certo sem usar delay()
        if (intervaloBip > 0) {
            buzinaReAtiva = true;
            if (millis() - ultimoTempoBip >= intervaloBip) {
                ultimoTempoBip = millis(); // Reseta o cronômetro
                estadoBuzinaRe = (estadoBuzinaRe == LOW) ? HIGH : LOW; // Inverte o estado
                digitalWrite(BUZINA_PIN, estadoBuzinaRe);
            }
        }
    }
}
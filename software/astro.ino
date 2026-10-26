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
// 2. CONFIGURAÇÃO DOS PINOS E MOTORES (UMA PONTE H)
// ==========================================
// Canal A (Motores do Lado Esquerdo)
const int ENA = 13;
const int IN1 = 14;
const int IN2 = 27;

// Canal B (Motores do Lado Direito)
const int IN3 = 26;
const int IN4 = 25;
const int ENB = 33;

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
    pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
    
    // Configura buzina e garante que comece desligada
    pinMode(BUZINA_PIN, OUTPUT);
    digitalWrite(BUZINA_PIN, LOW);

    // Configura os pinos do Sensor Ultrassônico
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Configura o PWM para o controle de velocidade (1000 Hz, 10 bits = 0 a 1023)
    ledcAttach(ENA, 1000, 10);
    ledcAttach(ENB, 1000, 10);
    
    parar(); // Garante que o robô inicie parado
}

// Função base para acionar a ponte H (Adaptada para 1 Ponte H)
void moverMotores(int v1, int v2, int v3, int v4, int val_pwm) {
    digitalWrite(IN1, v1); digitalWrite(IN2, v2);
    digitalWrite(IN3, v3); digitalWrite(IN4, v4);

    // Aplica a velocidade (PWM) em ambos os lados
    ledcWrite(ENA, val_pwm); ledcWrite(ENB, val_pwm);
}

// Comandos de Movimento
void parar()    { moverMotores(0, 0, 0, 0, 0); }
void frente()   { moverMotores(1, 0, 1, 0, 1023); }
void tras()     { moverMotores(0, 1, 0, 1, 1023); }
void esquerda() { moverMotores(0, 1, 1, 0, 1023); } 
void direita()  { moverMotores(1, 0, 0, 1, 1023); }

// ==========================================
// 3. SERVIDOR DE ARQUIVOS (LittleFS) E SENSOR
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
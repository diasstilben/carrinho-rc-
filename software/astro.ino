#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// ==========================================
// 1. CONFIGURAÇÃO DA REDE WI-FI (Access Point)
// ==========================================
const char* ssid = "Carrinho_Robo";
const char* password = "senha_segura123";

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
const int BUZINA_PIN = 15;

void configurarPinos() {
    // Configura os pinos de direção como SAÍDA
    pinMode(IN1_ESQ, OUTPUT); pinMode(IN2_ESQ, OUTPUT);
    pinMode(IN3_ESQ, OUTPUT); pinMode(IN4_ESQ, OUTPUT);
    pinMode(IN1_DIR, OUTPUT); pinMode(IN2_DIR, OUTPUT);
    pinMode(IN3_DIR, OUTPUT); pinMode(IN4_DIR, OUTPUT);
    
    // Configura buzina e garante que comece desligada
    pinMode(BUZINA_PIN, OUTPUT);
    digitalWrite(BUZINA_PIN, LOW);

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
// Função que lê os arquivos da pasta 'data' e envia para o navegador
void enviarArquivo(String caminho, String tipoConteudo) {
    if (LittleFS.exists(caminho)) {
        File arquivo = LittleFS.open(caminho, "r");
        server.streamFile(arquivo, tipoConteudo);
        arquivo.close();
    } else {
        server.send(404, "text/plain", "Erro 404: Arquivo nao encontrado no ESP32");
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

    // Rotas de Controle do Robô (acionadas pelo JS)
    server.on("/F", HTTP_GET, []() { frente(); server.send(200); });
    server.on("/B", HTTP_GET, []() { tras(); server.send(200); });
    server.on("/L", HTTP_GET, []() { esquerda(); server.send(200); });
    server.on("/R", HTTP_GET, []() { direita(); server.send(200); });
    server.on("/S", HTTP_GET, []() { parar(); server.send(200); });
    
    // Rotas da Buzina e Ping de Status
    server.on("/H", HTTP_GET, []() { digitalWrite(BUZINA_PIN, HIGH); server.send(200); });
    server.on("/Q", HTTP_GET, []() { digitalWrite(BUZINA_PIN, LOW); server.send(200); });
    server.on("/ping", HTTP_GET, []() { server.send(200); }); // Mantém o status "Online" verde

    // Inicia o servidor
    server.begin();
    Serial.println("Servidor pronto! Astro aguardando ordens.");
}

// ==========================================
// 5. LOOP PRINCIPAL
// ==========================================
void loop() {
    // O ESP32 fica ouvindo as requisições do navegador
    server.handleClient();
}
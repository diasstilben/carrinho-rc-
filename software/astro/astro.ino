#include <WiFi.h>
#include <WebServer.h>
#include "pagina.h"

// ==========================================
// 1. CONFIGURAÇÃO DA REDE WI-FI (Access Point)
// ==========================================
const char* ssid = "Astro_Rc";
const char* password = "dias1234";

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

// ==========================================
// PARADA DE SEGURANÇA (failsafe)
// ==========================================
// Enquanto o carrinho anda, a página repete o comando a cada 150ms.
// Se o ESP ficar TEMPO_LIMITE_MS sem receber a repetição (Wi-Fi caiu, celular
// travou, aba fechada...), ele para os motores / desliga a buzina sozinho.
const unsigned long TEMPO_LIMITE_MS = 500;
bool motoresAtivos = false;
bool buzinaManual = false;
unsigned long ultimoComandoMotor = 0;
unsigned long ultimoComandoBuzina = 0;

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

// PWM mínimo que de fato faz a roda girar. Abaixo disso o motor DC só chia e
// esquenta sem sair do lugar, então o curso do analógico começa daqui.
// Se o carrinho não andar com o analógico pouco empurrado, aumente; se ele
// já sair rápido demais, diminua.
const int PWM_MINIMO = 400;

// Aciona UM lado da ponte H com velocidade proporcional.
// velocidade: -1023 (ré total) .. 0 (parado) .. 1023 (frente total)
void acionarLado(int pinoA, int pinoB, int pinoEN, int velocidade) {
    velocidade = constrain(velocidade, -1023, 1023);
    if (velocidade == 0) {
        digitalWrite(pinoA, LOW); digitalWrite(pinoB, LOW);
        ledcWrite(pinoEN, 0);
        return;
    }
    digitalWrite(pinoA, velocidade > 0 ? HIGH : LOW);
    digitalWrite(pinoB, velocidade > 0 ? LOW : HIGH);
    ledcWrite(pinoEN, map(abs(velocidade), 1, 1023, PWM_MINIMO, 1023));
}

// Comandos de Movimento (Canal A = lado esquerdo, Canal B = lado direito)
void moverMotores(int velEsquerda, int velDireita) {
    acionarLado(IN1, IN2, ENA, velEsquerda);
    acionarLado(IN3, IN4, ENB, velDireita);
}
void parar() { moverMotores(0, 0); }

// ==========================================
// 3. SENSOR DE RÉ
// ==========================================
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

// Chamado a cada comando de movimento: marca que os motores estão ligados
// e renova o prazo do failsafe
void manterMotores() {
    motoresAtivos = true;
    ultimoComandoMotor = millis();
}

void pararTudo() {
    desativarAlarmeRe();
    parar();
    motoresAtivos = false;
}

// ==========================================
// 4. SETUP INICIAL DO SISTEMA
// ==========================================
void setup() {
    Serial.begin(115200);
    configurarPinos();

    // Cria a Rede Wi-Fi
    Serial.println("Iniciando Wi-Fi do Astro...");
    WiFi.softAP(ssid, password);
    Serial.print("Rede criada! IP de acesso: ");
    Serial.println(WiFi.softAPIP()); // O IP será 192.168.4.1

    // ------------------------------------------
    // ROTAS DO SERVIDOR WEB
    // ------------------------------------------
    
    // Página de controle (HTML, CSS e JS embutidos em pagina.h)
    server.on("/", HTTP_GET, []() { server.send(200, "text/html", index_html); });
    server.on("/index.html", HTTP_GET, []() { server.send(200, "text/html", index_html); });

    // Rotas de Comandos Motores e Buzina
    // /M?e=<esquerda>&d=<direita>: velocidade de cada lado, de -1023 a 1023,
    // já misturada pela página a partir dos analógicos (acelerador + direção)
    server.on("/M", []() {
        int esq = constrain(server.arg("e").toInt(), -1023, 1023);
        int dir = constrain(server.arg("d").toInt(), -1023, 1023);
        if (esq == 0 && dir == 0) {
            pararTudo();
        } else {
            // Alarme de ré só quando o carrinho, no geral, está indo para trás
            if (esq + dir < 0) emRe = true;
            else desativarAlarmeRe();
            moverMotores(esq, dir);
            manterMotores();
        }
        server.send(200);
    });
    server.on("/S", []() { pararTudo(); server.send(200); });
    server.on("/H", []() {
        digitalWrite(BUZINA_PIN, HIGH);
        buzinaManual = true;
        ultimoComandoBuzina = millis();
        server.send(200);
    });
    server.on("/Q", []() { digitalWrite(BUZINA_PIN, LOW); buzinaManual = false; server.send(200); });
    server.on("/ping", []() { server.send(200); });
    server.onNotFound([]() { server.send(404, "text/plain", "Nao encontrado"); });

    // Inicia o servidor
    server.begin();
    Serial.println("Servidor pronto! Astro aguardando ordens.");
}

// ==========================================
// 5. LOOP PRINCIPAL
// ==========================================
void loop() { 
    server.handleClient(); // Continua ouvindo o navegador rapidamente

    // Failsafe: sem repetição do comando dentro do prazo, desliga sozinho
    unsigned long agora = millis();
    if (motoresAtivos && agora - ultimoComandoMotor > TEMPO_LIMITE_MS) {
        pararTudo();
        Serial.println("Failsafe: sem comando ha 500ms, motores parados.");
    }
    if (buzinaManual && agora - ultimoComandoBuzina > TEMPO_LIMITE_MS) {
        digitalWrite(BUZINA_PIN, LOW);
        buzinaManual = false;
        Serial.println("Failsafe: buzina desligada.");
    }

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
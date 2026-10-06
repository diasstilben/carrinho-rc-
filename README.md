# 🤖 Carrinho RC com ESP32

Repositório dedicado ao desenvolvimento do hardware, esquemas elétricos e código de controle de um robô móvel de 4 rodas (4WD) baseado em **ESP32 DevKit V1**, controlado por duas pontes H **L298N** e equipado com sensor ultrassônico e alerta sonoro.

---

## 🛠️ Componentes Utilizados

* **Microcontrolador:** ESP32 DevKit V1 (30 pinos)
* **Tração:** 4x Motores DC com caixa de redução (4WD)
* **Acionamento de Motores:** 2x Pontes H L298N (divididas por lado: Esquerdo e Direito)
* **Alimentação:** 4x Baterias de Lítio (configuradas em pack **2S2P** ~7.4V)
* **Navegação/Sensores:** 1x Sensor Ultrassônico HC-SR04
* **Atuador Sonoro:** 1x Buzzer Ativo/Passivo
* **Outros:** Resistores (1kΩ e 2kΩ) para o divisor de tensão, protoboard e jumpers.

---

## ⚡ Esquema de Conexão (Pinout)

### 1. Alimentação e Terra (GND Comum)
* **Bateria (+):** Conectada aos bornes **12V** das duas pontes H (L298N 1 e L298N 2).
* **Bateria (-):** Conectada aos bornes **GND** das pontes H (fechando a malha de terra comum).
* **Alimentação do ESP32:** Puxada do borne **5V** da Ponte H 1 para o pino **VIN** (ou 5V) do ESP32.
* **Alimentação do Sensor:** Puxada do borne **5V** da Ponte H 1 para o pino **VCC** do HC-SR04.

### 2. Sinais de Controle - Motores (ESP32)
> *Nota: Os jumpers de ENA/ENB das pontes H foram removidos para permitir o controle de velocidade via PWM.*

| Pino do ESP32 | Função na Ponte H 1 (Lado Esquerdo) |
| :---: | :--- |
| **GPIO 13** | ENA (Velocidade Motor Esquerdo A) |
| **GPIO 14** | IN1 (Direção Motor Esquerdo A) |
| **GPIO 27** | IN2 (Direção Motor Esquerdo A) |
| **GPIO 26** | IN3 (Direção Motor Esquerdo B) |
| **GPIO 25** | IN4 (Direção Motor Esquerdo B) |
| **GPIO 33** | ENB (Velocidade Motor Esquerdo B) |

| Pino do ESP32 | Função na Ponte H 2 (Lado Direito) |
| :---: | :--- |
| **GPIO 32** | ENA (Velocidade Motor Direito A) |
| **GPIO 22** | IN1 (Direção Motor Direito A) |
| **GPIO 19** | IN2 (Direção Motor Direito A) |
| **GPIO 18** | IN3 (Direção Motor Direito B) |
| **GPIO 16** | IN4 (Direção Motor Direito B) |
| **GPIO 17** | ENB (Velocidade Motor Direito B) |

### 3. Periféricos (Sensor e Buzzer)
| Componente / Pino | Conexão no ESP32 | Observação |
| :--- | :--- | :--- |
| **HC-SR04 (Trig)** | GPIO **4** | Direto |
| **HC-SR04 (Echo)** | GPIO **35** | Protegido por Divisor de Tensão (1kΩ + 2kΩ para o GND) |
| **Buzzer (+)** | GPIO **23** | Sinal de acionamento |

---

## 📁 Estrutura do Repositório

```text
carrinho-rc/
├── README.md                           # Documentação geral do projeto
├── hardware/
│   ├── cad/
│   │   └── CONEXÃO DO CARRINHO RC.fzz  # Esquema elétrico e de montagem (Fritzing)
│   └── fotos/
│       └── carcaça do astro com motores.jpeg # Registros físicos da montagem
└── software/
    ├── software.ino                    # Código principal em C++ 
    └── data/                           # Pasta obrigatória para o LittleFS
        ├── index.html                  # Estrutura do painel de controle
        ├── style.css                   # Estilização visual
        └── script.js                   # Lógica de controle, leitura do teclado e requisições HTTP
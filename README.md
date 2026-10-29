# 🤖 Carrinho RC "Astro" com ESP32

Repositório dedicado ao desenvolvimento do hardware, esquemas elétricos e código de controle do "Astro", um robô móvel de 4 rodas (4WD) baseado em **ESP32 DevKit V1**. O projeto utiliza uma interface web de controle imersiva e conta com uma arquitetura de energia isolada para garantir a estabilidade do microcontrolador e a máxima eficiência dos motores.

---

## 🛠️ Componentes Utilizados

- **Microcontrolador:** ESP32 DevKit V1 (30 pinos)
- **Tração:** 4x Motores DC robustos projetados para 12V (4WD)
- **Acionamento de Motores:** 1x Ponte H L298N (controlando os dois lados do chassi)
- **Alimentação de Potência (Motores):** Pack de 2x Baterias de Lítio em série (7.4V) *— Escalável para 3S (11.1V) futuramente*
- **Alimentação Lógica (ESP32/Sensores):** 1x Power Bank (via conexão USB)
- **Navegação/Sensores:** 1x Sensor Ultrassônico HC-SR04
- **Atuador Sonoro:** 1x Buzzer Ativo/Passivo
- **Outros:** Resistores (1kΩ e 2kΩ) para divisor de tensão, protoboard e jumpers.

---

## ⚡ Esquema de Conexão (Pinout) e Energia

### 1. Arquitetura de Energia e GND Comum
Para evitar que o ruído e o alto consumo de corrente dos motores reiniciem o ESP32 (especialmente com o Wi-Fi ativo), os circuitos de potência e lógica são separados. **O ponto crítico desta arquitetura é a interligação de todos os GNDs para nivelar a referência elétrica.**

- **Alimentação dos Motores:** O polo positivo das baterias de lítio é conectado ao borne **12V** da Ponte H. O polo negativo vai para a **trilha do GND Comum**.
- **Alimentação Lógica:** O ESP32 é alimentado exclusivamente pelo cabo USB ligado ao **Power Bank**.
- **Alimentação de Periféricos:** O VCC do Sensor e do Buzzer é derivado do pino VIN/5V do ESP32.
- **Malha de Terra (GND Comum):** O GND do ESP32, o GND do Power Bank (via placa), o borne GND da Ponte H, o negativo das Baterias de Lítio, e os pinos GND do Sensor e do Buzzer estão unificados na mesma trilha.

### 2. Sinais de Controle - Motores (ESP32 para L298N)
Os motores do lado esquerdo estão ligados em paralelo nas saídas OUT1 e OUT2. Os motores do lado direito estão em paralelo nas saídas OUT3 e OUT4. 
> *Nota: Os jumpers de ENA/ENB da ponte H foram removidos para permitir o controle dinâmico de velocidade (sistema de marchas) via PWM.*

| Pino do ESP32 | Função na Ponte H (L298N) |
| :---: | :--- |
| **GPIO 13** | ENA (Velocidade - Lado Esquerdo) |
| **GPIO 14** | IN1 (Direção - Lado Esquerdo) |
| **GPIO 27** | IN2 (Direção - Lado Esquerdo) |
| **GPIO 26** | IN3 (Direção - Lado Direito) |
| **GPIO 25** | IN4 (Direção - Lado Direito) |
| **GPIO 33** | ENB (Velocidade - Lado Direito) |

### 3. Periféricos (Sensor e Buzzer)
| Componente / Pino | Conexão no ESP32 | Observação |
| :--- | :--- | :--- |
| **HC-SR04 (Trig)** | GPIO **4** | Direto |
| **HC-SR04 (Echo)** | GPIO **35** | Protegido por Divisor de Tensão (1kΩ + 2kΩ para o GND) |
| **Buzzer (+)** | GPIO **23** | Sinal de acionamento |

---

## 💻 Software e Interface (LittleFS)

O controle do Astro é realizado através de uma rede Wi-Fi gerada pelo próprio ESP32, servindo um painel de controle web embarcado diretamente na memória flash usando o sistema de arquivos **LittleFS**.

- **Dashboard Futurista:** A interface de controle adota uma estética sci-fi/HUD (Heads-Up Display) com esquema de cores neon e escuro, simulando um painel de comando avançado.
- **Sistema de Marchas (Em Desenvolvimento):** O mapeamento de botões na interface envia parâmetros ao backend em C++, que ajusta o teto do sinal PWM (0-255). Isso permite alternar entre diferentes "marchas", controlando a potência entregue pela bateria aos motores sem alterar a tensão da fonte.

---

## 📁 Estrutura do Repositório

```text
carrinho-rc/
├── README.md                           # Documentação geral do projeto
├── hardware/
│   ├── cad/
│   │   └── CONEXÃO DO CARRINHO RC.fzz  # Esquema elétrico e montagem (Fritzing)
│   └── fotos/
│       └── carcaça do astro com motores.jpeg # Registros físicos da montagem
│       └── conexôes_cad.jng # Foto da montagem no simulador 
└── software/
    ├── software.ino                    # Código principal em C++ (Servidor Web e Controle)
    └── data/                           # Arquivos estáticos do LittleFS
        ├── index.html                  # Estrutura do painel de controle (HUD)
        ├── style.css                   # Estilização visual futurista
        └── script.js                   # Lógica HTTP, leitura de teclas e sistema de marchas

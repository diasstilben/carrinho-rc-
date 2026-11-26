# 🤖 Carrinho RC "Astro" com ESP32

Repositório dedicado ao desenvolvimento do hardware, esquemas elétricos e código de controle do "Astro", um robô móvel de 4 rodas (4WD) baseado em **ESP32 DevKit V1**. O projeto utiliza uma interface web de controle imersiva e é alimentado por uma única bateria de lítio 3S3P, que move os motores e, pelo regulador da ponte H, também alimenta o ESP32.

---

## 🛠️ Componentes Utilizados

- **Microcontrolador:** ESP32 DevKit V1 (30 pinos)
- **Tração:** 4x Motores DC robustos projetados para 12V (4WD)
- **Acionamento de Motores:** 1x Ponte H L298N (controlando os dois lados do chassi)
- **Alimentação:** 1x Bateria de Lítio 3S3P (9 baterias: 3 em série × 3 em paralelo — 11,1V nominal, 12,6V cheia, ~9V vazia)
- **Alimentação Lógica (ESP32/Sensores):** regulador de 5V da própria Ponte H L298N
- **Navegação/Sensores:** 1x Sensor Ultrassônico HC-SR04 *(instalado e com os pinos reservados para uso futuro; o alarme de ré que usava o sensor foi retirado)*
- **Atuador Sonoro:** 1x Buzzer Ativo/Passivo
- **Outros:** Resistores (1kΩ e 2kΩ) para divisor de tensão, protoboard e jumpers.

---

## ⚡ Esquema de Conexão (Pinout) e Energia

### 1. Arquitetura de Energia e GND Comum
Uma única bateria 3S3P alimenta tudo: ela vai direto para a ponte H, e o regulador de 5V da ponte H alimenta o ESP32 e os periféricos. **Todos os GNDs estão interligados na mesma trilha.**

- **Alimentação dos Motores:** O polo positivo da bateria 3S3P é conectado ao borne **12V** da Ponte H. O polo negativo vai para a **trilha do GND Comum**. Descontando a queda de ~2V do L298N, os motores (de 12V) recebem de ~7V a ~10,5V, conforme a carga da bateria.
- **Alimentação Lógica:** O pino **5V** da Ponte H (saída do regulador 78M05 da placa) é ligado ao pino **VIN** do ESP32. Nunca ligue no pino 3V3.
- **Alimentação de Periféricos:** O VCC do Sensor e do Buzzer sai da mesma linha de 5V (pino 5V da Ponte H / VIN do ESP32).
- **Malha de Terra (GND Comum):** O GND do ESP32, o borne GND da Ponte H, o negativo da bateria e os pinos GND do Sensor e do Buzzer estão unificados na mesma trilha.

> **Cuidados com esta ligação:**
> - O **jumper de 12V** da Ponte H precisa ficar colocado: é ele que liga o regulador de 5V. Sem o jumper, o pino 5V vira entrada e o ESP32 fica sem energia. Com a bateria cheia (12,6V) já está um pouco acima dos 12V recomendados, então **não use 4S** com este esquema.
> - O regulador da Ponte H aguenta ~500mA e esquenta (o Wi-Fi do ESP32 puxa picos de 250–300mA). Se ele ficar muito quente ou o ESP32 reiniciar sozinho, troque por um regulador step-down (buck) de 5V ligado direto na bateria.
> - Recomendado: um capacitor eletrolítico de 470–1000µF entre o VIN e o GND do ESP32, para segurar as quedas de tensão quando os motores arrancam.
> - Para gravar o código pelo USB, desligue a bateria antes (nem toda placa DevKit tem o diodo que protege o computador).

### 2. Sinais de Controle - Motores (ESP32 para L298N)
Os motores do lado esquerdo estão ligados em paralelo nas saídas OUT1 e OUT2. Os motores do lado direito estão em paralelo nas saídas OUT3 e OUT4. 
> *Nota: Os jumpers de ENA/ENB da ponte H foram removidos para permitir o controle dinâmico de velocidade via PWM.*

> *Correção de frente/ré: no carrinho montado, virar para os lados saía certo, mas frente e ré saíam trocadas. Por isso o `astro.ino` (constante `CORRIGIR_FRENTE_RE`) troca os canais e inverte os dois: na prática o canal B move o lado esquerdo e o canal A o direito, ambos com o sentido invertido. A tabela abaixo mostra a ligação dos fios, que não mudou.*

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
| **HC-SR04 (Trig)** | GPIO **4** | Direto (reservado, sem uso no código por enquanto) |
| **HC-SR04 (Echo)** | GPIO **35** | Protegido por Divisor de Tensão (1kΩ + 2kΩ para o GND) (reservado) |
| **Buzzer (+)** | GPIO **23** | Buzina (botão da página / tecla F) |

---

## 💻 Software e Interface

O controle do Astro é realizado através de uma rede Wi-Fi gerada pelo próprio ESP32 (`Astro_Rc`, acesso em `192.168.4.1`), servindo um painel de controle web embutido no próprio firmware.

- **Página embutida no código:** o HTML, o CSS e o JS ficam juntos no arquivo `pagina.h`, que é compilado junto com o `astro.ino`. Não é preciso enviar arquivos para o sistema de arquivos do ESP32 (LittleFS).
- **Fonte editável em `data/`:** os arquivos `index.html`, `style.css` e `script.js` são a versão separada da mesma página, mais fácil de editar. Ao mudar a página, atualize também o `pagina.h`.

- **Controles analógicos:** o analógico da esquerda é o acelerador (para cima = frente, para baixo = ré) e o da direita é a direção. Quanto mais longe do centro, mais rápido/mais fechada a curva: a página mistura os dois e manda a velocidade de cada lado (`/M?e=<esq>&d=<dir>`, de -1023 a 1023). No computador, W/A/S/D (ou setas) continuam funcionando e dá para combinar (W + D faz curva). O `PWM_MINIMO` no `astro.ino` define a menor força que ainda faz a roda girar.
- **Feito para o celular:** em pé, parar e buzina ficam em cima e um analógico embaixo de cada polegar; deitado, o título e o status ficam numa linha só para sobrar altura para os analógicos. A área de toque de cada analógico é a coluna inteira (rótulo, círculo e porcentagem) e os 100% ficam perto da borda do círculo, para dar um controle mais fino. A legenda de teclas some em aparelhos só de toque.
- **Parada de Segurança (failsafe):** enquanto o carrinho anda (ou a buzina toca), a página repete o comando a cada 150ms. Se o ESP32 ficar 500ms sem receber a repetição (Wi-Fi caiu, celular travou, troca de aba), ele para os motores e desliga a buzina sozinho. A página também manda parar ao perder o foco.
- **Dashboard Futurista:** A interface de controle adota uma estética sci-fi/HUD (Heads-Up Display) com esquema de cores neon e escuro, simulando um painel de comando avançado.
- **Velocidade proporcional:** o PWM de cada lado (0-1023, 10 bits) segue o quanto o analógico foi empurrado, então não é preciso um sistema de marchas. Com a bateria 3S, o `PWM_MINIMO` de 400 deve bastar para as rodas saírem do lugar: se o carrinho der tranco com o analógico pouco empurrado, diminua; se as rodas só chiarem, aumente.

---

## 📁 Estrutura do Repositório

```text
carrinho-rc/
├── README.md                           # Documentação geral do projeto
├── hardware/
│   ├── cad/
│   │   └── CONEXÃO DO CARRINHO RC.fzz  # Esquema elétrico e montagem (Fritzing)
│   └── fotos-videos/
│       ├── astrofuncionando.mp4        # Vídeo do carrinho funcionando
│       ├── carcaça do astro com motores.jpeg # Registros físicos da montagem
│       └── conexões_cad.png            # Foto da montagem no simulador
└── software/
    └── astro/
        ├── astro.ino                   # Código principal em C++ (Servidor Web e Controle)
        ├── pagina.h                    # Página de controle completa, embutida no firmware
        └── data/                       # Fonte editável da página (mesmo conteúdo do pagina.h)
            ├── index.html              # Estrutura do painel de controle (HUD)
            ├── style.css               # Estilização visual futurista
            └── script.js               # Lógica HTTP, analógicos e teclado

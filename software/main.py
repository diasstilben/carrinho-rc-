from machine import Pin, PWM 
import time
import network
import socket

# ==================================================
#  1. DEFINIÇÃO DOS PINOS - MOTORES (LADO ESQUERDO)
# ==================================================

# PONTE H1 (LADO ESQUERDO)
ENA_ESQ = PWM( Pin(13), freq=1000)
IN1_ESQ = ( Pin(14), Pin.OUT)
IN2_ESQ = (Pin(27), Pin.OUT)
IN3_ESQ = (Pin(26), Pin.OUT)
IN4_ESQ = (Pin(25), Pin.OUT)
ENB_ESQ = PWM(Pin(33), freq=1000)

# =================================================
#  2. DEFINIÇÃO DOS PINOS - MOTORES (LADO DIREITO)
# =================================================

# PONTE H1 (LADO DIREITO)
ENA_DIR = PWM( Pin(32), freq=1000)
IN1_DIR = ( Pin(22), Pin.OUT)
IN2_DIR = (Pin(19), Pin.OUT)
IN3_DIR = (Pin(18), Pin.OUT)
IN4_DIR = (Pin(16), Pin.OUT)
ENB_DIR = PWM(Pin(17), freq=1000)

# ================
#  3. PERIFÉRICOS
# ================
TRIG = Pin(4, Pin.OUT)
ECHO = Pin(35, Pin.IN)
BUZZER = Pin(23, Pin.OUT)

# =========================
#  4. FUNÇÕES DE MOVIMENTO 
# =========================

# MOVER PARA FRENTE
def frente(velocidade = 1000):
    
    # ============================================================
    # ACELERADOR (Diz a ponte H quanta força enviar aos motores).
    # ============================================================
    ENA_ESQ.duty(velocidade)
    ENB_ESQ.duty(velocidade)
    ENA_DIR.duty(velocidade)
    ENB_DIR.duty(velocidade)
    
    
    # ==============================================
    # CÂMBIO (Escolher a direção do lado esquerdo).
    # ==============================================
    IN1_ESQ.value(1)
    IN2_ESQ.value(0)
    
    IN3_ESQ.value(1)
    IN4_ESQ.value(0)

    # =============================================
    # CÂMBIO (Escolher a direção do lado direito).
    # =============================================
    IN1_DIR.value(1)
    IN2_DIR.value(0) 
    
    IN3_DIR.value(1) 
    IN4_DIR.value(0) 
# MOVER PARA TRAS
def tras(velocidade = 1000):

    ENA_ESQ.duty(velocidade)
    ENB_ESQ.duty(velocidade)
    ENA_DIR.duty(velocidade)
    ENB_DIR.duty(velocidade)
     
    IN1_ESQ.value(0) 
    IN2_ESQ.value(1)  
    
    IN3_ESQ.value(0) 
    IN4_ESQ.value(1)

    IN1_DIR.value(0) 
    IN2_DIR.value(1) 

    IN3_DIR.value(0) 
    IN4_DIR.value(1)
# MOVER PARA ESQUERDA
def esquerda(velocidade = 1000):
    
    ENA_ESQ.dusty(velocidade)
    ENB_ESQ.dusty(velocidade)
    ENA_DIR.dusty(velocidade)
    ENB_ESQ.dusty(velocidade)

    IN1_ESQ.value(0)
    IN2_ESQ.value(1) # MOTOR DIANTEIRO ESQ EM RÉ
    IN3_ESQ.value(0)
    IN4_ESQ.value(1) # MOTOR TRASEIRO ESQ EM RÉ

    IN1_DIR.value(1) # MOTOR DIANTEIRO DIR PARA FRENTE
    IN2_DIR.value(0)
    IN3_DIR.value(1) # MOTOR TRASEIRO DIR PRA FRENTE
    IN4_DIR.value(0) 
# MOVER PARA DIREITA
def direita(velocidade=1000):
   
    ENA_ESQ.duty(velocidade)
    ENB_ESQ.duty(velocidade)
    ENA_DIR.duty(velocidade)
    ENB_DIR.duty(velocidade)
    
    IN1_ESQ.value(1)
    IN2_ESQ.value(0)
    IN3_ESQ.value(1)
    IN4_ESQ.value(0)
    
    
    IN1_DIR.value(0)
    IN2_DIR.value(1)
    IN3_DIR.value(0)
    IN4_DIR.value(1)
# PARAR O RC
def parar():
    
    ENA_ESQ.duty(0)
    ENB_ESQ.duty(0)
    ENA_DIR.duty(0)
    ENB_DIR.duty(0)
    
    IN1_ESQ.value(0)
    IN2_ESQ.value(0)
    IN3_ESQ.value(0)
    IN4_ESQ.value(0)
    
    IN1_DIR.value(0)
    IN2_DIR.value(0)
    IN3_DIR.value(0)
    IN4_DIR.value(0)

# =============================
# CONFIGURAÇÃO DA REDE DE WIFI
# =============================

# ==================
# . LOOP PRINCIPAL
# ==================

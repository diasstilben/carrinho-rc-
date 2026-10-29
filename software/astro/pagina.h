#ifndef PAGINA_H
#define PAGINA_H

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Astro RC</title>
  <style>
   /* =====================================================
   ASTRO 4WD - HUD CYBERPUNK / MECHA
   ===================================================== */
:root {
    --bg-0: #05070d;
    --bg-1: #0a1018;
    --metal: #121b27;
    --metal-2: #1b2735;
    --cyan: #00e5ff;
    --cyan-dim: rgba(0, 229, 255, 0.35);
    --lime: #b6ff00;
    --red: #ff2a4d;
    --orange: #ff8a00;
    --font-titulo: 'Orbitron', 'Eurostile', 'Bank Gothic', 'Segoe UI', sans-serif;
    --font-texto: 'Rajdhani', 'Segoe UI', Tahoma, sans-serif;
}

* { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

body {
    background-color: var(--bg-0);
    background-image:
        radial-gradient(ellipse at 50% 0%, rgba(0, 229, 255, 0.14) 0%, transparent 55%),
        radial-gradient(ellipse at 50% 100%, rgba(255, 42, 77, 0.08) 0%, transparent 50%),
        linear-gradient(to right, rgba(0, 229, 255, 0.05) 1px, transparent 1px),
        linear-gradient(to bottom, rgba(0, 229, 255, 0.05) 1px, transparent 1px);
    background-size: 100% 100%, 100% 100%, 36px 36px, 36px 36px;
    background-attachment: fixed;

    color: #d8f6ff;
    font-family: var(--font-texto);
    text-align: center;
    margin: 0;
    padding: 20px;
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    align-items: center;
    min-height: 100vh;
    overflow-x: hidden;
    position: relative;
    user-select: none;
    -webkit-user-select: none;
}

/* Linhas de varredura (scanlines) CRT */
body::before {
    content: "";
    position: fixed;
    inset: 0;
    pointer-events: none;
    z-index: 50;
    background: repeating-linear-gradient(
        to bottom,
        rgba(0, 0, 0, 0) 0px,
        rgba(0, 0, 0, 0) 2px,
        rgba(0, 0, 0, 0.18) 3px
    );
}

/* Vinheta nas bordas */
body::after {
    content: "";
    position: fixed;
    inset: 0;
    pointer-events: none;
    z-index: 49;
    box-shadow: inset 0 0 120px rgba(0, 0, 0, 0.85);
}

/* ================= CABEÇALHO ================= */
header { position: relative; z-index: 2; }

.hud-tag {
    font-family: var(--font-texto);
    font-size: 12px;
    letter-spacing: 4px;
    color: var(--lime);
    opacity: 0.85;
    margin-bottom: 4px;
}

h1 {
    font-family: var(--font-titulo);
    font-weight: 900;
    font-size: clamp(28px, 8vw, 44px);
    margin: 0 0 12px 0;
    color: var(--cyan);
    letter-spacing: 6px;
    text-transform: uppercase;
    text-shadow:
        0 0 6px var(--cyan),
        0 0 22px rgba(0, 229, 255, 0.55),
        0 0 48px rgba(0, 229, 255, 0.3);
    animation: flicker 6s infinite;
}

.status-box {
    position: relative;
    display: inline-block;
    background: linear-gradient(135deg, rgba(18, 27, 39, 0.9), rgba(10, 16, 24, 0.9));
    padding: 9px 22px;
    font-family: var(--font-texto);
    font-weight: 600;
    font-size: 15px;
    letter-spacing: 2px;
    text-transform: uppercase;
    margin-bottom: 30px;
    border: 1px solid var(--cyan-dim);
    clip-path: polygon(12px 0, 100% 0, 100% calc(100% - 12px), calc(100% - 12px) 100%, 0 100%, 0 12px);
    box-shadow: inset 0 0 18px rgba(0, 229, 255, 0.12);
}

.conectado,
.desconectado {
    font-weight: 700;
    letter-spacing: 2px;
}
.conectado::before,
.desconectado::before {
    content: "";
    display: inline-block;
    width: 9px;
    height: 9px;
    border-radius: 50%;
    margin-right: 8px;
    vertical-align: middle;
    background: currentColor;
    box-shadow: 0 0 8px currentColor;
}
.conectado    { color: var(--lime); text-shadow: 0 0 10px var(--lime); }
.desconectado { color: var(--red);  text-shadow: 0 0 10px var(--red); }
.conectado::before    { animation: pulse-dot 1.4s ease-in-out infinite; }
.desconectado::before { animation: pulse-dot 0.6s steps(2) infinite; }

/* ================= PAINEL DIRECIONAL ================= */
.d-pad {
    --tam: clamp(78px, 22vw, 96px);
    display: grid;
    grid-template-columns: repeat(3, var(--tam));
    grid-template-rows: repeat(3, var(--tam));
    gap: 12px;
    justify-content: center;
    padding: 26px;
    position: relative;
    z-index: 2;
    background:
        radial-gradient(circle at 50% 50%, rgba(0, 229, 255, 0.10), transparent 65%),
        linear-gradient(160deg, var(--metal-2), var(--metal) 60%, #0b121b);
    border: 1px solid var(--cyan-dim);
    box-shadow:
        0 0 40px rgba(0, 229, 255, 0.12),
        inset 0 0 40px rgba(0, 0, 0, 0.6);
    clip-path: polygon(24px 0, calc(100% - 24px) 0, 100% 24px, 100% calc(100% - 24px), calc(100% - 24px) 100%, 24px 100%, 0 calc(100% - 24px), 0 24px);
    touch-action: none;
}

/* Anel de radar atrás do direcional */
.d-pad::before {
    content: "";
    position: absolute;
    left: 50%;
    top: 50%;
    width: 78%;
    aspect-ratio: 1;
    transform: translate(-50%, -50%);
    border-radius: 50%;
    border: 1px dashed rgba(0, 229, 255, 0.22);
    box-shadow: 0 0 0 14px rgba(0, 229, 255, 0.03), inset 0 0 30px rgba(0, 229, 255, 0.06);
    pointer-events: none;
    animation: girar 40s linear infinite;
}

/* ================= BOTÕES BASE ================= */
.btn {
    position: relative;
    background:
        linear-gradient(160deg, rgba(27, 39, 53, 0.95), rgba(10, 16, 24, 0.95));
    color: var(--cyan);
    border: 1px solid var(--cyan);
    border-radius: 10px;
    font-family: var(--font-titulo);
    font-size: 15px;
    font-weight: 700;
    letter-spacing: 1px;
    text-shadow: 0 0 8px rgba(0, 229, 255, 0.8);
    cursor: pointer;
    user-select: none;
    -webkit-user-select: none;
    -webkit-touch-callout: none;
    touch-action: manipulation;
    transition: transform 0.08s ease, background 0.12s ease, box-shadow 0.12s ease, color 0.12s ease;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    box-shadow:
        0 0 10px rgba(0, 229, 255, 0.25),
        inset 0 0 14px rgba(0, 229, 255, 0.12),
        0 6px 14px rgba(0, 0, 0, 0.55);
}

/* Colchetes HUD nos cantos */
.btn::before,
.btn::after {
    content: "";
    position: absolute;
    width: 14px;
    height: 14px;
    pointer-events: none;
    border-color: var(--lime);
    border-style: solid;
    transition: border-color 0.12s ease;
}
.btn::before { top: -2px;    left: -2px;  border-width: 3px 0 0 3px; border-top-left-radius: 8px; }
.btn::after  { bottom: -2px; right: -2px; border-width: 0 3px 3px 0; border-bottom-right-radius: 8px; }

/* Estado ativo / pressionado */
.btn:active,
.btn.pressionado {
    background: linear-gradient(160deg, #19f0ff, #00a9c4);
    color: #021016;
    text-shadow: none;
    transform: scale(0.93);
    box-shadow:
        0 0 26px var(--cyan),
        0 0 60px rgba(0, 229, 255, 0.5),
        inset 0 0 18px rgba(255, 255, 255, 0.35);
}
.btn:active::before, .btn.pressionado::before,
.btn:active::after,  .btn.pressionado::after {
    border-color: #ffffff;
}

/* ================= BOTÃO PARAR ================= */
.btn-stop {
    background: radial-gradient(circle at 50% 35%, #ff4d6a, #a30f28 75%);
    color: #ffffff;
    border: 2px solid var(--red);
    border-radius: 50%;
    font-size: 30px;
    text-shadow: none;
    box-shadow:
        0 0 18px rgba(255, 42, 77, 0.6),
        inset 0 0 20px rgba(0, 0, 0, 0.35),
        0 6px 14px rgba(0, 0, 0, 0.55);
}
.btn-stop::before,
.btn-stop::after {
    width: auto;
    height: auto;
    border: 0;
    border-radius: 50%;
}
/* anel tracejado giratório */
.btn-stop::before {
    inset: -8px;
    border: 2px dashed rgba(255, 42, 77, 0.6);
    animation: girar 12s linear infinite reverse;
}
.btn-stop::after { display: none; }

.btn-stop:active,
.btn-stop.pressionado {
    background: radial-gradient(circle at 50% 35%, #ff8095, var(--red) 75%);
    color: #ffffff;
    box-shadow:
        0 0 30px var(--red),
        0 0 70px rgba(255, 42, 77, 0.65),
        inset 0 0 18px rgba(255, 255, 255, 0.3);
}
.btn-stop:active::before,
.btn-stop.pressionado::before { border-color: #ffffff; }

/* ================= BOTÃO BUZINA (ALERTA) ================= */
.buzina {
    grid-column: 3;
    grid-row: 1;
    color: #1a0600;
    border: 2px solid var(--orange);
    text-shadow: none;
    /* faixas de perigo (hazard) */
    background:
        repeating-linear-gradient(
            -45deg,
            #ffb300 0px,
            #ffb300 10px,
            #1a1200 10px,
            #1a1200 20px
        );
    animation: alerta 1.2s ease-in-out infinite;
}
.buzina::before,
.buzina::after { border-color: var(--red); }

/* placa central para legibilidade do ícone */
.buzina .icone-buzina,
.buzina .legenda-tecla { position: relative; z-index: 1; }

.icone-buzina {
    width: 32px;
    height: 32px;
    fill: currentColor;
    margin-bottom: 2px;
    color: #ffffff;
    filter: drop-shadow(0 0 3px #000) drop-shadow(0 0 6px var(--red));
}
.legenda-tecla {
    font-family: var(--font-titulo);
    font-size: 12px;
    font-weight: 900;
    color: #ffffff;
    padding: 0 6px;
    background: rgba(0, 0, 0, 0.75);
    border-radius: 3px;
    letter-spacing: 1px;
}

.buzina:active,
.buzina.pressionado {
    background: repeating-linear-gradient(-45deg, var(--red) 0px, var(--red) 10px, #5a0014 10px, #5a0014 20px);
    border-color: #ffffff;
    animation: strobe 0.18s steps(2) infinite;
    box-shadow:
        0 0 30px var(--red),
        0 0 80px rgba(255, 42, 77, 0.8),
        inset 0 0 20px rgba(255, 255, 255, 0.3);
}

/* ================= POSICIONAMENTO ================= */
.frente   { grid-column: 2; grid-row: 1; }
.esquerda { grid-column: 1; grid-row: 2; }
.parar    { grid-column: 2; grid-row: 2; }
.direita  { grid-column: 3; grid-row: 2; }
.tras     { grid-column: 2; grid-row: 3; }

/* ================= LEGENDA DE COMANDOS ================= */
.painel {
    display: flex;
    flex-wrap: wrap;            /* em telas estreitas a legenda sobe para cima do direcional */
    align-items: center;
    justify-content: center;
    gap: 22px;
    position: relative;
    z-index: 2;
}

.legenda {
    position: relative;
    width: 210px;
    padding: 18px 16px 14px;
    text-align: left;
    background:
        radial-gradient(circle at 0% 0%, rgba(0, 229, 255, 0.10), transparent 60%),
        linear-gradient(160deg, var(--metal-2), var(--metal) 60%, #0b121b);
    border: 1px solid var(--cyan-dim);
    box-shadow:
        0 0 30px rgba(0, 229, 255, 0.10),
        inset 0 0 30px rgba(0, 0, 0, 0.6);
    clip-path: polygon(18px 0, 100% 0, 100% calc(100% - 18px), calc(100% - 18px) 100%, 0 100%, 0 18px);
}

.legenda-titulo {
    font-family: var(--font-titulo);
    font-size: 12px;
    font-weight: 700;
    letter-spacing: 3px;
    color: var(--lime);
    text-shadow: 0 0 8px rgba(182, 255, 0, 0.6);
    padding-bottom: 8px;
    margin-bottom: 10px;
    border-bottom: 1px solid var(--cyan-dim);
}

.legenda ul {
    list-style: none;
    margin: 0;
    padding: 0;
    display: flex;
    flex-direction: column;
    gap: 8px;
}

.legenda li {
    display: flex;
    align-items: center;
    gap: 6px;
}

.tecla {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-width: 28px;
    height: 28px;
    padding: 0 6px;
    font-family: var(--font-titulo);
    font-size: 12px;
    font-weight: 700;
    color: var(--cyan);
    background: linear-gradient(160deg, rgba(27, 39, 53, 0.95), rgba(10, 16, 24, 0.95));
    border: 1px solid var(--cyan);
    border-radius: 5px;
    text-shadow: 0 0 6px rgba(0, 229, 255, 0.8);
    box-shadow: 0 0 8px rgba(0, 229, 255, 0.25), inset 0 0 8px rgba(0, 229, 255, 0.12);
}
.tecla-larga { font-size: 10px; letter-spacing: 1px; padding: 0 8px; }

.acao {
    margin-left: auto;
    font-family: var(--font-texto);
    font-weight: 700;
    font-size: 15px;
    letter-spacing: 2px;
    text-transform: uppercase;
    color: #d8f6ff;
}

/* Parar = vermelho / Buzina = laranja (mesmas cores dos botões) */
.item-parar .tecla  { color: #fff; border-color: var(--red); background: linear-gradient(160deg, #a30f28, #5a0014); text-shadow: none; box-shadow: 0 0 8px rgba(255, 42, 77, 0.5); }
.item-parar .acao   { color: var(--red); text-shadow: 0 0 8px rgba(255, 42, 77, 0.6); }
.item-buzina .tecla { color: #1a0600; border-color: var(--orange); background: linear-gradient(160deg, #ffb300, var(--orange)); text-shadow: none; box-shadow: 0 0 8px rgba(255, 138, 0, 0.6); }
.item-buzina .acao  { color: var(--orange); text-shadow: 0 0 8px rgba(255, 138, 0, 0.6); }

.legenda-nota {
    margin-top: 12px;
    padding-top: 8px;
    border-top: 1px dashed var(--cyan-dim);
    font-size: 12px;
    line-height: 1.35;
    letter-spacing: 1px;
    color: #7fa3b3;
    text-transform: uppercase;
}

@media (max-width: 380px) {
    .legenda { width: 100%; max-width: 300px; }
}

/* ================= RODAPÉ ================= */
footer {
    position: relative;
    z-index: 2;
    margin-top: 30px;
    color: #7fa3b3;
    font-family: var(--font-texto);
    font-size: 13px;
    letter-spacing: 1.5px;
    border-top: 1px solid var(--cyan-dim);
    padding-top: 12px;
    width: 100%;
    max-width: 520px;
}
footer p { margin: 4px 0; }
footer strong { color: var(--cyan); font-weight: 600; }

/* ================= ANIMAÇÕES ================= */
@keyframes girar {
    from { transform: translate(-50%, -50%) rotate(0deg); }
    to   { transform: translate(-50%, -50%) rotate(360deg); }
}
/* o anel do botão stop usa inset, então precisa de rotação simples */
.btn-stop::before { animation-name: girar-simples; }
@keyframes girar-simples {
    from { transform: rotate(0deg); }
    to   { transform: rotate(360deg); }
}

@keyframes pulse-dot {
    0%, 100% { opacity: 1; }
    50%      { opacity: 0.25; }
}

@keyframes flicker {
    0%, 92%, 100% { opacity: 1; }
    93% { opacity: 0.7; }
    94% { opacity: 1; }
    96% { opacity: 0.8; }
}

@keyframes alerta {
    0%, 100% { box-shadow: 0 0 10px rgba(255, 138, 0, 0.5), inset 0 0 10px rgba(0, 0, 0, 0.4), 0 6px 14px rgba(0, 0, 0, 0.55); }
    50%      { box-shadow: 0 0 26px rgba(255, 138, 0, 0.95), 0 0 50px rgba(255, 42, 77, 0.45), inset 0 0 10px rgba(0, 0, 0, 0.4), 0 6px 14px rgba(0, 0, 0, 0.55); }
}

@keyframes strobe {
    0%   { filter: brightness(1); }
    100% { filter: brightness(1.7); }
}

/* Respeita quem prefere menos animação */
@media (prefers-reduced-motion: reduce) {
    *, *::before, *::after { animation: none !important; }
}

/* Telas muito pequenas */
@media (max-width: 380px) {
    .d-pad { padding: 18px; gap: 9px; }
    .btn { font-size: 13px; }
}
  </style>
</head>
<body>

  <!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <title>Astro - Painel de Controle</title>
    <!-- Fontes sci-fi (opcional: precisam de internet no celular; sem elas o CSS usa fontes de reserva) -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@500;700;900&family=Rajdhani:wght@500;600;700&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <header>
        <div class="hud-tag">// SISTEMA DE CONTROLE TÁTICO</div>
        <h1>Astro 4WD</h1>
        <div class="status-box">
            Status da Conexão: <span id="status-texto" class="desconectado">Buscando Astro...</span>
        </div>
    </header>

    <div class="painel">
    <!-- Legenda dos comandos (apenas informativa) -->
    <aside class="legenda">
        <div class="legenda-titulo">// COMANDOS</div>
        <ul>
            <li><span class="tecla">W</span><span class="tecla">▲</span><span class="acao">Frente</span></li>
            <li><span class="tecla">S</span><span class="tecla">▼</span><span class="acao">Trás</span></li>
            <li><span class="tecla">A</span><span class="tecla">◀</span><span class="acao">Esquerda</span></li>
            <li><span class="tecla">D</span><span class="tecla">▶</span><span class="acao">Direita</span></li>
            <li class="item-parar"><span class="tecla tecla-larga">ESPAÇO</span><span class="acao">Parar</span></li>
            <li class="item-buzina"><span class="tecla">F</span><span class="acao">Buzina</span></li>
        </ul>
        <div class="legenda-nota">Segure para mover.<br>Solte para parar.</div>
    </aside>

    <main class="d-pad">
        <div class="btn frente" id="btn-F" onmousedown="comando('F')" onmouseup="comando('S')" ontouchstart="comando('F')" ontouchend="comando('S')">W / ▲</div>
        
        <div class="btn esquerda" id="btn-L" onmousedown="comando('L')" onmouseup="comando('S')" ontouchstart="comando('L')" ontouchend="comando('S')">A / ◀</div>
        
        <!-- Barra de espaço aciona o Parar central -->
        <div class="btn btn-stop parar" id="btn-S" onmousedown="comando('S')" ontouchstart="comando('S')">🛑</div>
        
        <div class="btn direita" id="btn-R" onmousedown="comando('R')" onmouseup="comando('S')" ontouchstart="comando('R')" ontouchend="comando('S')">D / ▶</div>
        
        <div class="btn tras" id="btn-B" onmousedown="comando('B')" onmouseup="comando('S')" ontouchstart="comando('B')" ontouchend="comando('S')">S / ▼</div>

        <!-- Botão da Buzina (Tecla F / Ícone Vetorial Elegante) -->
        <div class="btn buzina" id="btn-H" onmousedown="comando('H')" onmouseup="comando('Q')" ontouchstart="comando('H')" ontouchend="comando('Q')">
            <svg class="icone-buzina" viewBox="0 0 24 24">
                <path d="M3 9v6h4l5 5V4L7 9H3zm13.5 3c0-1.77-1.02-3.29-2.5-4.03v8.05c1.48-.73 2.5-2.25 2.5-4.02zM14 3.23v2.06c2.89.86 5 3.54 5 6.71s-2.11 5.85-5 6.71v2.06c4.01-.91 7-4.49 7-8.77s-2.99-7.86-7-8.77z"/>
            </svg>
            <span class="legenda-tecla">F</span>
        </div>
    </main>
    </div>

    <footer>
        <p>Projeto Astro - Desenvolvido por <strong>João Dias Stilben Teixeira</strong></p>
        <p>Acesse meu GitHub em <strong> https://github.com/diasstilben</strong></p>
    </footer>

    <script src="script.js"></script>
</body>
</html>

  <script>
    function atualizarStatus(conectado) {
    let statusText = document.getElementById('status-texto');
    if(conectado) {
        statusText.innerText = 'Online';
        statusText.className = 'conectado';
    } else {
        statusText.innerText = 'Desconectado';
        statusText.className = 'desconectado';
    }
}

function comando(cmd) {
    fetch('/' + cmd, { signal: AbortSignal.timeout(1000) })
        .then(response => {
            if(response.ok) atualizarStatus(true);
        })
        .catch(e => {
            atualizarStatus(false);
        });
}

setInterval(() => { comando('ping'); }, 1500);

// =====================================
// LÓGICA DO TECLADO (W, A, S, D, Espaço para Freio e F para Buzina)
// =====================================
let travado_movimento = false;
let travado_buzina = false;

document.addEventListener('keydown', (e) => {
    let tecla = e.key.toLowerCase();
    
    // Movimento (W, A, S, D ou Setas)
    if (!travado_movimento) {
        if (tecla === 'arrowup' || tecla === 'w') { comando('F'); destacar('btn-F'); travado_movimento = true; }
        if (tecla === 'arrowdown' || tecla === 's') { comando('B'); destacar('btn-B'); travado_movimento = true; }
        if (tecla === 'arrowleft' || tecla === 'a') { comando('L'); destacar('btn-L'); travado_movimento = true; }
        if (tecla === 'arrowright' || tecla === 'd') { comando('R'); destacar('btn-R'); travado_movimento = true; }
    }
    
    // Barra de Espaço aciona o FREIO (Stop)
    if (tecla === ' ') {
        comando('S');
        destacar('btn-S');
        e.preventDefault(); // Evita rolar a página para baixo
    }

    // Tecla F aciona a BUZINA (Independente do movimento)
    if (tecla === 'f' && !travado_buzina) { 
        comando('H'); 
        destacar('btn-H'); 
        travado_buzina = true; 
    }
});

document.addEventListener('keyup', (e) => {
    let tecla = e.key.toLowerCase();
    let teclasMovimento = ['arrowup', 'arrowdown', 'arrowleft', 'arrowright', 'w', 'a', 's', 'd'];
    
    if (teclasMovimento.includes(tecla)) {
        comando('S'); // Para o motor ao soltar a tecla de direção
        removerDestaqueMovimento();
        travado_movimento = false;
    }
    
    // Soltar a barra de espaço remove o destaque do botão de stop
    if (tecla === ' ') {
        document.getElementById('btn-S').classList.remove('pressionado');
    }
    
    // Desliga a buzina ao soltar a tecla F
    if (tecla === 'f') {
        comando('Q'); // Q de Quiet / Silêncio
        document.getElementById('btn-H').classList.remove('pressionado');
        travado_buzina = false;
    }
});

function destacar(idElemento) {
    let el = document.getElementById(idElemento);
    if(el) el.classList.add('pressionado');
}

function removerDestaqueMovimento() {
    let ids = ['btn-F', 'btn-B', 'btn-L', 'btn-R'];
    for(let id of ids) {
        let el = document.getElementById(id);
        if(el) el.classList.remove('pressionado');
    }
}

  </script>
</body>
</html>
)rawliteral";

#endif
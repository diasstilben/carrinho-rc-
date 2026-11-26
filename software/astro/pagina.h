#ifndef PAGINA_H
#define PAGINA_H

#include <pgmspace.h>

// Pagina de controle servida pelo ESP32 (HTML + CSS + JS num arquivo so).
// Montada a partir de data/index.html, data/style.css e data/script.js:
// ao mudar a pagina, edite os arquivos em data/ e atualize este arquivo tambem.
const char index_html[] PROGMEM = R"rawliteral(
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
    min-height: 100dvh;
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

/* ================= CONTROLES ANALÓGICOS ================= */
.controles {
    --stick: min(clamp(130px, 30vw, 210px), 52vh);
    --stick: min(clamp(130px, 30vw, 210px), 52dvh);
    --botao: clamp(64px, 16vw, 82px);
    display: grid;
    grid-template-columns: auto auto auto;
    grid-template-areas: "esq centro dir";
    align-items: center;
    justify-content: center;
    gap: 26px;
    padding: 22px 28px;
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

.stick-area {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 10px;
    touch-action: none;   /* a coluna inteira (rótulo + analógico + valor) pega o dedo */
    -webkit-touch-callout: none;
}
.stick-esq { grid-area: esq; }
.stick-dir { grid-area: dir; }

.stick-rotulo {
    font-family: var(--font-titulo);
    font-size: 11px;
    font-weight: 700;
    letter-spacing: 3px;
    color: var(--lime);
    text-shadow: 0 0 8px rgba(182, 255, 0, 0.6);
    white-space: nowrap;
}

.stick-valor {
    font-family: var(--font-titulo);
    font-size: 15px;
    font-weight: 700;
    letter-spacing: 1px;
    min-width: 6ch;
    color: var(--cyan);
    text-shadow: 0 0 8px rgba(0, 229, 255, 0.7);
}
.stick-valor.re { color: var(--red); text-shadow: 0 0 8px rgba(255, 42, 77, 0.7); }

/* Base do analógico */
.joystick {
    position: relative;
    width: var(--stick);
    height: var(--stick);
    border-radius: 50%;
    background: radial-gradient(circle, rgba(0, 229, 255, 0.07), rgba(5, 7, 13, 0.92) 72%);
    border: 1px solid var(--cyan-dim);
    box-shadow:
        inset 0 0 30px rgba(0, 0, 0, 0.85),
        0 0 24px rgba(0, 229, 255, 0.12);
    touch-action: none;  /* o dedo arrasta o analógico, não a página */
    cursor: grab;
    -webkit-touch-callout: none;
}
.joystick.ativo { cursor: grabbing; border-color: var(--cyan); }

/* Trilho: mostra para onde a manopla anda */
.joystick::before {
    content: "";
    position: absolute;
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
    border-radius: 999px;
    border: 1px dashed rgba(0, 229, 255, 0.35);
    background: rgba(0, 229, 255, 0.05);
    pointer-events: none;
}
.joystick.vertical::before   { width: 30%; height: 88%; }
.joystick.horizontal::before { width: 88%; height: 30%; }

/* Anel de radar girando */
.joystick::after {
    content: "";
    position: absolute;
    inset: 7%;
    border-radius: 50%;
    border: 1px dashed rgba(0, 229, 255, 0.16);
    pointer-events: none;
    animation: girar-simples 40s linear infinite;
}

/* Manopla (a "bolinha" que o dedo arrasta) */
.manopla {
    position: absolute;
    left: 50%;
    top: 50%;
    width: 38%;
    height: 38%;
    margin-left: -19%;
    margin-top: -19%;
    border-radius: 50%;
    z-index: 1;
    pointer-events: none;
    background: radial-gradient(circle at 40% 35%, #2b3d52, #0d1520 72%);
    border: 2px solid var(--cyan);
    box-shadow:
        0 0 14px rgba(0, 229, 255, 0.45),
        inset 0 0 14px rgba(0, 229, 255, 0.2),
        0 6px 14px rgba(0, 0, 0, 0.6);
    transition: transform 0.15s ease-out, background 0.12s ease, box-shadow 0.12s ease; /* volta suave ao centro */
}
/* anel interno (textura de "pegada") */
.manopla::after {
    content: "";
    position: absolute;
    inset: 22%;
    border-radius: 50%;
    border: 2px solid rgba(0, 229, 255, 0.45);
}
.joystick.ativo .manopla {
    transition: none; /* segue o dedo sem atraso */
    background: radial-gradient(circle at 40% 35%, #19f0ff, #00a9c4 75%);
    box-shadow:
        0 0 26px var(--cyan),
        0 0 50px rgba(0, 229, 255, 0.5),
        inset 0 0 14px rgba(255, 255, 255, 0.35);
}
.joystick.ativo .manopla::after { border-color: rgba(255, 255, 255, 0.7); }

/* Coluna do meio: parar, buzina e telemetria */
.centro {
    grid-area: centro;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 18px;
}
.centro .btn {
    width: var(--botao);
    height: var(--botao);
}

.telemetria {
    display: flex;
    flex-direction: column;
    gap: 6px;
    width: calc(var(--botao) + 20px);
}
.tele-lado {
    display: flex;
    align-items: center;
    gap: 6px;
    font-family: var(--font-titulo);
    font-size: 10px;
    font-weight: 700;
    color: #7fa3b3;
}
.barra {
    position: relative;
    flex: 1;
    height: 8px;
    background: rgba(0, 0, 0, 0.6);
    border: 1px solid var(--cyan-dim);
    border-radius: 2px;
    overflow: hidden;
}
/* risco do zero no meio da barra */
.barra::before {
    content: "";
    position: absolute;
    left: 50%;
    top: 0;
    bottom: 0;
    width: 1px;
    background: rgba(216, 246, 255, 0.5);
}
.barra-fill {
    position: absolute;
    top: 0;
    bottom: 0;
    left: 50%;
    width: 0;
    background: var(--cyan);
    box-shadow: 0 0 6px var(--cyan);
}
.barra-fill.re { background: var(--red); box-shadow: 0 0 6px var(--red); }

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
    touch-action: none; /* evita rolagem/zoom ao segurar o botão */
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

/* Celular em pé: parar/buzina em cima, um analógico em cada polegar embaixo */
@media (max-width: 640px) {
    body { padding: 12px 10px; }
    .status-box { margin-bottom: 18px; }
    .controles {
        --stick: min(43vw, 230px);
        --botao: 76px;
        grid-template-columns: auto auto;
        grid-template-areas:
            "centro centro"
            "esq    dir";
        gap: 18px 8px;
        padding: 18px 8px;
    }
    .centro { flex-direction: row; gap: 16px; }
    .telemetria { width: 90px; }
    .stick-rotulo { letter-spacing: 2px; }
}

/* Celular estreito (iPhone SE, Androids pequenos) */
@media (max-width: 360px) {
    body { padding: 10px 6px; }
    .controles { --stick: 42vw; --botao: 66px; gap: 16px 6px; padding: 16px 6px; }
    .stick-rotulo { font-size: 10px; letter-spacing: 1px; }
    .status-box { padding: 8px 14px; font-size: 13px; letter-spacing: 1px; }
}

/* Celular deitado: tela baixa, então encolhe o cabeçalho */
@media (max-height: 500px) and (orientation: landscape) {
    body { padding: 8px; }
    .hud-tag { display: none; }
    /* título e status lado a lado, numa linha só, para sobrar altura */
    header { display: flex; align-items: center; justify-content: center; gap: 16px; margin-bottom: 8px; }
    h1 { font-size: 22px; margin: 0; letter-spacing: 4px; }
    .status-box { margin: 0; padding: 6px 16px; font-size: 13px; }
    footer { margin-top: 14px; }
    .controles {
        --stick: min(34vw, calc(100vh - 140px));
        --stick: min(34vw, calc(100dvh - 140px)); /* desconta a barra de endereço */
        --botao: 66px;
        gap: 30px;
        padding: 12px 28px;
    }
    .centro { gap: 10px; }
    .stick-area { gap: 6px; }
}

/* Só toque (sem teclado/mouse): a legenda de teclas não serve, esconde */
@media (hover: none) and (pointer: coarse) {
    .legenda { display: none; }
}
    </style>
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
            <li><span class="tecla">W</span><span class="tecla">▲</span><span class="acao">Acelera</span></li>
            <li><span class="tecla">S</span><span class="tecla">▼</span><span class="acao">Ré</span></li>
            <li><span class="tecla">A</span><span class="tecla">◀</span><span class="acao">Esquerda</span></li>
            <li><span class="tecla">D</span><span class="tecla">▶</span><span class="acao">Direita</span></li>
            <li class="item-parar"><span class="tecla tecla-larga">ESPAÇO</span><span class="acao">Parar</span></li>
            <li class="item-buzina"><span class="tecla">F</span><span class="acao">Buzina</span></li>
        </ul>
        <div class="legenda-nota">W + A / W + D faz curva.<br>No celular, use os analógicos.</div>
    </aside>

    <main class="controles">
        <!-- Analógico esquerdo: acelerador (só eixo vertical) -->
        <div class="stick-area stick-esq">
            <div class="stick-rotulo">// ACELERADOR</div>
            <div class="joystick vertical" id="stick-acel">
                <div class="manopla"></div>
            </div>
            <div class="stick-valor" id="valor-acel">0%</div>
        </div>

        <div class="centro">
            <!-- Barra de espaço também aciona o Parar -->
            <div class="btn btn-stop" id="btn-S">🛑</div>

            <!-- Botão da Buzina (Tecla F) -->
            <div class="btn buzina" id="btn-H">
                <svg class="icone-buzina" viewBox="0 0 24 24">
                    <path d="M3 9v6h4l5 5V4L7 9H3zm13.5 3c0-1.77-1.02-3.29-2.5-4.03v8.05c1.48-.73 2.5-2.25 2.5-4.02zM14 3.23v2.06c2.89.86 5 3.54 5 6.71s-2.11 5.85-5 6.71v2.06c4.01-.91 7-4.49 7-8.77s-2.99-7.86-7-8.77z"/>
                </svg>
                <span class="legenda-tecla">F</span>
            </div>

            <!-- Potência que está indo para cada lado do carrinho -->
            <div class="telemetria">
                <div class="tele-lado"><span>E</span><div class="barra"><div class="barra-fill" id="barra-esq"></div></div></div>
                <div class="tele-lado"><span>D</span><div class="barra"><div class="barra-fill" id="barra-dir"></div></div></div>
            </div>
        </div>

        <!-- Analógico direito: direção (só eixo horizontal) -->
        <div class="stick-area stick-dir">
            <div class="stick-rotulo">// DIREÇÃO</div>
            <div class="joystick horizontal" id="stick-dir">
                <div class="manopla"></div>
            </div>
            <div class="stick-valor" id="valor-dir">0%</div>
        </div>
    </main>
    </div>

    <footer>
        <p>Projeto Astro - Desenvolvido por <strong>João Dias Stilben Teixeira</strong></p>
        <p>Acesse meu GitHub em <strong> https://github.com/diasstilben</strong></p>
    </footer>

    <script>
// =====================================
// CONEXÃO COM O ESP32
// =====================================
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

// Envia um comando ao ESP32. Sempre resolve (nunca rejeita), mesmo com erro.
function comando(cmd) {
    return fetch('/' + cmd, { signal: AbortSignal.timeout(1000) })
        .then(response => {
            if(response.ok) atualizarStatus(true);
        })
        .catch(e => {
            atualizarStatus(false);
        });
}

setInterval(() => { comando('ping'); }, 1500);

// =====================================
// AJUSTES DO CONTROLE
// =====================================
// Enquanto o carrinho anda (ou a buzina toca), o comando é repetido a cada
// INTERVALO_REPETICAO ms. O ESP32 desliga tudo sozinho se ficar 500ms sem
// receber a repetição: se a conexão cair, o carrinho para em vez de seguir andando.
const INTERVALO_REPETICAO = 150;

// Folga no centro do analógico (0.12 = 12% do curso): evita que o carrinho
// fique "arrastando" quando o dedo está quase no meio.
const ZONA_MORTA = 0.12;

// Na ré, virar para a direita leva a TRASEIRA para a direita, como num carro
// de verdade. Mude para false se preferir o jeito "tanque" (inverte).
const RE_COMO_CARRO = true;

// Pelo teclado, W + A/D faz uma curva aberta em vez de girar no lugar.
// (0 = nem vira, 1 = curva bem fechada)
const CURVA_TECLADO = 0.6;

// =====================================
// ESTADO
// =====================================
let analogico = { acel: 0, dir: 0 };   // -1..1 vindo dos analógicos da tela
let teclasSeguras = new Set();         // 'F', 'B', 'L', 'R' seguras no teclado
let motores = { esq: 0, dir: 0 };      // potência calculada: -1023 (ré) .. 1023 (frente)
let enviado = { esq: 0, dir: 0 };      // último valor mandado ao ESP32
let envioPendente = false;
let buzinaLigada = false;

function limitar(v) { return Math.max(-1, Math.min(1, v)); }

// Junta acelerador + direção numa potência para cada lado do carrinho
// (mistura "arcade" para tração diferencial, igual tanque/skid-steer).
function calcularMotores() {
    let tecladoY = (teclasSeguras.has('F') ? 1 : 0) - (teclasSeguras.has('B') ? 1 : 0);
    let tecladoX = (teclasSeguras.has('R') ? 1 : 0) - (teclasSeguras.has('L') ? 1 : 0);
    if (tecladoY !== 0) tecladoX *= CURVA_TECLADO;

    let y = limitar(analogico.acel + tecladoY);   // + frente / - ré
    let x = limitar(analogico.dir + tecladoX);    // + direita / - esquerda
    if (RE_COMO_CARRO && y < 0) x = -x;

    let esq = y + x;
    let dir = y - x;
    // Se algum lado passou de 100%, reduz os dois na mesma proporção
    let maior = Math.max(1, Math.abs(esq), Math.abs(dir));
    // Arredonda em passos de 2% para não mandar comando a cada pixel
    let passo = v => Math.round(v / maior * 50) / 50;
    return {
        y, x,
        esq: Math.round(passo(esq) * 1023),
        dir: Math.round(passo(dir) * 1023)
    };
}

function atualizarMotores() {
    let r = calcularMotores();
    motores = { esq: r.esq, dir: r.dir };
    mostrarTelemetria(r);
    enviarMotores(false);
}

// Manda a potência atual. Só um pedido por vez: se o valor mudar enquanto
// espera a resposta, o mais novo é mandado assim que ela chegar.
function enviarMotores(repetir) {
    if (envioPendente) return;
    let { esq, dir } = motores;
    let mudou = esq !== enviado.esq || dir !== enviado.dir;
    let andando = esq !== 0 || dir !== 0;
    if (!mudou && !(repetir && andando)) return;

    envioPendente = true;
    enviado = { esq, dir };
    comando(andando ? `M?e=${esq}&d=${dir}` : 'S').finally(() => {
        envioPendente = false;
        enviarMotores(false);
    });
}

setInterval(() => {
    enviarMotores(true);
    if (buzinaLigada) comando('H');
}, INTERVALO_REPETICAO);

// =====================================
// TELEMETRIA NA TELA
// =====================================
function mostrarBarra(id, valor) {
    let el = document.getElementById(id);
    let largura = Math.abs(valor) / 1023 * 50;   // metade da barra = 100%
    el.style.width = largura + '%';
    el.style.left = (valor >= 0 ? 50 : 50 - largura) + '%';
    el.classList.toggle('re', valor < 0);
}

function mostrarTelemetria(r) {
    let acel = document.getElementById('valor-acel');
    acel.innerText = (r.y > 0 ? '▲ ' : r.y < 0 ? '▼ ' : '') + Math.round(Math.abs(r.y) * 100) + '%';
    acel.classList.toggle('re', r.y < 0);

    // mostra a direção como o usuário mexeu (antes da inversão da ré)
    let xTela = (RE_COMO_CARRO && r.y < 0) ? -r.x : r.x;
    document.getElementById('valor-dir').innerText =
        (xTela < 0 ? '◀ ' : '') + Math.round(Math.abs(xTela) * 100) + '%' + (xTela > 0 ? ' ▶' : '');

    mostrarBarra('barra-esq', r.esq);
    mostrarBarra('barra-dir', r.dir);

    // Pelo teclado, a manopla também se mexe para mostrar o comando
    let tecladoY = (teclasSeguras.has('F') ? 1 : 0) - (teclasSeguras.has('B') ? 1 : 0);
    let tecladoX = (teclasSeguras.has('R') ? 1 : 0) - (teclasSeguras.has('L') ? 1 : 0);
    stickAcel.mostrar(-tecladoY);
    stickDir.mostrar(tecladoX);
}

// =====================================
// ANALÓGICOS DA TELA (mouse e toque)
// =====================================
// eixo 'y' = só sobe/desce (acelerador), 'x' = só esquerda/direita (direção).
// Cada analógico segue o seu próprio dedo, então dá para usar os dois juntos.
function criarAnalogico(id, eixo, aoMover) {
    let base = document.getElementById(id);
    let manopla = base.querySelector('.manopla');
    // Área de toque: a coluna inteira (rótulo + analógico + valor), não só o
    // círculo, para o polegar não "errar" o analógico no celular
    let zona = base.closest('.stick-area');
    let dedo = null;

    function curso() {
        // até onde o centro da manopla pode ir a partir do centro da base
        return (base.clientWidth - manopla.offsetWidth) / 2;
    }

    function posicionar(px) {
        manopla.style.transform = eixo === 'y' ? `translate(0, ${px}px)` : `translate(${px}px, 0)`;
    }

    function mover(e) {
        let r = base.getBoundingClientRect();
        // 100% é com o dedo perto da borda do círculo: curso maior = controle mais fino
        let alcance = r.width * 0.42;
        let d = eixo === 'y'
            ? e.clientY - (r.top + r.height / 2)
            : e.clientX - (r.left + r.width / 2);
        let v = Math.max(-1, Math.min(1, d / alcance));   // -1..1
        posicionar(v * curso());

        if (Math.abs(v) < ZONA_MORTA) v = 0;
        else v = Math.sign(v) * (Math.abs(v) - ZONA_MORTA) / (1 - ZONA_MORTA);
        aoMover(eixo === 'y' ? -v : v);   // para cima = frente
    }

    function soltar() {
        if (dedo === null) return;
        if (zona.hasPointerCapture(dedo)) zona.releasePointerCapture(dedo);
        dedo = null;
        base.classList.remove('ativo');
        posicionar(0);   // volta ao centro (a transição do CSS deixa suave)
        aoMover(0);
    }

    zona.addEventListener('pointerdown', (e) => {
        if (dedo !== null) return;   // já tem um dedo nesse analógico
        e.preventDefault();
        dedo = e.pointerId;
        // Continua recebendo o movimento mesmo se o dedo sair da área
        zona.setPointerCapture(dedo);
        base.classList.add('ativo');
        mover(e);
    });
    zona.addEventListener('pointermove', (e) => {
        if (e.pointerId === dedo) mover(e);
    });
    let fim = (e) => { if (e.pointerId === dedo) soltar(); };
    zona.addEventListener('pointerup', fim);
    zona.addEventListener('pointercancel', fim);
    zona.addEventListener('lostpointercapture', fim);
    zona.addEventListener('contextmenu', (e) => e.preventDefault());

    return {
        soltar,
        // usado pelo teclado: mostra a posição sem ninguém tocar
        mostrar(v) { if (dedo === null) posicionar(v * curso()); }
    };
}

let stickAcel = criarAnalogico('stick-acel', 'y', (v) => { analogico.acel = v; atualizarMotores(); });
let stickDir  = criarAnalogico('stick-dir',  'x', (v) => { analogico.dir = v;  atualizarMotores(); });

// =====================================
// PARAR E BUZINA
// =====================================
function pararMovimento() {
    teclasFisicas = {};
    teclasSeguras.clear();
    stickAcel.soltar();
    stickDir.soltar();
    analogico = { acel: 0, dir: 0 };
    atualizarMotores();
    comando('S');   // manda já, sem esperar a fila
}

function ligarBuzina() {
    if (buzinaLigada) return;
    buzinaLigada = true;
    destacar('btn-H');
    comando('H');
}

function desligarBuzina() {
    if (!buzinaLigada) return;
    buzinaLigada = false;
    removerDestaque('btn-H');
    comando('Q'); // Q de Quiet / Silêncio
}

// Se a página perder o foco (trocou de aba/app, tela apagou), para tudo:
// nesses casos o navegador nunca entrega o "soltar" da tecla ou do dedo.
function pararTudo() {
    pararMovimento();
    desligarBuzina();
}
window.addEventListener('blur', pararTudo);
document.addEventListener('visibilitychange', () => {
    if (document.hidden) pararTudo();
});

function destacar(idElemento) {
    let el = document.getElementById(idElemento);
    if(el) el.classList.add('pressionado');
}

function removerDestaque(idElemento) {
    let el = document.getElementById(idElemento);
    if(el) el.classList.remove('pressionado');
}

// Botões de segurar (pointer = mouse e dedo do mesmo jeito)
function ligarBotao(id, aoPressionar, aoSoltar) {
    let el = document.getElementById(id);
    let pressionado = false;

    el.addEventListener('pointerdown', (e) => {
        e.preventDefault();
        el.setPointerCapture(e.pointerId);
        pressionado = true;
        aoPressionar();
    });

    let aoLiberar = () => {
        if (!pressionado) return; // pointerup e lostpointercapture chegam juntos
        pressionado = false;
        aoSoltar();
    };
    el.addEventListener('pointerup', aoLiberar);
    el.addEventListener('pointercancel', aoLiberar);
    el.addEventListener('lostpointercapture', aoLiberar);
    el.addEventListener('contextmenu', (e) => e.preventDefault());
}

ligarBotao('btn-S', () => { pararMovimento(); destacar('btn-S'); }, () => removerDestaque('btn-S'));
ligarBotao('btn-H', ligarBuzina, desligarBuzina);

// =====================================
// TECLADO (W, A, S, D / setas, Espaço para Freio e F para Buzina)
// =====================================
const TECLAS_MOVIMENTO = {
    'w': 'F', 'arrowup': 'F',
    's': 'B', 'arrowdown': 'B',
    'a': 'L', 'arrowleft': 'L',
    'd': 'R', 'arrowright': 'R'
};
let teclasFisicas = {};   // tecla -> comando, para W e ▲ não brigarem

function recalcularTeclas() {
    teclasSeguras = new Set(Object.values(teclasFisicas));
    atualizarMotores();
}

document.addEventListener('keydown', (e) => {
    let tecla = e.key.toLowerCase();
    let cmd = TECLAS_MOVIMENTO[tecla];

    if (cmd || tecla === ' ') e.preventDefault(); // Evita rolar a página
    // Ignora a auto-repetição do teclado: quem repete o comando é o setInterval.
    // (Assim o freio funciona mesmo com a tecla de movimento ainda segura.)
    if (e.repeat) return;

    if (cmd) {
        teclasFisicas[tecla] = cmd;
        recalcularTeclas();
    }

    // Barra de Espaço aciona o FREIO (Stop)
    if (tecla === ' ') {
        pararMovimento();
        destacar('btn-S');
    }

    // Tecla F aciona a BUZINA (Independente do movimento)
    if (tecla === 'f') ligarBuzina();
});

document.addEventListener('keyup', (e) => {
    let tecla = e.key.toLowerCase();

    if (TECLAS_MOVIMENTO[tecla] && tecla in teclasFisicas) {
        delete teclasFisicas[tecla];
        recalcularTeclas();
    }

    if (tecla === ' ') removerDestaque('btn-S');
    if (tecla === 'f') desligarBuzina();
});

    </script>
</body>
</html>
)rawliteral";

#endif

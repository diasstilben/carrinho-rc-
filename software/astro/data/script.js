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
        let c = curso();
        let d = eixo === 'y'
            ? e.clientY - (r.top + r.height / 2)
            : e.clientX - (r.left + r.width / 2);
        d = Math.max(-c, Math.min(c, d));
        posicionar(d);

        let v = d / c;   // -1..1
        if (Math.abs(v) < ZONA_MORTA) v = 0;
        else v = Math.sign(v) * (Math.abs(v) - ZONA_MORTA) / (1 - ZONA_MORTA);
        aoMover(eixo === 'y' ? -v : v);   // para cima = frente
    }

    function soltar() {
        if (dedo === null) return;
        if (base.hasPointerCapture(dedo)) base.releasePointerCapture(dedo);
        dedo = null;
        base.classList.remove('ativo');
        posicionar(0);   // volta ao centro (a transição do CSS deixa suave)
        aoMover(0);
    }

    base.addEventListener('pointerdown', (e) => {
        if (dedo !== null) return;   // já tem um dedo nesse analógico
        e.preventDefault();
        dedo = e.pointerId;
        // Continua recebendo o movimento mesmo se o dedo sair da base
        base.setPointerCapture(dedo);
        base.classList.add('ativo');
        mover(e);
    });
    base.addEventListener('pointermove', (e) => {
        if (e.pointerId === dedo) mover(e);
    });
    let fim = (e) => { if (e.pointerId === dedo) soltar(); };
    base.addEventListener('pointerup', fim);
    base.addEventListener('pointercancel', fim);
    base.addEventListener('lostpointercapture', fim);
    base.addEventListener('contextmenu', (e) => e.preventDefault());

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

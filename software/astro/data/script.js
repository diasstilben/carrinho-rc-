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
// ESTADO DO CONTROLE E REPETIÇÃO (failsafe)
// =====================================
// Enquanto um movimento ou a buzina estiver ativo, o comando é repetido a cada
// INTERVALO_REPETICAO ms. O ESP32 desliga tudo sozinho se ficar 500ms sem
// receber a repetição: se a conexão cair, o carrinho para em vez de seguir andando.
const INTERVALO_REPETICAO = 150;
const BOTAO_DO_MOVIMENTO = { F: 'btn-F', B: 'btn-B', L: 'btn-L', R: 'btn-R' };

let movimentoAtual = null;   // 'F', 'B', 'L', 'R' ou null (parado)
let buzinaLigada = false;
let repeticaoPendente = false;

// Teclas e botões de movimento seguros agora, na ordem em que foram pressionados.
// O último manda; ao soltá-lo, volta para o anterior que ainda estiver seguro.
let segurados = [];  // [{ origem, cmd }]

function iniciarMovimento(cmd) {
    if (movimentoAtual === cmd) return;
    removerDestaqueMovimento();
    movimentoAtual = cmd;
    destacar(BOTAO_DO_MOVIMENTO[cmd]);
    comando(cmd);
}

function pararMovimento() {
    segurados = [];
    movimentoAtual = null;
    removerDestaqueMovimento();
    comando('S');
}

function segurar(origem, cmd) {
    if (!segurados.some(s => s.origem === origem)) segurados.push({ origem, cmd });
    iniciarMovimento(cmd);
}

function soltar(origem) {
    segurados = segurados.filter(s => s.origem !== origem);
    if (segurados.length > 0) {
        iniciarMovimento(segurados[segurados.length - 1].cmd);
    } else if (movimentoAtual) {
        pararMovimento();
    }
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

setInterval(() => {
    // Se a repetição anterior ainda não respondeu, não empilha outra:
    // com a conexão ruim o ESP para sozinho, que é o comportamento certo.
    if (repeticaoPendente) return;
    let pedidos = [];
    if (movimentoAtual) pedidos.push(comando(movimentoAtual));
    if (buzinaLigada) pedidos.push(comando('H'));
    if (pedidos.length === 0) return;
    repeticaoPendente = true;
    Promise.all(pedidos).finally(() => { repeticaoPendente = false; });
}, INTERVALO_REPETICAO);

// Se a página perder o foco (trocou de aba/app, tela apagou), para tudo:
// nesses casos o navegador nunca entrega o "soltar" da tecla ou do botão.
function pararTudo() {
    pararMovimento();
    desligarBuzina();
}
window.addEventListener('blur', pararTudo);
document.addEventListener('visibilitychange', () => {
    if (document.hidden) pararTudo();
});

// =====================================
// LÓGICA DO TECLADO (W, A, S, D, Espaço para Freio e F para Buzina)
// =====================================
const TECLAS_MOVIMENTO = {
    'w': 'F', 'arrowup': 'F',
    's': 'B', 'arrowdown': 'B',
    'a': 'L', 'arrowleft': 'L',
    'd': 'R', 'arrowright': 'R'
};

document.addEventListener('keydown', (e) => {
    let tecla = e.key.toLowerCase();
    let cmd = TECLAS_MOVIMENTO[tecla];

    if (cmd || tecla === ' ') e.preventDefault(); // Evita rolar a página
    // Ignora a auto-repetição do teclado: quem repete o comando é o setInterval.
    // (Assim o freio funciona mesmo com a tecla de movimento ainda segura.)
    if (e.repeat) return;

    // Movimento (W, A, S, D ou Setas)
    if (cmd) segurar('tecla:' + tecla, cmd);

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

    if (TECLAS_MOVIMENTO[tecla]) soltar('tecla:' + tecla);

    // Soltar a barra de espaço remove o destaque do botão de stop
    if (tecla === ' ') removerDestaque('btn-S');

    // Desliga a buzina ao soltar a tecla F
    if (tecla === 'f') desligarBuzina();
});

function destacar(idElemento) {
    let el = document.getElementById(idElemento);
    if(el) el.classList.add('pressionado');
}

function removerDestaque(idElemento) {
    let el = document.getElementById(idElemento);
    if(el) el.classList.remove('pressionado');
}

function removerDestaqueMovimento() {
    for (let id of Object.values(BOTAO_DO_MOVIMENTO)) removerDestaque(id);
}

// =====================================
// BOTÕES DA TELA (mouse e toque)
// =====================================
// Os eventos "pointer" tratam mouse e dedo do mesmo jeito e disparam uma
// única vez por toque (antes, mousedown + touchstart mandavam o comando 2x).
function ligarBotao(id, aoPressionar, aoSoltar) {
    let el = document.getElementById(id);
    let pressionado = false;

    el.addEventListener('pointerdown', (e) => {
        e.preventDefault();
        // Continua recebendo o "soltar" mesmo se o dedo escorregar para fora do botão
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

    // Sem menu de contexto ao segurar o botão no celular
    el.addEventListener('contextmenu', (e) => e.preventDefault());
}

for (let [cmd, id] of Object.entries(BOTAO_DO_MOVIMENTO)) {
    ligarBotao(id, () => segurar(id, cmd), () => soltar(id));
}
ligarBotao('btn-S', () => { pararMovimento(); destacar('btn-S'); }, () => removerDestaque('btn-S'));
ligarBotao('btn-H', ligarBuzina, desligarBuzina);

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
// OpenCog Inferno AGI - Renderer Process
// Handles UI interactions and cognitive operations

console.log('Renderer process initialized');

// ========================================
// State Management
// ========================================

const state = {
    currentView: 'atomspace',
    atoms: [],
    attentionalFocus: [],
    inferenceResults: [],
    systemInfo: null,
    isMockMode: false
};

// ========================================
// Initialization
// ========================================

document.addEventListener('DOMContentLoaded', async () => {
    console.log('DOM loaded, initializing app...');
    
    // Load system info
    await loadSystemInfo();
    
    // Setup event listeners
    setupNavigation();
    setupAtomSpaceView();
    setupAttentionView();
    setupInferenceView();
    setupPatternView();
    setupConsoleView();
    setupMenuListeners();
    
    // Load initial data
    await refreshStats();
    await loadAttentionalFocus();
    
    // Update status
    updateStatus('Ready', 'online');
    
    console.log('App initialized successfully');
});

// ========================================
// System Info
// ========================================

async function loadSystemInfo() {
    try {
        const result = await window.cognitive.getSystemInfo();
        if (result.success) {
            state.systemInfo = result.info;
            state.isMockMode = !result.info.cognitiveAddon;
            
            const infoText = `${result.info.platform} | ${result.info.arch} | Electron ${result.info.electronVersion}`;
            document.getElementById('system-info').textContent = infoText;
            
            if (state.isMockMode) {
                console.warn('Running in MOCK MODE - cognitive addon not loaded');
                updateStatus('Mock Mode', 'warning');
            }
        }
    } catch (err) {
        console.error('Failed to load system info:', err);
    }
}

// ========================================
// Navigation
// ========================================

function setupNavigation() {
    const navItems = document.querySelectorAll('.nav-item');
    
    navItems.forEach(item => {
        item.addEventListener('click', () => {
            const view = item.dataset.view;
            switchView(view);
        });
    });
}

function switchView(viewName) {
    // Update navigation
    document.querySelectorAll('.nav-item').forEach(item => {
        item.classList.toggle('active', item.dataset.view === viewName);
    });
    
    // Update content
    document.querySelectorAll('.view-panel').forEach(panel => {
        panel.classList.toggle('active', panel.id === `view-${viewName}`);
    });
    
    state.currentView = viewName;
    console.log(`Switched to view: ${viewName}`);
}

// ========================================
// AtomSpace View
// ========================================

function setupAtomSpaceView() {
    const btnCreate = document.getElementById('btn-create-atom');
    const btnRefresh = document.getElementById('btn-refresh-atoms');
    const btnSubmit = document.getElementById('btn-submit-atom');
    const btnCancel = document.getElementById('btn-cancel-atom');
    const strengthSlider = document.getElementById('atom-strength');
    const confidenceSlider = document.getElementById('atom-confidence');
    
    // Show/hide atom creator
    btnCreate.addEventListener('click', () => {
        document.getElementById('atom-creator').style.display = 'block';
        document.getElementById('atom-list').style.display = 'none';
    });
    
    btnCancel.addEventListener('click', () => {
        document.getElementById('atom-creator').style.display = 'none';
        document.getElementById('atom-list').style.display = 'block';
    });
    
    // Update slider values
    strengthSlider.addEventListener('input', (e) => {
        document.getElementById('strength-value').textContent = e.target.value;
    });
    
    confidenceSlider.addEventListener('input', (e) => {
        document.getElementById('confidence-value').textContent = e.target.value;
    });
    
    // Create atom
    btnSubmit.addEventListener('click', async () => {
        const type = document.getElementById('atom-type').value;
        const name = document.getElementById('atom-name').value;
        const strength = parseFloat(strengthSlider.value);
        const confidence = parseFloat(confidenceSlider.value);
        
        if (!name) {
            alert('Please enter an atom name');
            return;
        }
        
        await createAtom(type, name, strength, confidence);
    });
    
    // Refresh atoms
    btnRefresh.addEventListener('click', async () => {
        await refreshStats();
    });
}

async function createAtom(type, name, strength, confidence) {
    try {
        updateStatus('Creating atom...', 'working');
        
        const result = await window.cognitive.createAtom(type, name, strength, confidence);
        
        if (result.success) {
            console.log(`Created atom: ${type} ${name} [ID: ${result.atomId}]`);
            
            // Add to state
            state.atoms.push({
                id: result.atomId,
                type,
                name,
                tv: { strength, confidence }
            });
            
            // Update UI
            document.getElementById('atom-creator').style.display = 'none';
            document.getElementById('atom-list').style.display = 'block';
            document.getElementById('atom-name').value = '';
            
            await refreshStats();
            updateStatus('Atom created', 'success');
            
            // Show success message
            logToConsole(`Created: (${type} "${name}") [${result.atomId}]`);
        } else {
            console.error('Failed to create atom:', result.error);
            updateStatus('Failed to create atom', 'error');
        }
    } catch (err) {
        console.error('Error creating atom:', err);
        updateStatus('Error', 'error');
    }
}

// ========================================
// Attention View
// ========================================

function setupAttentionView() {
    const btnRefresh = document.getElementById('btn-refresh-attention');
    
    btnRefresh.addEventListener('click', async () => {
        await loadAttentionalFocus();
    });
}

async function loadAttentionalFocus() {
    try {
        updateStatus('Loading attentional focus...', 'working');
        
        const result = await window.cognitive.getAttentionalFocus();
        
        if (result.success) {
            state.attentionalFocus = result.focus;
            displayAttentionalFocus(result.focus);
            updateStatus('Focus loaded', 'success');
        } else {
            console.error('Failed to load focus:', result.error);
        }
    } catch (err) {
        console.error('Error loading focus:', err);
    }
}

function displayAttentionalFocus(focus) {
    const container = document.getElementById('attention-focus');
    
    if (!focus || focus.length === 0) {
        container.innerHTML = '<p class="placeholder">No atoms in attentional focus</p>';
        return;
    }
    
    let html = '<div class="focus-list">';
    focus.forEach(atom => {
        html += `
            <div class="focus-item" style="padding: 10px; margin: 5px 0; background: #2d2d30; border-radius: 4px;">
                <div style="display: flex; justify-content: space-between;">
                    <span style="color: #4ec9b0;">${atom.name || atom.id}</span>
                    <span style="color: #ce9178;">STI: ${atom.sti}</span>
                </div>
            </div>
        `;
    });
    html += '</div>';
    
    container.innerHTML = html;
    
    // Update stats
    document.getElementById('stat-focus').textContent = focus.length;
}

// ========================================
// Inference View
// ========================================

function setupInferenceView() {
    const btnPLN = document.getElementById('btn-run-pln');
    const btnURE = document.getElementById('btn-run-ure');
    
    btnPLN.addEventListener('click', async () => {
        await runPLNInference();
    });
    
    btnURE.addEventListener('click', async () => {
        await runUREInference();
    });
}

async function runPLNInference() {
    try {
        updateStatus('Running PLN inference...', 'working');
        
        const result = await window.cognitive.inferPLN([], null);
        
        if (result.success) {
            state.inferenceResults.push(result.result);
            displayInferenceResults([result.result]);
            updateStatus('PLN inference complete', 'success');
            logToConsole('PLN inference completed successfully');
        } else {
            console.error('PLN inference failed:', result.error);
            updateStatus('PLN failed', 'error');
        }
    } catch (err) {
        console.error('Error running PLN:', err);
        updateStatus('Error', 'error');
    }
}

async function runUREInference() {
    try {
        updateStatus('Running URE inference...', 'working');
        
        const result = await window.cognitive.inferURE([], null);
        
        if (result.success) {
            state.inferenceResults = state.inferenceResults.concat(result.results);
            displayInferenceResults(result.results);
            updateStatus('URE inference complete', 'success');
            logToConsole(`URE inference found ${result.results.length} results`);
        } else {
            console.error('URE inference failed:', result.error);
            updateStatus('URE failed', 'error');
        }
    } catch (err) {
        console.error('Error running URE:', err);
        updateStatus('Error', 'error');
    }
}

function displayInferenceResults(results) {
    const container = document.getElementById('inference-results');
    
    if (!results || results.length === 0) {
        container.innerHTML = '<p class="placeholder">No inference results</p>';
        return;
    }
    
    let html = '<div class="results-list">';
    results.forEach((result, index) => {
        html += `
            <div class="result-item" style="padding: 10px; margin: 5px 0; background: #2d2d30; border-radius: 4px;">
                <div style="color: #4ec9b0;">Result ${index + 1}</div>
                <div style="color: #d4d4d4; margin-top: 5px;">${JSON.stringify(result, null, 2)}</div>
            </div>
        `;
    });
    html += '</div>';
    
    container.innerHTML = html;
}

// ========================================
// Pattern Match View
// ========================================

function setupPatternView() {
    const btnExecute = document.getElementById('btn-execute-query');
    
    btnExecute.addEventListener('click', async () => {
        await executePatternQuery();
    });
}

async function executePatternQuery() {
    try {
        const pattern = document.getElementById('pattern-query').value;
        
        if (!pattern) {
            alert('Please enter a pattern query');
            return;
        }
        
        updateStatus('Executing query...', 'working');
        
        const result = await window.cognitive.queryAtomSpace(pattern);
        
        if (result.success) {
            displayQueryResults(result.results);
            updateStatus('Query complete', 'success');
            logToConsole(`Query returned ${result.results.length} results`);
        } else {
            console.error('Query failed:', result.error);
            updateStatus('Query failed', 'error');
        }
    } catch (err) {
        console.error('Error executing query:', err);
        updateStatus('Error', 'error');
    }
}

function displayQueryResults(results) {
    const container = document.getElementById('query-results');
    
    if (!results || results.length === 0) {
        container.innerHTML = '<p class="placeholder">No results found</p>';
        return;
    }
    
    let html = '<div class="query-results-list">';
    results.forEach((binding, index) => {
        html += `
            <div class="query-result" style="padding: 10px; margin: 5px 0; background: #2d2d30; border-radius: 4px;">
                <div style="color: #4ec9b0;">Binding ${index + 1}</div>
                <div style="color: #d4d4d4; margin-top: 5px;">${JSON.stringify(binding, null, 2)}</div>
            </div>
        `;
    });
    html += '</div>';
    
    container.innerHTML = html;
}

// ========================================
// Console View
// ========================================

function setupConsoleView() {
    const input = document.getElementById('console-input');
    const btnClear = document.getElementById('btn-clear-console');
    
    input.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') {
            const command = input.value.trim();
            if (command) {
                executeConsoleCommand(command);
                input.value = '';
            }
        }
    });
    
    btnClear.addEventListener('click', () => {
        const output = document.getElementById('console-output');
        output.innerHTML = '<div class="console-line">Console cleared</div>';
    });
}

function executeConsoleCommand(command) {
    logToConsole(`> ${command}`);
    
    // Simple command interpreter
    if (command === 'help') {
        logToConsole('Available commands: help, stats, clear, focus, atoms');
    } else if (command === 'stats') {
        refreshStats();
    } else if (command === 'focus') {
        loadAttentionalFocus();
    } else if (command === 'atoms') {
        logToConsole(`Total atoms: ${state.atoms.length}`);
    } else {
        logToConsole(`Unknown command: ${command}`);
    }
}

function logToConsole(message) {
    const output = document.getElementById('console-output');
    const line = document.createElement('div');
    line.className = 'console-line';
    line.textContent = message;
    output.appendChild(line);
    output.scrollTop = output.scrollHeight;
}

// ========================================
// Statistics
// ========================================

async function refreshStats() {
    try {
        const result = await window.cognitive.getAtomSpaceStats();
        
        if (result.success) {
            const stats = result.stats;
            document.getElementById('stat-total-atoms').textContent = stats.totalAtoms || 0;
            document.getElementById('stat-nodes').textContent = stats.nodes || 0;
            document.getElementById('stat-links').textContent = stats.links || 0;
        }
    } catch (err) {
        console.error('Error refreshing stats:', err);
    }
}

// ========================================
// Menu Listeners
// ========================================

function setupMenuListeners() {
    window.cognitive.onMenuEvent('menu-new-atomspace', () => {
        console.log('New AtomSpace requested');
        logToConsole('New AtomSpace created');
    });
    
    window.cognitive.onMenuEvent('menu-run-pln', () => {
        switchView('inference');
        runPLNInference();
    });
    
    window.cognitive.onMenuEvent('menu-run-ure', () => {
        switchView('inference');
        runUREInference();
    });
    
    window.cognitive.onMenuEvent('menu-attention', () => {
        switchView('attention');
        loadAttentionalFocus();
    });
    
    window.cognitive.onMenuEvent('menu-pattern-match', () => {
        switchView('pattern');
    });
    
    window.cognitive.onMenuEvent('view-atomspace', () => {
        switchView('atomspace');
    });
    
    window.cognitive.onMenuEvent('view-attention', () => {
        switchView('attention');
    });
    
    window.cognitive.onMenuEvent('view-inference', () => {
        switchView('inference');
    });
    
    window.cognitive.onMenuEvent('menu-about', () => {
        alert('OpenCog Inferno AGI v1.0.0\nKernel-Based Cognitive Operating System\n\nCopyright © 2025 OpenCog Community');
    });
}

// ========================================
// Status Updates
// ========================================

function updateStatus(text, type = 'online') {
    const indicator = document.getElementById('status-indicator');
    const statusText = indicator.querySelector('.status-text');
    const statusDot = indicator.querySelector('.status-dot');
    
    statusText.textContent = text;
    
    // Update dot color based on type
    const colors = {
        online: '#4ec9b0',
        working: '#dcdcaa',
        warning: '#ce9178',
        error: '#f48771',
        success: '#4ec9b0'
    };
    
    statusDot.style.background = colors[type] || colors.online;
}

console.log('Renderer script loaded successfully');

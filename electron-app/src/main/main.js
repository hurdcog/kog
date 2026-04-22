const { app, BrowserWindow, ipcMain, Menu } = require('electron');
const path = require('path');
const fs = require('fs');

// Load native cognitive addon (if available)
let cognitive = null;
try {
    cognitive = require('../../native/build/Release/cognitive-addon');
    console.log('✓ Cognitive addon loaded successfully');
} catch (err) {
    console.warn('⚠ Cognitive addon not available:', err.message);
    console.warn('Running in mock mode');
}

let mainWindow = null;

// Create main window
function createWindow() {
    mainWindow = new BrowserWindow({
        width: 1400,
        height: 900,
        minWidth: 1000,
        minHeight: 700,
        title: 'OpenCog Inferno AGI',
        backgroundColor: '#1e1e1e',
        webPreferences: {
            nodeIntegration: false,
            contextIsolation: true,
            preload: path.join(__dirname, '../preload/preload.js'),
            sandbox: false
        },
        icon: path.join(__dirname, '../../assets/icon.png')
    });

    // Load the app
    mainWindow.loadFile(path.join(__dirname, '../renderer/index.html'));

    // Open DevTools in development
    if (process.env.NODE_ENV === 'development') {
        mainWindow.webContents.openDevTools();
    }

    // Create application menu
    createMenu();

    mainWindow.on('closed', () => {
        mainWindow = null;
    });

    console.log('Main window created');
}

// Create application menu
function createMenu() {
    const template = [
        {
            label: 'File',
            submenu: [
                {
                    label: 'New AtomSpace',
                    accelerator: 'CmdOrCtrl+N',
                    click: () => {
                        mainWindow.webContents.send('menu-new-atomspace');
                    }
                },
                {
                    label: 'Load AtomSpace',
                    accelerator: 'CmdOrCtrl+O',
                    click: () => {
                        mainWindow.webContents.send('menu-load-atomspace');
                    }
                },
                {
                    label: 'Save AtomSpace',
                    accelerator: 'CmdOrCtrl+S',
                    click: () => {
                        mainWindow.webContents.send('menu-save-atomspace');
                    }
                },
                { type: 'separator' },
                {
                    label: 'Exit',
                    accelerator: 'CmdOrCtrl+Q',
                    click: () => {
                        app.quit();
                    }
                }
            ]
        },
        {
            label: 'Cognitive',
            submenu: [
                {
                    label: 'Run PLN Inference',
                    accelerator: 'CmdOrCtrl+I',
                    click: () => {
                        mainWindow.webContents.send('menu-run-pln');
                    }
                },
                {
                    label: 'Run URE Inference',
                    accelerator: 'CmdOrCtrl+U',
                    click: () => {
                        mainWindow.webContents.send('menu-run-ure');
                    }
                },
                {
                    label: 'Attention Allocation',
                    accelerator: 'CmdOrCtrl+A',
                    click: () => {
                        mainWindow.webContents.send('menu-attention');
                    }
                },
                { type: 'separator' },
                {
                    label: 'Pattern Matcher',
                    accelerator: 'CmdOrCtrl+P',
                    click: () => {
                        mainWindow.webContents.send('menu-pattern-match');
                    }
                }
            ]
        },
        {
            label: 'View',
            submenu: [
                {
                    label: 'AtomSpace Explorer',
                    click: () => {
                        mainWindow.webContents.send('view-atomspace');
                    }
                },
                {
                    label: 'Attention Visualization',
                    click: () => {
                        mainWindow.webContents.send('view-attention');
                    }
                },
                {
                    label: 'Inference Trace',
                    click: () => {
                        mainWindow.webContents.send('view-inference');
                    }
                },
                { type: 'separator' },
                { role: 'reload' },
                { role: 'toggleDevTools' },
                { type: 'separator' },
                { role: 'resetZoom' },
                { role: 'zoomIn' },
                { role: 'zoomOut' }
            ]
        },
        {
            label: 'Help',
            submenu: [
                {
                    label: 'Documentation',
                    click: () => {
                        require('electron').shell.openExternal('https://github.com/o9nn/occ');
                    }
                },
                {
                    label: 'About',
                    click: () => {
                        mainWindow.webContents.send('menu-about');
                    }
                }
            ]
        }
    ];

    const menu = Menu.buildFromTemplate(template);
    Menu.setApplicationMenu(menu);
}

// ========================================
// IPC Handlers - AtomSpace Operations
// ========================================

ipcMain.handle('create-atom', async (event, type, name, strength, confidence) => {
    console.log(`Creating atom: ${type} ${name} [${strength}, ${confidence}]`);
    
    if (cognitive) {
        try {
            const atomId = cognitive.createAtom(type, name, strength, confidence);
            return { success: true, atomId };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        const atomId = Math.floor(Math.random() * 1000000);
        return { success: true, atomId, mock: true };
    }
});

ipcMain.handle('get-atom', async (event, atomId) => {
    console.log(`Getting atom: ${atomId}`);
    
    if (cognitive) {
        try {
            const atom = cognitive.getAtom(atomId);
            return { success: true, atom };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        return {
            success: true,
            atom: {
                id: atomId,
                type: 'ConceptNode',
                name: 'MockAtom',
                tv: { strength: 0.8, confidence: 0.9 },
                av: { sti: 50, lti: 10, vlti: 0 }
            },
            mock: true
        };
    }
});

ipcMain.handle('query-atomspace', async (event, pattern) => {
    console.log(`Querying AtomSpace: ${pattern}`);
    
    if (cognitive) {
        try {
            const results = cognitive.queryAtomSpace(pattern);
            return { success: true, results };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        return {
            success: true,
            results: [
                { var: '$X', atom: { id: 1, type: 'ConceptNode', name: 'cat' } },
                { var: '$X', atom: { id: 2, type: 'ConceptNode', name: 'dog' } }
            ],
            mock: true
        };
    }
});

// ========================================
// IPC Handlers - Attention Operations
// ========================================

ipcMain.handle('stimulate-atom', async (event, atomId, amount) => {
    console.log(`Stimulating atom ${atomId} with ${amount} STI`);
    
    if (cognitive) {
        try {
            cognitive.stimulateAtom(atomId, amount);
            return { success: true };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        return { success: true, mock: true };
    }
});

ipcMain.handle('get-attention-focus', async (event) => {
    console.log('Getting attentional focus');
    
    if (cognitive) {
        try {
            const focus = cognitive.getAttentionalFocus();
            return { success: true, focus };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        return {
            success: true,
            focus: [
                { id: 1, name: 'cat', sti: 100 },
                { id: 2, name: 'dog', sti: 80 },
                { id: 3, name: 'mammal', sti: 60 }
            ],
            mock: true
        };
    }
});

// ========================================
// IPC Handlers - Inference Operations
// ========================================

ipcMain.handle('infer-pln', async (event, premises, conclusion) => {
    console.log('Running PLN inference');
    
    if (cognitive) {
        try {
            const result = cognitive.inferPLN(premises, conclusion);
            return { success: true, result };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        return {
            success: true,
            result: {
                conclusion: { id: 100, type: 'InheritanceLink', name: 'inferred' },
                tv: { strength: 0.7, confidence: 0.6 }
            },
            mock: true
        };
    }
});

ipcMain.handle('infer-ure', async (event, sources, target) => {
    console.log('Running URE inference');
    
    if (cognitive) {
        try {
            const results = cognitive.inferURE(sources, target);
            return { success: true, results };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        return {
            success: true,
            results: [
                { id: 101, type: 'InheritanceLink', name: 'ure-result-1' },
                { id: 102, type: 'SimilarityLink', name: 'ure-result-2' }
            ],
            mock: true
        };
    }
});

// ========================================
// IPC Handlers - System Operations
// ========================================

ipcMain.handle('get-system-info', async (event) => {
    return {
        success: true,
        info: {
            platform: process.platform,
            arch: process.arch,
            nodeVersion: process.version,
            electronVersion: process.versions.electron,
            cognitiveAddon: cognitive !== null,
            infernoKernel: cognitive !== null,
            addonVersion: cognitive ? (cognitive.version || 'unknown') : null,
            addonEngine: cognitive ? (cognitive.engine || 'unknown') : null
        }
    };
});

ipcMain.handle('get-atomspace-stats', async (event) => {
    console.log('Getting AtomSpace statistics');
    
    if (cognitive) {
        try {
            const stats = cognitive.getAtomSpaceStats();
            return { success: true, stats };
        } catch (err) {
            return { success: false, error: err.message };
        }
    } else {
        // Mock implementation
        return {
            success: true,
            stats: {
                totalAtoms: 1234,
                nodes: 800,
                links: 434,
                types: {
                    ConceptNode: 500,
                    PredicateNode: 200,
                    InheritanceLink: 300,
                    SimilarityLink: 134
                }
            },
            mock: true
        };
    }
});

// ========================================
// App Lifecycle
// ========================================

app.whenReady().then(() => {
    console.log('App ready, creating window...');
    createWindow();

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
});

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});

app.on('before-quit', () => {
    console.log('App shutting down...');
    
    // Cleanup cognitive resources
    if (cognitive && cognitive.cleanup) {
        try {
            cognitive.cleanup();
            console.log('Cognitive resources cleaned up');
        } catch (err) {
            console.error('Error cleaning up cognitive resources:', err);
        }
    }
});

// Handle uncaught exceptions
process.on('uncaughtException', (error) => {
    console.error('Uncaught exception:', error);
});

process.on('unhandledRejection', (reason, promise) => {
    console.error('Unhandled rejection at:', promise, 'reason:', reason);
});

console.log('OpenCog Inferno AGI - Main process initialized');

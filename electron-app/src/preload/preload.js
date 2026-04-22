const { contextBridge, ipcRenderer } = require('electron');

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('cognitive', {
    // AtomSpace operations
    createAtom: (type, name, strength, confidence) => 
        ipcRenderer.invoke('create-atom', type, name, strength, confidence),
    
    getAtom: (atomId) => 
        ipcRenderer.invoke('get-atom', atomId),
    
    queryAtomSpace: (pattern) => 
        ipcRenderer.invoke('query-atomspace', pattern),
    
    getAtomSpaceStats: () => 
        ipcRenderer.invoke('get-atomspace-stats'),
    
    // Attention operations
    stimulateAtom: (atomId, amount) => 
        ipcRenderer.invoke('stimulate-atom', atomId, amount),
    
    getAttentionalFocus: () => 
        ipcRenderer.invoke('get-attention-focus'),
    
    // Inference operations
    inferPLN: (premises, conclusion) => 
        ipcRenderer.invoke('infer-pln', premises, conclusion),
    
    inferURE: (sources, target) => 
        ipcRenderer.invoke('infer-ure', sources, target),
    
    // System operations
    getSystemInfo: () => 
        ipcRenderer.invoke('get-system-info'),
    
    // Event listeners
    onMenuEvent: (channel, callback) => {
        const validChannels = [
            'menu-new-atomspace',
            'menu-load-atomspace',
            'menu-save-atomspace',
            'menu-run-pln',
            'menu-run-ure',
            'menu-attention',
            'menu-pattern-match',
            'view-atomspace',
            'view-attention',
            'view-inference',
            'menu-about'
        ];
        
        if (validChannels.includes(channel)) {
            ipcRenderer.on(channel, callback);
        }
    }
});

console.log('Preload script loaded - Cognitive API exposed to renderer');

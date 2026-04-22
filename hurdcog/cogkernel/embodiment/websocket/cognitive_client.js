/**
 * Cognitive Mesh WebSocket Client for Web Agents
 * JavaScript/TypeScript library for browser-based cognitive agents
 * 
 * Usage:
 *   const client = new CognitiveMeshClient('ws://localhost:8000/ws');
 *   await client.connect();
 *   client.on('cognitive.state.update', (state) => console.log(state));
 */

class CognitiveMeshClient {
    constructor(websocketUrl, options = {}) {
        this.url = websocketUrl;
        this.ws = null;
        this.connected = false;
        this.eventHandlers = new Map();
        this.pendingTasks = new Map();
        this.reconnectAttempts = 0;
        this.maxReconnectAttempts = options.maxReconnectAttempts || 5;
        this.reconnectDelay = options.reconnectDelay || 1000;
        this.autoReconnect = options.autoReconnect !== false;
    }

    /**
     * Connect to the cognitive mesh WebSocket server
     */
    async connect() {
        return new Promise((resolve, reject) => {
            try {
                this.ws = new WebSocket(this.url);

                this.ws.onopen = () => {
                    console.log('Connected to cognitive mesh');
                    this.connected = true;
                    this.reconnectAttempts = 0;
                    this.emit('connected');
                    resolve();
                };

                this.ws.onmessage = (event) => {
                    this.handleMessage(JSON.parse(event.data));
                };

                this.ws.onerror = (error) => {
                    console.error('WebSocket error:', error);
                    this.emit('error', error);
                    reject(error);
                };

                this.ws.onclose = () => {
                    console.log('Disconnected from cognitive mesh');
                    this.connected = false;
                    this.emit('disconnected');

                    // Auto-reconnect if enabled
                    if (this.autoReconnect && this.reconnectAttempts < this.maxReconnectAttempts) {
                        this.reconnectAttempts++;
                        console.log(`Reconnect attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts}`);
                        setTimeout(() => this.connect(), this.reconnectDelay * this.reconnectAttempts);
                    }
                };
            } catch (error) {
                reject(error);
            }
        });
    }

    /**
     * Disconnect from the cognitive mesh
     */
    disconnect() {
        if (this.ws) {
            this.autoReconnect = false;
            this.ws.close();
            this.ws = null;
        }
    }

    /**
     * Handle incoming WebSocket messages
     */
    handleMessage(message) {
        const { event, data, timestamp } = message;

        // Emit to event handlers
        if (this.eventHandlers.has(event)) {
            for (const handler of this.eventHandlers.get(event)) {
                handler(data, timestamp);
            }
        }

        // Handle task results
        if (event === 'task.completion' && data.task_id) {
            if (this.pendingTasks.has(data.task_id)) {
                const { resolve } = this.pendingTasks.get(data.task_id);
                resolve(data);
                this.pendingTasks.delete(data.task_id);
            }
        }

        // Emit to wildcard handlers
        if (this.eventHandlers.has('*')) {
            for (const handler of this.eventHandlers.get('*')) {
                handler({ event, data, timestamp });
            }
        }
    }

    /**
     * Register event handler
     */
    on(event, handler) {
        if (!this.eventHandlers.has(event)) {
            this.eventHandlers.set(event, []);
        }
        this.eventHandlers.get(event).push(handler);
    }

    /**
     * Unregister event handler
     */
    off(event, handler) {
        if (this.eventHandlers.has(event)) {
            const handlers = this.eventHandlers.get(event);
            const index = handlers.indexOf(handler);
            if (index > -1) {
                handlers.splice(index, 1);
            }
        }
    }

    /**
     * Emit local event
     */
    emit(event, data) {
        if (this.eventHandlers.has(event)) {
            for (const handler of this.eventHandlers.get(event)) {
                handler(data);
            }
        }
    }

    /**
     * Send message to server
     */
    send(message) {
        if (!this.connected) {
            throw new Error('Not connected to cognitive mesh');
        }
        this.ws.send(JSON.stringify(message));
    }

    /**
     * Send ping to keep connection alive
     */
    async ping() {
        return new Promise((resolve) => {
            const handler = (data) => {
                if (data.type === 'pong') {
                    this.off('*', handler);
                    resolve(data);
                }
            };
            this.on('*', handler);
            this.send({ type: 'ping' });
        });
    }

    /**
     * Subscribe to specific events
     */
    async subscribe(events) {
        this.send({
            type: 'subscribe',
            events: Array.isArray(events) ? events : [events]
        });
    }

    /**
     * Submit cognitive task
     */
    async submitTask(task) {
        if (!this.connected) {
            throw new Error('Not connected to cognitive mesh');
        }

        // Generate task ID if not provided
        const taskId = task.task_id || this.generateTaskId();
        task.task_id = taskId;

        return new Promise((resolve, reject) => {
            // Store pending task
            this.pendingTasks.set(taskId, { resolve, reject });

            // Send task
            this.send({
                type: 'task',
                data: task
            });

            // Timeout after 30 seconds
            setTimeout(() => {
                if (this.pendingTasks.has(taskId)) {
                    this.pendingTasks.delete(taskId);
                    reject(new Error('Task timeout'));
                }
            }, 30000);
        });
    }

    /**
     * Generate unique task ID
     */
    generateTaskId() {
        return `task_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
    }
}

/**
 * REST API Client for cognitive mesh
 */
class CognitiveMeshAPI {
    constructor(baseUrl) {
        this.baseUrl = baseUrl || 'http://localhost:8000';
    }

    /**
     * Get current cognitive state
     */
    async getCognitiveState() {
        const response = await fetch(`${this.baseUrl}/api/v1/cognitive/state`);
        return await response.json();
    }

    /**
     * Process cognitive task
     */
    async processTask(task) {
        const response = await fetch(`${this.baseUrl}/api/v1/cognitive/process`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(task)
        });
        return await response.json();
    }

    /**
     * Get task result
     */
    async getTaskResult(taskId) {
        const response = await fetch(`${this.baseUrl}/api/v1/cognitive/task/${taskId}`);
        return await response.json();
    }

    /**
     * Get attention allocation
     */
    async getAttentionAllocation() {
        const response = await fetch(`${this.baseUrl}/api/v1/attention/allocation`);
        return await response.json();
    }

    /**
     * Set attention focus
     */
    async setAttentionFocus(target, weight = 1.0) {
        const response = await fetch(`${this.baseUrl}/api/v1/attention/focus`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ target, weight })
        });
        return await response.json();
    }

    /**
     * Register agent
     */
    async registerAgent(agentType, capabilities = [], metadata = {}) {
        const response = await fetch(`${this.baseUrl}/api/v1/agents/register`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                agent_type: agentType,
                capabilities,
                metadata
            })
        });
        return await response.json();
    }

    /**
     * Get agent information
     */
    async getAgent(agentId) {
        const response = await fetch(`${this.baseUrl}/api/v1/agents/${agentId}`);
        return await response.json();
    }

    /**
     * List all agents
     */
    async listAgents() {
        const response = await fetch(`${this.baseUrl}/api/v1/agents`);
        return await response.json();
    }

    /**
     * Unregister agent
     */
    async unregisterAgent(agentId) {
        const response = await fetch(`${this.baseUrl}/api/v1/agents/${agentId}`, {
            method: 'DELETE'
        });
        return await response.json();
    }

    /**
     * Health check
     */
    async healthCheck() {
        const response = await fetch(`${this.baseUrl}/api/v1/health`);
        return await response.json();
    }
}

// Export for Node.js or browser
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { CognitiveMeshClient, CognitiveMeshAPI };
}

# AGI-OS Conversational Interface Integration

**Date**: 2025-11-30  
**Status**: Deployed as separate web application  
**Repository**: cogpy/occ  
**Application URL**: https://3000-i0sfai5l0346r5rpkc613-8bb32ccf.manusvm.computer

---

## Overview

The AGI-OS Conversational Interface is a unified web application that integrates all chatbot and conversational systems from the OCC repository into a comprehensive cognitive interaction layer. It serves as the primary user interface for interacting with GHOST, agentic chatbots, language learning systems, and cognitive services.

---

## Architecture

### Technology Stack

**Backend**:
- Node.js 22 with TypeScript
- tRPC 11 for type-safe API
- Express 4 for HTTP server
- Drizzle ORM with MySQL/TiDB
- Manus OAuth for authentication

**Frontend**:
- React 19 with TypeScript
- Tailwind CSS 4 with dark mode
- shadcn/ui components
- Wouter for routing
- TanStack Query for data fetching

**Database Schema**:
- `chatSessions` - Conversation sessions
- `messages` - Chat messages with role and content
- `botConfigurations` - Available chatbot configurations
- `cognitiveStates` - AtomSpace and cognitive metrics snapshots

---

## Integrated Chatbot Systems

### 1. GHOST Engine (Primary)

**Source**: `opencog/opencog/ghost/`

**Description**: General Holistic Organism Scripting Tool - ChatScript-inspired DSL for scripting AI character behaviors.

**Integration Status**: ✅ Simulated adapter implemented

**Features**:
- Pattern matching and rule-based responses
- Context-aware conversation handling
- Goal-driven action selection via OpenPsi
- Attention allocation via ECAN
- Cognitive state tracking

**Production Requirements**:
- Guile Scheme interpreter
- CogServer running instance
- RelEx for NLP preprocessing
- Scheme-Python bridge implementation

### 2. Agentic Assistant

**Source**: `agentic-chatbots/`

**Description**: Modern LLM-based chatbot with tool-use capabilities.

**Integration Status**: ✅ Simulated adapter implemented

**Features**:
- Reasoning and planning capabilities
- Tool invocation simulation
- Multi-step agent workflows

### 3. Language Learning System

**Source**: `language-learning/`

**Description**: NLP and linguistic pattern learning system.

**Integration Status**: ✅ Simulated adapter implemented

**Features**:
- Sentence structure analysis
- Grammatical pattern extraction
- Conversational data learning

---

## Bot Adapter Architecture

All chatbots implement a unified `BotAdapter` interface:

```typescript
interface BotAdapter {
  botId: string;
  name: string;
  type: string;
  
  processMessage(message: string, context: BotContext): Promise<BotResponse>;
  getStatus(): Promise<{ status: "active" | "inactive" | "error" }>;
  resetContext(sessionId: number): Promise<void>;
}
```

**Bot Manager** (`server/bots/adapters.ts`):
- Centralized bot registration and routing
- Message processing coordination
- Status monitoring

---

## API Endpoints (tRPC)

### Chat Router

- `chat.createSession` - Create new conversation session
- `chat.getSessions` - Get user's chat sessions
- `chat.getMessages` - Retrieve session messages
- `chat.sendMessage` - Send message and get bot response

### Bots Router

- `bots.list` - List all available chatbots
- `bots.getStatus` - Get bot operational status

### Cognitive Router

- `cognitive.getState` - Get latest cognitive state for session
- `cognitive.getAtomSpaceGraph` - Get AtomSpace graph visualization data

---

## Cognitive Integration

### AtomSpace Bridge

**Location**: `src/atomspace-hurdcog-bridge/`

**Status**: Interface defined, production implementation pending

**Components**:
- C++ header with full API (`bridge.h`)
- Python bindings (`python/atomspace_hurdcog.py`)
- IPC mechanism for HurdCog communication

**Capabilities**:
- Atom storage and retrieval
- Inference execution
- Attention allocation queries
- Performance monitoring

### Cognitive State Tracking

Each bot response can include cognitive state:

```typescript
interface CognitiveState {
  atomspaceData?: unknown;
  attentionValues?: { sti: number; lti: number };
  activeGoals?: string[];
  inferenceActivity?: { plnInferences: number; rulesFired: number };
}
```

States are stored in database and displayed in real-time UI panel.

---

## User Interface

### Main Chat Interface (`/`)

**Features**:
- Multi-bot conversation view
- Bot selector dropdown (GHOST, Agentic, Language Learning)
- Session sidebar with conversation history
- Real-time message updates
- Cognitive state panel showing:
  - Active goals
  - Attention values (STI/LTI)
  - PLN inference activity

**Dark Mode**: Default theme with cyan/blue cognitive color scheme

### Cognitive Visualization (`/cognitive`)

**Features**:
- AtomSpace graph visualization (canvas-based)
- AGI-OS architecture overview
- Cognitive components documentation
- Integration status dashboard

---

## Testing

**Test Suite**: `server/chat.test.ts`

**Coverage**: 15 tests, all passing

**Test Categories**:
- Chat router (session creation, messaging, history)
- Bots router (listing, status, error handling)
- Cognitive router (state retrieval, graph data)
- Bot adapters (GHOST, Agentic, Language Learning)

**Run Tests**: `pnpm test`

---

## Deployment

**Development Server**: `pnpm dev`  
**Production Build**: `pnpm build && pnpm start`  
**Database Migration**: `pnpm db:push`

**Environment Variables** (auto-injected by Manus platform):
- `DATABASE_URL` - MySQL connection string
- `JWT_SECRET` - Session signing secret
- `VITE_APP_ID` - OAuth application ID
- `OAUTH_SERVER_URL` - OAuth backend URL

---

## Integration with OCC Components

### Current Integration

1. **GHOST**: Simulated pattern matching based on GHOST DSL concepts
2. **Bot Configurations**: Database-backed bot management
3. **Cognitive State**: Structured storage for AtomSpace snapshots

### Planned Integration

1. **Real GHOST Engine**:
   - Connect to CogServer via telnet/socket
   - Implement Guile Scheme bridge
   - Load GHOST rules from `opencog/opencog/ghost/`

2. **AtomSpace Bridge**:
   - Compile C++ bridge library
   - Integrate with HurdCog IPC
   - Real-time AtomSpace queries

3. **PLN Integration**:
   - Execute inference queries
   - Display inference chains
   - Track reasoning activity

4. **ECAN Integration**:
   - Real attention allocation data
   - Importance diffusion visualization
   - Attentional focus tracking

---

## Future Enhancements

### Short-term

- Voice input/output (Web Speech API + TTS)
- Conversation export functionality
- Search across conversations
- Multi-bot conversation fusion

### Medium-term

- Real GHOST engine integration
- Real AtomSpace connection
- WebSocket for real-time updates
- Advanced NLP pipeline integration

### Long-term

- Personality and emotion modeling
- Multi-modal interaction (images, files)
- Distributed AtomSpace synchronization
- Self-evolving architecture

---

## Repository Structure

```
agi-os-interface/
├── client/                    # Frontend React application
│   ├── src/
│   │   ├── pages/
│   │   │   ├── ChatInterface.tsx
│   │   │   └── CognitiveVisualization.tsx
│   │   ├── components/
│   │   │   └── AtomSpaceGraph.tsx
│   │   └── index.css         # Dark mode theme
├── server/                    # Backend tRPC server
│   ├── routers.ts            # API routes
│   ├── db.ts                 # Database queries
│   ├── bots/
│   │   └── adapters.ts       # Bot implementations
│   └── chat.test.ts          # Test suite
├── drizzle/
│   └── schema.ts             # Database schema
└── todo.md                   # Project task tracking
```

---

## Related Documentation

- [CHATBOT_INTEGRATION_ANALYSIS.md](CHATBOT_INTEGRATION_ANALYSIS.md) - Detailed analysis of all chatbot systems
- [AGI_OS_INTEGRATION_PLAN.md](AGI_OS_INTEGRATION_PLAN.md) - Overall integration roadmap
- [GHOST README](opencog/opencog/ghost/README.md) - GHOST engine documentation
- [AtomSpace Bridge](src/atomspace-hurdcog-bridge/) - Bridge implementation

---

## Conclusion

The AGI-OS Conversational Interface successfully unifies all chatbot systems from the OCC repository into a single, coherent web application with dark mode UI, cognitive visualization, and comprehensive backend infrastructure. While current implementations use simulated adapters, the architecture is designed for seamless integration with production GHOST, AtomSpace, and HurdCog systems.

**Next Steps**: Implement real GHOST engine connection, deploy AtomSpace C++ bridge, and integrate with HurdCog IPC for full cognitive OS capabilities.

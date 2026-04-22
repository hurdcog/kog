using UnityEngine;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace HurdCog.Unity
{
    /// <summary>
    /// Cognitive Agent component for Unity3D integration
    /// Provides cognitive behavior and real-time connection to the cognitive mesh
    /// </summary>
    public class CognitiveAgent : MonoBehaviour
    {
        [Header("Cognitive Configuration")]
        public string agentType = "unity3d_agent";
        public List<string> capabilities = new List<string> { "perception", "action", "learning" };
        
        [Header("Connection Settings")]
        public string serverUrl = "http://localhost:8000";
        public string websocketUrl = "ws://localhost:8000/ws";
        public bool autoConnect = true;
        
        [Header("Visualization")]
        public AttentionVisualizer attentionVisualizer;
        public CognitiveStateDisplay stateDisplay;
        
        // Current state
        private CognitiveState currentState;
        private AttentionAllocation currentAttention;
        private string agentId;
        private CognitiveWebSocketClient wsClient;
        
        void Start()
        {
            if (autoConnect)
            {
                ConnectToCognitiveMesh();
            }
        }
        
        /// <summary>
        /// Connect to the distributed cognitive mesh
        /// </summary>
        public async void ConnectToCognitiveMesh()
        {
            try
            {
                // Register agent via REST API
                var registration = new AgentRegistration
                {
                    agentType = agentType,
                    capabilities = capabilities,
                    metadata = new Dictionary<string, object>
                    {
                        { "scene", UnityEngine.SceneManagement.SceneManager.GetActiveScene().name },
                        { "position", transform.position.ToString() },
                        { "timestamp", DateTime.Now.ToString("o") }
                    }
                };
                
                var response = await RestClient.Post<AgentInfo>(
                    $"{serverUrl}/api/v1/agents/register",
                    registration
                );
                
                agentId = response.agentId;
                Debug.Log($"Cognitive agent registered: {agentId}");
                
                // Connect WebSocket for real-time communication
                wsClient = new CognitiveWebSocketClient(websocketUrl);
                wsClient.OnCognitiveStateUpdate += HandleCognitiveStateUpdate;
                wsClient.OnAttentionChange += HandleAttentionChange;
                wsClient.OnTaskCompletion += HandleTaskCompletion;
                await wsClient.Connect();
                
                Debug.Log("Connected to cognitive mesh via WebSocket");
            }
            catch (Exception e)
            {
                Debug.LogError($"Failed to connect to cognitive mesh: {e.Message}");
            }
        }
        
        /// <summary>
        /// Submit a cognitive task for processing
        /// </summary>
        public async Task<TaskResult> SubmitCognitiveTask(string description, int priority = 5)
        {
            var task = new CognitiveTask
            {
                description = description,
                priority = priority,
                context = new Dictionary<string, object>
                {
                    { "agent_id", agentId },
                    { "position", transform.position.ToString() },
                    { "timestamp", DateTime.Now.ToString("o") }
                }
            };
            
            if (wsClient != null && wsClient.IsConnected)
            {
                // Use WebSocket for real-time task submission
                return await wsClient.SubmitTask(task);
            }
            else
            {
                // Fallback to REST API
                return await RestClient.Post<TaskResult>(
                    $"{serverUrl}/api/v1/cognitive/process",
                    task
                );
            }
        }
        
        /// <summary>
        /// Handle cognitive state updates from the mesh
        /// </summary>
        private void HandleCognitiveStateUpdate(CognitiveState state)
        {
            currentState = state;
            
            if (stateDisplay != null)
            {
                stateDisplay.UpdateDisplay(state);
            }
            
            // Apply cognitive state to agent behavior
            ApplyCognitiveBehavior(state);
        }
        
        /// <summary>
        /// Handle attention allocation changes
        /// </summary>
        private void HandleAttentionChange(AttentionAllocation attention)
        {
            currentAttention = attention;
            
            if (attentionVisualizer != null)
            {
                attentionVisualizer.UpdateVisualization(attention);
            }
            
            // Adjust behavior based on attention allocation
            AdaptToAttention(attention);
        }
        
        /// <summary>
        /// Handle task completion notifications
        /// </summary>
        private void HandleTaskCompletion(TaskResult result)
        {
            Debug.Log($"Task completed: {result.taskId}, Status: {result.status}, Confidence: {result.confidence}");
            
            // Process task result
            ProcessTaskResult(result);
        }
        
        /// <summary>
        /// Apply cognitive behavior based on current state
        /// </summary>
        private void ApplyCognitiveBehavior(CognitiveState state)
        {
            // Adjust agent behavior based on cognitive load
            if (state.cognitiveLoad > 0.8f)
            {
                // High cognitive load - reduce activity
                ReduceActivity();
            }
            else if (state.cognitiveLoad < 0.3f)
            {
                // Low cognitive load - increase activity
                IncreaseActivity();
            }
        }
        
        /// <summary>
        /// Adapt behavior to attention allocation
        /// </summary>
        private void AdaptToAttention(AttentionAllocation attention)
        {
            // Check if this agent has attention
            if (attention.allocations.ContainsKey(agentId))
            {
                float attentionLevel = attention.allocations[agentId];
                // Adjust behavior based on attention level
                AdjustActivityLevel(attentionLevel);
            }
        }
        
        /// <summary>
        /// Process completed task results
        /// </summary>
        private void ProcessTaskResult(TaskResult result)
        {
            // Implement task result processing logic
            // This is where the agent would act on cognitive decisions
        }
        
        private void ReduceActivity() { /* Implementation */ }
        private void IncreaseActivity() { /* Implementation */ }
        private void AdjustActivityLevel(float level) { /* Implementation */ }
        
        void OnDestroy()
        {
            // Cleanup WebSocket connection
            if (wsClient != null)
            {
                wsClient.Disconnect();
            }
        }
    }
    
    // Data models matching API server
    [Serializable]
    public class CognitiveState
    {
        public DateTime timestamp;
        public int activeProcesses;
        public float memoryUsage;
        public List<string> attentionFocus;
        public float cognitiveLoad;
        public string status;
    }
    
    [Serializable]
    public class AttentionAllocation
    {
        public DateTime timestamp;
        public Dictionary<string, float> allocations;
        public float totalAttention;
        public string focusTarget;
    }
    
    [Serializable]
    public class CognitiveTask
    {
        public string taskId;
        public string description;
        public int priority;
        public Dictionary<string, object> context;
        public int? timeout;
    }
    
    [Serializable]
    public class TaskResult
    {
        public string taskId;
        public string status;
        public object result;
        public float confidence;
        public float processingTime;
        public DateTime timestamp;
    }
    
    [Serializable]
    public class AgentRegistration
    {
        public string agentType;
        public List<string> capabilities;
        public Dictionary<string, object> metadata;
    }
    
    [Serializable]
    public class AgentInfo
    {
        public string agentId;
        public string agentType;
        public List<string> capabilities;
        public string status;
        public DateTime connectionTime;
        public Dictionary<string, object> metadata;
    }
}

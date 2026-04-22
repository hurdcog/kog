using UnityEngine;
using System.Collections.Generic;

namespace HurdCog.Unity
{
    /// <summary>
    /// Visualizes cognitive attention allocation in 3D space
    /// Creates heat maps and particle effects to show attention flow
    /// </summary>
    public class AttentionVisualizer : MonoBehaviour
    {
        [Header("Visualization Settings")]
        public Material heatMapMaterial;
        public ParticleSystem attentionParticles;
        public Color highAttentionColor = Color.red;
        public Color lowAttentionColor = Color.blue;
        public float particleEmissionRate = 10f;
        
        [Header("Heat Map Settings")]
        public int heatMapResolution = 64;
        public float heatMapUpdateInterval = 0.1f;
        
        private Texture2D heatMapTexture;
        private float lastUpdateTime;
        private Dictionary<string, Vector3> attentionPositions = new Dictionary<string, Vector3>();
        
        void Start()
        {
            InitializeHeatMap();
            InitializeParticles();
        }
        
        void InitializeHeatMap()
        {
            heatMapTexture = new Texture2D(heatMapResolution, heatMapResolution);
            heatMapTexture.wrapMode = TextureWrapMode.Clamp;
            
            if (heatMapMaterial != null)
            {
                heatMapMaterial.mainTexture = heatMapTexture;
            }
        }
        
        void InitializeParticles()
        {
            if (attentionParticles != null)
            {
                var emission = attentionParticles.emission;
                emission.rateOverTime = particleEmissionRate;
            }
        }
        
        /// <summary>
        /// Update visualization based on current attention allocation
        /// </summary>
        public void UpdateVisualization(AttentionAllocation attention)
        {
            if (Time.time - lastUpdateTime < heatMapUpdateInterval)
                return;
            
            lastUpdateTime = Time.time;
            
            // Update heat map
            UpdateHeatMap(attention);
            
            // Update particle system
            UpdateAttentionParticles(attention);
        }
        
        void UpdateHeatMap(AttentionAllocation attention)
        {
            // Clear heat map
            Color[] pixels = new Color[heatMapResolution * heatMapResolution];
            
            // Generate heat map based on attention allocations
            foreach (var allocation in attention.allocations)
            {
                float attentionLevel = allocation.Value;
                Color attentionColor = Color.Lerp(lowAttentionColor, highAttentionColor, attentionLevel);
                
                // If we have a position for this agent, draw it on the heat map
                if (attentionPositions.ContainsKey(allocation.Key))
                {
                    Vector3 pos = attentionPositions[allocation.Key];
                    DrawAttentionBlob(pixels, pos, attentionColor, attentionLevel);
                }
            }
            
            // Apply to texture
            heatMapTexture.SetPixels(pixels);
            heatMapTexture.Apply();
        }
        
        void DrawAttentionBlob(Color[] pixels, Vector3 worldPos, Color color, float intensity)
        {
            // Convert world position to heat map coordinates
            int centerX = Mathf.RoundToInt((worldPos.x + 10) / 20 * heatMapResolution);
            int centerY = Mathf.RoundToInt((worldPos.z + 10) / 20 * heatMapResolution);
            
            int radius = Mathf.RoundToInt(intensity * 10);
            
            for (int y = -radius; y <= radius; y++)
            {
                for (int x = -radius; x <= radius; x++)
                {
                    int px = centerX + x;
                    int py = centerY + y;
                    
                    if (px >= 0 && px < heatMapResolution && py >= 0 && py < heatMapResolution)
                    {
                        float distance = Mathf.Sqrt(x * x + y * y);
                        if (distance <= radius)
                        {
                            float falloff = 1f - (distance / radius);
                            int index = py * heatMapResolution + px;
                            pixels[index] = Color.Lerp(pixels[index], color, falloff * intensity);
                        }
                    }
                }
            }
        }
        
        void UpdateAttentionParticles(AttentionAllocation attention)
        {
            if (attentionParticles == null)
                return;
            
            // Adjust particle emission based on total attention
            var emission = attentionParticles.emission;
            emission.rateOverTime = particleEmissionRate * attention.totalAttention;
            
            // Color particles based on focus target
            if (!string.IsNullOrEmpty(attention.focusTarget))
            {
                var main = attentionParticles.main;
                main.startColor = highAttentionColor;
            }
        }
        
        /// <summary>
        /// Register position for an agent to visualize in heat map
        /// </summary>
        public void RegisterAgentPosition(string agentId, Vector3 position)
        {
            attentionPositions[agentId] = position;
        }
        
        /// <summary>
        /// Visualize attention flow between two points
        /// </summary>
        public void VisualizeAttentionFlow(Vector3 from, Vector3 to, float strength)
        {
            if (attentionParticles != null)
            {
                // Emit particles along the flow path
                Vector3 direction = (to - from).normalized;
                int particleCount = Mathf.RoundToInt(strength * 10);
                
                for (int i = 0; i < particleCount; i++)
                {
                    float t = i / (float)particleCount;
                    Vector3 position = Vector3.Lerp(from, to, t);
                    
                    ParticleSystem.EmitParams emitParams = new ParticleSystem.EmitParams();
                    emitParams.position = position;
                    emitParams.velocity = direction * 2f;
                    emitParams.startColor = Color.Lerp(lowAttentionColor, highAttentionColor, strength);
                    emitParams.startSize = 0.1f;
                    
                    attentionParticles.Emit(emitParams, 1);
                }
            }
        }
        
        void OnDrawGizmos()
        {
            // Draw attention positions in editor
            foreach (var pos in attentionPositions)
            {
                Gizmos.color = Color.yellow;
                Gizmos.DrawWireSphere(pos.Value, 0.5f);
            }
        }
    }
}

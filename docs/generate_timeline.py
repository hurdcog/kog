import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from datetime import datetime, timedelta
import numpy as np

# Define phases and milestones
phases = {
    "Phase 1: Foundation": {
        "color": "#0f4c81",
        "milestones": [
            ("M1.1: Dev Environment Setup", 2),
            ("M1.2: Hypergraph Memory", 8),
            ("M1.3: Echo Propagation", 4),
            ("M1.4: Cognitive Grammar", 4),
            ("M1.5: Core Engine Integration", 4),
            ("M1.6: Basic Membrane Structure", 4),
        ]
    },
    "Phase 2: Integration": {
        "color": "#16537e",
        "milestones": [
            ("M2.1: Communication Protocols", 2),
            ("M2.2: Three-Membrane Structure", 4),
            ("M2.3: Security Infrastructure", 4),
            ("M2.4: Synergy Orchestrator", 6),
        ]
    },
    "Phase 3: Enhancement": {
        "color": "#1f5f7a",
        "milestones": [
            ("M3.1: Browser Automation", 4),
            ("M3.2: ML Integration", 4),
            ("M3.3: Evolution Engine", 4),
            ("M3.4: Introspection System", 4),
            ("M3.5: Monitoring Dashboard", 2),
            ("M3.6: Sensory Motor Interface", 2),
            ("M3.7: Performance Optimization", 6),
        ]
    },
    "Phase 4: Evolution": {
        "color": "#2a6b77",
        "milestones": [
            ("M4.1: Meta-Learning Framework", 4),
            ("M4.2: Autonomous Adaptation", 6),
            ("M4.3: Structural Plasticity", 6),
            ("M4.4: Multi-Scale Emergence", 10),
        ]
    },
    "Buffer & Polish": {
        "color": "#e94560",
        "milestones": [
            ("Final Testing & Documentation", 8),
        ]
    }
}

# Calculate timeline
start_date = datetime(2025, 11, 1)
current_week = 0
timeline_data = []

for phase_name, phase_info in phases.items():
    for milestone_name, duration_weeks in phase_info["milestones"]:
        timeline_data.append({
            "phase": phase_name,
            "milestone": milestone_name,
            "start_week": current_week,
            "duration": duration_weeks,
            "color": phase_info["color"]
        })
        current_week += duration_weeks

# Create figure
fig, ax = plt.subplots(figsize=(16, 10))
fig.patch.set_facecolor('white')
ax.set_facecolor('white')

# Plot milestones
y_pos = 0
y_labels = []
y_ticks = []

for item in timeline_data:
    ax.barh(y_pos, item["duration"], left=item["start_week"], 
            height=0.6, color=item["color"], alpha=0.8, 
            edgecolor='white', linewidth=1.5)

    # Add milestone label
    y_labels.append(item["milestone"])
    y_ticks.append(y_pos)

    # Add duration text
    mid_point = item["start_week"] + item["duration"] / 2
    ax.text(mid_point, y_pos, f'{item["duration"]}w', 
            ha='center', va='center', color='white', 
            fontweight='bold', fontsize=8)

    y_pos += 1

# Add phase separators and labels
current_phase = None
phase_start = 0
for i, item in enumerate(timeline_data):
    if item["phase"] != current_phase:
        if current_phase is not None:
            # Draw separator line
            ax.axhline(y=i-0.5, color='#cccccc', linewidth=2, linestyle='--')

            # Add phase label on the right
            phase_mid = (phase_start + i - 1) / 2
            ax.text(current_week + 2, phase_mid, current_phase, 
                    va='center', ha='left', fontsize=11, 
                    fontweight='bold', color=phases[current_phase]["color"])

        current_phase = item["phase"]
        phase_start = i

# Add last phase label
phase_mid = (phase_start + len(timeline_data) - 1) / 2
ax.text(current_week + 2, phase_mid, current_phase, 
        va='center', ha='left', fontsize=11, 
        fontweight='bold', color=phases[current_phase]["color"])

# Formatting
ax.set_yticks(y_ticks)
ax.set_yticklabels(y_labels, fontsize=9)
ax.set_xlabel('Weeks from Project Start', fontsize=12, fontweight='bold')
ax.set_title('Deep Tree Echo Architecture: Implementation Timeline\n24-Month Project Plan', 
             fontsize=16, fontweight='bold', pad=20)

# Add month markers on top
ax2 = ax.twiny()
month_positions = []
month_labels = []
for month_offset in range(0, 25):
    week_offset = month_offset * 4.33  # Average weeks per month
    if week_offset <= current_week:
        month_positions.append(week_offset)
        date = start_date + timedelta(weeks=week_offset)
        month_labels.append(date.strftime('%b %Y'))

ax2.set_xlim(ax.get_xlim())
ax2.set_xticks(month_positions)
ax2.set_xticklabels(month_labels, rotation=45, ha='left', fontsize=8)
ax2.set_xlabel('Timeline (Months)', fontsize=12, fontweight='bold')

# Grid
ax.grid(axis='x', alpha=0.3, linestyle=':', linewidth=0.8)
ax.set_axisbelow(True)

# Add legend
legend_elements = [
    mpatches.Patch(color=phases["Phase 1: Foundation"]["color"], label='Phase 1: Foundation (6 months)'),
    mpatches.Patch(color=phases["Phase 2: Integration"]["color"], label='Phase 2: Integration (4 months)'),
    mpatches.Patch(color=phases["Phase 3: Enhancement"]["color"], label='Phase 3: Enhancement (6 months)'),
    mpatches.Patch(color=phases["Phase 4: Evolution"]["color"], label='Phase 4: Evolution (6 months)'),
    mpatches.Patch(color=phases["Buffer & Polish"]["color"], label='Buffer & Polish (2 months)'),
]
ax.legend(handles=legend_elements, loc='lower right', fontsize=9, framealpha=0.9)

# Add total duration annotation
total_weeks = current_week
total_months = total_weeks / 4.33
ax.text(0.02, 0.98, f'Total Duration: {total_weeks} weeks (~{total_months:.1f} months)', 
        transform=ax.transAxes, fontsize=10, verticalalignment='top',
        bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

plt.tight_layout()
plt.savefig('/home/ubuntu/occ/docs/project_timeline.png', dpi=300, bbox_inches='tight', facecolor='white')
print("Timeline visualization created: /home/ubuntu/occ/docs/project_timeline.png")

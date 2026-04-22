# Diagram Resolution Information

## High-Resolution Diagrams

All diagrams in this directory have been rendered at **3x scale** for maximum clarity and detail.

### Resolution Specifications

| Diagram | Dimensions | File Size | Scale Factor |
|---------|-----------|-----------|--------------|
| Deep Tree Echo Architecture | 2352 x 1752 px | 197 KB | 3x |
| AAR Framework | 2352 x 2256 px | 248 KB | 3x |
| OCC Cognitive Synergy Integrated | 2352 x 1275 px | 227 KB | 3x |

**DPI**: 72 DPI (standard for screen display)  
**Format**: PNG with transparent background  
**Rendering Engine**: Mermaid CLI (mmdc) v11.x

### Quality Improvements

**Before** (Original Resolution):
- Deep Tree Echo: 11 KB, lower resolution
- AAR Framework: 25 KB, lower resolution
- OCC Integrated: 15 KB, lower resolution

**After** (High Resolution):
- Deep Tree Echo: 197 KB (~18x larger), **2352x1752 pixels**
- AAR Framework: 248 KB (~10x larger), **2352x2256 pixels**
- OCC Integrated: 227 KB (~15x larger), **2352x1275 pixels**

### Benefits of High Resolution

1. **Crisp Text**: All labels, titles, and annotations are sharp and readable
2. **Clear Lines**: Connection lines and arrows are smooth without pixelation
3. **Professional Quality**: Suitable for presentations, papers, and publications
4. **Zoom-Friendly**: Can be zoomed in without losing clarity
5. **Print-Ready**: High enough resolution for printed materials

### Rendering Command

To regenerate diagrams at high resolution:

```bash
cd /path/to/occ/docs/diagrams

# Install mermaid-cli if not already installed
npm install -g @mermaid-js/mermaid-cli

# Render at 3x scale with transparent background
mmdc -i deep_tree_echo_architecture.mmd -o deep_tree_echo_architecture.png -s 3 -b transparent
mmdc -i aar_framework.mmd -o aar_framework.png -s 3 -b transparent
mmdc -i occ_cognitive_synergy_integrated.mmd -o occ_cognitive_synergy_integrated.png -s 3 -b transparent
```

### Scale Factor Options

- **1x** (default): Standard web resolution (~800px width)
- **2x**: Retina display quality (~1600px width)
- **3x** (current): High-resolution for presentations and print (~2400px width)
- **4x+**: Ultra-high resolution (may result in very large files)

**Recommendation**: 3x provides the best balance between quality and file size for most use cases.

### File Management

**Current Files**:
- `*.mmd` - Mermaid source files (editable)
- `*.png` - High-resolution rendered images (3x scale)
- `*_lowres.png` - Original low-resolution versions (archived)

**Backup**: Original low-resolution versions are preserved as `*_lowres.png` for reference.

### Usage Guidelines

**For Web Display**:
- Use the high-resolution PNG files directly
- Browsers will automatically scale down for optimal display
- Retina displays will show crisp, clear diagrams

**For Presentations**:
- Import PNG files directly into PowerPoint, Keynote, or Google Slides
- Diagrams will maintain clarity even when projected on large screens

**For Publications**:
- Current resolution (2352px width) is suitable for most academic papers
- For journal submissions, check specific resolution requirements
- Can be further upscaled if needed using vector-based tools

**For Print**:
- At 300 DPI: ~7.8 inches wide (suitable for standard paper)
- At 150 DPI: ~15.6 inches wide (suitable for posters)
- Transparent background allows easy integration with colored backgrounds

### Technical Details

**Rendering Process**:
1. Mermaid source (`.mmd`) is parsed by mermaid-cli
2. Diagram is rendered in a headless browser (Puppeteer/Chromium)
3. SVG is generated with specified scale factor
4. SVG is converted to PNG with transparent background
5. PNG is saved with optimized compression

**Color Scheme** (preserved in high-resolution):
- Agent components: Red tones (#ffebee, #c62828)
- Arena components: Blue tones (#e3f2fd, #1565c0)
- Relation components: Purple tones (#f3e5f5, #6a1b9a)
- Deep Tree Echo: Orange tones (#fff3e0, #f57c00)
- Metrics: Green tones (#e8f5e9, #388e3c)
- Emergence: Yellow tones (#fff9c4, #f9a825)

### Troubleshooting

**If diagrams appear blurry**:
- Ensure you're viewing the PNG files, not the low-res versions
- Check that your image viewer supports high-DPI displays
- Try opening in a different application

**If file sizes are too large**:
- Current 3x scale provides good balance
- Can reduce to 2x scale if needed: `mmdc -i input.mmd -o output.png -s 2`
- Use image optimization tools if necessary

**If rendering fails**:
- Ensure mermaid-cli is installed: `npm install -g @mermaid-js/mermaid-cli`
- Check that Chromium/Puppeteer is properly installed
- Verify Mermaid syntax in `.mmd` files

### Version History

**v1.0** (Initial Release):
- Standard resolution diagrams (~800px width)
- File sizes: 11-25 KB

**v2.0** (Current - High Resolution):
- 3x scale factor (~2400px width)
- File sizes: 197-248 KB
- Improved clarity and detail
- Professional quality for presentations and publications

---

**Last Updated**: November 2, 2025  
**Rendering Tool**: Mermaid CLI v11.x  
**Scale Factor**: 3x  
**Format**: PNG with transparent background

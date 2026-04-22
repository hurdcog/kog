# Test Catalog Generator

This directory contains a comprehensive test catalog extracted from open issues documentation, along with tools for generating test parameters and integration hooks for cognitive grammar and GGML kernel shapes.

## Generated Files

### Core Catalog Files

- **`test-catalog.json`** - JSON format test catalog with comprehensive test parameters
- **`test-catalog.scm`** - Scheme format for cognitive kernel integration
- **`open-issues.md`** - Source documentation (manually maintained)

### Example Files

- **`example_unit_test.py`** - Example unit test generated from catalog data
- **`example_github_actions.yml`** - Example GitHub Actions workflow for test automation
- **`README_test_catalog.md`** - This documentation file

## Catalog Structure

Each test parameter entry includes:

- **issue_title** - Descriptive title of the issue
- **description** - Detailed description of the problem
- **impact_area** - Area of system impact
- **failure_mode** - Type of failure (performance, stability, etc.)
- **resolution_criteria** - Testable criteria for resolution
- **dependencies** - System components involved
- **category** - High-level grouping (performance, stability, hardware, etc.)
- **subcategory** - More specific categorization
- **status** - Current development status
- **solutions** - Proposed or implemented solutions

### Integration Hooks

#### Cognitive Grammar Hooks
- **concept_nodes** - AtomSpace concept representations
- **predicate_nodes** - Relationship predicates
- **inference_rules** - Logical inference patterns
- **attention_allocation** - Priority and focus areas

#### GGML Kernel Shapes
- **tensor_shapes** - Tensor dimensions for ML operations
- **kernel_operations** - Supported kernel operations
- **memory_layout** - Memory allocation specifications

## Usage

### Generating the Catalog

```bash
# Generate from default location
python3 .github/scripts/generate_test_catalog.py

# Generate from custom input file
python3 .github/scripts/generate_test_catalog.py /path/to/input.md /path/to/output/dir

# Make executable and run
chmod +x .github/scripts/generate_test_catalog.py
./.github/scripts/generate_test_catalog.py
```

### Using the Catalog

```bash
# Run demonstration script
python3 .github/scripts/demo_test_catalog.py

# View catalog metadata
jq '.metadata' docs/open-issues/test-catalog.json

# List all categories
jq '.test_catalog | keys' docs/open-issues/test-catalog.json

# Get performance issues
jq '.test_catalog.performance' docs/open-issues/test-catalog.json
```

### Integration with Cognitive Kernel

```scheme
;; Load the test catalog module
(use-modules (test-catalog))

;; Get test parameters for a category
(get-test-parameters 'performance)

;; Access cognitive hooks
cognitive-test-hooks

;; Access GGML shapes
ggml-kernel-shapes
```

## Categories

The catalog groups issues into the following categories:

- **performance** - Performance-related issues and bottlenecks
- **stability** - System stability and reliability issues  
- **hardware** - Hardware support and driver issues
- **technical** - Technical challenges and limitations
- **development** - Active development areas
- **implementation** - Implementation challenges
- **bugs** - Bug categories and classifications

## Test Generation Examples

### Unit Tests

The catalog can generate unit test scaffolding:

```python
# Generated test structure
class TestIssue(unittest.TestCase):
    def test_baseline(self):
        # Current failing state
        
    def test_resolution_criteria(self):
        # Test each resolution criteria
        
    def test_performance(self):
        # Performance regression tests
```

### GitHub Actions

Automated testing workflows:

```yaml
name: Test Catalog Validation
on: [push, pull_request, schedule]

jobs:
  validate-catalog:
    # Validate catalog structure
    
  test-categories:
    # Test each category
    
  cognitive-integration:
    # Test cognitive grammar hooks
    
  generate-report:
    # Generate progress reports
```

## Empirical Ranking

The catalog enables empirical ranking of features/solutions by test pass rate:

1. **Baseline Tests** - Establish current failure modes
2. **Resolution Tests** - Test implementation of solutions
3. **Regression Tests** - Prevent performance degradation
4. **Integration Tests** - Test cognitive/GGML integration

Pass rates can be tracked over time to measure progress and prioritize development efforts.

## Future Integration

### Cognitive Grammar Integration

The catalog includes hooks for:
- Concept extraction and representation
- Attention allocation based on issue priority
- Inference rules for problem-solution relationships
- Learning feedback from test results

### GGML Kernel Shapes

Specifications for:
- Issue embedding into vector spaces
- Category classification neural networks
- Solution ranking algorithms
- Memory-efficient processing layouts

## Maintenance

### Updating the Catalog

When `open-issues.md` is updated:

1. Run the generator script
2. Review generated catalog for accuracy
3. Update any custom integration code
4. Run validation tests

### Adding New Categories

To add new categories:

1. Update the `_normalize_category` method in the generator
2. Add category-specific test generation logic
3. Update cognitive and GGML hooks as needed
4. Regenerate the catalog

## Dependencies

- **Python 3.8+** - Core generator and demo scripts
- **jq** - JSON processing (optional, for CLI queries)
- **Guile Scheme** - For Scheme catalog usage (optional)

## Files in this Directory

```
docs/open-issues/
├── README_test_catalog.md          # This documentation
├── open-issues.md                  # Source documentation
├── test-catalog.json               # JSON format catalog
├── test-catalog.scm                # Scheme format catalog
├── example_unit_test.py            # Unit test example
└── example_github_actions.yml      # GitHub Actions example
```

## Related Scripts

```
.github/scripts/
├── generate_test_catalog.py        # Main generator script
└── demo_test_catalog.py            # Demonstration script
```

---

*This test catalog serves as the foundation for implementing empirical testing and ranking of GNU Hurd features and solutions, with scaffolding for future cognitive grammar and GGML kernel integration.*
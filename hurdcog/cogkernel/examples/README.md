# Cognitive Services Examples

This directory contains example programs demonstrating how to integrate OpenCog cognitive services with Hurd components.

## Examples

### 1. Simple Cognitive Server

**File**: `simple-cognitive-server.c`

A demonstration server that shows how to:
- Initialize the cognitive interface
- Register with the cognitive system
- Report events and metrics
- Query for optimization suggestions
- Apply cognitive optimizations
- Adapt behavior based on learned patterns

**Build**:
```bash
make simple-cognitive-server
```

**Run**:
```bash
./simple-cognitive-server
```

**Expected Output**:
The server will:
1. Initialize cognitive interface
2. Process 1000 simulated requests
3. Periodically report statistics
4. Receive and apply cognitive optimizations
5. Display final statistics showing adaptation

**Key Features Demonstrated**:
- Event reporting to AtomSpace
- State updates for cognitive learning
- Optimization query and application
- Performance metric tracking
- Adaptive configuration

### Building All Examples

```bash
make all
```

### Cleaning

```bash
make clean
```

## Integration Pattern

All examples follow this pattern:

```c
/* 1. Initialize cognitive interface */
error_t err = hurd_cognitive_init();
if (err) {
    /* Handle error or run without cognitive features */
}

/* 2. Register component */
hurd_cognitive_register("component-name");

/* 3. Report events during operation */
event_t event = {
    .type = EVENT_PERFORMANCE_METRIC,
    .component = "component-name",
    .data = &metric_value,
    .data_size = sizeof(metric_value)
};
hurd_cognitive_add_event(&event);

/* 4. Update state periodically */
state_update_t state = {
    .component = "component-name",
    .state_key = "some_metric",
    .value = current_value,
    .confidence = 1.0
};
hurd_cognitive_update_state(&state);

/* 5. Query for optimizations */
optimization_t opt;
err = hurd_cognitive_get_optimization(COMP_TYPE, &opt);
if (!err && opt.confidence > threshold) {
    /* Apply optimization */
}
```

## Learning More

- [OpenCog Hurd Integration](../../docs/OPENCOG_HURD_INTEGRATION.md)
- [Cognitive Services API](../../docs/COGNITIVE_SERVICES_API.md)
- [AtomSpace Implementation](../atomspace/)
- [Cognitive Kernel README](../README.md)

## Contributing

To add a new example:

1. Create a new `.c` file in this directory
2. Follow the integration pattern above
3. Add build rule to Makefile
4. Update this README with description
5. Test thoroughly

## Notes

These examples are designed to demonstrate the cognitive integration concepts. In a full implementation:

- The cognitive system would have real OpenCog AtomSpace backend
- PLN reasoning would provide actual optimization suggestions
- Pattern mining would learn real patterns from operation
- ECAN would prioritize attention allocation
- The system would truly adapt and improve over time

The current implementation provides the API and structure, showing how Hurd components can integrate with a cognitive architecture to become adaptive and intelligent.

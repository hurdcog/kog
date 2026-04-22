# libvalue.so

**Source:** `atomspace/opencog/atoms/value/`  
**Role:** The Value type system — everything that can be attached to an atom as an annotation, stream, or structured payload.

## What it does

*Values* are the data layer of AtomSpace.  While Atoms represent graph structure, Values carry numeric, string, and composite data.  This library provides concrete value types, factory infrastructure, and streaming primitives used throughout the AGI pipeline.

## Object files

| Object | Purpose |
|--------|---------|
| `Value.cc.o` | `Value` abstract base class and reference-counting |
| `ValueFactory.cc.o` | `createValue()` factory — deserialises values from s-expressions |
| `BoolValue.cc.o` | Boolean sequence value |
| `FloatValue.cc.o` | Floating-point vector value |
| `StringValue.cc.o` | String sequence value |
| `LinkValue.cc.o` | Nested list of Values |
| `ContainerValue.cc.o` | Mutable container Value |
| `UnisetValue.cc.o` | Unordered set Value |
| `SectionValue.cc.o` | Connector/section structure value (used by language-learning) |
| `QueueValue.cc.o` | `QueueValue` — thread-safe FIFO stream of Values |
| `FutureStream.cc.o` | Lazy future-based value stream |
| `RandomStream.cc.o` | Stream that generates random floats on demand |
| `FormulaStream.cc.o` | Formula-evaluated streaming value |
| `VoidValue.cc.o` | Sentinel null value |

## Key public headers

- `opencog/atoms/value/Value.h`
- `opencog/atoms/value/FloatValue.h`
- `opencog/atoms/value/QueueValue.h`
- `opencog/atoms/value/ValueFactory.h`

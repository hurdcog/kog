# libclearbox.so

**Source:** `atomspace/opencog/atoms/reduct/`  
**Role:** Arithmetic and boolean reduction atoms — Atomese operators for numeric computation, logic, and list processing that evaluate in the pattern-matcher.

## What it does

Implements the "clear-box" (transparent) arithmetic layer of Atomese: atoms whose evaluation rules are fully defined within the system, requiring no external grounding.  They form the basis of formula evaluation in truth values, attention allocation signals, and numerical reasoning.

## Object files

| Object | Purpose |
|--------|---------|
| `ArithmeticLink.cc.o` | `ArithmeticLink` — abstract base for numeric binary links |
| `PlusLink.cc.o` | `PlusLink` — variadic addition |
| `MinusLink.cc.o` | `MinusLink` — subtraction |
| `TimesLink.cc.o` | `TimesLink` — variadic multiplication |
| `DivideLink.cc.o` | `DivideLink` — division |
| `MaxLink.cc.o` | `MaxLink` — maximum of arguments |
| `MinLink.cc.o` | `MinLink` — minimum of arguments |
| `NumericFunctionLink.cc.o` | `NumericFunctionLink` — base for single-argument numeric functions |
| `ImpulseLink.cc.o` | `ImpulseLink` — unit impulse (returns 1 if condition true, else 0) |
| `BoolOpLink.cc.o` | `BoolOpLink` — boolean AND / OR base |
| `FoldLink.cc.o` | `FoldLink` — left fold / reduce over a list of atoms |
| `AccumulateLink.cc.o` | `AccumulateLink` — running accumulation operator |
| `DecimateLink.cc.o` | `DecimateLink` — removes atoms from a collection by index |
| `ElementOfLink.cc.o` | `ElementOfLink` — membership test in a set atom |

## Key public headers

- `opencog/atoms/reduct/ArithmeticLink.h`
- `opencog/atoms/reduct/PlusLink.h`
- `opencog/atoms/reduct/BoolOpLink.h`

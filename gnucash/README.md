# Gnucash OpenCog Integration

This package integrates Gnucash double-entry accounting software with OpenCog for cognitive financial reasoning and attention management.

## Features

- **Cognitive Accounting**: Convert financial transactions and accounts into AtomSpace representations
- **Attention Management**: Apply cognitive attention mechanisms to significant balance changes
- **ECAN Integration**: Enhanced Economic Attention Networks for financial pattern recognition
- **Scheme Bindings**: Guile Scheme interface for direct AtomSpace integration

## Architecture

```
Gnucash Core ←→ OpenCog Adapters ←→ AtomSpace
     ↓                  ↓                ↓
  Financial DB    Cognitive Layer   Knowledge Graph
```

## API Overview

### Scheme Functions

- `(gnucash-account->atom account-data)` - Convert account to AtomSpace
- `(gnucash-transaction->atom transaction-data)` - Convert transaction to cognitive representation  
- `(cognitive-balance-attention account-atom threshold)` - Apply attention to balance changes

### Usage Example

```scheme
(use-modules (opencog gnucash))

;; Convert account to cognitive representation
(define savings-account 
  (gnucash-account->atom '((name . "Savings") 
                          (balance . 5000.00))))

;; Create transaction representation
(define rent-payment
  (gnucash-transaction->atom '((from-account . "Checking")
                              (to-account . "Rent Expense") 
                              (amount . 1200.00))))

;; Apply cognitive attention to significant changes
(cognitive-balance-attention savings-account 1000.00)
```

## Integration with OpenCog

This package enables cognitive agents to:
- Reason about financial patterns and trends
- Apply attention mechanisms to important financial events
- Learn from transaction history for predictive modeling
- Integrate financial state into overall cognitive planning

## Build Requirements

- Gnucash 5.12+
- OpenCog AtomSpace
- Guile 3.0
- Boost libraries
- SQLite/PostgreSQL for data storage

## Service Management

The package includes Shepherd service definitions for running Gnucash as a cognitive accounting daemon:

```bash
# Start cognitive accounting service
aphrodite-opencog start gnucash-daemon

# Query account status
guile -c "(use-modules (opencog gnucash)) (display (account-cognitive-status \"Savings\"))"
```
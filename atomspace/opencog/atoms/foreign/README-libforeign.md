# libforeign.so

**Source:** `atomspace/opencog/atoms/foreign/`  
**Role:** Foreign AST atoms — atoms that wrap external abstract syntax trees (S-expressions and other formats) as first-class AtomSpace objects.

## What it does

Allows foreign data structures — particularly Lisp/Scheme S-expressions and other textual AST representations — to be stored as atoms and parsed/manipulated within the AtomSpace.  Used by the Scheme binding layer and language-learning pipelines to round-trip syntax trees.

## Object files

| Object | Purpose |
|--------|---------|
| `ForeignAST.cc.o` | `ForeignAST` — abstract base for atoms wrapping external ASTs |
| `SexprAST.cc.o` | `SexprASTNode` — concrete atom that stores a raw S-expression string |

## Key public headers

- `opencog/atoms/foreign/ForeignAST.h`

Design Thoughts
---------------
This file consists of some rambling thoughts, concerning ...

Things of ponder/discuss:
* trustworthiness
* information importance
* attention of information source

Social media, via David Marx @digthatdata.bsky.social
 * popular != reputable
 * unpopular != wrong
 * attention grabbing != important

Bennnet:
 * Valence (attraction, repulsion)
 * "Weakness" vs "simplicity": Weak systems are those that allow more
   variety of output than strong systems. Weak systems outperform simple
   systems (by a lot) in training/fitting data.
 * Weak tools can be employed in a greater variety of tasks.

* Phylogeneitc compression
  Identifying common sequences and marking them as such.

Algorithms:
 * BLAST (Basic Local Alignment Search Tool) for genome database search.

Paper:
 * Efficient and robust search of microbial genomes via phylogenetic compression
   Karel Břinda, etal.
   Nature methods Published: 09 April 2025
   https://www.nature.com/articles/s41592-025-02625-2
   https://www.biorxiv.org/content/10.1101/2023.04.15.536996v3.full.pdf
   Abstract:
   ... Propose phylogenetic compression ... applied to modern diverse
   collections approaching millions of genomes, lossless phylogenetic
   compression improves the compression ratios of assemblies, de Bruijn
   graphs and k-mer indexes by one to two orders of magnitude.

Paper:
 * nvestigating the impact of trial retractions on the healthcare evidence ecosystem (VITALITY Study I): retrospective cohort study
  BMJ 2025; 389 doi: https://doi.org/10.1136/bmj-2024-082068 (Published 23 April 2025)
  https://www.bmj.com/content/389/bmj-2024-082068
Abstract
Objective To investigate the impact of retracted trials on the
production and use of healthcare evidence in the evidence ecosystem. ...

So this is a specific deep dive into how bad info pollutes the
evidence-based information ecosystem.

Manuscript Notes
----------------
Notes taken while reading:
Are Biological Systems More Intelligent Than Artificial Intelligence?
Michael Timothy Bennett
https://arxiv.org/abs/2405.02325

Section 3.1 of version 4 of the PDF appears to contain a key typo that
makes it difficult to understand. It says that a "vocabulary" is a
subset of the set of states. Version 3 correctly defines the
vocabulary as the a subset of the power set of states.

Recapping the definitions:
 * There is a set of "states" Phi.
 * A "program" is a subset of Phi.
 * A "vocabulary" is a set of programs. It is fixed externally, a
   priori, by the reader/external observer.
 * A "language" is the set of all(?) subsets of the vocabulary, such that
   each subset has a non-empty intersection. That is, each element of
   the language is a set of programs, such that there is at least one
   state is common to all programs.
 * The definition is not clear; he may be defining the "language" to
   be the union of all ultrafilters. Or he may be defining a language
   to be the union of just some filters.
 * A statement is an element of the language. The extension of a
   statement is the filter containing that statement.
 * The "weakness" of a statement is the cardinality of it's extension.
 * ... definitions are a bit garbled and riddled with typos that make
   comprehension difficult.

I dunno. This paper tries to formulate hierarchical systems in terms of
filters (as in topology) and boolean algebras thereon, using
non-standard terminology, but I just don't see where it goes. If I blur
my eyes, this all just collapses to pretty standard topology. The
problem is that general topology has many far deeper and far more
sophisticated statements of which above is a scraping of the surface.
Alas.


Other stuff
-----------
TBD. I am going to write the next blob in Lyx. See where that goes.

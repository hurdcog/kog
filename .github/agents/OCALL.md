---
name: OCALL
description: Workflow to Build and Test All OpenCog Components
---

# .github/workflows/ocall.yml

name: opencog all

on:
  push:
    branches: main
  pull_request:
    branches: main

jobs:
  build-and-test:
    name: Build and Test All Components
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential cmake libboost-all-dev libcpprest-dev librocksdb-dev liboctomap-dev guile-3.0-dev uuid-dev cython3 python3-nose valgrind doxygen
          
      - name: Install Cython
        run: python3 -m pip install --upgrade pip cython

      - name: build & install all
        run: |
          for repo in agents agi-bio asmoses atomese-simd atomspace atomspace-agents atomspace-bridge atomspace-cog atomspace-dht atomspace-explorer atomspace-ipfs atomspace-js atomspace-metta atomspace-pgres atomspace-restful atomspace-rocks atomspace-rpc atomspace-storage atomspace-typescript atomspace-websockets attention benchmark blender_api blender_api_msgs cheminformatics ci_artifacts cogprotolab cogserver cogutil destin dimensional-embedding distributional-value docker docs evidence external-tools generate ghost_bridge guile-dbi language-learning learn lg-atomese link-grammar linkgrammar-relex-web link-grammar-website logicmoo_cogserver loving-ai loving-ai-ghost matrix miner moses motor ocpkg opencog opencog-cycl opencog-debian opencog-neo4j opencog-nix opencog.org opencog_rpi opencog-to-minecraft pattern-index pau2motors perception pi_vision pln pln-brca-xp profile python-attic python-client python-destin relex rest-api-documentation robots_config rocca ros-behavior-scripting ros_opencog_robot_embodiment scripts semantic-vision sensory spacetime stochastic-language-generation test-datasets TinyCog tv-toolbox unify unity3d-opencog-game ure vision visualization; do echo "spawning $repo..."; mkdir -p $repo/build/; cd $repo/build; cmake ..; make; sudo make install; sudo ldconfig; cd ../..; sudo apt update; sudo apt install; done

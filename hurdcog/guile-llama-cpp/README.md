# guile_llama_cpp

GNU Guile binding for llama.cpp

Version 0.2, Copyright 2024 Li-Cheng (Andy) Tai, atai@atai.org

Guile_llama_cpp wraps around llama.cpp APIs so llama.cpp can be accessed from Guile scripts and programs, in a manner
similar to llama-cpp-ython allowing the use of llama.cpp in Python programs.
Currently simple Guile scripts are provided to allow simple "chat" with a LLM in gguf format.

## Project Information

guile_llama_cpp releases are available from http://download.savannah.nongnu.org/releases/guile-llama-cpp/
sources in git repo at https://git.savannah.nongnu.org/git/guile-llama-cpp.git
Bug tracker at https://savannah.nongnu.org/bugs/?group=guile-llama-cpp
Mailing lists can be accessed at https://savannah.nongnu.org/mail/?group=guile-llama-cpp
Currently, we have three mailing lists:
    bug-guile-llama-cpp@nongnu.org for bug reports and general development discussions
    guile-llama-cpp@nongnu.org for general  user discussions
    info-guile-llama-cpp@nongnu.org for release and other announcements

## setup and build

guile_llama_cpp is written in GNU Guile and C++ and requires

Swig 4.0 or later, GNU guile 3.0, and llama.cpp (obviously) with pkg-config support (at or after commit 017e21)

installed on your system.

From sources, guile_llama_cpp can be built via the usual GNU convention,

./configure --prefix=<install dir>
make
make install

If you are running GNU Guix on your system, you can get a shell with all needed dependencies set up with

guix shell -D -f guix.scm

and then use the usual

configure && make && make install

commands to build.

## run

### Basic LLM Chat

To use guile_llama_cpp to chat with a LLM (Large Language Model), you need to first download a LLM in gguf format.
See instructions on the web such as https://stackoverflow.com/questions/67595500/how-to-download-a-model-from-huggingface

As an example, using a "smaller" LLM "Phi-3-mini" from Microsoft; we would first download the model in gguf format via wget:

wget https://huggingface.co/microsoft/Phi-3-mini-4k-instruct-gguf/resolve/main/Phi-3-mini-4k-instruct-q4.gguf

then you can chat with it, in the build directory:

./pre-inst-env simple.scm  "What is llama.cpp?" Phi-3-mini-4k-instruct-q4.gguf

The general form to do a chat with a model is to invoke the script scripts/simple.scm

simple.scm prompt_text model_file_path

The script chat.scm provides more complete control over the chat parameters

chat.scm [-v] [-h]
 [-c context_length]
 [-n prediction_length]
 -m model_path
 -p prompt

### ECMA-262 JavaScript Integration

You can also use modern JavaScript features with LLM operations:

```bash
# Run ECMA-262 demo
./pre-inst-env scripts/ecma262-bridge.scm demo Phi-3-mini-4k-instruct-q4.gguf

# Execute JavaScript code with LLM
./pre-inst-env scripts/ecma262-bridge.scm exec Phi-3-mini-4k-instruct-q4.gguf "ECMA.llm.prompt('What is AI?')"

# Test the JavaScript example
node examples/ecma262-llm-example.js
```


in the build directory, pretend the command with

./pre-inst-env

as it sets up the needed paths and environment variables for proper guile invocation.

## features

* **Core LLM Operations**: Simple Guile scripts for LLM chat interaction with gguf format models
* **ECMA-262 JavaScript Integration**: Modern JavaScript language features for LLM interaction
* **Interactive Chat**: Command-line based conversation with LLMs
* **Configurable Parameters**: Full control over chat parameters (context length, prediction length, etc.)
* **GNU Guile Integration**: Seamless integration with Guile Scheme ecosystem

## ECMA-262 JavaScript Integration

guile_llama_cpp now includes comprehensive ECMA-262 JavaScript support, allowing you to use modern JavaScript features with LLM operations:

```javascript
// Modern JavaScript syntax with LLM
const result = await ECMA.llm.prompt(`Explain ${"machine learning"} in simple terms`);

// Array processing for batch operations
const topics = ["AI", "ML", "DL"];
const responses = topics.map(topic => ECMA.llm.prompt(`Define ${topic}`));

// Configuration with destructuring
const config = ECMA.config.createLLMConfig({ temperature: 0.8, maxTokens: 100 });
```

See [ECMA262_INTEGRATION.md](docs/ECMA262_INTEGRATION.md) for complete documentation.

## roadmap

* support for interactive, continuous chat
* support for expose the LLM as a web end point, using a web server built in Guile, so
  the LLM can be exposed via a web interface, to allow chatting with remote users
* support for embedding LLMs in Guile programs for scenarios like LLM driven software
  agents
* **Enhanced ECMA-262 Integration**: WebAssembly support, streaming operations, browser integration
* **SKZ Framework Integration**: Full integration with SKZ autonomous agents framework

* support for interactive, continuous chat
* support for expose the LLM as a web end point, using a web server built in Guile, so
  the LLM can be exposed via a web interface, to allow chatting with remote users
* support for embedding LLMs in Guile programs for scenarios like LLM driven software
  agents

## license

Copyright 2024 Li-Cheng (Andy) Tai
atai@atai.org

guile_llama_cpp is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

guile_llama_cpp is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with guile_llama_cpp. If not, see <https://www.gnu.org/licenses/>.

Hopefully this program is useful to you.

## changes
   version 0.2      20240629
      * introduction of chat.scm, with more control over char parameters
      * chat scripts now output long replies from LLM

   version 0.1.2    20240604
      * further fixes for license consistency

   Version 0.1.1    20240603
      * fixing packaging issues, license notices and typos

   Version 0.1      20240602
      * initial release

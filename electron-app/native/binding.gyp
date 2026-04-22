{
  "targets": [
    {
      "target_name": "cognitive-addon",
      "sources": [
        "cognitive-addon.cc",
        "atomspace-bridge.cc",
        "attention-bridge.cc",
        "inference-bridge.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "conditions": [
        ["OS=='win'", {
          "defines": [
            "_WIN32",
            "_USE_MATH_DEFINES",
            "NOMINMAX",
            "OCC_BUILTIN_ATOMSPACE"
          ],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1,
              "AdditionalOptions": [ "/std:c++17" ]
            }
          }
        }],
        ["OS=='linux'", {
          "defines": [ "OCC_BUILTIN_ATOMSPACE" ],
          "cflags_cc": [ "-std=c++17", "-fexceptions" ]
        }],
        ["OS=='mac'", {
          "defines": [ "OCC_BUILTIN_ATOMSPACE" ],
          "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            "CLANG_CXX_LIBRARY": "libc++",
            "MACOSX_DEPLOYMENT_TARGET": "10.15",
            "OTHER_CFLAGS": [ "-std=c++17" ]
          }
        }]
      ]
    }
  ]
}

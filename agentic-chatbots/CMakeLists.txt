#
# Agentic Chatbots Integration
# Part of the OpenCog Collection (OCC) cognitive architecture
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(agentic-chatbots
    VERSION 1.0.0
    DESCRIPTION "Agentic Chatbots Integration with AtomSpace"
    LANGUAGES CXX
)

# Version information
SET(SEMANTIC_VERSION 1.0.0)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find dependencies
find_package(Threads REQUIRED)

# Agentic Chatbots library source files
set(AGENTIC_CHATBOTS_SOURCES
    src/agent.cpp
    src/chatbot_manager.cpp
    src/knowledge_integration.cpp
    src/collaborative_reasoner.cpp
)

# Agentic Chatbots library headers
set(AGENTIC_CHATBOTS_HEADERS
    include/agentic_chatbots/agent.h
    include/agentic_chatbots/chatbot_manager.h
    include/agentic_chatbots/knowledge_integration.h
    include/agentic_chatbots/collaborative_reasoner.h
)

# Build Agentic Chatbots shared library
add_library(agentic_chatbots SHARED ${AGENTIC_CHATBOTS_SOURCES})

target_include_directories(agentic_chatbots
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(agentic_chatbots
    PUBLIC
        Threads::Threads
)

# Installation
install(TARGETS agentic_chatbots
    EXPORT agentic_chatbots-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/agentic_chatbots
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

MESSAGE(STATUS "Agentic Chatbots Integration configuration complete")

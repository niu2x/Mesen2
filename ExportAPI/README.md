# Mesen-CAPI - Simplified C Interface for Mesen Emulator [WIP]

## Overview

This repository provides a simplified C API wrapper for the Mesen emulator (https://github.com/SourMesen/Mesen). The purpose is to make it easier to integrate Mesen emulator functionality into various environments, particularly command-line applications and other projects that require a simple C interface.

## Key Differences from Original Mesen

| Feature               | Original Mesen | This Project |
|-----------------------|----------------|-------------|
| Build System          | Custom build scripts | CMake-based |
| Output Format         | Full GUI application | Library (static/shared) + headers |
| Target Use Cases      | Desktop GUI apps | CLI, custom frontends |


## Features

- **Modern CMake build system** for cross-platform compatibility
- **Clean C API** for easy integration with various languages and environments
- **Simplified headers** focusing on core emulator functionality
<!-- - **Reduced dependencies** compared to full GUI version -->


## Current Limitations

- Only basic emulation core functionality is exposed
- Limited controller API implementation

## Example
[Terminal NES Emulator Demo](/TerminalPlayer/main.cpp) - Terminal NES Emulator Demo

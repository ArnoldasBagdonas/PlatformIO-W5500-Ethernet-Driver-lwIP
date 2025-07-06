# Starting from Scratch: lwIP Wrapper Library

This guide demonstrates a clean and maintainable way to integrate the [lwIP (Lightweight IP)](https://savannah.nongnu.org/projects/lwip/) TCP/IP stack into a PlatformIO-based Arduino project using a **wrapper library approach**. This approach keeps the upstream lwIP source untouched while encapsulating all PlatformIO-specific configuration in a separate, version-controlled layer.

## 🌟 Features

- **Mixed Arduino and Non-Arduino Library Support**  
  Seamlessly combines high-level Arduino APIs (e.g., `SPI`, `digitalWrite`) with low-level C/C++ libraries like `lwIP`, offering both ease of use and full control.

- **Clean PlatformIO Integration**  
  Uses `library.json` to define build flags, source filters, and include paths tailored for PlatformIO.

- **Third-Party Source Isolation**  
  Keeps upstream code under `thirdparty/` and avoids patching or modifying vendor libraries directly.

## 📁 Project Structure

```
project-root/
├── lib/
│   └── lwip_wrapper/                <-- PlatformIO wrapper library
│       ├── port/
│       │   ├── src/                 <-- lwIP port sources
│       │   │   ├── ethif.c
│       │   │   ├── w5500.c
│       │   │   └── sys_arch.cpp
│       │   └── include/             <-- lwIP headers and config
│       │       ├── arch/            <-- Architecture-specific headers
│       │       │   ├── cc.h
│       │       │   └── sys_arch.h
│       │       ├── ethif.h
│       │       └── lwipopts.h       <-- lwIP configuration header
│       ├── README.md                <-- Starting from Scratch: lwIP Wrapper Library <<< YOU ARE HERE
│       └── library.json             <-- PlatformIO build instructions
├── thirdparty/
│   └── lwip/                        <-- lwIP source (Git submodule, read-only)
│       └── src/
├── src/
│   └── main.cpp                     <-- Application code
├── platformio.ini                   <-- PlatformIO project configuration
└── README.md                        <-- PlatformIO W5500 Ethernet Driver (lwIP)
```

## 💡 Concept: Clean Third-Party Library Integration

To maintain a clean separation between upstream sources and local configuration, this structure uses two key folders:

- `thirdparty/` — contains third-party source code (e.g., `lwIP`) as Git submodules.  
  ⚠️ **Do not modify** files in this folder. treat as read-only.

- `lib/` — Contains PlatformIO-compatible **wrapper libraries** that:
  - Define build instructions, source filtering, and include paths  (`library.json`).
  - Provide configuration headers like `lwipopts.h`.
  - Are fully version-controlled and safe to modify.

This design lets us:

- Easily update upstream libraries without conflicts.
- Keep our PlatformIO build process clean and maintainable.
- Avoid patching or forking third-party code unnecessarily.

## ⚙️ Setup Instructions

Follow these steps when **starting from scratch**.

### Step 1: Add lwIP as a Git Submodule (Initial Setup)

```bash
mkdir thirdparty
git submodule add https://github.com/lwip-tcpip/lwip.git thirdparty/lwip
git add .gitmodules thirdparty/lwip
git commit -m "Add lwIP as submodule"
```

### Step 2: Create PlatformIO Wrapper Library `lwip_wrapper`

- Create the following folder structure inside `lib/lwip_wrapper/`:

  ```
  lib/lwip_wrapper/
  └── port/
      ├── src/           # Copy or add our port-specific lwIP source files here (e.g. sys_arch.c)
      └── include/
          ├── arch/      # Port-specific headers (cc.h, perf.h, sys_arch.h)
          └── lwipopts.h # Our lwIP configuration header
  ```

- Create `lib/lwip_wrapper/library.json`:

  ```json
  {
    "name": "lwip_wrapper",
    "version": "1.0.0",
    "build": {
      "flags": [
        "-Iport/include",
        "-DLWIP_TIMEVAL_PRIVATE=0"
      ],
      "srcFilter": [
        "+<port/src/**>",
        "+<../../thirdparty/lwip/src/**>",
        "-<../../thirdparty/lwip/src/apps/http/makefsdata/**>"
      ],
      "includeDir": "../../thirdparty/lwip/src/include"
    }
  }
  ```

### Step 3: Update platformio.ini to use the wrapper library

- Modify `platformio.ini` to add build flags:

  ```ini
  ; PlatformIO Project Configuration File
  ;
  ;   Build options: build flags, source filter
  ;   Upload options: custom upload port, speed and extra flags
  ;   Library options: dependencies, extra library storages
  ;   Advanced options: extra scripting
  ;
  ; Please visit documentation for the other options and examples
  ; https://docs.platformio.org/page/projectconf.html

  [env:seeed_xiao]
  platform = atmelsam
  board = seeed_xiao
  framework = arduino

  ; Reference lwIP library (if it has a library manifest or PlatformIO can detect it)
  lib_deps =
      lwip_wrapper
  ```

## License

- This project is licensed under the [MIT License](../../LICENSE).
- lwIP is used under its original BSD-style license.  



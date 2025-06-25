# Ethernet Module lwIP

This project demonstrates how to cleanly integrate the [lwIP (Lightweight IP)](https://savannah.nongnu.org/projects/lwip/) TCP/IP stack into a PlatformIO-based Arduino project.  
It uses a **wrapper approach** to keep the upstream `lwIP` source code untouched while applying PlatformIO-specific build configuration separately.

---

## 📚 Concept: Clean Third-Party Library Integration

We separate the third-party code from our project logic using this structure:

- `thirdparty/` — Contains raw third-party libraries (e.g., `lwIP`) as Git submodules.  
  ⚠️ **Do not modify** files in this folder.
  
- `lib/` — Contains PlatformIO-compatible **wrapper libraries** that:
  - Define how to build and filter third-party sources.
  - Provide configuration headers (e.g., `lwipopts.h`).
  - Are fully under your version control.

This allows you to:
- Easily update upstream libraries via Git.
- Keep your PlatformIO build logic clean and maintainable.
- Avoid modifying or forking third-party code unnecessarily.

---

## 📦 Project Structure

```
project-root/
├── lib/
│   └── lwip/                   ← PlatformIO wrapper library
│       ├── library.json        ← Defines build instructions
│       └── include/
│           └── lwipopts.h      ← lwIP configuration
├── thirdparty/
│   └── lwip/                   ← lwIP source as Git submodule
│       └── src/
├── src/
│   └── main.cpp                ← Your application code
├── platformio.ini              ← PlatformIO project config
└── README.md
```

---

## 🧰 How to Set Up This Project

### Step 1: Add lwIP as a Git Submodule (Initial Setup)

If you are **starting from scratch** and want to add `lwIP` to your project as a submodule, do this:

```bash
mkdir thirdparty
git submodule add https://github.com/lwip-tcpip/lwip.git thirdparty/lwip
git add .gitmodules thirdparty/lwip
git commit -m "Add lwIP as submodule"
```

Then create the PlatformIO wrapper for `lwIP`:

- Create `lib/lwip/library.json`:

```json
{
  "name": "lwip",
  "version": "2.2.0",
  "build": {
    "srcFilter": [
      "+<../../thirdparty/lwip/src/core/>",
      "+<../../thirdparty/lwip/src/api/>",
      "+<../../thirdparty/lwip/src/include/>",
      "-<*>"
    ],
    "includeDir": "../../thirdparty/lwip/src/include"
  }
}
```

- Add `lib/lwip/include/lwipopts.h` with your configuration:

```c
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#define NO_SYS 1
#define LWIP_RAW 1
#define LWIP_NETCONN 0
#define LWIP_SOCKET 0

#endif
```

- Modify `platformio.ini` to add build flags:

```ini
build_flags =
    -Ilib/lwip/include
    -Ithirdparty/lwip/src/include
    -DLWIP_TIMEVAL_PRIVATE=0
```

---

### Step 2: Clone the Whole Project with Submodules (For New Developers / Later Use)

If the project is already set up and committed with submodules, clone it with:

```bash
git clone --recurse-submodules https://gitlab.com/seeed-studio-xiao-samd21/platformio/ethernet-module-lwip.git
cd ethernet-lwip-project
```

If you already cloned **without** `--recurse-submodules`, initialize and update them with:

```bash
git submodule update --init --recursive
```

---

## 🚀 Build and Upload

Build and upload your project using PlatformIO CLI or VS Code PlatformIO extension:

```bash
pio run
pio run -t upload
```

---

## 🔁 Updating lwIP

To update the lwIP submodule to the latest upstream version:

```bash
cd thirdparty/lwip
git pull origin master
cd ../..
git add thirdparty/lwip
git commit -m "Update lwIP to latest version"
```

---

## ✅ Summary

| Component        | Description                                      |
|------------------|--------------------------------------------------|
| `thirdparty/lwip` | Original lwIP source as Git submodule (read-only) |
| `lib/lwip/`      | Your PlatformIO build wrapper (fully customizable) |
| `lwipopts.h`     | Your lwIP configuration                          |
| `library.json`   | Controls which lwIP files are included in build  |

---

## 📚 Resources

- [lwIP Project Page](https://savannah.nongnu.org/projects/lwip/)
- [PlatformIO Documentation](https://docs.platformio.org/)

---

## 📄 License

lwIP is used under its original BSD-style license.  
Your project can use any license compatible with it.

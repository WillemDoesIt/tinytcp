# TinyTCP
Version: 0.4.0-beta

> [!WARNING]
> This project is in early stages. Functionality is available, but use at your own risk.  
> A proper release branch will follow once fully tested.
> Originally intended for a class this has become big enough that I will continue to work on it.

Simple TCP listener/server program (and library). The program is used in command line interface and is compilable on all major Operating Systems.

---

## Windows Run (Installer or MSYS2 + MinGW64)

### Option 1 — Installer (Recommended)

1. Run the NSIS installer (`tinytcp-installer.msi`) included in the [latest release](https://github.com/WillemDoesIt/tinytcp/releases).
2. After installation, **open a new terminal** and run:

```cmd
tinytcp server
tinytcp client <host>
```

---

### Option 2 — Manual build (Dev / Testing)

There is a `run.bat` file that you can execute to automatically compile and run the generated executable if you have the C++ compiler installed. 

I have found C++ and Windows doesn't often work nicely together, if this doesn't work here is how I got it running on Windows.

1. **Install MSYS2 via winget:**

```cmd
winget install -e --id MSYS2.MSYS2
```

2. Open the **MinGW64 shell** (`C:\msys64\mingw64.exe`).
3. Update and install the compiler:

```bash
pacman -Syu    # then close and reopen shell
pacman -S mingw-w64-x86_64-gcc
```
> [!NOTE]
> Despite pacman being the offical package manager for Arch Linux, this acutally runs in a Windows MSYS2 Shell

4. Navigate to the project directory and run:

```bash
cd /c/Users/<you>/path/to/tinytcp
./run.bat --help
```
> [!NOTE]
> Replace <you>/path/to/tinytcp
> `pwd` shows current path, and `whoami` shows current user

5. Optional debugging with Netcat:

```cmd
winget install Insecure.Nmap
"C:\Program Files (x86)\Nmap\nc.exe" localhost 50001
```

6. Optional compiling your own installer:

```cmd
winget install WiXToolset.WiXCLI 
wix build ./create_installer.wxs -ext WixToolset.UI.wixext -o ./build/tinytcp_installer.msi
```

---

## macOS / WSL / Linux

### Option 1 — Nix (Recommended)

1. Install Nix:

```bash
# Linux and WSL install
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install) --no-daemon

# macOS install
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install)
```
> [!NOTE]
> Not necessary if already on NixOS

2. Enter a dev shell for fast iterative builds:

```bash
nix develop             # once per session
chmod +x run.sh         # one-time
./run.sh --help
```

3. Optional Build and install to your profile:

```bash
nix build .#tinytcp
nix profile install .#tinytcp

# Now you can run anywhere:
tinytcp --help

# To uninstall:
nix profile remove tinytcp
```

---

### Option 2 — Native Build (Non-Nix)

This has not yet been requested to be hosted on any package managers, nor have an Appimage, so you instead must compile it yourself.

1. Make sure you have a C++ compiler:

**macOS:**

```bash
xcode-select --install
```

**Debian/Ubuntu:**

```bash
sudo apt update
sudo apt install g++ make
```

**Fedora:**

```bash
sudo dnf install gcc-c++ make
```

**Arch:**

```bash
sudo pacman -S gcc make
```

2. Build and run:

```bash
chmod +x run.sh          # one-time
./run.sh --help
```
> [!NOTE]
> The compiled executable is placed in `build/` so `./build/tinytcp` is faster if pre-compiled

---

## Using the Library

The library is a header-only file stored wtih `./headers/` titled `tcp.hpp`. The file is so small that it is not worth seperating into a `.h` and `.cpp` to crate some `.so` binary of the library because it adds nearly no compile time. The `./examples/` as well as the documentation within the file should make it clear how to use it.

to compile the main.cpp in examples, run:
```
# macOS, Linux
c++ -Iheaders $(find examples -name '*.cpp') -std=c++17 -O2 -g -o build/example
./build/example

# Windows
c++ -Iheaders examples\\main.cpp -std=c++17 -O2 -g -static -static-libgcc -static-libstdc++ -o build\\example.exe -lws2_32
build\\example.exe
```

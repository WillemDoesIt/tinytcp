# TinyTCP
Version: 0.0.1-alpha

> [!WARNING]
> This project is in early stages. Functionality is available, but use at your own risk.  
> A proper release branch will follow once fully tested.
> Originally intended for a class this has become big enough that I will continue to work on it.

Simple TCP listener/server program. Supports sending a file between client and server.

---

## Windows Run (Installer or MSYS2 + MinGW64)

### Option 1 — Installer (Recommended)

1. Run the NSIS installer (`tinytcp-setup.exe`) included in the release.
2. After installation, **open a new terminal** and run:

```cmd
tinytcp server
tinytcp client <host>
```

This works **without admin rights** and no additional dependencies.

---

### Option 2 — MSYS2 + MinGW64 (Dev / Testing)

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
> Replace path/to/tinytcp
> `pwd` shows current path, and `whoami` shows current user

5. Optional debugging with Netcat:

```cmd
winget install Insecure.Nmap
"C:\Program Files (x86)\Nmap\nc.exe" localhost 49153
```

> `run.bat` compiles all `.cpp` files in `source/`, includes `headers/`, and produces a self-contained `tinytcp.exe`.

6. Optional compiling your own installer:

```cmd
winget install -e --id NSIS.NSIS
"C:\Program Files (x86)\NSIS\makensis.exe" path/to/tinytcp/create_install.nsis
```
> [!NOTE]
> Replace path/to/tinytcp

---

## macOS / WSL / Linux

### Option 1 — Native Build (Non-Nix)

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

The compiled executable is placed in `build/`.

---

### Option 2 — Nix (Optional / Recommended for Iterative Dev)

1. Install Nix:

```bash
sh <(curl -L https://nixos.org/nix/install) --no-daemon
```
> [!NOTE]
> Not necessary if already on NixOS

2. Enter a dev shell for fast iterative builds:

```bash
nix develop             # once per session
chmod +x run.sh         # one-time
./run.sh --help
```

3. Build and install to your profile:

```bash
nix build .#tinytcp
nix profile install .#tinytcp

# Now you can run anywhere:
tinytcp --help

# To uninstall:
nix profile remove tinytcp
```

---

## Running Notes

* Compiled executables live in `build/`.
* `run.sh` / `run.bat` handles compilation automatically.
* On Windows, the installer produces a **static exe**, so no MSYS2 is required.
* Netcat (`nc`) can be used to test TCP connections manually.

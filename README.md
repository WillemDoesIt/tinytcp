# TinyTCP
Version: 0.2.1

> [!WARNING]
> This project is in early stages. Functionality is available, but use at your own risk.  
> A proper release branch will follow once fully tested.

TinyTCP is a lightweight, cross-platform TCP client/server utility.  
It allows listening on a TCP port and sending/receiving messages on that port.  

---

# User Install
[Click here](https://github.com/WillemDoesIt/tinytcp/releases/latest) for the download page of the latest pre-release

---

## 🧪 Example Usage

```bash
# Start server
ttcp server

# From another terminal or host
ttcp client 127.0.0.1 "hello world"
```

---

## Manual build and Install with Nix
For developers who want to compile the code form it's source and optionally install it systemwide. Nix can be used on any Unix system, WSL2, macOS, and Linux.

```bash
# Clone repository
git clone git@github.com:WillemDoesIt/tinytcp.git
cd tinytcp

# Build the package
nix build .#ttcp

# Run directly
./result/bin/ttcp --help

# Install and uninstall commands
nix profile install .#ttcp
nix profile remove ttcp 
````

### Compile and run as developer with Nix

```bash
nix develop
chmod +x deploy.sh

./deploy.sh --help
```

### Linux Developer test Windows .exe (Optional)

```bash
nix-shell -p pkgsCross.mingwW64.buildPackages.gcc wine
x86_64-w64-mingw32-g++ -static -o bin/ttcp.exe src/*.cpp -lws2_32

wine bin/ttcp.exe --help

# Make installer
makensis make_win_install.nsi
```

---

## Manual Build (Non-Nix)

These instructions allow developers to compile and run TinyTCP without Nix, using standard OS tooling.

### Windows (clang via winget)

```powershell
# Install clang
winget install llvm

# Navigate to project
cd path\to\tinytcp

# Compile
clang++ src\main.cpp -o bin\ttcp.exe -std=c++20 -lws2_32

# Run
.\bin\ttcp.exe --help
```

### macOS (clang from Xcode CLT)

```bash
# Install Xcode Command Line Tools if missing
xcode-select --install

# Navigate to project
cd /path/to/tinytcp

# Compile
clang++ src/main.cpp -o bin/ttcp -std=c++20

# Run
./bin/ttcp --help
```

### Linux

#### Debian / Ubuntu

```bash
sudo apt update
sudo apt install -y g++

cd /path/to/tinytcp
g++ src/main.cpp -o bin/ttcp -std=c++20
./bin/ttcp --help
```

#### Fedora

```bash
sudo dnf install -y gcc-c++

cd /path/to/tinytcp
g++ src/main.cpp -o bin/ttcp -std=c++20
./bin/ttcp --help
```

#### Arch / Manjaro

```bash
sudo pacman -S --needed base-devel

cd /path/to/tinytcp
g++ src/main.cpp -o bin/ttcp -std=c++20
./bin/ttcp --help
```

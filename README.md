# TinyTCP
Version: 0.2.1

> [!WARNING]
> This project is in early stages. Functionality is available, but use at your own risk.  
> A proper release branch will follow once fully tested.

TinyTCP is a lightweight, cross-platform TCP client/server utility.  
It allows listening on a TCP port and sending/receiving messages on that port.  

# User Install
[Click here](https://github.com/WillemDoesIt/tinytcp/releases/latest) for the download page of the latest pre-release

## 🧪 Example Usage

```bash
# Start server
ttcp server

# From another terminal or host
ttcp client 127.0.0.1 "hello world"
```

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

# Install and uninstall commands (makes `ttcp` a command)
nix profile install .#ttcp
nix profile remove ttcp 
````

### Easy Compile and Run with Nix

```bash
chmod +x deploy.sh

# Enter shell
nix develop

./deploy.sh --help

# Exit shell
exit
```

## Manual Build (Non-Nix)

```bash
# Clone in preferred dierectory
git clone git@github.com:WillemDoesIt/tinytcp.git
cd tinytcp

# Compile + run
./deploy.sh
```
> [!NOTE]
> Dependency is a C++ compiler (`clang++` or `g++`).
> Windows: install with `winget install llvm`, 
> MacOS: `xcode-select --install`


## Make Windows installer
```bash
# Create executable (works on linux and windows)
x86_64-w64-mingw32-g++ -static -o bin/ttcp.exe src/*.cpp -lws2_32

# Make installer
makensis make_win_install.nsi
```
> [!NOTE]
> Nix: `nix-shell -p pkgsCross.mingwW64.buildPackages.gcc nsis`

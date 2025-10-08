# TinyTCP
Version: 0.1.0

> [!WARNING]
> This project is in early stages. Functionality is available, but use at your own risk.  
> A proper release branch will follow once fully tested.

TinyTCP is a lightweight, cross-platform TCP client/server utility.  
It allows listening on a TCP port and sending/receiving messages on that port.  

---

## Features

- Minimal TCP server and client
- Cross-platform (Linux/macOS/Windows via Nix)
- Reproducible builds via Nix flakes
- Executable: `ttcp`

---

## Installation & Usage (Unix / Linux / macOS)

### Option 1: Using Nix Flake

```bash
git clone git@github.com:WillemDoesIt/tinytcp.git
cd tinytcp

# Build the package
nix build .#ttcp

# Run directly
./result/bin/ttcp server
./result/bin/ttcp client 127.0.0.1
````

To install into your user profile (global executable):

```bash
nix profile install .#ttcp
ttcp server       # run from anywhere
ttcp client 127.0.0.1
nix profile remove ttcp  # uninstall cleanly
```

### Option 2: Using Deploy Script

```bash
nix develop
chmod +x deploy.sh
./deploy.sh server
./deploy.sh client 127.0.0.1
```

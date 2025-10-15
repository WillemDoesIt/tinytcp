# TinyTCP
Version: 0.3.1-alpha

> [!WARNING]
> This project is in early stages. Functionality is available, but use at your own risk.  
> A proper release branch will follow once fully tested.
> Originally intended for a class this has become big enough that I will continue to work on it.

TinyTCP is a lightweight, cross-platform TCP client/server utility.  
It allows listening on a TCP port and sending/receiving messages on that port.  

# User Install
This orginally instended to run on windows and linux is not (to my knowledge) compile-able on windows. The easiest install would be using NixOS or installing the nix package on a Linux distrobution with
```bash
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install) --no-daemon
```

Then download the repo and install manually.
```bash
# Clone repository at preferred directory
git clone git@github.com:WillemDoesIt/tinytcp.git
cd tinytcp


# Option1: Build the package with nix
nix build .#ttcp
# Run directly
./result/bin/ttcp --help

# Optionally Install and uninstall commands (makes `ttcp` a command)
nix profile install .#ttcp
nix profile remove ttcp  # uninstall

# Option2: Open a shell that auto installs the dependencies
nix develop
# Then run the program with
./build.sh
./bin/ttcp --help


## Example Usage (if you did Option2 instead write `./bin/ttcp` in place of `ttcp`)

# Start server, listening on default port
ttcp server
ttcp server --port 9001 # custom port 9001

# From another terminal or host
ttcp client 127.0.0.1 # send to self
ttcp client 10.10.10.10 --message "Hello Server!" --port 9001 # send "Hello Server!" to 10.10.10.10:9001 
```
> [!NOTE]
> If it stalls not seeming to send you may have the port you are trying to use blocked by default firewall settings. search online for the `networking.firewall.allowedTCPPorts` setting on nixos and after adjusting the `/etc/nixos/configuration.nix` file run `sudo nixos-rebuild switch` to have the setting go into effect a reboot may be necessary.


## Manual Build (Non-Nix)

```bash
# Clone in preferred dierectory
git clone git@github.com:WillemDoesIt/tinytcp.git
cd tinytcp

# Compile + run
./deploy.sh
./bin/ttcp --help
```
> [!NOTE]
> Dependency is a C++ compiler (`clang++` or `g++`).



## Defaults

By default the message will be "Hello from client" and the port number will be 50001, these can be found and adjusted in `./config/ttcp` which will be created upon the first time you run the binary.

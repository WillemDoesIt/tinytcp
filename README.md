# TinyTCP

Simple TCP listener/server program. Supports sending a file between client and server.

---

## Windows Run (MSYS2 + MinGW64)

1. **Install MSYS2 via winget:**
```cmd
winget install -e --id MSYS2.MSYS2
````

2. **Run the MinGW64 shell:**
   Open `C:\msys64\mingw64.exe`.

3. **Update and install C++ compiler:**

```bash
pacman -Syu    # then close and re-open the mingw64 shell
pacman -S mingw-w64-x86_64-gcc
```

4. **Navigate to the project directory** and run the program:

```bash
cd /c/Users/<you>/path/to/tinytcp
./run.bat --help
```

5. **Optional debugging with Netcat:**

```cmd
winget install Insecure.Nmap
"C:\Program Files (x86)\Nmap\nc.exe" localhost 49153
```

---

## macOS / WSL / Linux

### Option 1 — Non-Nix / Native Build

1. Ensure you have a C++ compiler installed:

**macOS:**

```bash
xcode-select --install
```

**Debian/Ubuntu:**

```bash
sudo apt update
sudo apt install g++ make
```

**Fedora**

```bash
sudo dnf install gcc-c++ make       # Fedora
```
**Arch:**

```bash
sudo pacman -S gcc make             # Arch
```

2. Run the program:

```bash
chmod +x run.sh                     # only need to run once
./run.sh --help
```

The `build/` folder will contain the compiled executable.

---

### Option 2 — Using Nix (optional)

1. Install Nix if you don't have it:

```bash
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install) --no-daemon
```

2. Enter a dev shell for faster iterative builds:

```bash
nix develop                         # only need to run once per session
chmod +x run.sh                     # only need to run once

./run.sh --help
```

3. To build and install the package to your profile:

```bash
nix build .#tinytcp
nix profile install .#tinytcp

# Now you can run it like this
tinytcp --help

# To uninstall:
nix profile remove tinytcp
```

> Note: `nix develop` is faster for iterative development because it lets you compile and run without rebuilding a full Nix package.

---

## Running Notes

* On all platforms, compiled executables live in `build/`.
* `run.sh` / `run.bat` handles compilation automatically.
* Netcat (`nc`) can be used to test TCP connections manually.

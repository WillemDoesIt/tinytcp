{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    miniCompileCommands = {
      url = "github:danielbarter/mini_compile_commands/v0.6";
      flake = false;
    };
    koturNixPkgs = {
      url = "github:nkoturovic/kotur-nixpkgs/v0.8.0";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, miniCompileCommands, koturNixPkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        mcc = pkgs.callPackage miniCompileCommands {};
        mcc-env = mcc.wrap pkgs.stdenv;
        mcc-hook = mcc.hook;

        package = mcc-env.mkDerivation (self: {
          pname = "ttcp";
          version = "0.3.0-alpha";
          src = builtins.path {
            path = ./.;
            filter = path: type: !(pkgs.lib.hasPrefix "." (baseNameOf path));
          };
          nativeBuildInputs = with pkgs; [ mcc-hook ncurses cmake gnumake ];
          buildInputs = with pkgs; [ fmt tomlplusplus catch2 ];
          cmakeFlags = [ "--no-warn-unused-cli" ];
          passthru = { inherit pkgs shell; };
        });

        shell = (pkgs.mkShell.override { stdenv = mcc-env; }) {
          inputsFrom = [ package ];
          shellHook = ''
            echo -e "\033[0;32m[  OK  ]\033[0m Entered dev shell."
          '';
        };
      in {
        packages.ttcp = package;
        devShells.default = shell;
        formatter = pkgs.alejandra;
      });
}


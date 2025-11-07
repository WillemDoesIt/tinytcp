{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";

  outputs = { self, nixpkgs }:

  let
    systems = {
      x86_64-linux = nixpkgs.legacyPackages.x86_64-linux;
      aarch64-linux = nixpkgs.legacyPackages.aarch64-linux;
      x86_64-darwin = nixpkgs.legacyPackages.x86_64-darwin;
    };

    makePackage = pkgs: {
      tinytcp = pkgs.stdenv.mkDerivation {
        pname = "tinytcp";
        version = "v0.4.0-beta";
        src = ./.;
        buildInputs = [ pkgs.gcc ];
        buildPhase = ''
          mkdir -p $out/bin
          $CXX -Iheaders $(find source -name '*.cpp') -std=c++17 -O2 -g -o $out/bin/tinytcp         
        '';
      };
      devShell = pkgs.mkShell {
        buildInputs = with pkgs; [ gcc lazygit lldb];
      };
    };
  in
  {
    packages = builtins.mapAttrs (arch: pkgs: makePackage pkgs) systems;
    defaultPackage = builtins.mapAttrs (arch: pkgs: self.packages.${arch}.tinytcp) systems;
    devShells = builtins.mapAttrs (arch: pkgs: { dev = self.packages.${arch}.devShell; }) systems;
  };
}


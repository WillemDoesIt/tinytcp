{
  description = "Tiny TCP listener";

  outputs = { self, nixpkgs }: 
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in {
    packages.${system}.tinytcp = pkgs.stdenv.mkDerivation {
      pname = "tinytcp";
      version = "1.0";

      src = ./.;

      buildInputs = [ pkgs.gcc ];

      buildPhase = ''
        mkdir -p $out/bin
        $CXX main.cpp -o $out/bin/tinytcp
      '';
    };

    defaultPackage.${system} = self.packages.${system}.tinytcp;
  };
}

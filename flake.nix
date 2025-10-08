{
  inputs = { nixpkgs.url = "github:nixos/nixpkgs"; };
  outputs = { self, nixpkgs }:
  let 
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    devShell.${system} = pkgs.mkShell { 
      nativeBuildInputs = with pkgs; [ 
        gcc
      ];

      shellHook = ''
        echo -e "\033[0;32m[  OK  ]\033[0m Entered dev shell."
      '';
    };
  };
}

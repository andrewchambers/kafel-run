let
  pkgs = import <nixpkgs> {};
  libkafel = pkgs.callPackage ./kafel.nix {};
in
  pkgs.stdenv.mkDerivation {
    name = "kafel-run";
    buildInputs = [ 
      libkafel
      pkgs.pkgconfig
      pkgs.clang-tools
    ];
  }

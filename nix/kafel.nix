{fetchgit, stdenv, flex ,bison}:
stdenv.mkDerivation {
  name = "kafel";
  src = import ./kafel-src.nix { inherit fetchgit; };
  buildInputs = [ flex bison ];
  checkPhase = ''
    make test
  '';
  installPhase = ''
    mkdir -p $out/lib/pkgconfig
    cp libkafel.a $out/lib
    cp -r ./include $out/
    cat <<EOF > $out/lib/pkgconfig/kafel.pc
      Name: kafel
      Version: 0.0.1
      Description: A language and library for specifying syscall filtering policies.
      Cflags: -I$out/include
      Libs: -L $out/lib -l kafel
    EOF
  '';
}

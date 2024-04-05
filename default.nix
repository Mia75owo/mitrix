let
  nixpkgs = fetchTarball "https://nixos.org/channels/nixos-unstable/nixexprs.tar.xz";
in
  with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "mitrix";
  src = ./.;

  nativeBuildInputs = [
    pkgs.nasm
  ];

  buildInputs = [
    pkgs.qemu
    pkgs.bear
    pkgs.glibc_multi
  ];

  buildPhase = ''
    make build/OS.flp
  '';

  installPhase = ''
    cp build/OS.flp $out
  '';

  PROVIDED_CC="${pkgs.pkgsCross.i686-embedded.buildPackages.gcc}/bin/i686-elf-gcc";
  PROVIDED_AS="${pkgs.nasm}/bin/nasm";
}

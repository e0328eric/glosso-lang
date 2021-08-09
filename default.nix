with import <nixpkgs> {}; rec {
     pinpogEnv = stdenv.mkDerivation {
        name = "glosso-lang";
        buildInputs = [ gcc clang gdb cmake ];
    };
}

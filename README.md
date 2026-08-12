# si
A stack based functional language similar to C

This repository contains boilerplate for a reverse polish notation script runtime
with a recursive-descent parser and Unicode grapheme-aware tokenization.

## Build

```sh
make
```

## Test

```sh
make test
```

## Nix + direnv

```sh
direnv allow
```

## Dev machine setup

1. Install Nix:
   - https://nixos.org/download/
2. Install direnv:
   - https://direnv.net/docs/installation.html
3. Enable direnv in your shell:
   - https://direnv.net/docs/hook.html
4. Activate this repository environment:

```sh
cd /path/to/si
direnv allow
```

After activation, the flake-based development dependencies are loaded automatically.

## Dependencies

- Development dependencies are provided through `flake.nix`.
- Niche dependencies should be vendored under `vendor/` with explicit licenses.
- Project license is MIT (`LICENSE`).

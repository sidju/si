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

## Dependencies

- Development dependencies are provided through `flake.nix`.
- Niche dependencies should be vendored under `vendor/` with explicit licenses.
- Project license is MIT (`LICENSE`).

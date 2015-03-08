# ljar
A wrapper for `jar` which behaves nicer in the context of `Makefile`s.

Use instead of `jar` in a `Makefile`.

## Features
- Creates a lock file in order to prevent race conditions on the `.jar`. file.
- Replaces `c` with `u` in case the `.jar` file already exists.
- Therefore can be used with GNU `make` *archive rules*.

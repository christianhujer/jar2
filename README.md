# ljar
A wrapper for `jar` which behaves nicer in the context of `Makefile`s.

Use instead of `jar` in a `Makefile`.

## Features
- Creates a lock file in order to prevent race conditions on the `.jar`. file.
- Replaces `c` with `u` in case the `.jar` file already exists.
- Replaces `u` with `c` in case the `.jar` file doesn't exist yet.
- Therefore can be used with GNU `make` *archive rules*.

## Disclaimer
GNU make currently only supports the POSIX library archive format.
Zip files as archives are not supported by GNU make.
Therefore, the behavior of archive member rules in Makefiles will not entirely be as intended.
The archive will partially be remade even if it's up-to-date.

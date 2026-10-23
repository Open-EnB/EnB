# Earth & Beyond client: C++ reconstruction

A rebuild of Westwood's Earth & Beyond client (`client.exe`, built 2004-03-16) in its original
language, with the original class names and Westwood's coding conventions.

Earth & Beyond shares Westwood's WWVegas libraries (WWLib, WW3D2, WWMath, WWAudio, ...) with
Command & Conquer Renegade and Generals/Zero Hour, whose source EA has published. Where the client
contains that code, we import it; the rest (E&B's own code) is reverse engineered.

## Layout

The repository root stands for the original build tree, `e:\build\beta\G\`
([docs/source-tree.md](docs/source-tree.md)):

```
app/        E&B's own code (G\app: client, common, shared, packets, kernel, main, ...)
libs/       G\libs (wwonline)
docs/       documentation: start at docs/README.md
tools/re/   reverse-engineering tools (Python; not part of the original tree)
```

Imported Westwood libraries will sit beside `app/` (`wwlib/`, `ww3d2/`, ...), as WWLib did in the
original tree.

## Documentation

[docs/README.md](docs/README.md) is the index. Begin with [docs/progress.md](docs/progress.md)
for where things stand and what's undecided.

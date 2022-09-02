# Ocean
A minimalistic implementation of the UNIX Shell Command Language

*Shells can be found in the ocean* (as simple as that)

For now, only the grammar of the Shell Command Language is implemented (Lexing & Parsing). It was really fun implementing it, but I don't really intend to go any further (actual execution of commands).

# Installation

You'll need a working cmake install (and a C compiler that works with cmake).

```
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build . --target Ocean
```

# Usage

To get some help, try `./Ocean -h`.

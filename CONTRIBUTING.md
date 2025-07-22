# CONTRIBUTING

## Speed. Simple. Stable.
The engine adheres to three core principles: Speed, Stability, Simplicity.

- Speed: The C language is fast.
The engine does not take responsibility for converting .obj or .bmp files;
it directly loads ready-made bytes.
- Stability: The standard C library cannot terminate the program due to an
error on its own. Since the engine doesn't convert .obj and .bmp files itself,
it remains stable because this conversion has already been performed by other
programs that have caught all errors and provided a simple file.
- Simplicity: C is a straightforward language.
Reading the book “The C Programming Language” 2nd edition will be sufficient
to start using it. By not taking responsibility for loading .obj files,
the engine's code becomes smaller and faster.

## Rules
There are only a few basic rules regarding code writing:

### No neural networks in code.
Neural networks can be used for clarification but not for generating code.
This project isn't aimed at money, so there’s no point in using neural
networks here.

### Only the C language is used.
C is fast, simple, and stable.
It is ideal for creating engines.
Instead of scripts like Python, small converter utilities such as obj2kmdl and
bmp2ktf (also written in C) are utilized.
It's better if any errors occur during the transformation phase rather than
when loading these files into the engine.

### Minimal external libraries.
External libraries should only be added when necessary.
If you can implement functionality from a simple library yourself, avoid adding
extra dependencies.

### No system-dependent code.
Although C is inherently cross-platform, headers like windows.h and linux/\*.h
introduce system dependency.
Use #ifdef constructs where the code differs between systems.

### Dynamic memory allocation occurs exactly once with required size.
This eliminates the need for custom memory management solutions.
Additionally, it enhances stability since fragmentation issues won't arise.
Memory is freed upon exiting the engine.

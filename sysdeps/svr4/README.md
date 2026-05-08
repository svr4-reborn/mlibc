SVR4 sysdeps scaffold

This tree is intentionally modeled after the Linux sysdeps layout, but only advertises the POSIX option set for now.

Notes:
- The i386 syscall ABI uses the SVR4 call gate (`lcall $0x7, $0`) rather than the Linux `int $0x80` ABI.
- The `abi-bits` headers in this scaffold are seeded from an existing mlibc port so the tree has the expected public surface while the real SVR4 ABI gets audited.
- Threading, cancellation, and signal return paths are placeholder scaffolding and still need a correctness pass against the kernel.
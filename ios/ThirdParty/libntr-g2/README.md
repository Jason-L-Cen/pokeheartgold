# libntr 2D rasterizer

Adapted from [cybervisi0n/libntr](https://github.com/cybervisi0n/libntr), commit
`1555d911ac80c0190ac1c321f99f1323de8b3c09`,
`libraries/sim/src/sim_g2.cpp`. The original MIT license is included in `LICENSE`.

This source is compiled as native C++, with no SDL, OpenGL, DS CPU interpreter or JIT.
Its output is individual 256×192 layers and up to 512-pixel OBJ scanline samples; `Original/Native2D.cpp`
composes those samples into a caller-owned RGBA frame.

Local changes remove OpenGL upload and dual-display placement, use the project's native
registers/memory snapshots, defer window clipping and mosaic to the compositor, fix
extended BG palette slot selection, direct-color bitmap sampling, signed affine
reference coordinates, and the ignored tile-number bit for 2D 256-color OBJ mapping.
Battle views sample text backdrops at their original tile pitch and offset whole OBJ cells
into a wider viewport, preserving their size, affine transforms and relative positions.
OBJ mosaic samples the global vertical grid before normal/affine sprite lookup and
retains transparent priority metadata for the compositor's horizontal pixel latch.
The compositor's latch rules follow the hardware notes in
[melonDS GPU2D](https://github.com/melonDS-emu/melonDS/blob/master/src/GPU2D.cpp)
and [its software renderer](https://github.com/melonDS-emu/melonDS/blob/master/src/GPU2D_Soft.cpp).
Per-scanline register changes still require further work; unsupported
rendering modes must fail explicitly rather than emit a purported complete frame.

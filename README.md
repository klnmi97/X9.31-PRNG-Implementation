# X9.31-PRNG-Implementation
Implementation of pseudorandom number generator of X9.31 standard based on AES (https://github.com/BrianGladman/AES).

## How to use
You can build the project with the help of VS 2019 (tools v142 are needed).
Open PRNG.sln file. In the PRNG.cpp you can set your key, seed and size of output file for the PRNG
by changing key, seed and bits_to_generate local variables inside the main() function. Then you can
either run or build solution inside VS.

This program uses dynamically linked AES library by Brian Gladman.
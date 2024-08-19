emcc -pedantic -Wall -Wextra -std=c2x -fPIC ../../src/adf.c ../../src/crc.c ../../src/lookup_table.c -o adflib.wasm --no-entry -sEXPORTED_FUNCTIONS=_get_version_bytes  

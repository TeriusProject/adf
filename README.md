# Agriculture Data Format (ADF)

This repository contains the implementation of the ADF file specification, a portable and extensible file format for the storage of data relative to crops and farming processes. The detailed specifications of the format will be available soon in the `doc` dir. 

### Implementation details

For some implementative details concerning datatypes and functions defined in the ADF library, check the  [the header](https://github.com/aestriplex/adf/blob/main/src/adf.h) itself.

## Build and install

To build the library it is sufficient to run the following commands. By default, the `all` rule will compile and execute all the unit tests too.

After cloning the repository, run
```bash
cd adf
make init # generates the file sample.adf
make      # compiles the library and the tests
```
Or, you can execute separately the two rules
```bash
cd adf
make init  # generates the file sample.adf
make adf   # just compiles the library libadf.a
make tests # just compiles and execute the tests
```
> **_NOTE:_** The command `make tests` does **_not_** require the library `libadf.a` to be neither compiled nor installed, so it can be executed without having executed  `make adf`. In order to make tests work, `sample.adf` _must_ be generated, so you have to execute `make init` first.

In order to use a compiler different from GCC, you should modify the CC var in the Makefile (and consequently adjust the options in CVARS).

To install the library just execute 
```bash
sudo make install
```
> **_NOTE:_** Currently it just works on macOS and GNU/Linux (see `Further developments` section below).

## APIs

C implementation of the ADF format is *not* intended to be used directly. Some APIs for modern programming languages are available (and others will be available soon).

> **_BEWARE:_** All the APIs described below need `libadf.a` to be installed in the standard directory. See build it and install it with the instructions above.

### C++
> **_NOTE:_** In order to build the C++ API is required a compiler that fully support C++20, including `std::format`. If it's unavailable on your machine, change the command line options in the Makefile, from `-std=c++20` to `-std=c++17`.

To compile C++ APIs, just run 
```bash
make cpp # Compiles libadfpp.dylib into api/cc folder
sudo make cpp_install
```

This will install a **dynamic** library on your system, as well as its header file. By default the library will be installed under `/usr/local/lib`, and its header under `/usr/local/include`.

As of now, these scripts unfortunately just works on macOS, and probably some versions of GNU/Linux or other UNIX-like systems. We are planning to extend the compatibility to MS Windows too (see Further developments section later on).In the meantime, you can chang the deafult directories by changing the two variables `INCLUDE` and `LIB_DIR` in `api/cc/Makefile`.

An example of how to link and use this library is given in `api/cc/ecample.cc`. To compile it and runt it, execute
```bash
cd api/cc
make example
./example
```
> **_NOTE:_** For newer version of macOS, you have to set the environment variable `export DYLD_LIBRARY_PATH=/usr/local/lib` before running the example binary.

### Go

Golang APIs are quite similar to to those for C++ in their structure.

You can install the library in your Golang project by simply typing
```bash
go get github.com/TeriusProject/adf/api/go/adflib@latest
```

You can find an example under `api/go`. To execute it, just run
```bash
go run example.go
```
> **_NOTE:_** This example works **without** installing the library with `go get`. It just link it to the local folder `api/go/adflib`.

> **_NOTE:_** For newer version of macOS, you have to set the environment variable `export DYLD_LIBRARY_PATH=/usr/local/lib` before running the example binary.

## Test suites

Test suites accomplish 3 main purposes:
* They test the behavior of the functionality, as any unit test should do.
* They give examples of the way a feature should be used.
* They are tools for finding memory leaks in implementations. Each test must, in order to be a committed, proof that every tested features - including the test itself - has no memory leak. That's the reason why they **always** free the objects they use.

## Example of usage

Here's a short operative example of how it could be created an ADF structure, and save to file
```c
#include <adf.h>    // libadf.a already installed
#include <stdio.h>
#include <stdlib.h>

#define OUT_FILE_PATH "output.adf"
#define N_CHUNKS 10
#define N_WAVELENGTH 10
#define N_DEPTH 3

int main(void)
{
	adf_t adf;
	adf_header_t header;
	wavelength_info_t w_info;
	soil_depth_info_t s_info;
	reduction_info_t r_info = { 0 }; // Init to 0
	
	w_info = (wavelength_info_t) {
		.min_w_len_nm = { 0 },
		.max_w_len_nm = { 10000 },
		.n_wavelength = { N_WAVELENGTH },
	};

	s_info = (soil_depth_info_t) {
		.min_soil_depth_mm = { 0 },
		.max_soil_depth_mm = { 300 },
		.n_depth = { N_DEPTH },
	};

	header = create_header(ADF_FT_REGULAR, w_info, s_info, r_info, N_CHUNKS);
	adf_init(&adf, header, ADF_DAY); // each series takes 1 day
	
	/* register_data is just a procedure that adds some random series to adf. 
	In a real-world scenario, those series should be filled with data coming 
	from an agricolture database or - if you have a tech greenhouse - from your 
	own sensors. */
	register_data(&adf);
	printf("Writing on file `%s`...\n", OUT_FILE_PATH);
	write_file(adf);
	adf_free(&adf);
	printf("*DONE*\n");
}
```
You can find a working example of how to use `libadf` under the `example` directory. Other examples of how ADF works can be found in the `test` directory.

## Further developments

- [ ] Improve Makefiles and build pipeline. Wouldn't it be nice to add a `configure` script to make build and install procedures cross-platform, as well as Github actions?
- [ ] Extend the support to `valgrind` and `GNU gprof`. Currently the script `./memtest` only works for `leaks` (macOS).
- [ ] Improve efficiency (speed and memory usage).
- [ ] Bindings for the most common languages: Java, Python (via Cython) and Javascript/Typescript (via WebAssembly).
- [ ] Add SIMD support for Intel Intrinsics and ARM Neon. The development is already ongoing in a separate private repository. Once the first release will be ready, it will be linked into this one.
- [ ] Improve factory functions. Probably we have to wait a bit in order to see how we could make them easier to use.

## Contributions

Any contribution is welcome.  Though, try to discuss your idea or your implementation on the [subreddit](https://www.reddit.com/r/terius/) of the Terius Project first. Feel free to create new posts, but please mark them with the `format` flair.

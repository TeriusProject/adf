# Agriculture Data Format (ADF)

This repository contains the implementation of the ADF file specification, a portable and extensible file format for the storage of data relative to crops. The detailed specifications of the format are available in the `doc` dir. 

## At a glance

l'Information technology industry sta sempre più rivoluzionando quasi ogni settore del mercato, anche grazie alla straordinaria crescita dei tool di AI. Prima di poter implementare gli algoritmi più sofisticati per risolvere un qualunque genere di problema, è necessario che tale problema sia **_codificato_** in un formato elettronico. Possiamo pensare a tutti gli sviluppi che ci sono stati nell'elaborazione dei formati per le immagini digitali, la codifica dei colori, dei pixel etc. Codifiche che oggi rendono possibili, tra gli altri, gli algoritmi di text-to-image. Lo stesso vale per i suoni o l'encoding dei caratteri.

Questo è esattamente quello che si ripromette di fare l'Agriculture Data Format. L'idea è quella di elaborare un **_open format _** in cui codificare i dati relativi all'agricoltura. Il formato 
ADF servirà quindi per poter creare dei file per salvare tutti i dati relativi ai parametri con cui si coltivano delle colture. Accenniamo qui i principali
* Misurazioni relative all'irrigazione
* Misurazioni relative alla temperatura
* Misurazioni relative all'esposizione alla luce solare
* Misurazioni relative al suolo (pH, densità, etc)
* Misurazioni relative agli additivi (composti chimici) presenti nel suolo
* Misurazioni relative agli additivi (composti chimici) presenti nell'atmosfera
* Frequenza con cui vengono effettuate queste misurazioni (precisa al secondo)

Sul sito della _Food and Agricolture Organization_ (FAO) delle Nazioni Unite, ad esempio, ci sono diverse pagine dedicate alla raccolta di dati sulle colture (e.g. [questa](https://www.fao.org/land-water/databases-and-software/crop-information/wheat/en/)). Così come sono espressi, però, questi dati sono difficilmente utilizzabili dai sistemi automatici. È molto difficile infatti utilizzarli per costruire basi di dati centralizzate, o per fare il training di algoritmi di machine learning. Il discorso che vale per la FAO vale anche per gli altri siti che trattano questo genere di dati.

_Costruire questo standard significa posare la prima pietra per una rivoluzione IT dell'agricoltura_. L'ADF sarà per l'automatizzazione dell'agricoltura, quello che l'UNICODE è stato per la digitalizzazione della scrittura e l'HTML è stato per il web.

## Goals

L'obiettivo finale del progetto Terius è incrementare la conoscenza agricola, in maniera tale da dare un boost al mondo della ricerca agronomica. ADF è il primo prodotto del progetto Terius Per raggiungere questo obiettivo a lungo termine, è necessario svilupparlo in obiettivi più piccoli, come ad esempio:
* Rendere facilmente serializzabili i dati relativi alle colture.
* Avere un formato che permetta di registrare delle serie di dati relativi ad una coltura in modo che sia resi espliciti **univocamente** ed **esplicitamente** i composti chimici utilizzati, le quantità e la frequenza con cui vengono fatte le misurazioni.
* Un formato con cui si possano fare analisi statistiche di varia natura, in maniera automatica o semiautomatica.
* Last, but not least, avere un formato che possa essere utilizzato per istruire un algoritmo di machine learning in maniera agevole ed efficiente.

## Implementation details

For some implementative details about of datatypes and functions, see [the header adf.h](https://github.com/aestriplex/adf/blob/main/src/adf.h).

## Build and install

To build the library it is sufficient to run `make` within the repository root. By default, the `all` rule will compile and execute all the unit tests too.
```bash
cd adf
make
```
You can split the command in two though
```bash
cd adf
make adf   # just compile the library libadf.a
make tests # just compile and execute the tests
```
> **_NOTE:_** The command `make tests` does **_not_** require the library `libadf.a` to be neither compiled nor installed.

In order to use a compiler different from GCC, you should modify the CC var in the Makefile (and consequently adjust the options in CVARS).

To install the library just execute 
```bash
sudo make install
```
> **_NOTE:_** Currently it just works on macOS and GNU/Linux (see `Further developments` section below).

## Example of usage

Here's a short operative example of how it could be created an adf structure, and save to file
```c
#include <adf.h>    // libadf.a already installed
#include <stdio.h>
#include <stdlib.h>

#define OUT_FILE_PATH "output.adf"

int main(void)
{
	adf_header_t header;
	adf_t adf;
	
	header = (adf_header_t) {
		.signature = { __ADF_SIGNATURE__ },
		.version = __ADF_VERSION__,
		.farming_tec = 0x01u,
		.min_w_len_nm = { 0 },
		.max_w_len_nm = { 10000 },
		.n_chunks = { 10 },
		.n_wavelength = { 10 }
	};
	adf_init(&adf, header, 86400); // each series takes 1 day
	
	/* register_data is just a procedure that adds some random series to adf. 
	In a real-world scenario, those series should be filled with data coming 
	from an agricolture database or - if you have a tech greenhouse - from your 
	own sensors. */
	register_data(&adf);
	printf("Writing on file `%s`...\n", OUT_FILE_PATH);
	write_file(adf);
	printf("*DONE*\n");
}
```
You can find a working example of how to use `libadf` under the `example` directory. Other examples can be found in the `test` directory.

## Further developments

- [ ] Check for memory related bugs, and optimize memory usage.
- [ ] Improve efficiency (speed and memory usage).
- [ ] Bindings for the most common languages: C++, Java, Go and Javascript (through WebAssembly).
- [ ] Add SIMD support for Intel Intrinsics and ARM Neon. The development is already ongoing in [this repository](https://github.com/aestriplex/intrinsic). Once the first release will be ready, it will be linked into this one.
- [ ] Improve factory functions. Right now create functions are (almost) useless. Probably we have to wait a bit in order to see how could we make them easier to use.
- [ ] Improve Makefiles. Wouldn't it be nice to add a `configure` script to make build and install procedures platform-dependent?

## Contributions

Any contribution is welcome.  Though, try to discuss your idea or your implementation on the [subreddit](https://www.reddit.com/r/terius/) of the Terius Project first. Feel free to create new posts, but please mark them with the `format` flair.
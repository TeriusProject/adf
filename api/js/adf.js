/* adf.js - Implementation of the JS interface for the ADF library
 * ------------------------------------------------------------------------
 * ADF - Agriculture Data Format
 * Copyright (C) 2024 Matteo Nicoli
 *
 * This file is part of Terius
 *
 * ADF is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Terius is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// const fs = require('fs');
// var source = fs.readFileSync('./adflib.wasm');
// const env = {
// 	memoryBase: 0,
// 	tableBase: 0,
// 	memory: new WebAssembly.Memory({
// 		initial: 256
// 	}),
// 	table: new WebAssembly.Table({
// 		initial: 0,
// 		element: 'anyfunc'
// 	})
// };

// var typedArray = new Uint8Array(source);

// WebAssembly.instantiate(typedArray, {
// 	env: env
// }).then(result => {
// 	console.log(result.instance.exports.size_header());
// }).catch(e => {
// 	console.log(e);
// });

const fs = require('fs');
const nullptr = 0x00;

async function loadWasm() {
	const wasmBuffer = fs.readFileSync('./adflib.wasm');

	// Instantiate the WebAssembly module
	const wasmModule = await WebAssembly.instantiate(wasmBuffer, {
		env: {
			// Provide any required imports here, e.g., memory, malloc, free, etc.
		},
	});

	const { instance } = wasmModule;
	const memory = instance.exports.memory; // Access linear memory
	const size_adf_t = instance.exports.size_adf_t; // The exported function

	// Helper to write data to the WebAssembly memory
	function writeStruct(pointer, data) {
		const view = new DataView(memory.buffer);
		let offset = pointer;

		// Write each field of the struct based on your struct definition
		view.setUint32(offset, data.header, true); // Assuming `header` is a uint32_t
		offset += 4;
		view.setUint32(offset, data.metadata, true); // Assuming `metadata` is a uint32_t
		offset += 4;
		view.setUint32(offset, data.series, true); // Assuming `series` is a pointer (uint32_t)
	}

	// Allocate memory for the struct
	const structSize = 12; // Adjust this size based on sizeof(adf_t)
	const structPointer = instance.exports.malloc(structSize);

	// Populate the struct data
	const exampleData = {
		header: 0x01,
		metadata: 0x02,
		series: 0x00, // NULL pointer for this example
	};
	writeStruct(structPointer, exampleData);

	// Call the function with the pointer
	const result = size_adf_t(structPointer);
	console.log('Size of adf_t:', result);

	// Free the memory
	instance.exports.free(structPointer);
}

loadWasm().catch(console.error);

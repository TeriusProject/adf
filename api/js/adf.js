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

const StatusCode = Obnject.freeze({
	OK: instance.exports.get_status_code_OK(),
	HEADER_CORRUPTED: instance.exports.get_status_code_HEADER_CORRUPTED(),
	METADATA_CORRUPTED: instance.exports.get_status_code_METADATA_CORRUPTED(),
	SERIES_CORRUPTED: instance.exports.get_status_code_SERIES_CORRUPTED(),
	ZERO_REPEATED_SERIES: instance.exports.get_status_code_ZERO_REPEATED_SERIES(),
	EMPTY_SERIES: instance.exports.get_status_code_EMPTY_SERIES(),
	TIME_OUT_OF_BOUND: instance.exports.get_status_code_TIME_OUT_OF_BOUND(),
	ADDITIVE_OVERFLOW: instance.exports.get_status_code_ADDITIVE_OVERFLOW(),
	NULL_HEADER_SOURCE: instance.exports.get_status_code_NULL_HEADER_SOURCE(),
	NULL_HEADER_TARGET: instance.exports.get_status_code_NULL_HEADER_TARGET(),
	NULL_META_SOURCE: instance.exports.get_status_code_NULL_META_SOURCE(),
	NULL_META_TARGET: instance.exports.get_status_code_NULL_META_TARGET(),
	NULL_SERIES_SOURCE: instance.exports.get_status_code_NULL_SERIES_SOURCE(),
	NULL_SERIES_TARGET: instance.exports.get_status_code_NULL_SERIES_TARGET(),
	NULL_SOURCE: instance.exports.get_status_code_NULL_SOURCE(),
	NULL_TARGET: instance.exports.get_status_code_NULL_TARGET(),
	NULL_ADDITIVE_SOURCE: instance.exports.get_status_code_NULL_ADDITIVE_SOURCE(),
	NULL_ADDITIVE_TARGET: instance.exports.get_status_code_NULL_ADDITIVE_TARGET(),
	RUNTIME_ERROR: instance.exports.get_status_code_RUNTIME_ERROR(),
});

const FarmingTechnique = Object.freeze({
	REGULAR: instance.exports.get_farming_tec_code_REGULAR(),
	INDOOR: instance.exports.get_farming_tec_code_INDOOR(),
	INDOOR_PROTECTED: instance.exports.get_farming_tec_code_INDOOR_PROTECTED(),
	OUTDOOR: instance.exports.get_farming_tec_code_OUTDOOR(),
	ARTIFICIAL_SOIL: instance.exports.get_farming_tec_code_ARTIFICIAL_SOIL(),
	HYDROPONICS: instance.exports.get_farming_tec_code_HYDROPONICS(),
	ANTHROPONICS: instance.exports.get_farming_tec_code_ANTHROPONICS(),
	AEROPONICS: instance.exports.get_farming_tec_code_AEROPONICS(),
	FOGPONICS: instance.exports.get_farming_tec_code_FOGPONICS(),
});

const ReductionCode = Object.freeze({
	NONE: instance.exports.get_reduction_code_NONE(),
	AVG: instance.exports.get_reduction_code_AVG(),
	MAVG: instance.exports.get_reduction_code_MAVG(),
});

class Additive {
	constructor(code, concentration) {
		this.code = code;
		this.concentration = concentration;
	}

	toCAdditive(instance) {
		return instance.exports.create_additive(this.code, this.concentration);
	}
}

class AdditiveList {
	constructor(adds) {
		this.adds = adds;
	}

	size() {
		return this.adds.length;
	}

	toCList(instance) {
		return this.adds.map(additive => additive.toCAdditive(instance));
	}
}

class Matrix {
	constructor(rows = 0, columns = 0) {
		this.mat = new Array(rows * columns).fill(undefined);
		this.rows = rows;
		this.columns = columns;
	}

	static newEmptyMatrix(rows, columns) {
		return new Matrix(rows, columns);
	}

	static newMatrix(columns) {
		return new Matrix(0, columns);
	}

	addRow(row) {
		this.mat.push(...row);
		this.rows += 1;
	}

	shape() {
		return { rows: this.rows, columns: this.columns };
	}

	at(row, column) {
		if (row < this.rows && column < this.columns) {
			return this.mat[column + row * this.columns];
		} else {
			throw new Error("Index out of bounds");
		}
	}
}

class Series {
	constructor(lightExposure, soilTempC, envTempC, waterUseMl, pH, pBar, soilDensityKgM3, soilAdditives, atmAdditives, repeated) {
		this.lightExposure = lightExposure;
		this.soilTempC = soilTempC;
		this.envTempC = envTempC;
		this.waterUseMl = waterUseMl;
		this.pH = pH;
		this.pBar = pBar;
		this.soilDensityKgM3 = soilDensityKgM3;
		this.soilAdditives = soilAdditives;
		this.atmAdditives = atmAdditives;
		this.repeated = repeated;
	}

	toCSeries(instance) {
		return instance.exports.create_series(
			this.lightExposure,
			this.soilTempC,
			this.envTempC,
			this.waterUseMl,
			this.pH,
			this.pBar,
			this.soilDensityKgM3,
			this.soilAdditives.size(),
			this.atmAdditives.size(),
			this.soilAdditives.toCList(instance),
			this.atmAdditives.toCList(instance),
			this.repeated
		);
	}
}

class WaveInfo {
	constructor(minWavelenNm, maxWavelenNm, nWavelengths) {
		this.minWavelenNm = minWavelenNm;
		this.maxWavelenNm = maxWavelenNm;
		this.nWavelengths = nWavelengths;
	}

	toCWaveInfo(instance) {
		return instance.exports.create_wavelength_info(this.minWavelenNm, this.maxWavelenNm, this.nWavelengths);
	}
}

class SoilDepthInfo {
	constructor(transY, maxSoilDepthMm, nDepth) {
		this.transY = transY;
		this.maxSoilDepthMm = maxSoilDepthMm;
		this.nDepth = nDepth;
	}

	toCSoilDepthInfo(instance) {
		if (this.transY === 0) {
			return instance.exports.create_soil_depth_info(this.maxSoilDepthMm, this.nDepth);
		}
		return instance.exports.create_trans_soil_depth_info(this.transY, this.maxSoilDepthMm, this.nDepth);
	}
}

class ReductionInfo {
	constructor(soilDensity, pressure, lightExposure, waterUse, soilTemp, envTemp) {
		this.soilDensity = soilDensity;
		this.pressure = pressure;
		this.lightExposure = lightExposure;
		this.waterUse = waterUse;
		this.soilTemp = soilTemp;
		this.envTemp = envTemp;
	}

	toCReductionInfo(instance) {
		return instance.exports.create_reduction_info(
			this.soilDensity,
			this.pressure,
			this.lightExposure,
			this.waterUse,
			this.soilTemp,
			this.envTemp
		);
	}
}

class PrecisionInfo {
	constructor(soilDensity, pressure, lightExposure, waterUse, soilTemp, envTemp, additiveConc) {
		this.soilDensity = soilDensity;
		this.pressure = pressure;
		this.lightExposure = lightExposure;
		this.waterUse = waterUse;
		this.soilTemp = soilTemp;
		this.envTemp = envTemp;
		this.additiveConc = additiveConc;
	}

	toCPrecisionInfo(instance) {
		return instance.exports.create_precision_info(
			this.soilDensity,
			this.pressure,
			this.lightExposure,
			this.waterUse,
			this.soilTemp,
			this.envTemp,
			this.additiveConc
		);
	}
}

class Header {
	constructor(farmingTec, wInfo, sInfo, redInfo, precInfo, nChunks) {
		this.farmingTec = farmingTec;
		this.wInfo = wInfo;
		this.sInfo = sInfo;
		this.redInfo = redInfo;
		this.precInfo = precInfo;
		this.nChunks = nChunks;
	}

	toCHeader(instance) {
		return instance.exports.create_header(
			this.farmingTec,
			this.wInfo.toCWaveInfo(instance),
			this.sInfo.toCSoilDepthInfo(instance),
			this.redInfo.toCReductionInfo(instance),
			this.precInfo.toCPrecisionInfo(instance),
			this.nChunks
		);
	}
}

const fs = require('fs');
const nullptr = 0x00;

async function loadWasm() {
	const wasmBuffer = fs.readFileSync('./adflib.wasm');

	const wasmModule = await WebAssembly.instantiate(wasmBuffer, {
		env: {},
	});

	const { instance } = wasmModule;
	return instance;
}
loadWasm()
	.then(instance => {
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
			series: nullptr,
		};
		writeStruct(structPointer, exampleData);

		// Call the function with the pointer
		const result = size_adf_t(structPointer);
		console.log('Size of adf_t:', result);

		// Free the memory
		instance.exports.free(structPointer);
	})
	.catch(console.error);

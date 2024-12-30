import fs from 'fs';
const nullptr = 0x00;

const wasmBuffer = fs.readFileSync('./adflib.wasm');
const wasmModule = await WebAssembly.instantiate(wasmBuffer, {
	env: {},
});
const { instance } = wasmModule;
const memory = instance.exports.memory;

export const StatusCode = Object.freeze({
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

export const FarmingTechnique = Object.freeze({
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

export const ReductionCode = Object.freeze({
	NONE: instance.exports.get_reduction_code_NONE(),
	AVG: instance.exports.get_reduction_code_AVG(),
	MAVG: instance.exports.get_reduction_code_MAVG(),
});

export const SeriesTime = Object.freeze({
	DAY: instance.exports.get_ADF_DAY(),
	WEEK: instance.exports.get_ADF_WEEK(),
	MONTH_28: instance.exports.get_ADF_MONTH_28(),
	MONTH_29: instance.exports.get_ADF_MONTH_29(),
	MONTH_30: instance.exports.get_ADF_MONTH_30(),
	MONTH_31: instance.exports.get_ADF_MONTH_31(),
});

const DatatypeSize = Object.freeze({
	UINT_BIG_T: instance.exports.get_UINT_BIG_T_SIZE(),
	UINT_T: instance.exports.get_UINT_T_SIZE(),
	UINT_SMALL_T: instance.exports.get_UINT_SMALL_T_SIZE(),
	UINT_TINY_T: instance.exports.get_UINT_TINY_T_SIZE(),
	REAL_T: instance.exports.get_REAL_T_SIZE(),
	ADDITIVE_T: instance.exports.get_ADD_T_SIZE(),
});

export class Additive {
	constructor(code, concentration) {
		this.code = code;
		this.concentration = concentration;
	}
}

export class AdditiveList {
	constructor(adds) {
		this.adds = adds;
	}

	size() { return this.adds.length; }

	toCList() {
		const byteSize = this.adds.length * DatatypeSize.ADDITIVE_T;
		const ptr = instance.exports.malloc(byteSize);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		this.adds.forEach(additive => {
			view.setUint32(offset, additive.code, true);
			offset += 4;
			view.setFloat32(offset, additive.concentration, true);
			offset += 4;
		});
		return ptr;
	}
}

export class Matrix {
	constructor(rows = 1, columns = 1, datatype = DatatypeSize.REAL_T) {
		this.mat = new Array(rows * columns).fill(undefined);
		this.rows = rows;
		this.columns = columns;
		this.datatype = datatype;
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

	toCArray() {
		const byteSize = this.mat.length * this.datatype;
		const ptr = instance.exports.malloc(byteSize);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		const getUpdateFn = () => {
			switch(this.datatype) {
				case DatatypeSize.UINT_BIG_T:
					return view.setBigUint64;
				case DatatypeSize.UINT_T:
					return view.setUint32;
				case DatatypeSize.UINT_SMALL_T:
					return view.setUint16;
				case DatatypeSize.UINT_TINY_T:
					return view.setUint8;
				case DatatypeSize.REAL_T:
					return view.setFloat32;
				case DatatypeSize.ADDITIVE_T:
				default:
					throw new Error(`Invalid datatype: ${this.datatype}`)
			}
		}
		const updateFn = getUpdateFn();
		this.mat.forEach(e => {
			updateFn(offset, e, true);
			offset += this.datatype;
		});
		return ptr;
	}
}

export class Series {
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

	toCSeries() {
		return instance.exports.create_series(
			this.lightExposure.toCArray(),
			this.soilTempC.toCArray(),
			this.envTempC.toCArray(),
			this.waterUseMl.toCArray(),
			this.pH,
			this.pBar,
			this.soilDensityKgM3,
			this.soilAdditives.size(),
			this.atmAdditives.size(),
			this.soilAdditives.toCList(),
			this.atmAdditives.toCList(),
			this.repeated
		);
	}
}

export class WaveInfo {
	constructor(minWavelenNm, maxWavelenNm, nWavelengths) {
		this.minWavelenNm = minWavelenNm;
		this.maxWavelenNm = maxWavelenNm;
		this.nWavelengths = nWavelengths;
	}

	toCWaveInfo() {
		return instance.exports.create_wavelength_info(this.minWavelenNm, this.maxWavelenNm, this.nWavelengths);
	}
}

export class SoilDepthInfo {
	constructor(transY, maxSoilDepthMm, nDepth) {
		this.transY = transY;
		this.maxSoilDepthMm = maxSoilDepthMm;
		this.nDepth = nDepth;
	}

	toCSoilDepthInfo() {
		if (this.transY === 0) {
			return instance.exports.create_soil_depth_info(this.maxSoilDepthMm, this.nDepth);
		}
		return instance.exports.create_trans_soil_depth_info(this.transY, this.maxSoilDepthMm, this.nDepth);
	}
}

export class ReductionInfo {
	constructor(soilDensity, pressure, lightExposure, waterUse, soilTemp, envTemp) {
		this.soilDensity = soilDensity;
		this.pressure = pressure;
		this.lightExposure = lightExposure;
		this.waterUse = waterUse;
		this.soilTemp = soilTemp;
		this.envTemp = envTemp;
	}

	toCReductionInfo() {
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

export class PrecisionInfo {
	constructor(soilDensity, pressure, lightExposure, waterUse, soilTemp, envTemp, additiveConc) {
		this.soilDensity = soilDensity;
		this.pressure = pressure;
		this.lightExposure = lightExposure;
		this.waterUse = waterUse;
		this.soilTemp = soilTemp;
		this.envTemp = envTemp;
		this.additiveConc = additiveConc;
	}

	toCPrecisionInfo() {
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

export class Header {
	constructor(farmingTec, wInfo, sInfo, redInfo, precInfo, nChunks) {
		this.farmingTec = farmingTec;
		this.wInfo = wInfo;
		this.sInfo = sInfo;
		this.redInfo = redInfo;
		this.precInfo = precInfo;
		this.nChunks = nChunks;
	}

	toCHeader() {
		return instance.exports.create_header(
			this.farmingTec,
			this.wInfo.toCWaveInfo(),
			this.sInfo.toCSoilDepthInfo(),
			this.redInfo.toCReductionInfo(),
			this.precInfo.toCPrecisionInfo(),
			this.nChunks
		);
	}
}

export class Adf {
	constructor(header, periodSec) {
		// this.header = header;
		// this.periodSec = periodSec;
		// const byteSize = ;
		// const ptr = instance.exports.malloc(byteSize);
		// const view = new DataView(memory.buffer);
		// let offset = ptr;
	}

	static unmarshal(bytes) {
		// const adf = new Adf();
		// const res = C.unmarshal(adf.cAdf, new Uint8Array(bytes));
		// if (res !== C.ADF_OK) {
		// 	return { adf: null, error: getAdfError(res) };
		// }
		// return adf;
	}

	sizeBytes() {
		// return C.size_adf_t(this.cAdf);
	}

	marshal() {
		// const cBytes = new Uint8Array(this.sizeBytes());
		// const res = C.marshal(cBytes, this.cAdf);
		// if (res !== C.ADF_OK) {
		// 	return { bytes: null, error: getAdfError(res) };
		// }
		// return { bytes: cBytes, error: null };
	}

	addSeries(series) {
	}

	updateSeries(series, time) {
	}

	removeSeries() {
	}

	getVersion() {
		const version = instance.exports.get_hex_version();
		return `${(version & 0xFF00) >> 8}.${(version & 0x00F0) >> 4}.${ version & 0x000F}`
	}

	dispose() {
		C.adf_free(this.cAdf);
	}
}

const l = new AdditiveList([new Additive(12, 34.67), new Additive(1, 4.99)]);
const p = l.toCList();
console.info(p);
console.info(memory.buffer);

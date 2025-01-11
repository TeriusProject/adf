/* adf.ts - Implementation of the TS interface for the ADF library
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

import { readFile } from 'fs/promises';

const memoryConfig = new WebAssembly.Memory({
	initial: 10,    /* 10 pages, 64KB each   =  640 KB */
	maximum: 1000,  /* 1000 pages, 64KB each =  64 MB  */
});

const wasmPath = new URL('./adflib.wasm', import.meta.url);
const bytes = await readFile(wasmPath);
const wasmModule = await WebAssembly.instantiate(bytes, {
	env: { memoryConfig },
});
const { instance } = wasmModule;
const exports = instance.exports;
const memory = exports.memory as WebAssembly.Memory;
const isLittleEndian = (): boolean => {
	const buffer = new ArrayBuffer(2);
	const uint16View = new Uint16Array(buffer);
	const uint8View = new Uint8Array(buffer);
	uint16View[0] = 0x1122;
	return uint8View[0] === 0x22;
};
const littleEndian = isLittleEndian();

export const nullptr = 0x00;
export type pointer = number;

const adflib = Object.freeze({
	get_status_code_OK: exports.get_status_code_OK as () => number,
	get_status_code_HEADER_CORRUPTED: exports.get_status_code_HEADER_CORRUPTED as () => number,
	get_status_code_METADATA_CORRUPTED: exports.get_status_code_METADATA_CORRUPTED as () => number,
	get_status_code_SERIES_CORRUPTED: exports.get_status_code_SERIES_CORRUPTED as () => number,
	get_status_code_ZERO_REPEATED_SERIES: exports.get_status_code_ZERO_REPEATED_SERIES as () => number,
	get_status_code_EMPTY_SERIES: exports.get_status_code_EMPTY_SERIES as () => number,
	get_status_code_TIME_OUT_OF_BOUND: exports.get_status_code_TIME_OUT_OF_BOUND as () => number,
	get_status_code_ADDITIVE_OVERFLOW: exports.get_status_code_ADDITIVE_OVERFLOW as () => number,
	get_status_code_NULL_HEADER_SOURCE: exports.get_status_code_NULL_HEADER_SOURCE as () => number,
	get_status_code_NULL_HEADER_TARGET: exports.get_status_code_NULL_HEADER_TARGET as () => number,
	get_status_code_NULL_META_SOURCE: exports.get_status_code_NULL_META_SOURCE as () => number,
	get_status_code_NULL_META_TARGET: exports.get_status_code_NULL_META_TARGET as () => number,
	get_status_code_NULL_SERIES_SOURCE: exports.get_status_code_NULL_SERIES_SOURCE as () => number,
	get_status_code_NULL_SERIES_TARGET: exports.get_status_code_NULL_SERIES_TARGET as () => number,
	get_status_code_NULL_SOURCE: exports.get_status_code_NULL_SOURCE as () => number,
	get_status_code_NULL_TARGET: exports.get_status_code_NULL_TARGET as () => number,
	get_status_code_NULL_ADDITIVE_SOURCE: exports.get_status_code_NULL_ADDITIVE_SOURCE as () => number,
	get_status_code_NULL_ADDITIVE_TARGET: exports.get_status_code_NULL_ADDITIVE_TARGET as () => number,
	get_status_code_RUNTIME_ERROR: exports.get_status_code_RUNTIME_ERROR as () => number,
	get_farming_tec_code_REGULAR: exports.get_farming_tec_code_REGULAR as () => number,
	get_farming_tec_code_INDOOR: exports.get_farming_tec_code_INDOOR as () => number,
	get_farming_tec_code_INDOOR_PROTECTED: exports.get_farming_tec_code_INDOOR_PROTECTED as () => number,
	get_farming_tec_code_OUTDOOR: exports.get_farming_tec_code_OUTDOOR as () => number,
	get_farming_tec_code_ARTIFICIAL_SOIL: exports.get_farming_tec_code_ARTIFICIAL_SOIL as () => number,
	get_farming_tec_code_HYDROPONICS: exports.get_farming_tec_code_HYDROPONICS as () => number,
	get_farming_tec_code_ANTHROPONICS: exports.get_farming_tec_code_ANTHROPONICS as () => number,
	get_farming_tec_code_AEROPONICS: exports.get_farming_tec_code_AEROPONICS as () => number,
	get_farming_tec_code_FOGPONICS: exports.get_farming_tec_code_FOGPONICS as () => number,
	get_reduction_code_NONE: exports.get_reduction_code_NONE as () => number,
	get_reduction_code_AVG: exports.get_reduction_code_AVG as () => number,
	get_reduction_code_MAVG: exports.get_reduction_code_MAVG as () => number,
	get_ADF_DAY: exports.get_ADF_DAY as () => number,
	get_ADF_WEEK: exports.get_ADF_WEEK as () => number,
	get_ADF_MONTH_28: exports.get_ADF_MONTH_28 as () => number,
	get_ADF_MONTH_29: exports.get_ADF_MONTH_29 as () => number,
	get_ADF_MONTH_30: exports.get_ADF_MONTH_30 as () => number,
	get_ADF_MONTH_31: exports.get_ADF_MONTH_31 as () => number,
	get_UINT_BIG_T_SIZE: exports.get_UINT_BIG_T_SIZE as () => number,
	get_UINT_T_SIZE: exports.get_UINT_T_SIZE as () => number,
	get_UINT_SMALL_T_SIZE: exports.get_UINT_SMALL_T_SIZE as () => number,
	get_UINT_TINY_T_SIZE: exports.get_UINT_TINY_T_SIZE as () => number,
	get_REAL_T_SIZE: exports.get_REAL_T_SIZE as () => number,
	get_ADD_T_SIZE: exports.get_ADD_T_SIZE as () => number,
	new_additive: exports.new_additive as (code: number, concentration: number) => pointer,
	malloc: exports.malloc as (size: number) => pointer,
	free: exports.free as (obj: pointer) => void,
	new_series: exports.new_series as (lightExposure: pointer, soilTempC: pointer, envTempC: pointer, waterUseMl: pointer, pH: number, pBar: number, soilDensityKgM3: number, soilAdditivesSize: number, atmAdditivesSize: number, soilAdditives: pointer, atmAdditives: pointer, repeated: number) => pointer,
	new_wavelength_info: exports.new_wavelength_info as (minWavelenNm: number, maxWavelenNm: number, nWavelengths: number) => pointer,
	new_precision_info: exports.new_precision_info as (soilDensity: number, pressure: number, lightExposure: number, waterUse: number, soilTemp: number, envTemp: number, additiveConc: number) => pointer,
	new_soil_depth_info: exports.new_soil_depth_info as (maxSoilDepthMm: number, nDepth: number) => pointer,
	new_trans_soil_depth_info: exports.new_trans_soil_depth_info as (transY: number, maxSoilDepthMm: number, nDepth: number) => pointer,
	new_reduction_info: exports.new_reduction_info as (soilDensity: number, pressure: number, lightExposure: number, waterUse: number, soilTemp: number, envTemp: number, additiveConc: number) => pointer,
	new_header: exports.new_header as (farmingTec: number, wInfo: pointer, sInfo: pointer, redInfo: pointer, precInfo: pointer, nChunks: number) => pointer,
	new_adf: exports.new_adf as (header: pointer, periodSec: number) => pointer,
	new_empty_adf: exports.new_empty_adf as () => pointer,
	add_series: exports.add_series as (adf: pointer, series: pointer) => number,
	unmarshal: exports.unmarshal as (adf: pointer, bytes: pointer) => number,
	marshal: exports.marshal as (bytes: pointer, adf: pointer) => number,
	get_hex_version: exports.get_hex_version as () => number,
	update_series: exports.update_series as (adf: pointer, series: pointer, time: number) => number,
	remove_series: exports.remove_series as (adf: pointer) => number,
	series_delete: exports.series_delete as (series: pointer) => void,
	adf_delete: exports.adf_delete as (adf: pointer) => void,
	size_adf_t: exports.size_adf_t as (adf: pointer) => number,
	get_header: exports.get_header as (adf: pointer) => pointer,
	get_metadata: exports.get_metadata as (adf: pointer) => pointer,
	get_series_list: exports.get_series_list as (adf: pointer) => pointer,
	get_w_info: exports.get_w_info as (header: pointer) => pointer,
	get_soil_info: exports.get_soil_info as (header: pointer) => pointer,
	get_red_info: exports.get_red_info as (header: pointer) => pointer,
	get_prec_info: exports.get_prec_info as (header: pointer) => pointer,
	set_seed_time: exports.set_seed_time as (adf: pointer, seedTime: bigint) => number,
	set_harvest_time: exports.set_harvest_time as (adf: pointer, harvestTime: bigint) => number,
	size_series_t: exports.size_series_t as (adf: pointer, series: pointer) => number,
});

export const AdfDatatype = Object.freeze({
	BIG_INT: adflib.get_UINT_BIG_T_SIZE(),
	INT: adflib.get_UINT_T_SIZE(),
	SMALL_INT: adflib.get_UINT_SMALL_T_SIZE(),
	TINY_INT: adflib.get_UINT_TINY_T_SIZE(),
	FLOAT: adflib.get_REAL_T_SIZE(),
	ADDITIVE_T: adflib.get_ADD_T_SIZE(),
});

class AdflibConverter {

	static toCAdditive(additive: Additive): pointer {
		return adflib.new_additive(additive.code, additive.concentration);
	}

	static fromCAdditive(additive: pointer, view: DataView): Additive {
		return {
			code: view.getUint32(additive + 2, littleEndian),
			concentration: view.getFloat32(additive + 6, littleEndian),
		};
	}

	static toCAdditiveList(adds: Additive[]): pointer {
		const byteSize = adds.length * AdfDatatype.ADDITIVE_T;
		const ptr = adflib.malloc(byteSize);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		adds.forEach(additive => {
			view.setUint32(offset, additive.code, littleEndian);
			offset += 4;
			view.setFloat32(offset, additive.concentration, littleEndian);
			offset += 4;
		});
		return ptr;
	}

	static fromCAdditiveList(additiveList: pointer, length: number, view: DataView): Additive[] {
		const additives: Additive[] = [];

		if (length === 0) return additives;

		const additivesPtr = view.getUint32(additiveList, littleEndian);
		for (let i = 0; i < length; i++) {
			additives.push(AdflibConverter.fromCAdditive(additivesPtr + (i * 4), view));
		}
		return additives;
	}

	static toCArray(matrix: Matrix): pointer {
		const byteSize = matrix.innerArray().length * AdfDatatype.FLOAT;
		const ptr = adflib.malloc(byteSize);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		matrix.innerArray().forEach(e => {
			view.setFloat32(offset, e, littleEndian);
			offset += AdfDatatype.FLOAT;
		});
		return ptr;
	}

	static fromCArray(array: pointer, view: DataView, shape: MatrixShape): Matrix {
		const m = new Matrix(shape.rows, shape.columns);
		const arr: any[] = [];
		for (let i = 0; i < shape.rows * shape.columns; i++)
			arr.push(view.getFloat32(array + (i * AdfDatatype.FLOAT), littleEndian));
		m.setInnerArray(arr);
		return m;
	}

	static toCSeries(series: Series): pointer {
		return adflib.new_series(
			AdflibConverter.toCArray(series.lightExposure),
			AdflibConverter.toCArray(series.soilTempC),
			AdflibConverter.toCArray(series.envTempC),
			AdflibConverter.toCArray(series.waterUseMl),
			series.pH,
			series.pBar,
			series.soilDensityKgM3,
			series.soilAdditives.length,
			series.atmAdditives.length,
			AdflibConverter.toCAdditiveList(series.soilAdditives),
			AdflibConverter.toCAdditiveList(series.atmAdditives),
			series.repeated
		);
	}

	static fromCSeries(series: pointer, header: Header, view: DataView): Series {
		const lightExposurePtr = view.getUint32(series, littleEndian);
		const lightExposure = AdflibConverter.fromCArray(lightExposurePtr, view, { rows: header.wInfo.nWavelengths, columns: header.nChunks });
		const soilTempPtr = view.getUint32(series + 4, littleEndian);
		const soilTempC = AdflibConverter.fromCArray(soilTempPtr, view, { rows: header.sInfo.nDepth, columns: header.nChunks });
		const envTempPtr = view.getUint32(series + 8, littleEndian);
		const envTempC = AdflibConverter.fromCArray(envTempPtr, view, { rows: 1, columns: header.nChunks });
		const waterUsePtr = view.getUint32(series + 12, littleEndian);
		const waterUseMl = AdflibConverter.fromCArray(waterUsePtr, view, { rows: 1, columns: header.nChunks });
		const pH = view.getUint8(series + 16);
		const pBar = view.getFloat32(series + 17, littleEndian);
		const soilDensityKgM3 = view.getFloat32(series + 21, littleEndian);
		const nSoilAdditives = view.getUint16(series + 25, littleEndian);
		const nAtmAdditives = view.getUint16(series + 27, littleEndian);
		const soilAdditives = AdflibConverter.fromCAdditiveList(series + 29, nSoilAdditives, view);
		const atmAdditives = AdflibConverter.fromCAdditiveList(series + 33, nAtmAdditives, view);
		const repeated = view.getUint32(series + 37, littleEndian);
		return { lightExposure, soilTempC, envTempC, waterUseMl, pH, pBar, soilDensityKgM3, soilAdditives, atmAdditives, repeated };
	}

	static toCWaveInfo(wInfo: WaveInfo): pointer {
		return adflib.new_wavelength_info(wInfo.minWavelenNm, wInfo.maxWavelenNm, wInfo.nWavelengths);
	}

	static fromCWaveInfo(cWaveInfo: pointer, view: DataView): WaveInfo {
		return {
			minWavelenNm: view.getUint16(cWaveInfo, littleEndian),
			maxWavelenNm: view.getUint16(cWaveInfo + 2, littleEndian),
			nWavelengths: view.getUint16(cWaveInfo + 4, littleEndian)
		};
	}

	static toCSoilDepthInfo(sInfo: SoilDepthInfo): pointer {
		if (sInfo.transY === 0) {
			return adflib.new_soil_depth_info(sInfo.maxSoilDepthMm, sInfo.nDepth);
		}
		return adflib.new_trans_soil_depth_info(sInfo.transY, sInfo.maxSoilDepthMm, sInfo.nDepth);
	}

	static fromCSoilInfo(cSoilInfo: pointer, view: DataView): SoilDepthInfo {
		return {
			maxSoilDepthMm: view.getUint16(cSoilInfo, littleEndian),
			transY: view.getUint16(cSoilInfo + 2, littleEndian),
			nDepth: view.getUint16(cSoilInfo + 4, littleEndian)
		};
	}

	static toCReductionInfo(rInfo: ReductionInfo): pointer {
		return adflib.new_reduction_info(rInfo.soilDensity, rInfo.pressure, rInfo.lightExposure,
			rInfo.waterUse, rInfo.soilTemp, rInfo.envTemp, rInfo.additiveConc);
	}

	static fromCReductionInfo(cReductionInfo: pointer, view: DataView): ReductionInfo {
		return {
			soilDensity: view.getUint32(cReductionInfo, littleEndian),
			pressure: view.getUint32(cReductionInfo + 4),
			lightExposure: view.getUint32(cReductionInfo + 8),
			waterUse: view.getUint32(cReductionInfo + 12),
			soilTemp: view.getUint32(cReductionInfo + 16),
			envTemp: view.getUint32(cReductionInfo + 20),
			additiveConc: view.getUint32(cReductionInfo + 24)
		};
	}

	static toCPrecisionInfo(pInfo: PrecisionInfo): pointer {
		return adflib.new_precision_info(
			pInfo.soilDensity,
			pInfo.pressure,
			pInfo.lightExposure,
			pInfo.waterUse,
			pInfo.soilTemp,
			pInfo.envTemp,
			pInfo.additiveConc
		);
	}

	static fromCPrecisionInfo(cPrecisionInfo: pointer, view: DataView): PrecisionInfo {
		return {
			soilDensity: view.getFloat32(cPrecisionInfo, littleEndian),
			pressure: view.getFloat32(cPrecisionInfo + 4, littleEndian),
			lightExposure: view.getFloat32(cPrecisionInfo + 8, littleEndian),
			waterUse: view.getFloat32(cPrecisionInfo + 12, littleEndian),
			soilTemp: view.getFloat32(cPrecisionInfo + 16, littleEndian),
			envTemp: view.getFloat32(cPrecisionInfo + 20, littleEndian),
			additiveConc: view.getFloat32(cPrecisionInfo + 24, littleEndian),
		};
	}

	static toCHeader(header: Header): pointer {
		return adflib.new_header(
			header.farmingTec,
			AdflibConverter.toCWaveInfo(header.wInfo),
			AdflibConverter.toCSoilDepthInfo(header.sInfo),
			AdflibConverter.toCReductionInfo(header.redInfo),
			AdflibConverter.toCPrecisionInfo(header.precInfo),
			header.nChunks
		);
	}

	static fromCHeader(cHeader: pointer, view: DataView): Header {
		const farmingTec = view.getUint32(cHeader, littleEndian);
		const wInfo = AdflibConverter.fromCWaveInfo(cHeader + 4, view);
		const sInfo = AdflibConverter.fromCSoilInfo(cHeader + 10, view);
		const redInfo = AdflibConverter.fromCReductionInfo(cHeader + 16, view);
		const precInfo = AdflibConverter.fromCPrecisionInfo(cHeader + 44, view);
		const nChunks = view.getUint32(cHeader + 72, littleEndian);
		return { farmingTec, wInfo, sInfo, redInfo, precInfo, nChunks };
	}

	static fromCMetadata(cMetadata: pointer, view: DataView): Metadata {
		const sizeSeries = view.getUint32(cMetadata, littleEndian);
		const nSeries = view.getBigUint64(cMetadata + 4, littleEndian);
		const periodSec = view.getUint32(cMetadata + 12, littleEndian);
		const seeded = view.getBigUint64(cMetadata + 16, littleEndian);
		const harvested = view.getBigUint64(cMetadata + 24, littleEndian);
		const nAdditives = view.getUint16(cMetadata + 32, littleEndian);
		const additivePtr = view.getUint32(cMetadata + 34, littleEndian);
		const additiveCodes = [];
		for (let i = 0; i < nAdditives; i++) {
			additiveCodes.push(view.getUint32(additivePtr + (i * 4), littleEndian));
		}
		return { sizeSeries, nSeries, periodSec, seeded, harvested, nAdditives, additiveCodes };
	}
}

export const StatusCode = Object.freeze({
	OK: adflib.get_status_code_OK(),
	HEADER_CORRUPTED: adflib.get_status_code_HEADER_CORRUPTED(),
	METADATA_CORRUPTED: adflib.get_status_code_METADATA_CORRUPTED(),
	SERIES_CORRUPTED: adflib.get_status_code_SERIES_CORRUPTED(),
	ZERO_REPEATED_SERIES: adflib.get_status_code_ZERO_REPEATED_SERIES(),
	EMPTY_SERIES: adflib.get_status_code_EMPTY_SERIES(),
	TIME_OUT_OF_BOUND: adflib.get_status_code_TIME_OUT_OF_BOUND(),
	ADDITIVE_OVERFLOW: adflib.get_status_code_ADDITIVE_OVERFLOW(),
	NULL_HEADER_SOURCE: adflib.get_status_code_NULL_HEADER_SOURCE(),
	NULL_HEADER_TARGET: adflib.get_status_code_NULL_HEADER_TARGET(),
	NULL_META_SOURCE: adflib.get_status_code_NULL_META_SOURCE(),
	NULL_META_TARGET: adflib.get_status_code_NULL_META_TARGET(),
	NULL_SERIES_SOURCE: adflib.get_status_code_NULL_SERIES_SOURCE(),
	NULL_SERIES_TARGET: adflib.get_status_code_NULL_SERIES_TARGET(),
	NULL_SOURCE: adflib.get_status_code_NULL_SOURCE(),
	NULL_TARGET: adflib.get_status_code_NULL_TARGET(),
	NULL_ADDITIVE_SOURCE: adflib.get_status_code_NULL_ADDITIVE_SOURCE(),
	NULL_ADDITIVE_TARGET: adflib.get_status_code_NULL_ADDITIVE_TARGET(),
	RUNTIME_ERROR: adflib.get_status_code_RUNTIME_ERROR(),
});

export const FarmingTechnique = Object.freeze({
	REGULAR: adflib.get_farming_tec_code_REGULAR(),
	INDOOR: adflib.get_farming_tec_code_INDOOR(),
	INDOOR_PROTECTED: adflib.get_farming_tec_code_INDOOR_PROTECTED(),
	OUTDOOR: adflib.get_farming_tec_code_OUTDOOR(),
	ARTIFICIAL_SOIL: adflib.get_farming_tec_code_ARTIFICIAL_SOIL(),
	HYDROPONICS: adflib.get_farming_tec_code_HYDROPONICS(),
	ANTHROPONICS: adflib.get_farming_tec_code_ANTHROPONICS(),
	AEROPONICS: adflib.get_farming_tec_code_AEROPONICS(),
	FOGPONICS: adflib.get_farming_tec_code_FOGPONICS(),
});

export const ReductionCode = Object.freeze({
	NONE: adflib.get_reduction_code_NONE(),
	AVG: adflib.get_reduction_code_AVG(),
	MAVG: adflib.get_reduction_code_MAVG(),
});

export const SeriesTime = Object.freeze({
	DAY: adflib.get_ADF_DAY(),
	WEEK: adflib.get_ADF_WEEK(),
	MONTH_28: adflib.get_ADF_MONTH_28(),
	MONTH_29: adflib.get_ADF_MONTH_29(),
	MONTH_30: adflib.get_ADF_MONTH_30(),
	MONTH_31: adflib.get_ADF_MONTH_31(),
});

export interface MatrixShape {
	rows: number;
	columns: number;
}

export class Matrix {

	private mat: number[];
	private rows: number;
	private columns: number;

	constructor(rows: number = 1, columns: number = 1) {
		this.mat = new Array(rows * columns).fill(0);
		this.rows = rows;
		this.columns = columns;
	}

	addRow(row: number[]): void {
		this.mat.push(...row);
		this.rows += 1;
	}

	setInnerArray(arr: number[]): void {
		if (arr.length != (this.rows * this.columns))
			throw new Error();
		this.mat = arr;
	}

	shape(): MatrixShape {
		return { rows: this.rows, columns: this.columns };
	}

	innerArray(): number[] {
		return this.mat;
	}

	at(row: number, column: number): number {
		if (row >= this.rows || column >= this.columns) { throw new Error("Index out of bounds"); }
		return this.mat[column + row * this.columns];
	}
}

export interface Additive {
	code: number;
	concentration: number;
}

export interface Series {
	lightExposure: Matrix;
	soilTempC: Matrix;
	envTempC: Matrix;
	waterUseMl: Matrix;
	pH: number;
	pBar: number;
	soilDensityKgM3: number;
	soilAdditives: Additive[];
	atmAdditives: Additive[];
	repeated: number;
}

export interface WaveInfo {
	minWavelenNm: number;
	maxWavelenNm: number;
	nWavelengths: number;
}

export interface SoilDepthInfo {
	transY: number;
	maxSoilDepthMm: number;
	nDepth: number;
}

export interface ReductionInfo {
	soilDensity: number;
	pressure: number;
	lightExposure: number;
	waterUse: number;
	soilTemp: number;
	envTemp: number;
	additiveConc: number;
}

export interface PrecisionInfo {
	soilDensity: number;
	pressure: number;
	lightExposure: number;
	waterUse: number;
	soilTemp: number;
	envTemp: number;
	additiveConc: number;
}

export interface Header {
	farmingTec: number;
	wInfo: WaveInfo;
	sInfo: SoilDepthInfo;
	redInfo: ReductionInfo;
	precInfo: PrecisionInfo;
	nChunks: number;
}

export interface Metadata {
	sizeSeries: number;
	nSeries: bigint;
	periodSec: number;
	seeded: bigint;
	harvested: bigint;
	nAdditives: number;
	additiveCodes: number[];
}

export class Adf {
	private cAdf: pointer = nullptr;
	private header: Header | undefined;
	private metadata: Metadata | undefined;
	private series: Series[] | undefined;

	private constructor() { }

	static new(header: Header, periodSec: number): Adf {
		const adf = new Adf();
		adf.cAdf = adflib.new_adf(AdflibConverter.toCHeader(header), periodSec);
		adf.header = header;
		return adf;
	}

	static unmarshal(bytes: Buffer<ArrayBufferLike>): Adf {
		const cAdf = adflib.new_empty_adf();
		const ptr = adflib.malloc(bytes.byteLength);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		(new Uint8Array(bytes)).forEach(byte => {
			view.setUint8(offset, byte);
			offset++;
		})
		const res = adflib.unmarshal(cAdf, ptr);
		adflib.free(ptr);
		if (res !== StatusCode.OK)
			throw new Error(`Cannot unmarshal. Error code: ${res}`);
		const adf = new Adf();
		adf.cAdf = cAdf;
		return adf;
	}

	sizeBytes(): number {
		return adflib.size_adf_t(this.cAdf);
	}

	marshal(): Uint8Array<ArrayBuffer> {
		const ptr = adflib.malloc(this.sizeBytes());
		const res = adflib.marshal(ptr, this.cAdf);
		if (res !== StatusCode.OK)
			throw new Error(`Cannot marshal. Error code: ${res}`);
		const bytes = new Uint8Array(this.sizeBytes());
		bytes.set(new Uint8Array(memory.buffer, ptr, this.sizeBytes()));
		adflib.free(ptr);
		return bytes;
	}

	addSeries(series: Series): void {
		const seriesPtr = AdflibConverter.toCSeries(series);
		const res = adflib.add_series(this.cAdf, seriesPtr);
		if (res !== StatusCode.OK)
			throw new Error(`Error adding series. Error code: ${res}`);
		adflib.series_delete(seriesPtr);
	}

	removeSeries(): void {
		const res = adflib.remove_series(this.cAdf);
		if (res !== StatusCode.OK)
			throw new Error(`Error removing series. Error code: ${res}`);
	}

	updateSeries(series: Series, time: number): void {
		const seriesPtr = AdflibConverter.toCSeries(series);
		const res = adflib.update_series(this.cAdf, seriesPtr, time);
		if (res !== StatusCode.OK)
			throw new Error(`Error updating series. Error code: ${res}`);
		adflib.series_delete(seriesPtr);
	}

	getVersion(): string {
		const version = adflib.get_hex_version();
		return `${(version & 0xFF00) >> 8}.${(version & 0x00F0) >> 4}.${version & 0x000F}`
	}

	getHeader(): Header {
		if (this.header) return this.header;
		const cHeader = adflib.get_header(this.cAdf);
		const view = new DataView(memory.buffer);
		this.header = AdflibConverter.fromCHeader(cHeader + 6, view);
		return this.header;
	}

	getMetadata(): Metadata {
		if (this.metadata) return this.metadata;
		const cMetadata = adflib.get_metadata(this.cAdf);
		const view = new DataView(memory.buffer);
		this.metadata = AdflibConverter.fromCMetadata(cMetadata, view);
		return this.metadata;
	}

	getSeries(): Series[] {
		if (this.series) return this.series;
		this.series = [];
		const view = new DataView(memory.buffer);
		const nSeries = this.getMetadata().sizeSeries;
		const cSeries = adflib.get_series_list(this.cAdf);
		for (let i = 0; i < nSeries; i++) {
			this.series.push(AdflibConverter.fromCSeries(cSeries + (i * 41), this.getHeader(), view));
		}
		return this.series;
	}

	setSeedTime(seedTime: bigint): void {
		const res = adflib.set_seed_time(this.cAdf, seedTime);
		if (res !== StatusCode.OK) {
			throw new Error(`Error setting the seed time: ${res}`);
		}
	}

	setHarvestTime(harvestTime: bigint): void {
		const res = adflib.set_harvest_time(this.cAdf, harvestTime);
		if (res !== StatusCode.OK) {
			throw new Error(`Error setting the seed time: ${res}`);
		}
	}

	dispose(): void {
		adflib.adf_delete(this.cAdf);
		this.cAdf = nullptr;
	}
}

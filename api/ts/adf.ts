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
	new_reduction_info: exports.new_reduction_info as (soilDensity: number, pressure: number, lightExposure: number, waterUse: number, soilTemp: number, envTemp: number) => pointer,
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
});

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

const DatatypeSize = Object.freeze({
	UINT_BIG_T: adflib.get_UINT_BIG_T_SIZE(),
	UINT_T: adflib.get_UINT_T_SIZE(),
	UINT_SMALL_T: adflib.get_UINT_SMALL_T_SIZE(),
	UINT_TINY_T: adflib.get_UINT_TINY_T_SIZE(),
	REAL_T: adflib.get_REAL_T_SIZE(),
	ADDITIVE_T: adflib.get_ADD_T_SIZE(),
});

export class Additive {

	private code: number;
	private concentration: number;

	constructor(code: number, concentration: number) {
		this.code = code;
		this.concentration = concentration;
	}

	toCAdditive(): pointer {
		return adflib.new_additive(this.code, this.concentration);
	}

	getCode(): number { return this.code; }

	getConcentration(): number { return this.concentration; }
}

export class AdditiveList {

	private adds: Additive[];

	constructor(adds: Additive[]) {
		this.adds = adds;
	}

	size(): number { return this.adds.length; }

	toCStruct(): pointer {
		const byteSize = this.adds.length * DatatypeSize.ADDITIVE_T;
		const ptr = adflib.malloc(byteSize);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		this.adds.forEach(additive => {
			view.setUint32(offset, additive.getCode(), true);
			offset += 4;
			view.setFloat32(offset, additive.getConcentration(), true);
			offset += 4;
		});
		return ptr;
	}
}

export class Matrix<T extends number | bigint> {

	private mat: T[];
	private rows: number;
	private columns: number;
	private datatypeSize: number;

	constructor(rows: number = 1, columns: number = 1, datatype: number = DatatypeSize.REAL_T) {
		this.mat = new Array(rows * columns).fill(0);
		this.rows = rows;
		this.columns = columns;
		this.datatypeSize = datatype;
	}

	addRow(row: T[]): void {
		this.mat.push(...row);
		this.rows += 1;
	}

	shape(): Object {
		return { rows: this.rows, columns: this.columns };
	}

	at(row: number, column: number): T {
		if (row < this.rows && column < this.columns) {
			return this.mat[column + row * this.columns];
		} else {
			throw new Error("Index out of bounds");
		}
	}

	toCArray(): pointer {
		const byteSize = this.mat.length * this.datatypeSize;
		const ptr = adflib.malloc(byteSize);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		const getUpdateFn = (): ((byteOffset: number, value: any, littleEndian?: boolean) => void) => {
			switch (this.datatypeSize) {
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
					throw new Error(`<Matrix class> Invalid datatype: ${this.datatypeSize}`)
			}
		}
		const updateFn = getUpdateFn();
		this.mat.forEach(e => {
			updateFn(offset, e, true);
			offset += this.datatypeSize;
		});
		return ptr;
	}
}

export class Series {

	private lightExposure: Matrix<number>;
	private soilTempC: Matrix<number>;
	private envTempC: Matrix<number>;
	private waterUseMl: Matrix<number>;
	private pH: number;
	private pBar: number;
	private soilDensityKgM3: number;
	private soilAdditives: AdditiveList;
	private atmAdditives: AdditiveList;
	private repeated: number;

	constructor(
		lightExposure: Matrix<number>,
		soilTempC: Matrix<number>,
		envTempC: Matrix<number>,
		waterUseMl: Matrix<number>,
		pH: number,
		pBar: number,
		soilDensityKgM3: number,
		soilAdditives: AdditiveList,
		atmAdditives: AdditiveList,
		repeated: number
	) {
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

	toCSeries(): pointer {
		return adflib.new_series(
			this.lightExposure.toCArray(),
			this.soilTempC.toCArray(),
			this.envTempC.toCArray(),
			this.waterUseMl.toCArray(),
			this.pH,
			this.pBar,
			this.soilDensityKgM3,
			this.soilAdditives.size(),
			this.atmAdditives.size(),
			this.soilAdditives.toCStruct(),
			this.atmAdditives.toCStruct(),
			this.repeated
		);
	}
}

export class WaveInfo {

	private minWavelenNm: number;
	private maxWavelenNm: number;
	private nWavelengths: number;

	constructor(minWavelenNm: number, maxWavelenNm: number, nWavelengths: number) {
		this.minWavelenNm = minWavelenNm;
		this.maxWavelenNm = maxWavelenNm;
		this.nWavelengths = nWavelengths;
	}

	toCWaveInfo(): pointer {
		return adflib.new_wavelength_info(this.minWavelenNm, this.maxWavelenNm, this.nWavelengths);
	}
}

export class SoilDepthInfo {

	private transY: number;
	private maxSoilDepthMm: number;
	private nDepth: number;

	constructor(transY: number, maxSoilDepthMm: number, nDepth: number) {
		this.transY = transY;
		this.maxSoilDepthMm = maxSoilDepthMm;
		this.nDepth = nDepth;
	}

	toCSoilDepthInfo(): pointer {
		if (this.transY === 0) {
			return adflib.new_soil_depth_info(this.maxSoilDepthMm, this.nDepth);
		}
		return adflib.new_trans_soil_depth_info(this.transY, this.maxSoilDepthMm, this.nDepth);
	}
}

export class ReductionInfo {

	private soilDensity: number;
	private pressure: number;
	private lightExposure: number;
	private waterUse: number;
	private soilTemp: number;
	private envTemp: number;

	constructor(
		soilDensity: number,
		pressure: number,
		lightExposure: number,
		waterUse: number,
		soilTemp: number,
		envTemp: number
	) {
		this.soilDensity = soilDensity;
		this.pressure = pressure;
		this.lightExposure = lightExposure;
		this.waterUse = waterUse;
		this.soilTemp = soilTemp;
		this.envTemp = envTemp;
	}

	toCReductionInfo(): pointer {
		return adflib.new_reduction_info(
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

	private soilDensity: number;
	private pressure: number;
	private lightExposure: number;
	private waterUse: number;
	private soilTemp: number;
	private envTemp: number;
	private additiveConc: number;

	constructor(
		soilDensity: number,
		pressure: number,
		lightExposure: number,
		waterUse: number,
		soilTemp: number,
		envTemp: number,
		additiveConc: number
	) {
		this.soilDensity = soilDensity;
		this.pressure = pressure;
		this.lightExposure = lightExposure;
		this.waterUse = waterUse;
		this.soilTemp = soilTemp;
		this.envTemp = envTemp;
		this.additiveConc = additiveConc;
	}

	toCPrecisionInfo(): pointer {
		return adflib.new_precision_info(
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

	private farmingTec: number;
	private wInfo: WaveInfo;
	private sInfo: SoilDepthInfo;
	private redInfo: ReductionInfo;
	private precInfo: PrecisionInfo;
	private nChunks: number;

	constructor(
		farmingTec: number,
		wInfo: WaveInfo,
		sInfo: SoilDepthInfo,
		redInfo: ReductionInfo,
		precInfo: PrecisionInfo,
		nChunks: number
	) {
		this.farmingTec = farmingTec;
		this.wInfo = wInfo;
		this.sInfo = sInfo;
		this.redInfo = redInfo;
		this.precInfo = precInfo;
		this.nChunks = nChunks;
	}

	toCHeader(): pointer {
		return adflib.new_header(
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
	private cAdf: number;

	constructor(header?: Header, periodSec?: number) {
		if (header && periodSec)
			this.cAdf = adflib.new_adf(header.toCHeader(), periodSec);
		else
			this.cAdf = nullptr;
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
		const seriesPtr = series.toCSeries();
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
		const seriesPtr = series.toCSeries();
		const res = adflib.update_series(this.cAdf, seriesPtr, time);
		if (res !== StatusCode.OK)
			throw new Error(`Error updating series. Error code: ${res}`);
		adflib.series_delete(seriesPtr);
	}

	getVersion(): string {
		const version = adflib.get_hex_version();
		return `${(version & 0xFF00) >> 8}.${(version & 0x00F0) >> 4}.${version & 0x000F}`
	}

	dispose(): void {
		adflib.adf_delete(this.cAdf);
		this.cAdf = nullptr;
	}
}

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
	get_header: exports.get_header as (adf: pointer) => pointer,
	get_metadata: exports.get_metadata as (adf: pointer) => pointer,
	get_series_list: exports.get_series_list as (adf: pointer) => pointer,
	get_w_info: exports.get_w_info as (header: pointer) => pointer,
	get_soil_info: exports.get_soil_info as (header: pointer) => pointer,
	get_red_info: exports.get_red_info as (header: pointer) => pointer,
	get_prec_info: exports.get_prec_info as (header: pointer) => pointer,
});

const DatatypeSize = Object.freeze({
	UINT_BIG_T: adflib.get_UINT_BIG_T_SIZE(),
	UINT_T: adflib.get_UINT_T_SIZE(),
	UINT_SMALL_T: adflib.get_UINT_SMALL_T_SIZE(),
	UINT_TINY_T: adflib.get_UINT_TINY_T_SIZE(),
	REAL_T: adflib.get_REAL_T_SIZE(),
	ADDITIVE_T: adflib.get_ADD_T_SIZE(),
});

class AdflibConverter {

	static toCAdditive(additive: Additive): pointer {
		return adflib.new_additive(additive.code, additive.concentration);
	}

	static toCAdditiveList(adds: Additive[]): pointer {
		const byteSize = adds.length * DatatypeSize.ADDITIVE_T;
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

	static toCArray<T extends number | bigint>(matrix: Matrix<T>): pointer {
		const datatypeSize = matrix.getDatatypeSize();
		const byteSize = matrix.innerArray().length * datatypeSize;
		const ptr = adflib.malloc(byteSize);
		const view = new DataView(memory.buffer);
		let offset = ptr;
		const getUpdateFn = (): ((byteOffset: number, value: any, littleEndian?: boolean) => void) => {
			switch (datatypeSize) {
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
					throw new Error(`<Matrix class> Invalid datatype: ${datatypeSize}`)
			}
		}
		const updateFn = getUpdateFn();
		matrix.innerArray().forEach(e => {
			updateFn(offset, e, littleEndian);
			offset += datatypeSize;
		});
		return ptr;
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

	static fromCSoilInfo(cSoilInfo: pointer): SoilDepthInfo {
		const view = new DataView(memory.buffer);
		return {
			transY: view.getUint16(cSoilInfo, littleEndian),
			maxSoilDepthMm: view.getUint16(cSoilInfo + 2, littleEndian),
			nDepth: view.getUint16(cSoilInfo + 4, littleEndian)
		};
	}

	static toCReductionInfo(rInfo: ReductionInfo): pointer {
		return adflib.new_reduction_info(rInfo.soilDensity, rInfo.pressure, rInfo.lightExposure,
			rInfo.waterUse, rInfo.soilTemp, rInfo.envTemp);
	}

	static fromCReductionInfo(cReductionInfo: pointer): ReductionInfo {
		const view = new DataView(memory.buffer);
		return {
			soilDensity: view.getUint8(cReductionInfo),
			pressure: view.getUint8(cReductionInfo + 1),
			lightExposure: view.getUint8(cReductionInfo + 2),
			waterUse: view.getUint8(cReductionInfo + 3),
			soilTemp: view.getUint8(cReductionInfo + 4),
			envTemp: view.getUint8(cReductionInfo + 5)
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

	static fromCPrecisionInfo(cPrecisionInfo: pointer): PrecisionInfo {
		const view = new DataView(memory.buffer);
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
		const farmingTec = view.getUint8(cHeader + 6);
		cHeader++;
		const wInfoPtr = adflib.get_w_info(cHeader);
		const wInfo = AdflibConverter.fromCWaveInfo(wInfoPtr, view);
		cHeader += 6;
		const sInfo = AdflibConverter.fromCSoilInfo(cHeader);
		cHeader += 6;
		const redInfo = AdflibConverter.fromCReductionInfo(cHeader);
		cHeader += 7;
		const precInfo = AdflibConverter.fromCPrecisionInfo(cHeader);
		cHeader += 28;
		const nChunks = view.getUint32(cHeader, littleEndian);
		return { farmingTec, wInfo, sInfo, redInfo, precInfo, nChunks };
	}

	static fromCMetadata(cMetadata: pointer): Metadata {
		const view = new DataView(memory.buffer);
		const sizeSeries = view.getUint32(cMetadata, littleEndian);
		const nSeries = view.getBigUint64(cMetadata + 4, littleEndian);
		const periodSec = view.getUint32(cMetadata + 12, littleEndian);
		const seeded = view.getBigUint64(cMetadata + 16, littleEndian);
		const harvested = view.getBigUint64(cMetadata + 24, littleEndian);
		const nAdditives = view.getUint16(cMetadata + 32, littleEndian);
		const additiveCodes = [];
		for (let i = 0; i < nAdditives; i++) {
			additiveCodes.push(view.getUint32(cMetadata + (i * 4), littleEndian));
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

export interface Additive {
	code: number;
	concentration: number;
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

	innerArray(): T[] {
		return this.mat;
	}

	getDatatypeSize(): number {
		return this.datatypeSize;
	}

	at(row: number, column: number): T {
		if (row < this.rows && column < this.columns) {
			return this.mat[column + row * this.columns];
		} else {
			throw new Error("Index out of bounds");
		}
	}
}

export interface Series {
	lightExposure: Matrix<number>;
	soilTempC: Matrix<number>;
	envTempC: Matrix<number>;
	waterUseMl: Matrix<number>;
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
	private cAdf: number;

	constructor(header?: Header, periodSec?: number) {
		if (header && periodSec)
			this.cAdf = adflib.new_adf(AdflibConverter.toCHeader(header), periodSec);
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
		const seriesPtr =  AdflibConverter.toCSeries(series);
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
		const cHeader = adflib.get_header(this.cAdf);
		const view = new DataView(memory.buffer);
		console.log(view.getUint32(cHeader, littleEndian));
		console.log(view.getUint16(cHeader+4, littleEndian));
		console.log(view.getUint16(cHeader+6, littleEndian));
		console.log(view.getUint16(cHeader+10, littleEndian));
		console.log(memory.buffer.slice(cHeader, cHeader+20));
		return AdflibConverter.fromCHeader(cHeader, view);
	}

	getMetadata(): Metadata {
		const cMetadata = adflib.get_metadata(this.cAdf);
		return AdflibConverter.fromCMetadata(cMetadata);
	}

	dispose(): void {
		adflib.adf_delete(this.cAdf);
		this.cAdf = nullptr;
	}
}

const adfBuffer = await readFile('./output.adf');
const a = Adf.unmarshal(adfBuffer);
console.log(a.getHeader());

/*
 * adf.go - Golang interface for ADF library
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

package adflib

/*
#cgo CFLAGS: -std=c2x
#cgo CFLAGS: -I/usr/local/include
#cgo LDFLAGS: -L/usr/local/lib -ladf
#include <adf.h>
*/
import "C"
import (
	"fmt"
	"runtime"
)

var seriesPinner runtime.Pinner = runtime.Pinner{}

const (
	DAY_TIME_SEC     uint32 = C.ADF_DAY
	WEEK_TIME_SEC    uint32 = C.ADF_WEEK
	MONTH28_TIME_SEC uint32 = C.ADF_MONTH_28
	MONTH29_TIME_SEC uint32 = C.ADF_MONTH_29
	MONTH30_TIME_SEC uint32 = C.ADF_MONTH_30
	MONTH31_TIME_SEC uint32 = C.ADF_MONTH_31
)

type ReductionCode int8

const (
	NONE           ReductionCode = C.ADF_RM_NONE
	AVERAGE        ReductionCode = C.ADF_RM_AVG
	MOVING_AVERAGE ReductionCode = C.ADF_RM_MAVG
)

type FarmingTechnique uint8

const (
	REGULAR          FarmingTechnique = C.ADF_FT_REGULAR
	INDOOR           FarmingTechnique = C.ADF_FT_INDOOR
	INDOOR_PROTECTED FarmingTechnique = C.ADF_FT_INDOOR_PROTECTED
	OUTDOOR          FarmingTechnique = C.ADF_FT_OUTDOOR
	ARTIFICIAL_SOIL  FarmingTechnique = C.ADF_FT_ARTIFICIAL_SOIL
	HYDROPONICS      FarmingTechnique = C.ADF_FT_HYDROPONICS
	ANTHROPONICS     FarmingTechnique = C.ADF_FT_ANTHROPONICS
	AEROPONICS       FarmingTechnique = C.ADF_FT_AEROPONICS
	FOGPONICS        FarmingTechnique = C.ADF_FT_FOGPONICS
)

type ErrorCode uint16

const (
	OK                   ErrorCode = C.ADF_OK
	HEADER_CORRUPTED     ErrorCode = C.ADF_HEADER_CORRUPTED
	METADATA_CORRUPTED   ErrorCode = C.ADF_METADATA_CORRUPTED
	SERIES_CORRUPTED     ErrorCode = C.ADF_SERIES_CORRUPTED
	ZERO_REPEATED_SERIES ErrorCode = C.ADF_ZERO_REPEATED_SERIES
	EMPTY_SERIES         ErrorCode = C.ADF_EMPTY_SERIES
	TIME_OUT_OF_BOUND    ErrorCode = C.ADF_TIME_OUT_OF_BOUND
	ADDITIVE_OVERFLOW    ErrorCode = C.ADF_ADDITIVE_OVERFLOW
	NULL_HEADER_SOURCE   ErrorCode = C.ADF_NULL_HEADER_SOURCE
	NULL_HEADER_TARGET   ErrorCode = C.ADF_NULL_HEADER_TARGET
	NULL_META_SOURCE     ErrorCode = C.ADF_NULL_META_SOURCE
	NULL_META_TARGET     ErrorCode = C.ADF_NULL_META_TARGET
	NULL_SERIES_SOURCE   ErrorCode = C.ADF_NULL_SERIES_SOURCE
	NULL_SERIES_TARGET   ErrorCode = C.ADF_NULL_SERIES_TARGET
	NULL_SOURCE          ErrorCode = C.ADF_NULL_SOURCE
	NULL_TARGET          ErrorCode = C.ADF_NULL_TARGET
	NULL_ADDITIVE_SOURCE ErrorCode = C.ADF_NULL_ADDITIVE_SOURCE
	NULL_ADDITIVE_TARGET ErrorCode = C.ADF_NULL_ADDITIVE_TARGET
	RUNTIME_ERROR        ErrorCode = C.ADF_RUNTIME_ERROR
)

type Additive struct {
	code          uint32
	concentration float32
}

func NewAdditive(code uint32, concentration float32) Additive {
	return Additive{
		code:          code,
		concentration: concentration,
	}
}

func (add *Additive) toCAdditive() C.additive_t {
	return C.create_additive(C.uint32_t(add.code), C.float(add.concentration))
}

type AdditiveList struct {
	adds []Additive
}

func NewAdditiveList(adds []Additive) AdditiveList {
	return AdditiveList{
		adds: adds,
	}
}

func (al *AdditiveList) Size() int {
	return len(al.adds)
}

func (al *AdditiveList) toCList() []C.additive_t {
	additives := make([]C.additive_t, len(al.adds))
	for i, e := range al.adds {
		additives[i] = e.toCAdditive()
	}
	return additives
}

type Series struct {
	lightExposure   Matrix[float32]
	soilTempC       Matrix[float32]
	envTempC        []float32
	waterUseMl      []float32
	pH              uint8
	pBar            float32
	soilDensityKgM3 float32
	soilAdditives   AdditiveList
	atmAdditives    AdditiveList
	repeated        uint32
}

func NewSeries(lightExposure Matrix[float32], soilTempC Matrix[float32],
	envTempC []float32, waterUseMl []float32, pH uint8, pBar float32,
	soilDensityKgM3 float32, soilAdditives AdditiveList, atmAdditives AdditiveList,
	repeated uint32) Series {
	s := Series{
		lightExposure:   lightExposure,
		soilTempC:       soilTempC,
		envTempC:        envTempC,
		waterUseMl:      waterUseMl,
		pH:              pH,
		pBar:            pBar,
		soilDensityKgM3: soilDensityKgM3,
		soilAdditives:   soilAdditives,
		atmAdditives:    atmAdditives,
		repeated:        repeated,
	}
	seriesPinner.Pin(&s.lightExposure.mat[0])
	seriesPinner.Pin(&s.soilTempC.mat[0])
	seriesPinner.Pin(&s.envTempC[0])
	seriesPinner.Pin(&s.waterUseMl[0])
	seriesPinner.Pin(&s.soilAdditives.toCList()[0])
	seriesPinner.Pin(&s.atmAdditives.toCList()[0])
	return s
}

func (s *Series) DisposeSeries() {
	seriesPinner.Unpin()
}

func (s *Series) toCSeries() C.series_t {
	return C.create_series(
		(*C.float)(&s.lightExposure.mat[0]),
		(*C.float)(&s.soilTempC.mat[0]),
		(*C.float)(&s.envTempC[0]),
		(*C.float)(&s.waterUseMl[0]),
		C.uint8_t(s.pH),
		C.float(s.pBar),
		C.float(s.soilDensityKgM3),
		C.uint16_t(s.soilAdditives.Size()),
		C.uint16_t(s.atmAdditives.Size()),
		(*C.additive_t)(&s.soilAdditives.toCList()[0]),
		(*C.additive_t)(&s.atmAdditives.toCList()[0]),
		C.uint32_t(s.repeated))
}

type WaveInfo struct {
	minWavelenNm uint16
	maxWavelenNm uint16
	nWavelengths uint16
}

func NewWaveInfo(minWavelenNm uint16, maxWavelenNm uint16, nWavelengths uint16) WaveInfo {
	return WaveInfo{
		minWavelenNm: minWavelenNm,
		maxWavelenNm: maxWavelenNm,
		nWavelengths: nWavelengths,
	}
}

func (w *WaveInfo) toCWaveInfo() C.wavelength_info_t {
	return C.create_wavelength_info(
		C.uint16_t(w.minWavelenNm),
		C.uint16_t(w.maxWavelenNm),
		C.uint16_t(w.nWavelengths),
	)
}

type SoilDepthInfo struct {
	minSoilDepthMm uint16
	maxSoilDepthMm uint16
	nDepth         uint16
}

func NewSoilDepthInfo(minSoilDepthMm uint16, maxSoilDepthMm uint16, nDepth uint16) SoilDepthInfo {
	return SoilDepthInfo{
		minSoilDepthMm: minSoilDepthMm,
		maxSoilDepthMm: maxSoilDepthMm,
		nDepth:         nDepth,
	}
}

func (s *SoilDepthInfo) toSoilDepthInfo() C.soil_depth_info_t {
	return C.create_soil_depth_info(
		C.uint16_t(s.minSoilDepthMm),
		C.uint16_t(s.maxSoilDepthMm),
		C.uint16_t(s.nDepth),
	)
}

type ReductionInfo struct {
	soilDensity   ReductionCode
	pressure      ReductionCode
	lightExposure ReductionCode
	waterUse      ReductionCode
	soilTemp      ReductionCode
	envTemp       ReductionCode
}

func NewReductionInfo(soilDensity ReductionCode, pressure ReductionCode,
	lightExposure ReductionCode, waterUse ReductionCode, soilTemp ReductionCode,
	envTemp ReductionCode) ReductionInfo {
	return ReductionInfo{
		soilDensity:   soilDensity,
		pressure:      pressure,
		lightExposure: lightExposure,
		waterUse:      waterUse,
		soilTemp:      soilTemp,
		envTemp:       envTemp,
	}
}

func (r *ReductionInfo) toCReductionInfo() C.reduction_info_t {
	return C.create_reduction_info(
		C.uint8_t(r.soilDensity),
		C.uint8_t(r.pressure),
		C.uint8_t(r.lightExposure),
		C.uint8_t(r.waterUse),
		C.uint8_t(r.soilTemp),
		C.uint8_t(r.envTemp),
	)
}

type Header struct {
	version    uint16
	farmingTec FarmingTechnique
	wInfo      WaveInfo
	sInfo      SoilDepthInfo
	redInfo    ReductionInfo
	nChunks    uint32
}

func NewHeader(farmingTec FarmingTechnique, wInfo WaveInfo, sInfo SoilDepthInfo,
	redInfo ReductionInfo, nChunks uint32) Header {
	return Header{
		version:    uint16(C.__ADF_VERSION__),
		farmingTec: farmingTec,
		wInfo:      wInfo,
		sInfo:      sInfo,
		redInfo:    redInfo,
		nChunks:    nChunks,
	}
}

func (h Header) toCHeader() C.adf_header_t {
	return C.create_header(
		C.uint8_t(h.farmingTec),
		h.wInfo.toCWaveInfo(),
		h.sInfo.toSoilDepthInfo(),
		h.redInfo.toCReductionInfo(),
		C.uint32_t(h.nChunks),
	)
}

type Adf struct {
	cAdf C.adf_t
}

func NewAdf(header Header, periodSec uint32) Adf {
	var adf Adf
	C.adf_init(&adf.cAdf, header.toCHeader(), C.uint32_t(periodSec))
	return adf
}

func Unmarshal(bytes []byte) (Adf, *AdfError) {
	var adf Adf
	res := C.unmarshal(&adf.cAdf, (*C.uint8_t)(&bytes[0]))
	if res != C.ADF_OK {
		return Adf{}, getAdfError(uint16(res))
	}
	return adf, nil
}

func (adf *Adf) SizeBytes() uint32 {
	return uint32(C.size_adf_t(&adf.cAdf))
}

func (adf *Adf) Marshal() ([]byte, *AdfError) {
	cBytes := make([]byte, adf.SizeBytes())
	res := uint16(C.marshal((*C.uint8_t)(&cBytes[0]), &adf.cAdf))
	if res != C.ADF_OK {
		return nil, getAdfError(uint16(res))
	}
	return cBytes, nil
}

func (adf *Adf) AddSeries(series Series) *AdfError {
	cSeries := series.toCSeries()
	res := C.add_series(&adf.cAdf, &cSeries)
	if res != C.ADF_OK {
		return getAdfError(uint16(res))
	}
	return nil
}

func (adf *Adf) UpdateSeries(series Series, time uint64) *AdfError {
	cSeries := series.toCSeries()
	res := C.update_series(&adf.cAdf, &cSeries, C.uint64_t(time))
	if res != C.ADF_OK {
		return getAdfError(uint16(res))
	}
	return nil
}

func (adf *Adf) RemoveSeries() *AdfError {
	res := C.remove_series(&adf.cAdf)
	if res != C.ADF_OK {
		return getAdfError(uint16(res))
	}
	return nil
}

func (adf *Adf) GetVersion() string {
	version := C.get_adf_version()
	return fmt.Sprintf("%d.%d.%d", version.major, version.minor, version.patch)
}

func (adf *Adf) Dispose() {
	C.adf_free(&adf.cAdf)
}

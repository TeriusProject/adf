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
	Code          uint32
	Concentration float32
}

func NewAdditive(code uint32, concentration float32) Additive {
	return Additive{
		Code:          code,
		Concentration: concentration,
	}
}

func (add *Additive) toCAdditive() C.additive_t {
	return C.create_additive(C.uint32_t(add.Code), C.float(add.Concentration))
}

type AdditiveList struct {
	Adds []Additive
}

func NewAdditiveList(adds []Additive) AdditiveList {
	return AdditiveList{
		Adds: adds,
	}
}

func (al *AdditiveList) Size() int {
	return len(al.Adds)
}

func (al *AdditiveList) toCList() []C.additive_t {
	additives := make([]C.additive_t, len(al.Adds))
	for i, e := range al.Adds {
		additives[i] = e.toCAdditive()
	}
	return additives
}

type Series struct {
	LightExposure   Matrix[float32] `json:"lightExposure"`
	SoilTempC       Matrix[float32] `json:"soilTempC"`
	EnvTempC        []float32       `json:"envTempC"`
	WaterUseMl      []float32       `json:"waterUseMl"`
	PH              uint8           `json:"pH"`
	PBar            float32         `json:"pBar"`
	SoilDensityKgM3 float32         `json:"soilDensityKgM3"`
	SoilAdditives   AdditiveList    `json:"soilAdditives"`
	AtmAdditives    AdditiveList    `json:"atmAdditives"`
	Repeated        uint32          `json:"repeated"`
}

func NewSeries(lightExposure Matrix[float32], soilTempC Matrix[float32],
	envTempC []float32, waterUseMl []float32, pH uint8, pBar float32,
	soilDensityKgM3 float32, soilAdditives AdditiveList, atmAdditives AdditiveList,
	repeated uint32) Series {
	s := Series{
		LightExposure:   lightExposure,
		SoilTempC:       soilTempC,
		EnvTempC:        envTempC,
		WaterUseMl:      waterUseMl,
		PH:              pH,
		PBar:            pBar,
		SoilDensityKgM3: soilDensityKgM3,
		SoilAdditives:   soilAdditives,
		AtmAdditives:    atmAdditives,
		Repeated:        repeated,
	}
	seriesPinner.Pin(&s.LightExposure.mat[0])
	seriesPinner.Pin(&s.SoilTempC.mat[0])
	seriesPinner.Pin(&s.EnvTempC[0])
	seriesPinner.Pin(&s.WaterUseMl[0])
	seriesPinner.Pin(&s.SoilAdditives.toCList()[0])
	seriesPinner.Pin(&s.AtmAdditives.toCList()[0])
	return s
}

func (s *Series) DisposeSeries() {
	seriesPinner.Unpin()
}

func (s *Series) toCSeries() C.series_t {
	return C.create_series(
		(*C.float)(&s.LightExposure.mat[0]),
		(*C.float)(&s.SoilTempC.mat[0]),
		(*C.float)(&s.EnvTempC[0]),
		(*C.float)(&s.WaterUseMl[0]),
		C.uint8_t(s.PH),
		C.float(s.PBar),
		C.float(s.SoilDensityKgM3),
		C.uint16_t(s.SoilAdditives.Size()),
		C.uint16_t(s.AtmAdditives.Size()),
		(*C.additive_t)(&s.SoilAdditives.toCList()[0]),
		(*C.additive_t)(&s.AtmAdditives.toCList()[0]),
		C.uint32_t(s.Repeated))
}

type WaveInfo struct {
	MinWavelenNm uint16
	MaxWavelenNm uint16
	NWavelengths uint16
}

func NewWaveInfo(minWavelenNm uint16, maxWavelenNm uint16, nWavelengths uint16) WaveInfo {
	return WaveInfo{
		MinWavelenNm: minWavelenNm,
		MaxWavelenNm: maxWavelenNm,
		NWavelengths: nWavelengths,
	}
}

func (w *WaveInfo) toCWaveInfo() C.wavelength_info_t {
	return C.create_wavelength_info(
		C.uint16_t(w.MinWavelenNm),
		C.uint16_t(w.MaxWavelenNm),
		C.uint16_t(w.NWavelengths),
	)
}

type SoilDepthInfo struct {
	MinSoilDepthMm uint16
	MaxSoilDepthMm uint16
	NDepth         uint16
}

func NewSoilDepthInfo(minSoilDepthMm uint16, maxSoilDepthMm uint16, nDepth uint16) SoilDepthInfo {
	return SoilDepthInfo{
		MinSoilDepthMm: minSoilDepthMm,
		MaxSoilDepthMm: maxSoilDepthMm,
		NDepth:         nDepth,
	}
}

func (s *SoilDepthInfo) toSoilDepthInfo() C.soil_depth_info_t {
	return C.create_soil_depth_info(
		C.uint16_t(s.MinSoilDepthMm),
		C.uint16_t(s.MaxSoilDepthMm),
		C.uint16_t(s.NDepth),
	)
}

type ReductionInfo struct {
	SoilDensity   ReductionCode
	Pressure      ReductionCode
	LightExposure ReductionCode
	WaterUse      ReductionCode
	SoilTemp      ReductionCode
	EnvTemp       ReductionCode
}

func NewReductionInfo(soilDensity ReductionCode, pressure ReductionCode,
	lightExposure ReductionCode, waterUse ReductionCode, soilTemp ReductionCode,
	envTemp ReductionCode) ReductionInfo {
	return ReductionInfo{
		SoilDensity:   soilDensity,
		Pressure:      pressure,
		LightExposure: lightExposure,
		WaterUse:      waterUse,
		SoilTemp:      soilTemp,
		EnvTemp:       envTemp,
	}
}

func (r *ReductionInfo) toCReductionInfo() C.reduction_info_t {
	return C.create_reduction_info(
		C.uint8_t(r.SoilDensity),
		C.uint8_t(r.Pressure),
		C.uint8_t(r.LightExposure),
		C.uint8_t(r.WaterUse),
		C.uint8_t(r.SoilTemp),
		C.uint8_t(r.EnvTemp),
	)
}

type Header struct {
	Version    uint16
	FarmingTec FarmingTechnique
	WInfo      WaveInfo
	SInfo      SoilDepthInfo
	RedInfo    ReductionInfo
	NChunks    uint32
}

func NewHeader(farmingTec FarmingTechnique, wInfo WaveInfo, sInfo SoilDepthInfo,
	redInfo ReductionInfo, nChunks uint32) Header {
	return Header{
		Version:    uint16(C.__ADF_VERSION__),
		FarmingTec: farmingTec,
		WInfo:      wInfo,
		SInfo:      sInfo,
		RedInfo:    redInfo,
		NChunks:    nChunks,
	}
}

func (h Header) toCHeader() C.adf_header_t {
	return C.create_header(
		C.uint8_t(h.FarmingTec),
		h.WInfo.toCWaveInfo(),
		h.SInfo.toSoilDepthInfo(),
		h.RedInfo.toCReductionInfo(),
		C.uint32_t(h.NChunks),
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

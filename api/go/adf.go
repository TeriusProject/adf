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

package main

/*
#cgo CFLAGS: -std=c2x
#cgo CFLAGS: -I/usr/local/include
#cgo LDFLAGS: -L/usr/local/lib -ladf
#include <adf.h>
*/
import "C"
import (
	"errors"
	"fmt"
)

type Additive struct {
	code          uint32
	concentration float32
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
	nSoilAdds       uint16
	nAtmAdds        uint16
	soilAdditives   AdditiveList
	atmAdditives    AdditiveList
	repeated        uint32
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
		C.uint16_t(s.nSoilAdds),
		C.uint16_t(s.nAtmAdds),
		&s.soilAdditives.toCList()[0],
		&s.atmAdditives.toCList()[0],
		C.uint32_t(s.repeated))
}

type WaveInfo struct {
	minWavelenNm uint16
	maxWavelenNm uint16
	nWavelengths uint16
}

type SoilDepthInfo struct {
	minSoilDepthMm uint16
	maxSoilDepthMm uint16
	nDepth         uint16
}

type ReductionInfo struct {
	soilDensity   uint8
	pressure      uint8
	lightExposure uint8
	waterUse      uint8
	soilTemp      uint8
	envTemp       uint8
}

type Header struct {
	version    uint16
	farmingTec uint8
	wInfo      WaveInfo
	sInfo      SoilDepthInfo
	redInfo    ReductionInfo
	nChunks    uint32
}

func NewHeader(farmingTec uint8, nChunks uint32,
	wInfo WaveInfo, sInfo SoilDepthInfo, redInfo ReductionInfo) Header {
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
		h.wInfo,
		h.sInfo,
		h.redInfo,
		C.uint32_t(h.nChunks),
	)
}

type Adf struct {
	_adf C.adf_t
}

func NewAdf(header Header, periodSec uint32) Adf {
	var adf Adf
	C.adf_init(&adf._adf, header.toCHeader(), C.uint32_t(periodSec))
	return adf
}

func Unmarshal(bytes []byte) (Adf, error) {
	var adf Adf
	res := C.unmarshal(&adf._adf, (*C.uint8_t)(&bytes[0]))
	if res != C.ADF_OK {
		return Adf{}, nil
	}
	return adf, nil
}

func (adf *Adf) SizeBytes() uint32 {
	return uint32(C.size_adf_t(&adf._adf))
}

func (adf *Adf) Marshal() ([]byte, error) {
	cBytes := make([]byte, adf.SizeBytes())
	res := uint16(C.marshal((*C.uint8_t)(&cBytes[0]), &adf._adf))
	if res != C.ADF_OK {
		return nil, nil
	}
	return cBytes, nil
}

func (adf *Adf) AddSeries(series Series) error {
	cSeries := series.toCSeries()
	res := C.add_series(&adf._adf, &cSeries)
	if res != C.ADF_OK {
		return errors.New("")
	}
	return nil
}

func (adf *Adf) UpdateSeries(series Series, time uint64) error {
	cSeries := series.toCSeries()
	res := C.update_series(&adf._adf, &cSeries, C.uint64_t(time))
	if res != C.ADF_OK {
		return errors.New("")
	}
	return nil
}

func (adf *Adf) RemoveSeries() error {
	res := C.remove_series(&adf._adf)
	if res != C.ADF_OK {
		return errors.New("")
	}
	return nil
}

func (adf *Adf) GetVersion() string {
	version := C.get_adf_version()
	return fmt.Sprintf("%d.%d.%d", version.major, version.minor, version.patch)
}

func (adf *Adf) Dispose() {
	C.adf_free(&adf._adf)
}

func main() {
	var h Header
	a := NewAdf(h, 100)
	fmt.Println(a.GetVersion())
	bytes, _ := a.Marshal()
	fmt.Println(bytes)
	newAdf, _ := Unmarshal(bytes)
	fmt.Println(newAdf)
	a.Dispose()
	newAdf.Dispose()
}

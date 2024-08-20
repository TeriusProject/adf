/*
 * example.go - GO API example
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

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"math/rand"
	"os"

	adf "github.com/TeriusProject/adf/api/go/adflib"
)

const (
	fileName    string = "output.adf"
	nSeries     uint32 = 250
	nChunks     uint32 = 5
	nWavelength uint16 = 15
	nDepth      uint16 = 2
)

func getRandomSeries() adf.Series {
	lightExposure, err := adf.NewMatrix[float32](uint32(nWavelength))
	if err != nil {
		log.Fatal(err)
	}
	soilTemperature, err := adf.NewMatrix[float32](uint32(nDepth))
	if err != nil {
		log.Fatal(err)
	}
	var (
		temperatureCelsius []float32
		waterUseMl         []float32
	)
	soilAdditivesList := adf.NewAdditiveList([]adf.Additive{adf.NewAdditive(1, 16.504)})
	atmosphereAdditivesList := adf.NewAdditiveList([]adf.Additive{adf.NewAdditive(2, 3.5)})

	for i := 0; i < int(nChunks); i++ {
		var (
			lightExposureRow   []float32
			soilTemperatureRow []float32
		)

		for j := 0; j < int(nWavelength); j++ {
			lightExposureRow = append(lightExposureRow, rand.Float32())
		}
		lightExposure.AddRow(lightExposureRow)
		for j := 0; j < int(nDepth); j++ {
			soilTemperatureRow = append(soilTemperatureRow, rand.Float32())
		}
		soilTemperature.AddRow(soilTemperatureRow)
		temperatureCelsius = append(temperatureCelsius, rand.Float32())
		waterUseMl = append(waterUseMl, rand.Float32())
	}
	return adf.NewSeries(lightExposure,
		soilTemperature,
		temperatureCelsius,
		waterUseMl,
		2,
		3324.67,
		11.0,
		soilAdditivesList,
		atmosphereAdditivesList,
		1)
}

func writeAdfOnFile(bytes []byte) {
	f, err := os.Create(fileName)
	if err != nil {
		log.Fatal(err)
	}
	f.Write(bytes)
	f.Close()

	fmt.Printf("Wrote ADF file (%d bytes)\nfilename: %s\n", len(bytes), fileName)
}

func createAdfAndMarshal() {
	header := adf.NewHeader(
		adf.FOGPONICS,
		adf.NewWaveInfo(350, 1000, nWavelength),
		adf.NewSoilDepthInfo(0, 0, nDepth),
		adf.NewReductionInfo(adf.AVERAGE, adf.AVERAGE, adf.AVERAGE,
			adf.AVERAGE, adf.AVERAGE, adf.AVERAGE),
		adf.NewPrecisionInfo(0, 0, 0, 0, 0, 0, 0),
		nChunks)
	adf := adf.NewAdf(header, adf.DAY_TIME_SEC)
	fmt.Printf("Current ADF version: %s\n", adf.GetVersion())
	for i := 0; i < int(nSeries); i++ {
		series := getRandomSeries()
		err := adf.AddSeries(series)
		if err != nil {
			log.Fatal(err.Error())
		}

		series.DisposeSeries()
	}
	bytes, err := adf.Marshal()
	if err != nil {
		log.Fatal(err.Error())
	}
	adf.Dispose()
	writeAdfOnFile(bytes)
}

func readFromFile() []byte {
	file, err := os.Open(fileName)
	if err != nil {
		log.Fatal(err)
	}

	stat, err := file.Stat()
	if err != nil {
		log.Fatal(err)
	}

	bytes := make([]byte, stat.Size())
	_, err = bufio.NewReader(file).Read(bytes)
	if err != nil && err != io.EOF {
		log.Fatal(err)
	}
	file.Close()
	fmt.Printf("Read file %s (%d bytes)\n", fileName, stat.Size())
	return bytes
}

func unmarshalAndCreate() {
	bytes := readFromFile()
	adf, err := adf.Unmarshal(bytes)
	if err != nil {
		log.Fatal(err.Error())
	}
	fmt.Printf("ADF size: %d bytes\n", adf.SizeBytes())
	adf.Dispose()
}

func main() {
	createAdfAndMarshal()
	unmarshalAndCreate()
}

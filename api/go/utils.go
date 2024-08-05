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

import (
	"errors"
	"fmt"
)

type Matrix[T any] struct {
	mat     []T
	rows    uint32
	columns uint32
}

func MakeMatrix[T any](size uint32, rows uint32, columns uint32) (Matrix[T], error) {
	if size != rows*columns {
		err := fmt.Sprintf("An array of size %d, cannot be expressed as a matrix of shape %dx%d",
			size, rows, columns)
		return Matrix[T]{}, errors.New(err)
	}
	return Matrix[T]{
		mat:     make([]T, size),
		rows:    rows,
		columns: columns,
	}, nil
}

func (m *Matrix[T]) Shape(row uint32, column uint32) (uint32, uint32) {
	return m.rows, m.columns
}

func (m *Matrix[T]) At(row uint32, column uint32) T {
	return m.mat[column+row*m.columns]
}

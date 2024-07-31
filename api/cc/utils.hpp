/* 
 * utils.hpp
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

#pragma once

#include <cstdint>
#include <format> 
#include <functional>
#include <vector>

namespace adf {

class InvalidMatrixShapeException : public std::exception {
  private:
  	using std::exception::what;
	std::string message;

  public:
	InvalidMatrixShapeException(uint32_t size, uint32_t rows, uint32_t columns)
	{
		this->message = std::format("An array of size {}, cannot be expressed as a matrix of shape {}x{}",
		size,  rows, columns); 
	}
	
	const char *what() { return (const char *)message.c_str(); }
};

template<typename T, typename U>
static std::vector<U> map(const std::vector<T> &vec, std::function<U(T)> f)
{
	std::vector<U> newVec;
	for (auto const &i : vec) {
		newVec.push_back(f(i));
	}
	return newVec;
}

template <typename T> 
class Matrix {
	private:
	uint32_t nRows;
	uint32_t nColumns;
	std::vector<T> mat;

	public:
	Matrix(uint32_t rows, uint32_t columns)
	{
		this->mat.resize(rows * columns, 0);
		this->nRows = rows;
		this->nColumns = columns;
	}

	Matrix(std::vector<T> init, uint32_t rows, uint32_t columns)
	{
		if (init.size() != rows * columns)
			throw InvalidMatrixShapeException(init.size(), rows, columns);
		this->mat = init;
		this->nRows = rows;
		this->nColumns = columns;
	}

	std::vector<T> getVector() { return this->mat; }
	T *getFirstElement() { return this->mat.data(); }
	T &operator()(uint32_t row, uint32_t column) 
	{
		return this->mat.at(column + row * nColumns);
	}
};

} /* namespace adf */

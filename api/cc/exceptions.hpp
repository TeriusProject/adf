/* 
 * exceptions.hpp - wrappers for ADF error codes
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

#include <adf.h>
#include <string>
#include <format> 

namespace adf {

class RuntimeException : public std::exception {
	private:
  	using std::exception::what;
	std::string message = "An error occurred ";

	public:
	RuntimeException() { }
	const char *what() { return (const char *)message.c_str(); }
};

class EmptySeriesException : public std::exception {
	private:
  	using std::exception::what;
	std::string message = "";

	public:
	EmptySeriesException() { }
	const char *what() { return (const char *)message.c_str(); }
};

class AdfHeaderCorruptedException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	AdfHeaderCorruptedException()
	{
		this->message = "Header section is corrupted. Cannot unmarshal\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

class AdfMetadataCorruptedException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	AdfMetadataCorruptedException()
	{
		this->message = "Metadata section is corrupted. Cannot unmarshal\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

class AdfSeriesCorruptedException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	AdfSeriesCorruptedException()
	{
		this->message = "Series is corrupted. Cannot unmarshal\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

class ZeroRepeatedSeriesException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	ZeroRepeatedSeriesException()
	{
		this->message = "Series is corrupted. Cannot unmarshal\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

class AdfTimeOutOfBounException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	AdfTimeOutOfBounException()
	{
		this->message = "\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

class AdfAdditiveOverflowException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	AdfAdditiveOverflowException()
	{
		this->message = "\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

class NullTargetException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	NullTargetException()
	{
		this->message = "\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

class NullSourceException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	NullSourceException()
	{
		this->message = "\n";
	}
	const char *what() { return (const char *)message.c_str(); }
};

using NullHeaderTargetException = NullTargetException;
using NullHeaderSourceException = NullSourceException;
using NullMetadataTargetException = NullTargetException;
using NullMetadataSourceException = NullSourceException;
using NullSeriesTargetException = NullTargetException;
using NullSeriesSourceException = NullSourceException;
using NullAddditiveTargetException = NullTargetException;
using NullAddditiveSourceException = NullSourceException;

void throwAdfError(uint32_t errorCode)
{
	switch (errorCode) {
		case ADF_HEADER_CORRUPTED:
			throw AdfHeaderCorruptedException();
		case ADF_METADATA_CORRUPTED:
			throw AdfMetadataCorruptedException();
		case ADF_SERIES_CORRUPTED:
			throw AdfSeriesCorruptedException();
		case ADF_ZERO_REPEATED_SERIES:
			throw ZeroRepeatedSeriesException();
		case ADF_EMPTY_SERIES:
			throw EmptySeriesException();
		case ADF_TIME_OUT_OF_BOUND:
			throw AdfTimeOutOfBounException();
		case ADF_ADDITIVE_OVERFLOW:
			throw AdfAdditiveOverflowException();
		case ADF_NULL_HEADER_SOURCE:
			throw NullHeaderSourceException();
		case ADF_NULL_HEADER_TARGET:
			throw NullHeaderTargetException();
		case ADF_NULL_META_SOURCE:
			throw NullMetadataSourceException();
		case ADF_NULL_META_TARGET:
			throw NullMetadataTargetException();
		case ADF_NULL_SERIES_SOURCE:
			throw NullSeriesSourceException();
		case ADF_NULL_SERIES_TARGET:
			throw NullSeriesTargetException();
		case ADF_NULL_SOURCE:
			throw NullSourceException();
		case ADF_NULL_TARGET:
			throw NullTargetException();
		case ADF_NULL_ADDITIVE_SOURCE:
			throw NullAddditiveSourceException();
		case ADF_NULL_ADDITIVE_TARGET:
			throw NullAddditiveTargetException();
		case ADF_RUNTIME_ERROR:
			throw RuntimeException();
		default:
			break;
	}
}
} /* namespace adf */

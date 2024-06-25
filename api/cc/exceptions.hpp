//
//  exceptions.h
//  adf
//
//  Created by Matteo Nicoli on 6/15/24.
//

#pragma once

class RuntimeException : public std::exception {
	private:
	std::string message = "";

	public:
	RuntimeException() { }
	
	char * what () {
		return (char *)message.c_str();
	}
};

class EmptySeriesException : public std::exception {
	private:
	std::string message = "";

	public:
	EmptySeriesException() { }
	
	char * what () {
		return (char *)message.c_str();
	}
};


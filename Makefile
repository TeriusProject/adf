all: adf test

test:
	(cd test && make)
adf:
	(cd src && make)

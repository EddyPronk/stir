all : stir

stir : stir.cpp
	g++ -W -lcurl -o stir stir.cpp
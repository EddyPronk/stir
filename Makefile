all : stir

stir : stir.cpp
	g++ -W -lcurl -leditline -o stir stir.cpp

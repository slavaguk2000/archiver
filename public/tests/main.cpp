// Copyright 2012 The Emscripten Authors.  All rights reserved.
// Emscripten is available under two separate licenses, the MIT license and the
// University of Illinois/NCSA Open Source License.  Both these licenses can be
// found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "archiver.cpp"
#include <math.h>

using namespace emscripten;

FILE* openFile(const char* filePath, const char* method)
{
	FILE *file = fopen(filePath, method);
	if (!file) {
		throw std::runtime_error("cannot open file\n");
	} 	
	return file;
}

long long getFileLength(FILE* file)
{
	fseek(file, 0, SEEK_END);
	long long end = ftell(file);
	fseek(file, 0, SEEK_SET);	
	return end - ftell(file);
}

void printFileLength(const char* filePath)
{
	FILE* file = openFile(filePath, "rb");
	printf("%lld\n", getFileLength(file	));
	fclose(file);
}

void openWriteFile()
{
	FILE *file = fopen("data/1.txt", "wb");
  if (!file) {
    throw std::runtime_error("cannot open file\n");
  }
  for (char i = '0'; i <= '9'; i++){
    fputc(i, file);
	fputc('\n', file);	
  }
  puts("everything is ok!!!");
  fclose (file);
}

std::vector<char> getFileData(const char* filePath)
{
	FILE* file = openFile(filePath, "rb");
	std::vector<char> fileData(getFileLength(file));
	fread(&fileData[0], sizeof(char), fileData.size(), file);
	fclose(file);
	return fileData;
}

extern "C"{
	int startParseFile(int)
	{
		const int uncompressedSize = 10000;
		try{
			std::vector<char> compressedData = getFileData("data/compress_0_to_10000.zzz");
			CompressedMemory compressedArray((int)&(compressedData[0]), compressedData.size(), uncompressedSize);
			char uncompressedBuffer[uncompressedSize];
			compressedArray.uncompressMemory((int)uncompressedBuffer);
			for (int i = 0; i < uncompressedSize/100; i++)
			{
				printf("%d\n", (int)uncompressedBuffer[i]);
			}
		}catch(std::exception ex){puts(ex.what());}
		return 0;
	}
	
	int getCompressedDataSize(char* ptr, int size)
	{
		CompressedMemory compr((int)ptr, size);
		return compr.getCompressedSize();
	}
}

void print_Hello(int count, int str){
	char outString[100];
	//char str[] = "Slava";
	strcpy(outString, "Hello, ");
	strcat(outString, (char*)str);
	for(int i = 0; i < count; i++){
		puts(outString);
	}
}

int getPointer(){
	return sizeof(int);
}



EMSCRIPTEN_BINDINGS(my_module) {

	class_<CompressedMemory>("CompressedMemory")
	.constructor<int, int>()
	.constructor<int, int, int>()
	.function("getCompressedData", &CompressedMemory::getCompressedData)
	.function("getCompressedSize", &CompressedMemory::getCompressedSize)
	.function("uncompressMemory", &CompressedMemory::uncompressMemory)
	.function("getSourceSize", &CompressedMemory::getSourceSize)
	.function("clear", &CompressedMemory::clear);
	

	function("compress", &my_compress, allow_raw_pointers());
	function("decompress", &my_decompress, allow_raw_pointers());
	function("clear", &clear, allow_raw_pointers());
	function("print_Hello", &print_Hello, allow_raw_pointers());
    function("getPointer", &getPointer, allow_raw_pointers());
}

int main() {
	puts("start");
	//startParseFile(5);
	return 0;
}
//emcc tests\hello_world_file.cpp -o hello.html --preload-file tests\hello_world_file.txt --preload-file data\1.jpg
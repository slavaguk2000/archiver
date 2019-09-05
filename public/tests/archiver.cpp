#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "zlib/zlib.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

#include "gzip.cpp"

#define MEMORY_ERROR -1

using namespace emscripten;

class CompressedMemory
{
	int source_size;
	uLongf compress_buff_size;
	void* compress_buff = nullptr;
	int error_msg;
	
	void init(const char* source, int _source_size) {
		compress_buff_size = compressBound(source_size);
		error_msg = 0;
		compress_buff = malloc(compress_buff_size);
		if (!compress_buff)
		{
			error_msg = MEMORY_ERROR;
			throw std::runtime_error("Memory Error");
		}
		error_msg = compress2((Bytef*)compress_buff, (uLongf*)&compress_buff_size, (const Bytef*)source, source_size, Z_BEST_COMPRESSION);
		switch (error_msg)
		{
		case Z_MEM_ERROR:
			throw std::runtime_error("Memory Error");
		case Z_BUF_ERROR:
			throw std::runtime_error("Buffer Error");
		default:
			break;
		}
	}
	void copy(const CompressedMemory &src) {
		source_size = src.source_size;
		compress_buff_size = src.compress_buff_size;
		error_msg = src.error_msg;
		compress_buff = realloc(compress_buff, compress_buff_size);
		memcpy(compress_buff, src.compress_buff, compress_buff_size);
	}
public:
	CompressedMemory(int/*for embind*/ compressed_memory, int compressed_size, int _source_size) :
		source_size(_source_size), compress_buff_size(compressed_size)
	{
		error_msg = 0;
		compress_buff = malloc(compress_buff_size);
		if (!compress_buff)
		{
			error_msg = MEMORY_ERROR;
			throw std::runtime_error("Memory Error");
		}
		memcpy(compress_buff, (const char*)compressed_memory, compress_buff_size);
	}	
	CompressedMemory(int/*for embind*/ source, int _source_size) : source_size(_source_size)
	{
		init((const char*)source, source_size);
		puts("CONSTRUCTOR!!!!!");
	}
	
	CompressedMemory(CompressedMemory &&src)
	{
		this->source_size = src.source_size;
		this->compress_buff_size = src.compress_buff_size;
		this->compress_buff = src.compress_buff;
		src.compress_buff = nullptr;
		this->error_msg = src.error_msg;		
	}
	CompressedMemory(const CompressedMemory &src) {
		copy(src);
	}
	CompressedMemory& operator=(const CompressedMemory& src) {
		copy(src);
	}
	CompressedMemory& operator=(CompressedMemory&& src) {
		source_size = src.source_size;
		compress_buff_size = src.compress_buff_size;
		error_msg = src.error_msg;
		compress_buff = src.compress_buff;
		src.compress_buff = nullptr;
	}
	int getCompressedData() {
		return/*for embind*/ (int)compress_buff;
	}
	int getCompressedSize() {
		return compress_buff_size;
	}
	int getSourceSize() {
		return source_size;
	}
	int getError() {
		return error_msg;
	}
	int uncompressMemory(/*for embind*/ int uncompressed_buffer) {
		uLong uncompressed_size = source_size;
		error_msg = uncompress((Bytef*)uncompressed_buffer, &uncompressed_size, (const Bytef*)compress_buff, compress_buff_size);
		return uncompressed_size;
	}
	void clear(){
		if (compress_buff) free(compress_buff);
		compress_buff = nullptr;
	}
	~CompressedMemory()
	{
		clear();
		puts("DESTRUCTORRRR!!!!!!!!!!");
	}
};

void clear(CompressedMemory compr){
	compr.clear();
}

int my_compress(int pointer, int source_size){
	int error_msg;
	uLongf compress_buff_size = compressBound(source_size);
	error_msg = 0;
	void* compress_buff = malloc(compress_buff_size+sizeof(int));
	if (!compress_buff)
	{
		error_msg = MEMORY_ERROR;
		throw std::runtime_error("Memory Error");
	}
	error_msg = compress2((Bytef*)compress_buff+sizeof(int), (uLongf*)&compress_buff_size, (const Bytef*)pointer, source_size, Z_BEST_COMPRESSION);
	switch (error_msg)
	{
	case Z_MEM_ERROR:
		throw std::runtime_error("Memory Error");
	case Z_BUF_ERROR:
		throw std::runtime_error("Buffer Error");
	default:
		break;
	}
	((int*)compress_buff)[0] = compress_buff_size;
	return (int)compress_buff;
}

int my_decompress(int compressedBuffer, int compressedSize, int uncompressedBuffer, int uncompressedSize) {
	uLong uLongUncompressedSize = uncompressedSize;
	int error_msg = uncompress((Bytef*)uncompressedBuffer, &uLongUncompressedSize, (const Bytef*)compressedBuffer, compressedSize);
	return uncompressedSize;
}

// int main() {
	// /*char input[] = "D:\\1.jpg", output[] = "D:\\2.jpg";
	// CompressedMemory in(input);
	// CompressedMemory in2 = in;
	// in2.uncompressMemoryToFile(output);*/
	// return 0;
// }

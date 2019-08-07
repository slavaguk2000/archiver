#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "zlib.h"
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

#define MEMORY_ERROR -1

using namespace emscripten;

int getFileLength(FILE* input_file){
	long int start, end;
	fseek(input_file, 0, SEEK_END);
	end = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);
	start = ftell(input_file);
	return end - start;
}

class CompressedMemory
{
	int source_size;
	uLongf compress_buff_size;
	void* compress_buff = nullptr;
	int error_msg;
	char* file_name = nullptr;
	void EMSCRIPTEN_KEEPALIVE init(const char* source, int _source_size) {
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
	void EMSCRIPTEN_KEEPALIVE copy(const CompressedMemory &src) {
		source_size = src.source_size;
		compress_buff_size = src.compress_buff_size;
		error_msg = src.error_msg;
		compress_buff = realloc(compress_buff, compress_buff_size);
		memcpy(compress_buff, src.compress_buff, compress_buff_size);
		file_name = (char*)realloc(file_name, strlen(src.file_name));
		strcpy(file_name, src.file_name);
	}
public:
	EMSCRIPTEN_KEEPALIVE CompressedMemory(const char* compressed_memory, int compressed_size, int _source_size, char* name_of_file = nullptr) :
		source_size(_source_size), compress_buff_size(compressed_size), file_name(name_of_file)
	{
		error_msg = 0;
		compress_buff = malloc(compress_buff_size);
		if (!compress_buff)
		{
			error_msg = MEMORY_ERROR;
			throw std::runtime_error("Memory Error");
		}
		memcpy(compress_buff, compressed_memory, compress_buff_size);
	}
	EMSCRIPTEN_KEEPALIVE CompressedMemory(const char* source, int _source_size) : source_size(_source_size)
	{
		init(source, source_size);
	}
	EMSCRIPTEN_KEEPALIVE CompressedMemory(char* name_of_file)
	{
		FILE* input_file;
		input_file = fopen(name_of_file, "rb");
		source_size = getFileLength(input_file);
		void* buf_source = malloc(source_size);
		fread(buf_source, 1, source_size, input_file);
		fclose(input_file);
		unsigned int compressed_length;
		char* pointer = name_of_file, a;
		do{
			a = *pointer++;
			if (a == '\\') name_of_file = pointer;
		} while (a);
		int len = pointer - name_of_file;
		file_name = (char*)malloc(len);
		if (!file_name)
		{
			error_msg = MEMORY_ERROR;
			throw std::runtime_error("Memory Error");
		}
		strcpy(file_name, name_of_file);
		init((const char*)buf_source, source_size);
		free(buf_source);
	}
	EMSCRIPTEN_KEEPALIVE CompressedMemory(CompressedMemory &&src)
	{
		this->source_size = src.source_size;
		this->compress_buff_size = src.compress_buff_size;
		this->compress_buff = src.compress_buff;
		this->file_name = src.file_name;
		src.compress_buff = nullptr;
		this->error_msg = src.error_msg;
		src.file_name = nullptr;
	}
	EMSCRIPTEN_KEEPALIVE CompressedMemory(const CompressedMemory &src) {
		copy(src);
	}
	EMSCRIPTEN_KEEPALIVE CompressedMemory& operator=(const CompressedMemory& src) {
		copy(src);
	}
	EMSCRIPTEN_KEEPALIVE CompressedMemory& operator=(CompressedMemory&& src) {
		source_size = src.source_size;
		compress_buff_size = src.compress_buff_size;
		error_msg = src.error_msg;
		compress_buff = src.compress_buff;
		src.compress_buff = nullptr;
		file_name = src.file_name;
		src.file_name = nullptr;
	}
	void* EMSCRIPTEN_KEEPALIVE getCompressedData() {
		return compress_buff;
	}
	int EMSCRIPTEN_KEEPALIVE getCompressedSize() {
		return compress_buff_size;
	}
	int EMSCRIPTEN_KEEPALIVE getSourceSize() {
		return source_size;
	}
	int EMSCRIPTEN_KEEPALIVE getError() {
		return error_msg;
	}
	int EMSCRIPTEN_KEEPALIVE uncompressMemory(Bytef* uncompressed_buffer) {
		uLong uncompressed_size = source_size;
		error_msg = uncompress(uncompressed_buffer, &uncompressed_size, (const Bytef*)compress_buff, compress_buff_size);
		return uncompressed_size;
	}
	int EMSCRIPTEN_KEEPALIVE uncompressMemoryToFile(char* file = nullptr) {
		if (!file) return 0;
		FILE *output_file;
		Bytef* uncompressed_buffer = (Bytef*)malloc(source_size);
		int uncompressed_size = uncompressMemory(uncompressed_buffer);
		if(error_msg != Z_OK) return error_msg;
		output_file = fopen(file, "wb");
		if (!output_file) throw std::runtime_error("file open error");
		int written = fwrite(uncompressed_buffer, 1, uncompressed_size, output_file);
		if (written < uncompressed_size) throw std::runtime_error("file write error");
		fclose(output_file);
		free(uncompressed_buffer);
		return Z_OK;
	}
	char* EMSCRIPTEN_KEEPALIVE getFileName() {
		return file_name;
	}
	EMSCRIPTEN_KEEPALIVE ~CompressedMemory()
	{
		if (compress_buff) free(compress_buff);
		if (file_name) free(file_name);
	}
};

int flen(std::string a){
	FILE* inf = fopen(a.c_str(), "rb");
	return getFileLength(inf);
}

EMSCRIPTEN_BINDINGS(archiver) {
    // class_<CompressedMemory>("CompressedMemory").constructor<string, int, int, string>()
    //   .function("getFileName",&CompressedMemory::getFileName)
    //   .function("getCompressedData",&CompressedMemory::getCompressedData);
		function("flen", &flen);
}

int main() {
	/*char input[] = "D:\\1.jpg", output[] = "D:\\2.jpg";
	CompressedMemory in(input);
	CompressedMemory in2 = in;
	in2.uncompressMemoryToFile(output);*/
	return 0;
}

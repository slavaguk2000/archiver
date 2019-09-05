//#include "zlib.h"
#include "gzguts.h"

void initAllocators(gz_statep state) {
	state->strm.zalloc = Z_NULL;
	state->strm.zfree = Z_NULL;
	state->strm.opaque = Z_NULL;
}
bool stateInit(unsigned char* source, unsigned char* destanition, unsigned length, gz_statep state) {
	state->x.have = 0;
	state->x.pos = 0;
	state->direct = 0;
	state->seek = 0;
	state->err = 0;
	state->msg = nullptr;
	state->strm.avail_in = 0;

	state->mode = GZ_WRITE;
	state->want = length;
	state->size = state->want;
	state->level = Z_BEST_COMPRESSION;
	state->strategy = Z_DEFAULT_STRATEGY;

	state->in = source;
	state->strm.avail_out = state->size+20;
	state->out = destanition;
	if (state->out == NULL) return false;
	
	initAllocators(state);

	deflateInit2(&(state->strm), state->level, Z_DEFLATED, MAX_WBITS + 16, DEF_MEM_LEVEL, state->strategy);
	state->strm.next_in = state->in;
	state->strm.next_out = state->out;
	state->x.next = state->out;

	state->strm.avail_in = length;
	state->x.pos = length;


	return true;
}

unsigned deflateStream(gz_statep state) { 		
	deflate(&(state->strm), Z_FINISH);
	unsigned count = state->strm.total_out;
	deflateReset(&(state->strm));
	deflateEnd(&(state->strm));
	return count;
}

int gzipCompress(int sourcePointer, int gzipPointer, int size){
	gz_state state;
	stateInit((unsigned char*)sourcePointer, (unsigned char*)gzipPointer, size, &state);
	unsigned count = deflateStream(&state);
	return count;
}
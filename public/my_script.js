{
    var count = 10000
    var sourcePointer = Module._malloc(count);
    var sourceArray = new Uint8Array(wasmMemory.buffer, sourcePointer, count);
    for (var i = 0; i < sourceArray.length; i++)
        sourceArray[i] = i*i;

    var compressedMemory = new Module.CompressedMemory(sourcePointer, count)

    var compressedSize = compressedMemory.getCompressedSize();
    var compressedArray = new Uint8Array(wasmMemory.buffer, compressedMemory.getCompressedData(), compressedSize);


    var distanitionPointer = Module._malloc(count);
    var distanitionArray = new Uint8Array(wasmMemory.buffer, distanitionPointer, count);
    compressedMemory.uncompressMemory(distanitionPointer);
}

function compress(sourceArray){
    var size = sourceArray.length
    var sourcePointer = Module._malloc(size);
    uncompressedArray = new Uint8Array(wasmMemory.buffer, sourcePointer, size);
    uncompressedArray.set(sourceArray);
    var compressedPointer = Module.compress(sourcePointer, size);
    var compressedSizeArray = new Uint32Array(wasmMemory.buffer, compressedPointer, 1);
    var compressedSize = compressedSizeArray[0];
    wasmCompressedArray =  new Uint8Array(wasmMemory.buffer, compressedPointer+4, compressedSize);
    const compressedArray = [...wasmCompressedArray]; 
    Module._free(sourcePointer);
    Module._free(compressedPointer);
    return {
        size,
        compressedArray
    }
}

function decompress(compressedArray, sourceSize){
    var size = compressedArray.length;
    var compressedPointer = Module._malloc(size);
    var decompressedPointer = Module._malloc(sourceSize);
    wasmCompressedArray = new Uint8Array(wasmMemory.buffer, compressedPointer, size);
    wasmDecompressedArray = new Uint8Array(wasmMemory.buffer, decompressedPointer, sourceSize);
    wasmCompressedArray.set(compressedArray);
    var decompressedSize = Module.decompress(compressedPointer, size, decompressedPointer, sourceSize);
    if (decompressedSize != sourceSize) console.log("error");
    const decompressedArray = [...wasmDecompressedArray]; 
    Module._free(compressedPointer);
    Module._free(decompressedPointer);
    return decompressedArray;
}
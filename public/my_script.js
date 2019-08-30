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
    return new Uint8Array(decompressedArray);
}


var sourceArray
function openFile(callback) {
  var input = document.getElementById("Bfile");
  var reader = new FileReader();
  reader.onload = function (event){
    var reader = event.target;
    var arrayBuffer = reader.result;
    callback(new Int8Array(arrayBuffer));
  };
  reader.readAsArrayBuffer(input.files[0]);
};
function openCurrent(event){
   openFile(function(ARRAY) {
     sourceArray =  [...ARRAY];
   });
 }

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

var saveByteArray = (function () {
    var a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";
    return function (data, name) {
        var blob = new Blob([data], {type: "application/octet-binary"}),
            url = window.URL.createObjectURL(blob);
        a.href = url;
        a.download = name;
        a.click();
        window.URL.revokeObjectURL(url);
    };
}());
saveByteArray([sampleBytes], 'example.txt');
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
	returnedArray = new Uint8Array(compressedArray)
    return {
        size,
        returnedArray
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
	returnedArray = new Uint8Array(decompressedArray);
    return returnedArray;
}

function getGzipArray(sourceArray){
  var size = sourceArray.length
  var outSize = size + 20;
  var sourcePointer = Module._malloc(size);
  var gzipPointer = Module._malloc(outSize) 
  wasmSourceArray = new Uint8Array(wasmMemory.buffer, sourcePointer, size);
  wasmSourceArray.set(sourceArray);
  var gzipSize = Module.gzipCompress(sourcePointer, gzipPointer, size);
  wasmGzipArray = new Uint8Array(wasmMemory.buffer, gzipPointer, gzipSize);
  const gzipArray = [...wasmGzipArray];
  Module._free(sourcePointer);
  Module._free(gzipPointer);
  returnedArray = new Uint8Array(gzipArray);
  return returnedArray;
}


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
     getGzipFile(new Uint8Array(sourceArray), 'example.gz');
   });
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

function getGzipFile(inputArray, name){
  saveByteArray(getGzipArray(inputArray), name);
}



{
	var sourceArray

    var count = 1000
    var sourcePointer = Module._malloc(count);
    var sourceArray = new Uint8Array(wasmMemory.buffer, sourcePointer, count);
    for (var i = 0; i < sourceArray.length; i++)
        sourceArray[i] = i%10+48;

    var compressedMemory = new Module.CompressedMemory(sourcePointer, count)

    var compressedSize = compressedMemory.getCompressedSize();
    var compressedArray = new Uint8Array(wasmMemory.buffer, compressedMemory.getCompressedData(), compressedSize);


    var distanitionPointer = Module._malloc(count);
    var distanitionArray = new Uint8Array(wasmMemory.buffer, distanitionPointer, count);
    compressedMemory.uncompressMemory(distanitionPointer);
}


var count = 1000
var sourceArray = new Uint8Array(count);
for (var i = 0; i < sourceArray.length; i++)
    sourceArray[i] = i%10+48;

saveByteArray([sampleBytes], 'example.txt');
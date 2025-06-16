import $ from "jquery";

async function fetchFileAsByteArray(url) {
    // Fetch the file from the specified URL
    const response = await fetch(url);
    // Check if the fetch was successful
    if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
    }
    // Read the response as an ArrayBuffer
    const arrayBuffer = await response.arrayBuffer();

    // Convert the ArrayBuffer to a byte array (Uint8Array)
    const byteArray = new Uint8Array(arrayBuffer);
    return byteArray;
}


// 检测是否支持WebWorker
function isWebWorkerSupported() {
    return typeof Worker !== 'undefined';
}

function isSharedArrayBufferSupported() {
    if (typeof SharedArrayBuffer === 'undefined') {
        return false;
    }
    
    try {
        const sab = new SharedArrayBuffer(1);
        return true;
    } catch (e) {
        return false;
    }
}

// 执行检测并输出结果
function checkSupport() {
    return isWebWorkerSupported() && isSharedArrayBufferSupported();
}

globalThis.nativeOnReady = () => {
    // if(!checkSupport())
    //     return; 

    fetchFileAsByteArray(`/test.nes`)
    .then((data)=>{
        globalThis.FS.writeFile("/test.nes", data)
        globalThis.Module.ccall(
          'test',  
          'void',       
          ['string'],
          ["/test.nes"]   
        );
    })
    .catch((err)=>{
        console.error(err)
    })
}

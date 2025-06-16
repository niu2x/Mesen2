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
globalThis.nativeOnReady = () => {
    // var url = new URL(window.location.href);
    // var name = url.searchParams.get("name");

    console.log("nativeOnReady 1")

    fetchFileAsByteArray(`/test.nes`)
    .then((data)=>{
        console.log("nativeOnReady 2")
        globalThis.FS.writeFile("/test.nes", data)
        globalThis.Module.ccall(
          'test',  
          'void',       
          ['string'],
          ["/test.nes"]   
        );
        console.log("nativeOnReady 3")
    })
    .catch((err)=>{
        console.log("nativeOnReady 4")
        console.error(err)
    })
    console.log("nativeOnReady 5")
}

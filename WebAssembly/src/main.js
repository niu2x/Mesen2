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

    // fetchFileAsByteArray(`/cart.db`)
    // .then((data)=>{
    //     let ptr = globalThis.Module._malloc(data.length);
    //     globalThis.Module.HEAPU8.set(data, ptr);
    //     globalThis.Module._load_cart_db(ptr, data.length);
    //     globalThis.Module._free(ptr);
    // })
    // .then(()=>{
    //     return fetchFileAsByteArray(`/ROM/iNES/${name}`)
    // })
    // .then((data)=>{
    //     let ptr = globalThis.Module._malloc(data.length);
    //     globalThis.Module.HEAPU8.set(data, ptr);
    //     globalThis.Module._load_nes(ptr, data.length);
    //     globalThis.Module._free(ptr);
    // })
    // .catch((err)=>{
    //     console.error(err)
    // })
    globalThis.Module._test();
    console.log("succ")
}

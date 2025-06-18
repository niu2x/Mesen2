import $ from "jquery";
import './style.scss'
import * as bootstrap from 'bootstrap'


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

globalThis.nativeReady = false;

globalThis.nativeOnReady = () => {
    globalThis.nativeReady = true
}


document.addEventListener('DOMContentLoaded', function() {
    // Button event handlers
    document.getElementById('load-rom').addEventListener('click', function() {
        if(!globalThis.nativeReady)
            return;
        
        document.getElementById('rom-file').click();
    });

    document.getElementById('rom-file').addEventListener('change', function(e) {
        const file = e.target.files[0];
        if (file) {
            document.getElementById('game-title').textContent = file.name;
            document.getElementById('game-status').textContent = 'Loading file';
            // Here you would load the ROM file into your emulator

            const reader = new FileReader();

            reader.onload = function(e) {
                const arrayBuffer = e.target.result;
                const byteArray = new Uint8Array(arrayBuffer);

                document.getElementById('game-status').textContent = 'Ready to play';
                globalThis.FS.writeFile(`/${file.name}`, byteArray)

                globalThis.Module.ccall(
                    'load_NES_file',
                    'void',
                    ['string'],
                    [`/${file.name}`]
                );
            };

            reader.onerror = function() {
                document.getElementById('game-status').textContent = 'Error loading file';
                console.error('Error reading file');
            };

            reader.readAsArrayBuffer(file);

        }
    });


    document.getElementById('reset-btn').addEventListener('click', function() {
        document.getElementById('game-status').textContent = 'Reset';
        // Reset emulator
    });
});
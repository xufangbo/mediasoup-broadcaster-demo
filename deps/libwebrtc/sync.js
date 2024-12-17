const fs = require('fs');
const path = require('path');

function copyIfExists(srcBase, destBase) {
    try {
        // Read the source directory synchronously.
        const srcItems = fs.readdirSync(srcBase, { withFileTypes: true });

        for (const item of srcItems) {
            const srcPath = path.join(srcBase, item.name);
            const destPath = path.join(destBase, item.name);

            if (item.isDirectory()) {
                // If it's a directory, create it in the destination and recurse.
                if (fs.existsSync(destPath)) {
                    console.log(`正在复制目录: ${item.name}`);
                    // Ensure the destination directory exists.
                    fs.mkdirSync(destPath, { recursive: true });
                    copyIfExists(srcPath, destPath); // Recurse into subdirectory.
                }
            } else {
                // If it's a file, check if it exists in the destination and copy if it does.
                if (fs.existsSync(destPath)) {
                    console.log(`正在覆盖文件: ${item.name}`);
                    // Copy the file to the destination, overwriting existing files.
                    fs.copyFileSync(srcPath, destPath);
                }
            }
        }
    } catch (err) {
        console.error('操作过程中出现错误:', err);
    }
}

// Define the source and destination directories.
const src = "//home/kevin/googlesource/webrtc/src";
const dest = "/home/kevin/git/mediasoup-broadcaster-demo/deps/libwebrtc";

// Start copying process.
copyIfExists(src, dest);
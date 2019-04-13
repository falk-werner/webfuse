import { Client } from "./webfuse/client.js";
import { ConnectionView } from "./connection_view.js";
import { FileSystemProvider } from "./filesystem_provider.js";


function mode(value) {
    return parseInt(value, 8);
}

function startup() {
    const provider = new FileSystemProvider({
        inode: 1,
        mode: mode("0755"),
        type: "dir",
        entries: {
            "hello.txt"   : { inode: 2, mode: mode("0444"), type: "file", contents: "Hello, World!"},
            "say_hello.sh": { inode: 3, mode: mode("0555"), type: "file", contents: "#!/bin/sh\necho hello\n"}
        }
    });
    const client = new Client();
    const connectionView = new ConnectionView(client, provider);    
    document.getElementById('connection').appendChild(connectionView.element);
}

window.onload = startup;

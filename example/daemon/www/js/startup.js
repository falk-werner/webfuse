import { Client } from "./wsfs/client.js";
import { ConnectionView } from "./connection_view.js";
import { FileSystemProvider } from "./filesystem_provider.js";


function mode(value) {
    return parseInt(value, 8);
}

function startup() {
    let provider = new FileSystemProvider({
        inode: 1,
        mode: mode("0755"),
        type: "dir",
        entries: {
            "hello.txt"   : { inode: 2, mode: mode("0444"), type: "file", contents: "Hello, World!"},
            "say_hello.sh": { inode: 3, mode: mode("0555"), type: "file", contents: "#!/bin/sh\necho hello\n"}
        }
    });
    let client = new Client(provider);
    let connectionView = new ConnectionView(client);    
    document.getElementById('connection').appendChild(connectionView.element);
    provider.setView(connectionView);
}

window.onload = startup;

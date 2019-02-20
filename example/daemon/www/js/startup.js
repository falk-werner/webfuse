import { Connection } from "./connection.js";
import { ConnectionView } from "./connection_view.js";
import { FileSystem } from "./filesystem.js";
import { FileSystemHandler } from "./filesystem_handler.js";


function mode(value) {
    return parseInt(value, 8);
}

function startup() {
    let connection = new Connection();
    let connectionView = new ConnectionView(connection);
    document.getElementById('connection').appendChild(connectionView.element);

    let fs = new FileSystem({
        inode: 1,
        mode: mode("0755"),
        type: "dir",
        entries: {
            "hello.txt"   : { inode: 2, mode: mode("0444"), type: "file", contents: "Hello, World!"},
            "say_hello.sh": { inode: 3, mode: mode("0555"), type: "file", contents: "#!/bin/sh\necho hello\n"}
        }
    });

    let handler = new FileSystemHandler(fs, connection);
}

window.onload = startup;

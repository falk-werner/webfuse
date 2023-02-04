
import { Webfuse } from "./webfuse/webfuse.js";
import { StaticFileSystem } from "./static_filesystem.js";

let webfuse = null;
const filesystem = new StaticFileSystem(new Map([
    ["/foo", "foo"],
    ["/bar", "foo"]
]));

function onConnectButtonClicked() {
    if (webfuse) { webfuse.close(); }

    const urlTextfield = document.querySelector('#url');
    const url = urlTextfield.value;
    console.log(url);

    webfuse = new Webfuse(url, filesystem);
}

function startup() {
    const connectButton = document.querySelector('#connect');
    connectButton.addEventListener('click', onConnectButtonClicked);
}

document.addEventListener('DOMContentLoaded', startup(),false);

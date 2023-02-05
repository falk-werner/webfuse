
import { Webfuse } from "./webfuse/webfuse.js";
import { FileSystem } from "./filesystem.js";


function encode(value) {
    const encoder = new TextEncoder('utf-8');
    return encoder.encode(value);
}

function get_contents() {
    const contentTextArea = document.querySelector("#contents");
    const contents = contentTextArea.value;
    return encode(contents);
}

function get_token() {
    const tokenTextfield = document.querySelector('#token');
    const token = tokenTextfield.value;
    return token;
}

function update_state(state) {
    const stateTextField = document.querySelector("#state");
    stateTextField.textContent = (state == "connected") ? "connected" : "disconnected";
}

let webfuse = null;
const filesystem = new FileSystem(get_token, update_state, [
    {name: "README.md", contents: get_contents }
]);

function onConnectButtonClicked() {
    if (webfuse) { webfuse.close(); }

    const urlTextfield = document.querySelector('#url');
    const url = urlTextfield.value;

    webfuse = new Webfuse(url, filesystem);
}

function startup() {
    const connectButton = document.querySelector('#connect');
    connectButton.addEventListener('click', onConnectButtonClicked);
}

document.addEventListener('DOMContentLoaded', startup(),false);

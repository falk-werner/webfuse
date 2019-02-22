export class ConnectionView {
    constructor(client) {
        this._client = client;
        this._client.onopen = () => { this._onConnectionOpened(); };
        this._client.onclose = () => { this._onConnectionClosed(); };

        this.element = document.createElement("div");

        const urlLabel = document.createElement("span");
        urlLabel.textContent = "URL:";
        this.element.appendChild(urlLabel);
        
        this.urlTextbox = document.createElement("input");
        this.urlTextbox.type = "text";
        this.urlTextbox.value = window.location.href.replace(/^http/, "ws");
        this.element.appendChild(this.urlTextbox);

        this.connectButton = document.createElement("input");
        this.connectButton.type = "button";
        this.connectButton.value = "connect";
        this.connectButton.addEventListener("click", () => { this._onConnectButtonClicked(); });
        this.element.appendChild(this.connectButton);
    }

    _onConnectButtonClicked() {
        if (!this._client.isConnected()) {
            let url = this.urlTextbox.value;
            this._client.connectTo(url);
        }
        else {
            this._client.disconnect();
        }
    }

    _onConnectionOpened() {
        this.connectButton.value = "disconnect";
    }

    _onConnectionClosed() {
        this.connectButton.value = "connect";
    }

}

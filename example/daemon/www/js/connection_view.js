/* exported ConnectionView */

class ConnectionView {
    constructor(connection) {
        this.connection = connection;
        this.connection.onclose = () => { this.onConnectionClosed(); };
        this.connection.onopen = () => { this.onConnectionOpened(); };

        this.element = document.createElement("div");

        let urlLabel = document.createElement("span");
        urlLabel.textContent = "URL:";
        this.element.appendChild(urlLabel);
        
        this.urlTextbox = document.createElement("input");
        this.urlTextbox.type = "text";
        this.urlTextbox.value = window.location.href.replace(/^http/, "ws");
        this.element.appendChild(this.urlTextbox);

        this.connectButton = document.createElement("input");
        this.connectButton.type = "button";
        this.connectButton.value = "connect";
        this.connectButton.addEventListener("click", () => { this.onConnectButtonClicked(); });
        this.element.appendChild(this.connectButton);
    }

    onConnectButtonClicked() {
        if (!this.connection.isConnected) {
            let url = this.urlTextbox.value;
            this.connection.connectTo(url);
        }
        else {
            this.connection.close();
        }
    }
    onConnectionOpened() {
        this.connectButton.value = "disconnect";
    }

    onConnectionClosed() {
        this.connectButton.value = "connect";
    }

}

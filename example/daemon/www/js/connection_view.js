export class ConnectionView {
    constructor(client, provider) {
        this._provider = provider;
        this._client = client;
        this._client.onopen = () => { this._onConnectionOpened(); };
        this._client.onclose = () => { this._onConnectionClosed(); };

        this.element = document.createElement("div");

        const connectBox = document.createElement("div");
        this.element.appendChild(connectBox);

        const urlLabel = document.createElement("span");
        urlLabel.textContent = "URL:";
        connectBox.appendChild(urlLabel);
        
        this.urlTextbox = document.createElement("input");
        this.urlTextbox.type = "text";
        this.urlTextbox.value = window.location.href.replace(/^http/, "ws");
        connectBox.appendChild(this.urlTextbox);

        this.connectButton = document.createElement("input");
        this.connectButton.type = "button";
        this.connectButton.value = "connect";
        this.connectButton.addEventListener("click", () => { this._onConnectButtonClicked(); });
        connectBox.appendChild(this.connectButton);


        const authenticateBox = document.createElement("div");
        this.element.appendChild(authenticateBox);

        const usernameLabel = document.createElement("span");
        usernameLabel.textContent = "user:";
        authenticateBox.appendChild(usernameLabel);
        
        this.usernameTextbox = document.createElement("input");
        this.usernameTextbox.type = "text";
        this.usernameTextbox.value = "bob";
        authenticateBox.appendChild(this.usernameTextbox);

        const passwordLabel = document.createElement("span");
        passwordLabel.textContent = "user:";
        authenticateBox.appendChild(passwordLabel);
        
        this.passwordTextbox = document.createElement("input");
        this.passwordTextbox.type = "password";
        this.passwordTextbox.value = "secret";
        authenticateBox.appendChild(this.passwordTextbox);

        this.authenticateButton = document.createElement("input");
        this.authenticateButton.type = "button";
        this.authenticateButton.value = "authenticate";
        this.authenticateButton.addEventListener("click", () => { this._onAuthenticateButtonClicked(); });
        authenticateBox.appendChild(this.authenticateButton);
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

    _onAuthenticateButtonClicked() {
        if (this._client.isConnected()) {
            const username = this.usernameTextbox.value;
            const password = this.passwordTextbox.value;

            this._client.authenticate("username", { username, password });
        }
    }

    _onConnectionOpened() {
        this._client.addProvider("test", this._provider);
        this.connectButton.value = "disconnect";
    }

    _onConnectionClosed() {
        this.connectButton.value = "connect";
    }

}

import { BadState } from "./bad_state.js";

export class Client {
    static get _PROTOCOL() { return "fs"; }

    constructor(provider) {
        this._provider = { };
        this._ws = null;
        this.onopen = () => { };
        this.onclose = () => { };
        this.onerror = () => { };
    }

    connectTo(url) {
        this.disconnect();

        this._ws = new WebSocket(url, Client._PROTOCOL);
        this._ws.onopen = this.onopen;
        this._ws.onclose = this.onclose;
        this._ws.onerror = this.onerror;

        this._ws.onmessage = (message) => {
            this._onmessage(message);
        };
    }

    authenticate(type, credentials) {
        const request = {
            "method": "authenticate",
            "params": [type, credentials],
            "id": 42
        };
        
        this._ws.send(JSON.stringify(request));
    }

    addProvider(name, provider) {
        this._provider[name] = provider;
        const request = {
            "method": "add_filesystem",
            "params": [name],
            "id": 23
        };
        
        this._ws.send(JSON.stringify(request));
    }

    disconnect() {
        if (this._ws) {
            this._ws.close();
            this._ws = null;
        }
    }

    isConnected() {
        return ((this._ws) && (this._ws.readyState === WebSocket.OPEN));
    }

    _onmessage(message) {
        try {
            const request = JSON.parse(message.data);
            const method = request.method;
            const id = request.id;
            const params = request.params;

            if ("string" !== typeof(method)) {
                throw new Error("parse error: missing field: \"method\"");
            }

            if (!params) {
                throw new Error("parse error: missing field: \"params\"");
            }

            if ("number" === typeof(request.id)) {
                this._invoke(method, params, id);
            }
            else {
                this._notify(method, params);
            }
        }
        catch (ex) {
            // swallow
        }
    }

    _invoke(method, params, id) {
        this._invokeAsync(method, params).
        then((result) => {
            const response = { result, id };
            this._ws.send(JSON.stringify(response));
        }).
        catch((ex) => {
            const code = ex.code || BadState.BAD;
            const response = {error: {code}, id};
            this._ws.send(JSON.stringify(response));
        });

    }

    async _invokeAsync(method, params) {
        switch(method)
        {
            case "lookup":
                return this._lookup(params);
            case "getattr":
                return this._getattr(params);
            case "readdir":
                return this._readdir(params);
            case "open":
                return this._open(params);
            case "read":
                return this._read(params);
            default:
                throw new BadState(BadState.NOT_IMPLEMENTED);
        }
    }

    _notify(method, params) {
        switch(method) {
            case 'close':
                this._close(params);
                break;
            default:
                throw new Error(`Invalid method: "${method}"`);
        }
    }

    _getProvider(name) {
        if (this._provider.hasOwnProperty(name)) {
            return this._provider[name];
        }
        else {
            throw new Error('Unknown provider');
        }
    }

    async _lookup([providerName, parent, name]) {
        const provider = this._getProvider(providerName);
        return provider.lookup(parent, name);
    }

    async _getattr([providerName, inode]) {
        const provider = this._getProvider(providerName);
        return provider.getattr(inode);
    }

    async _readdir([providerName, inode]) {
        const provider = this._getProvider(providerName);
        return provider.readdir(inode);
    }

    async _open([providerName, inode, mode]) {
        const provider = this._getProvider(providerName);
        return provider.open(inode, mode);
    }

    _close([providerName, inode, handle, mode]) {
        const provider = this._getProvider(providerName);
        provider.close(inode, handle, mode);
    }

    async _read([providerName, inode, handle, offset, length]) {
        const provider = this._getProvider(providerName);
        const data = await provider.read(inode, handle, offset, length);

        if ("string" === typeof(data)) {
			return {
				data: btoa(data),
				format: "base64",
				count: data.length
			};
        }
        else {
            throw new BadState(BadState.BAD);
        }
    }    
} 
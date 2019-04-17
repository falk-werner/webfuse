import { BadState } from "./bad_state.js";

export class Client {
    static get _PROTOCOL() { return "fs"; }

    constructor(provider) {
        this._provider = { };
        this._pendingRequests = {};
        this._id = 0;
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

    _invokeRequest(method, params) {
        this._id += 1;
        const id = this._id;
        const request = {method, params, id};

        return new Promise((resolve, reject) => {
            this._pendingRequests[id] = {resolve, reject};
            this._ws.send(JSON.stringify(request));
        });
    }

    authenticate(type, credentials) {
        return this._invokeRequest("authenticate", [type, credentials]);
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

    _isRequest(request) {
        const method = request.method;

        return (("string" === typeof(method)) && ("params" in request));
    }

    _isResponse(response) {
        const id = response.id;

        return (("number" === typeof(id)) && (("result" in response) || ("error" in response)));
    }

    _removePendingRequest(id) {
        let result = null;

        if (id in this._pendingRequests) {
            result = this._pendingRequests[id];
            Reflect.deleteProperty(this._pendingRequests, id);
        }

        return result;
    }

    _onmessage(message) {
        try {
            const data = JSON.parse(message.data);

            if (this._isRequest(data)) {
                const method = data.method;
                const id = data.id;
                const params = data.params;
        
                if ("number" === typeof(id)) {
                    this._invoke(method, params, id);
                }
                else {
                    this._notify(method, params);
                }    
            }
            else if (this._isResponse(data)) {
                const id = data.id;
                const result = data.result;
                const error = data.error;

                const request = this._removePendingRequest(id);
                if (request) {
                    if (result) {
                        request.resolve(result);
                    }
                    else {
                        request.reject(error);
                    }
                }
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
        if (name in this._provider) {
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
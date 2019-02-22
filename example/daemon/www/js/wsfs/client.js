import { BadState } from "./bad_state.js";

export class Client {
    static get _PROTOCOL() { return "fs"; }

    constructor(provider) {
        this._provider = provider;
        this._ws = null;
        this.isConnected = false;
    }

    connectTo(url) {
        this.disconnect();

        this._ws = new WebSocket(url, Client._PROTOCOL);
        this._ws.onopen = () => {
            this.isConnected = true;
            this._provider.connected(); 
        };
        this._ws.onclose = () => {
            this.isConnected = false;
            this._provider.disconnected();
        };
        this._ws.onmessage = (message) => {
            this._onmessage(message);
        };
    }

    disconnect() {
        if (this._ws) {
            this._ws.close();
        }
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

    async _lookup([parent, name]) {
        return this._provider.lookup(parent, name);
    }

    async _getattr([inode]) {
        return this._provider.getattr(inode);
    }

    async _readdir([inode]) {
        return this._provider.readdir(inode);
    }

    async _open([inode, mode]) {
        return this._provider.open(inode, mode);
    }

    _close([inode, handle, mode]) {
        this._provider.close(inode, handle, mode);
    }

    async _read([inode, handle, offset, length]) {
        const data = await this._provider.read(inode, handle, offset, length);

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
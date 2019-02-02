
class FileSystemHandler {

    constructor(filesystem, connection) {
        this._fs = filesystem;
        this._connection = connection;
        this._connection.onmessage = (message) => {
            this._onmessage(message); 
        };
    }

    _onmessage(message) {

        try {
            let request = JSON.parse(message.data);
            let result = -42;
            let response;
    
            console.log(request);
            if (("string" === typeof(request.method)) && 
                ("number" === typeof(request.id)) && 
                            (request.params)) {
                switch(request.method)
                {
                    case "getattr":
                    {
                        const path = request.params[0];
                        result = this._fs.getattr(path);
                    }
                    break;
                    case "getattr_ll":
                    {
                        const inode = request.params[0];
                        result = this._fs.getattr(inode);
                    }
                    break;
                    case "readdir":
                    {
                        const path = request.params[0];
                        result = this._fs.readdir(path);
                    }
                    break;
                    case "open":
                    {
                        const path = request.params[0];
                        const mode = request.params[1];
                        result = this._fs.open(path, mode);
                    }
                    break;
                    case "close":
                    {
                        const path = request.params[0];
                        const handle = request.params[1];
                        const mode = request.params[2];
                        result = this._fs.open(path, handle, mode);
                    }
                    break;
                    case "read":
                    {
                        const path = request.params[0];
                        const handle = request.params[1];
                        const offset = request.params[2];
                        const length = request.params[3];
                        result = this._fs.read(path, handle, offset, length);
                    }
                    break;
                    default:
                        break;
                }
                
                if ("number" !== typeof(result)) {
                    response = {result: result, id: request.id};
                }
                else {
                    response = {error: {code: result}, id: request.id};
                }
                console.log(response);
                this._connection.send(JSON.stringify(response));
            }
        }
        catch (ex) { console.log(ex, message); }	
    };    
}

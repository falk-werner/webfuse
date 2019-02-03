
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
                    case "lookup":
                    {
                        const parent = request.params[0];
                        const name = request.params[1];
                        result = this._fs.lookup(parent, name);
                    }
                    break;
                    case "getattr":
                    {
                        const inode = request.params[0];
                        result = this._fs.getattr(inode);
                    }
                    break;
                    case "readdir":
                    {
                        const inode = request.params[0];
                        result = this._fs.readdir(inode);
                    }
                    break;
                    case "open":
                    {
                        const inode = request.params[0];
                        const mode = request.params[1];
                        result = this._fs.open(inode, mode);
                    }
                    break;
                    case "close":
                    {
                        const inode = request.params[0];
                        const handle = request.params[1];
                        const mode = request.params[2];
                        result = this._fs.open(inode, handle, mode);
                    }
                    break;
                    case "read":
                    {
                        const inode = request.params[0];
                        const handle = request.params[1];
                        const offset = request.params[2];
                        const length = request.params[3];
                        result = this._fs.read(inode, handle, offset, length);
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

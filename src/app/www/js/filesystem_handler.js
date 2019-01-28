
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
                        result = this._fs.getattr(request.params[0]);
                        break;
                    case "readdir":
                        result = this._fs.readdir(request.params[0]);
                        break;
                    case "open":
                        result = this._fs.open(request.params[0], request.params[1]);
                        break;
                    case "read":
                        result = this._fs.read(request.params[0], request.params[1], request.params[2]);
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

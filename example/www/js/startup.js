function onmessage(message) {

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
                    result = fs.getattr(request.params[0]);
                    break;
                case "readdir":
                    result = fs.readdir(request.params[0]);
                    break;
                case "open":
                    result = fs.open(request.params[0], request.params[1]);
                    break;
                case "read":
                    result = fs.read(request.params[0], request.params[1], request.params[2]);
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
            ws.send(JSON.stringify(response));
        }
    }
    catch (ex) { console.log(ex, message); }	
};

function startup() {
    let connection = new Connection();
    let connectionView = new ConnectionView(connection);
    document.getElementById('connection').appendChild(connectionView.element);

    let fs = new FileSystem({
        inode: 1,
        mode: 0755,
        type: "dir",
        entries: {
            "hello.txt"   : { inode: 2, mode: 0444, type: "file", contents: "Hello, World!"},
            "say_hello.sh": { inode: 3, mode: 0555, type: "file", contents: "#!/bin/sh\necho hello\n"}
        }
    });
    let handler = new FileSystemHandler(fs, connection);


}

window.onload = startup;

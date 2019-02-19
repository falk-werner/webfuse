/* exported Connection */

class Connection {

    constructor() {
        this.ws = null;
        this.isConnected = false;
        this.onopen = () => {};
        this.onclose = () => {};
        this.onmessage = () => {};
    }

    connectTo(url) {
        if (this.ws) { this.close(); }

        this.ws = new WebSocket(url, "fs");        
        this.ws.onopen = () => { 
            this.isConnected = true;
            this.onopen();
        };
        this.ws.onclose = () => { 
            this.isConnected = false;
            this.onclose();
         };
         this.ws.onmessage = (message) => {
             this.onmessage(message);
         };
    }

    send(message) {
        this.ws.send(message);
    }

    close() {
        this.ws.close();
    }

}
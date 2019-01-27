function startup()
{
	var FileSystem = function(root) {
		this.root = root;
	};

	FileSystem.BAD_NO_ENTRY = -1;

	FileSystem.prototype.getEntry = function(path) {
		var items = path.split('/');
		var curItem = this.root;
		var item;
		var i, len;
	
		for(i = 0, len = items.length; curItem && (i < len); i++) {
			item = items[i];
			if ("" !== item) {
				curItem = curItem.entries && curItem.entries[item];
			}
		}

		return curItem;
	}

	FileSystem.prototype.getattr = function(path) {
		var entry = this.getEntry(path);
		if (entry) {
			return {
				mode: entry.mode || 0755,
				type: entry.type || 'file',
				size: entry.size || 0,
				atime: entry.atime || 0,
				mtime: entry.mtime || 0,
				ctime: entry.ctime || 0
			};
		}
		else {
			return FileSystem.BAD_NO_ENTRY;
		}
	};

	FileSystem.prototype.readdir = function(path) {
		var result, entry, subdir, i, len;

		entry = this.getEntry(path);
		if ((entry) && (entry.type == "dir")) {
			result = [".", ".."];
			for(subdir in entry.entries) {
				if (entry.entries.hasOwnProperty(subdir)) {
					result.push(subdir);
				}				
			}
		}
		else {
			result = FileSystem.BAD_NO_ENTRY;
		}

		return result;
	};

	var fs = new FileSystem({
		mode: 0755,
		type: "dir",
		entries: {
			"hello": { mode: 0755, type: "file", size: 10, contents: "Hello, World!"}
		}
	});

	var ws = new WebSocket('ws://localhost:4711/', 'fs');
	ws.onopen = function() {
		console.log('open');
	};
	ws.onclose = function() {
		console.log('close');
	};
	ws.onmessage = function(message) {
		console.log(message);

		try {
			var request = JSON.parse(message.data);
			var result = -42;
			var response;

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
		catch (ex) { console.log(ex); }	
	};

	var sendButton = document.getElementById('sendButton');
	sendButton.addEventListener('click', function() {
		var content = document.getElementById('content').value;		
		ws.send(content);
		console.log(content);
	});
}

document.onload=startup();

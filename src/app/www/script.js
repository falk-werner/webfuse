function startup()
{
	var FileSystem = function(root) {
		this.root = root;
	};

	FileSystem.GOOD = 0;
	FileSystem.BAD  = 1;

	FileSystem.BAD_NOTIMPLEMENTED = 2;
	FileSystem.BAD_TIMEOUT        = 3;
	FileSystem.BAD_FORMAT         = 4;

	FileSystem.BAD_NOENTRY = 101;
	FileSystem.BAD_NOACCESS = 102;

	FileSystem.O_ACCMODE = 00003;
	FileSystem.O_RDONLY  = 00000;
	FileSystem.O_WRONLY  = 00001;
	FileSystem.O_RDWR    = 00002;
	FileSystem.O_CREAT   = 00100;
	FileSystem.O_EXCL    = 00200;
	FileSystem.O_TRUNK   = 01000;
	FileSystem.O_APPEND  = 02000;
	

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
				size: entry.size || (entry.contents && entry.contents.length) || 0,
				atime: entry.atime || 0,
				mtime: entry.mtime || 0,
				ctime: entry.ctime || 0
			};
		}
		else {
			return FileSystem.BAD_NOENTRY;
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
			result = FileSystem.BAD_NOENTRY;
		}

		return result;
	};

	FileSystem.prototype.open = function(path, mode) {
		var result = FileSystem.BAD_NOENTRY;
		var entry = this.getEntry(path);
		
		if (entry.type == "file") {
			result = ((mode & FileSystem.O_ACCMODE) == FileSystem.O_RDONLY) ? true : FileSystem.BAD_NOACCESS;			
		}
	
		return result;
	};

	FileSystem.prototype.read = function(path, offset, length) {
		var result = FileSystem.BAD_NOENTRY;
		var entry = this.getEntry(path);
				
		if (entry.type == "file") {
			var end = Math.min(offset + length, entry.contents.length);
			var data = (offset < entry.contents.length) ? entry.contents.substring(offset, end) : "";	
			result = {
				data: data,
				format: "identity",
				count: data.length
			};
		}
	
		return result;
	};

	var fs = new FileSystem({
		mode: 0755,
		type: "dir",
		entries: {
			"hello": { mode: 0755, type: "file", /* size: 10 , */contents: "Hello, World!"}
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

		try {
			var request = JSON.parse(message.data);
			var result = -42;
			var response;

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

	var sendButton = document.getElementById('sendButton');
	sendButton.addEventListener('click', function() {
		var content = document.getElementById('content').value;		
		ws.send(content);
		console.log(content);
	});
}

document.onload=startup();

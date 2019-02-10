class FileSystem {
    static get GOOD() { return 0; }
    static get BAD()  { return 1; }

    static get BAD_NOTIMPLEMENTED()  { return 2; }
    static get BAD_TIMEOUT()         { return 3; }
    static get BAD_FORMAT()          { return 4; }

    static get BAD_NOENTRY()  { return 101; }
    static get BAD_NOACCESS() { return 102; }

    static get O_ACCMODE() { return 0x003; }
    static get O_RDONLY()  { return 0x000; }
    static get O_WRONLY()  { return 0x001; }
    static get O_RDWR()    { return 0x002; }
    static get O_CREAT()   { return 0x040; }
    static get O_EXCL()    { return 0x080; }
    static get O_TRUNK()   { return 0x200; }
    static get O_APPEND()  { return 0x400; }

    constructor(root) {
        this.root = root;
        this._inodes = { };
        
        this._walk(this.root, (entry) => { this._inodes[entry.inode] = entry; });
    }

    _walk(node, callback) {
        callback(node);

        const entries = node.entries;
        if (entries) {
            for(let entry of Object.entries(entries)) {
                this._walk(entry[1], callback);
            }
        }
    }
   
    
    lookup(parent, name) {
        const parentEntry = this._inodes[parent];
        const entry = (parentEntry && parentEntry.entries && parentEntry.entries[name]) || null;
        if (entry) {
			return {
                inode: entry.inode,
				mode: entry.mode || parseInt("755", 8),
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
    }


    getattr(inode) {
        let entry = this._inodes[inode];
        if (entry) {
			return {
				mode: entry.mode || parseInt("755", 8),
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
    }

    readdir(inode) {
        let result = FileSystem.BAD_NOENTRY;
        let entry = this._inodes[inode];

        if ((entry) && ("dir" === entry.type)) {
            result = [
                {name: '.', inode: entry.inode},
                {name: '..', inode: entry.inode}
            ];
            for(let subdir of Object.entries(entry.entries)) {
                const name = subdir[0];
                const inode = subdir[1].inode;
                result.push({name: name, inode: inode});               
            }
        }

        return result;
    }

    open(inode, mode) {
        let result = FileSystem.BAD_NOENTRY;
        let entry = this._inodes[inode];

		if (entry.type == "file") {
			result = ((mode & FileSystem.O_ACCMODE) == FileSystem.O_RDONLY) ? {handle: 1337} : FileSystem.BAD_NOACCESS;			
		}
	
		return result;
    }

    close(inode, handle, mode) {
        // do nothing
		return true;
    }

    read(inode, handle, offset, length) {
		let result = FileSystem.BAD_NOENTRY;
		let entry = this._inodes[inode];
				
		if (entry.type == "file") {
			let end = Math.min(offset + length, entry.contents.length);
			let data = (offset < entry.contents.length) ? entry.contents.substring(offset, end) : "";	
			result = {
				data: btoa(data),
				format: "base64",
				count: data.length
			};
		}
	
		return result;
    }
}
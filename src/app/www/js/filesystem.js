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
    }

    _getEntry(path) {
        let curItem = this.root;

        for(let item of path.split('/')) {
            if ('' !== item) {
                curItem = curItem.entries && curItem.entries[item];
                if (!curItem) {
                    return null;
                }
            }
        }

        return curItem;
    }

    getattr(path) {
        let entry = this._getEntry(path);
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
            return FilesSystem.BAD_NOENTRY;
        }
    }

    readdir(path) {
        let result = FileSystem.BAD_NOENTRY;
        let entry = this._getEntry(path);

        if ((entry) && ("dir" === entry.type)) {
            result = [".", ".."];
            for(let subdir of Object.keys(entry.entries)) {
                result.push(subdir);                
            }
        }

        return result;
    }

    open(path, mode) {
        let result = FileSystem.BAD_NOENTRY;
        let entry = this._getEntry(path);

		if (entry.type == "file") {
			result = ((mode & FileSystem.O_ACCMODE) == FileSystem.O_RDONLY) ? true : FileSystem.BAD_NOACCESS;			
		}
	
		return result;
    }

    read(path, offset, length) {
		let result = FileSystem.BAD_NOENTRY;
		let entry = this._getEntry(path);
				
		if (entry.type == "file") {
			let end = Math.min(offset + length, entry.contents.length);
			let data = (offset < entry.contents.length) ? entry.contents.substring(offset, end) : "";	
			result = {
				data: data,
				format: "identity",
				count: data.length
			};
		}
	
		return result;
    }
}
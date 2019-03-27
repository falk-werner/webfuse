/* eslint no-unused-vars: ["error", { "argsIgnorePattern": "^_" }] */

import { BadState } from "./webfuse/bad_state.js";
import { FileMode } from "./webfuse/file_mode.js";
import { Provider } from "./webfuse/provider.js";

export class FileSystemProvider extends Provider {
    constructor(root) {
        super();

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
   
    
    async lookup(parent, name) {
        const parentEntry = this._inodes[parent];
        const entry = (parentEntry && parentEntry.entries && parentEntry.entries[name]) || null;
        if (entry) {
			return {
                inode: entry.inode,
				mode: entry.mode || parseInt("755", 8),
				type: entry.type || "file",
				size: entry.size || (entry.contents && entry.contents.length) || 0,
				atime: entry.atime || 0,
				mtime: entry.mtime || 0,
				ctime: entry.ctime || 0
			};
        }
        else {
            throw new BadState(BadState.NO_ENTRY);
        }
    }


    async getattr(inode) {
        let entry = this._inodes[inode];
        if (entry) {
			return {
				mode: entry.mode || parseInt("755", 8),
				type: entry.type || "file",
				size: entry.size || (entry.contents && entry.contents.length) || 0,
				atime: entry.atime || 0,
				mtime: entry.mtime || 0,
				ctime: entry.ctime || 0
			};
        }
        else {
            throw new BadState(BadState.NO_ENTRY);
        }
    }

    async readdir(inode) {
        let entry = this._inodes[inode];

        if ((entry) && ("dir" === entry.type)) {
            let result = [
                {name: ".", inode: entry.inode},
                {name: "..", inode: entry.inode}
            ];
            for(let subdir of Object.entries(entry.entries)) {
                const name = subdir[0];
                const inode = subdir[1].inode;
                result.push({name, inode});               
            }

            return result;
        }
        else {
            throw new BadState(BadState.NO_ENTRY);
        }

    }

    async open(inode, mode) {
        let entry = this._inodes[inode];

		if (entry.type === "file") {
			if ((mode & FileMode.ACCESS_MODE) === FileMode.READONLY) {
                return {handle: 1337}; 
            }
            else {
                throw new BadState(BadState.NO_ACCESS);                
            }
        }
        else {
            throw new BadState(BadState.NO_ENTRY);
        }
    }

    close(_inode, _handle, _mode) {
        // do nothing
		return true;
    }

    async read(inode, handle, offset, length) {
		let entry = this._inodes[inode];
				
		if (entry.type === "file") {
			const end = Math.min(offset + length, entry.contents.length);
            const data = (offset < entry.contents.length) ? entry.contents.substring(offset, end) : "";

            return data;
        }
        else {
            throw new BadState(BadState.NO_ENTRY);
        }
    }
}
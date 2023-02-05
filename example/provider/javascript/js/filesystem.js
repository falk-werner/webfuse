
import { BaseFileSystem, ERRNO, Mode, AccessMode, OpenFlags } from "./webfuse/webfuse.js"

class FileSystem extends BaseFileSystem {

    constructor(tokenProvider, stateListener, files) {
        super();

        this.tokenProvider = tokenProvider;
        this.stateListener = stateListener
        this.files = new Map();
        for(const file of files) {
            this.files.set("/" + file.name, file);
        }
    }

    access(path, mode) {
        // we do not allow write or execute
        if ((mode & AccessMode.W_OK) || (mode & AccessMode.X_OK)) {
            return ERRNO.EACCES;
        }

        if ((path = "/") || (this.files.has(path))) {
            return 0;
        }

        return ERRNO.ENOENT;
    }

    getattr(path) {
        if (path == "/") {
            return {
                nlink: 2,
                mode: Mode.DIR | 0o555
            };
        }
        else if (this.files.has(path)) {
            const file = this.files.get(path);
            const contents = file.contents();
            return {
                nlink: 1,
                mode: Mode.REG | 0o444,
                size: contents.length
            };
        }

        return ERRNO.ENOENT;
    }

    readdir(path) {
        if (path == "/") {
            const list = [];
            for(const file of this.files.values()) {
                list.push(file.name);
            }
            return list;
        }
        
        return ERRNO.ENOENT;
    }

    open(path, flags) {
        if (this.files.has(path)) {
            const accessMode = flags & OpenFlags.ACCESS_MODE;
            if (accessMode == OpenFlags.RDONLY) {
                return [0, 0];
            }
            else {
                return [ERRNO.EPERM, 0];
            }
        }

        return [ERRNO.ENOENT, 0];
    }

    read(path, size, offset, fd) {
        if (this.files.has(path)) {
            const file = this.files.get(path);
            const contents = file.contents();
            if (offset < contents.length) {
                const available = contents.length - offset;
                const length = (size < available) ? size : available;
                const data = contents.slice(offset, offset + length);
                return data;
            }

            return [];
        }
        else {
            return ERRNO.EBADF;
        }
    }

    getcreds() {
        const token = this.tokenProvider();
        return token;
    }

    connectionstatechanged(state) {
        this.stateListener(state);
    }    
}

export { FileSystem }

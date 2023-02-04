
import { BaseFileSystem, ERRNO, Mode } from "./webfuse/webfuse.js"

class StaticFileSystem extends BaseFileSystem {

    constructor(files) {
        super();
        this.files = files;
    }

    getattr(path) {
        console.log("getattr", path);

        if (path == "/") {
            return {
                nlink: 2,
                mode: Mode.DIR | 0o555
            };
        }
        else if (this.files.has(path)) {
            const contents = this.files.get(path);
            return {
                nlink: 1,
                mode: Mode.REG | 0o444,
                size: contents.length
            }
        }

        return ERRNO.ENOENT;
    }

    readdir(path) {
        if (path == "/") {
            return ["foo", "bar"]
        }
        
        return ERRNO.ENOENT;
    }
}

export { StaticFileSystem }

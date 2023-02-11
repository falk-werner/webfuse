import { MessageWriter } from "./messagewriter.js";
import { MessageReader } from "./messagereader.js";
import { ERRNO } from "./errno.js";
import { AccessMode } from "./accessmode.js";
import { OpenFlags } from "./openflags.js";
import { BaseFileSystem } from "./basefilesystem.js";


const Mode = {
    REG  : 0o100000,
    DIR  : 0o040000,
    CHR  : 0o020000,
    BLK  : 0o060000,
    FIFO : 0o010000,
    LNK  : 0o120000,
    SOCK : 0o140000    
};



function fs_access(reader, writer, filesystem) {
    const path = reader.read_str();
    const mode = reader.read_u8();
    const result = filesystem.access(path, mode);
    writer.write_i32(result);
}

function fs_getattr(reader, writer, filesystem) {
    const path = reader.read_str();
    const result = filesystem.getattr(path);
    if (typeof(result) !== "number") {
        writer.write_i32(0);
        writer.write_u64(result.ino | 0);
        writer.write_u64(result.nlink | 0);
        writer.write_u32(result.mode | 0);
        writer.write_i32(result.uid | 0);
        writer.write_i32(result.gid | 0);
        writer.write_u64(result.dev | 0);
        writer.write_u64(result.size | 0);
        writer.write_u64(result.blocks | 0);
        writer.write_time(result.atime | 0);
        writer.write_time(result.mtime | 0);
        writer.write_time(result.ctime | 0);    
    }
    else {
        writer.write_i32(result);
    }
}

function fs_readlink(reader, writer, filesystem) {
    const path = reader.read_str();
    const result = filesystem.readlink(path);
    if (typeof(result) != "number") {
        writer.write_i32(0);
        writer.write_str(result);
    }
    else {
        writer.write_i32(result);
    }
}

function fs_symlink(reader, writer, filesystem) {
    const target = reader.read_str();
    const linkpath = reader.read_str();
    const result = filesystem.symlink(target, linkpath);
    writer.write_i32(result);
}


function fs_link(reader, writer, filesystem) {
    const oldpath =  reader.read_str();
    const newpath = reader.read_str();
    const result = filesystem.link(oldpath, newpath);
    writer.write_i32(result);
}

function fs_rename(reader, writer, filesystem) {
    const oldpath =  reader.read_str();
    const newpath = reader.read_str();
    const flags = reader.read_u8();
    const result = filesystem.rename(oldpath, newpath, flags);
    writer.write_i32(result);
}

function fs_chmod(reader, writer, filesystem) {
    const path = reader.read_str();
    const mode = reader.read_u32();
    const result = filesystem.chmod(path, mode);
    writer.write_i32(result);
}

function fs_chown(reader, writer, filesystem) {
    const path = reader.read_str();
    const uid = reader.read_u32();
    const gid = reader.read_u32();
    const result = filesystem.chown(path, uid, gid);
    writer.write_i32(result);
}

function fs_truncate(reader, writer, filesystem) {
    const path = reader.read_str();
    const size = reader.read_u64();
    const fd = reader.read_u64();
    const result = filesystem.truncate(path, size, fd);
    writer.write_i32(result);
}

function fs_fsync(reader, writer, filesystem) {
    const path = reader.read_str();
    const isDataSync = reader.read_bool();
    const fd = reader.read_fd();
    const result = filesystem.fsync(path, isDataSync, fd);
    writer.write_i32(result);
}

function fs_open(reader, writer, filesystem) {
    const path = reader.read_str();
    const flags = reader.read_u32();
    const [result, fd] = filesystem.open(path, flags);
    writer.write_i32(result);
    writer.write_u64(fd);
}

function fs_mknod(reader, writer, filesystem) {
    const path = reader.read_str();
    const mode = reader.read_u32();
    const rdev = reader.read_u64();
    const result = filesystem.mknod(path, mode, rdev);
    writer.write_i32(result);
}

function fs_create(reader, writer, filesystem) {
    const path = reader.read_str();
    const mode = reader.read_u32();
    const [result, fd] = filesystem.create(path, mode);
    writer.write_i32(result);
    writer.write_u64(fd);
}

function fs_release(reader, writer, filesystem) {
    const path = reader.read_str();
    const fd = reader.read_u64();
    const result = filesystem.release(path, fd);
    writer.write_i32(result);
}

function fs_unlink(reader, writer, filesystem) {
    const path = reader.read_str();
    const result = filesystem.unlink(path);
    writer.write_i32(result);
}

function fs_read(reader, writer, filesystem) {
    const path = reader.read_str();
    const size = reader.read_u32();
    const offset = reader.read_u64();
    const fd = reader.read_u64();
    const result = filesystem.read(path, size, offset, fd);
    if (typeof(result) != "number") {
        writer.write_i32(result.length);
        writer.write_bytes(result);
    }
    else {
        writer.write_i32(result);
    }
}

function fs_write(reader, wriuter, filesystem) {
    const path = reader.read_str();
    const data = reader.read_bytes();
    const offset = reader.read_u64();
    const fd = reader.read_u64();
    const result = filesystem.write(path, data, offset, fd);
    writer.write_i32(result);
}

function fs_mkdir(reader, writer, filesystem) {
    const path = reader.read_str()
    const mode = reader.read_u32();
    const result = filesystem.mkdir(path, mode);
    writer.write_i32(result);
}

function fs_readdir(reader, writer, filesystem) {
    const path = reader.read_str();
    const result = filesystem.readdir(path);
    if (typeof(result) != "number") {
        writer.write_i32(0);
        writer.write_strings(result);
    }
    else {
        writer.write_i32(result);
    }
}

function fs_rmdir(reader, writer, filesystem) {
    const path = reader.read_str();
    const result = filesystem.rmdir(path);
    writer.write_i32(result);
}

function fs_statfs(reader, writer, filesystem) {
    const path = reader.read_str();
    const result = filesystem.statfs(path);
    if (typeof(result) != "number") {
        writer.write_i32t(0)
        writer.write_u64(result.bsize | 0);
        writer.write_u64(result.frsize | 0);
        writer.write_u64(result.blocks | 0);
        writer.write_u64(result.bfree | 0);
        writer.write_u64(result.bavail | 0);
        writer.write_u64(result.files | 0);
        writer.write_u64(result.ffree | 0);
        writer.write_u64(result.namemax | 0);
    }
    else {
        writer.write_i32(result);
    }
}

function fs_utimens(reader, writer, filesystem) {
    const path = reader.read_str();
    const atime = reader.read_time();
    const mtime = reader.read_time();
    const result = filesystem.utimens(path, atime, mtime);
    writer.write_i32(result);
}

function fs_getcreds(reader, writer, filesystem) {
    const credentials = filesystem.getcreds();
    writer.write_str(credentials);
}

const commands = new Map([
    [0x01, fs_access],
    [0x02, fs_getattr],
    [0x03, fs_readlink],
    [0x04, fs_symlink],
    [0x05, fs_link],
    [0x06, fs_rename],
    [0x07, fs_chmod],
    [0x08, fs_chown],
    [0x09, fs_truncate],
    [0x0a, fs_fsync],
    [0x0b, fs_open],
    [0x0c, fs_mknod],
    [0x0d, fs_create],
    [0x0e, fs_release],
    [0x0f, fs_unlink],
    [0x10, fs_read],
    [0x11, fs_write],
    [0x12, fs_mkdir],
    [0x13, fs_readdir],
    [0x14, fs_rmdir],
    [0x15, fs_statfs],
    [0x16, fs_utimens],
    [0x17, fs_getcreds]
]);

class Webfuse {

    constructor(url, filesystem) {
        this.ws = new WebSocket(url, ["webfuse2"]);
        this.ws.binaryType = 'arraybuffer';
        this.ws.addEventListener('open', (event) => this.on_connected(event));
        this.ws.addEventListener('close', (event) => this.on_closed(event));
        this.ws.addEventListener('error', (event) => this.on_error(event));
        this.ws.addEventListener('message', (event) => this.on_message(event));

        this.filesystem = filesystem;
    }

    close() {
        this.ws.close();
    }

    on_message(event) {
        const reader = new MessageReader(event.data);
        const message_id = reader.read_u32();
        const message_type = reader.read_u8();

        const writer = new MessageWriter(message_id, 0x80 + message_type);
        if (commands.has(message_type)) {
            const command = commands.get(message_type);
            command(reader, writer, this.filesystem);
        }
        else {
            console.warn(`unknow message type: ${message_type}`);
        }

        this.ws.send(writer.get_data());
    }

    on_connected(event) {
        this.filesystem.connectionstatechanged("connected");
    }

    on_error(event) {
        console.info("connection error");
        this.ws.close();
    }

    on_closed(event) {
        this.filesystem.connectionstatechanged("closed");
    }

}

export { Webfuse, BaseFileSystem, ERRNO, Mode, AccessMode, OpenFlags }

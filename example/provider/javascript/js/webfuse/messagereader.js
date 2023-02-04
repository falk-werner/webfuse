class MessageReader {
    
    constructor(data) {
        // console.log(new Uint8Array(data));
        this.raw = data;
        this.data = new DataView(data);
        this.pos = 0;
        this.decoder = new TextDecoder('utf-8');
    }

    read_u8() {
        const result = this.data.getUint8(this.pos);
        this.pos++;
        return result;
    }

    read_bool() {
        return this.read_u8() == 1;
    }

    read_u32() {
        const result = this.data.getUint32(this.pos);
        this.pos += 4;
        return result;
    }

    read_u64() {
        const result = this.data.getBigUint64(this.pos);
        this.pos += 8;
        return Number(result);
    }

    read_str() {
        const length = this.read_u32();
        if (length > 0) {
            const view = new Uint8Array(this.raw, this.pos, length);
            this.pos += length;
            return this.decoder.decode(view);
        }
        else {
            return "";
        }
    }

    read_bytes() {
        const length = this.read_u32();
        if (length > 0) {
            const view = new Uint8Array(this.raw, this.pos, length);
            this.pos += length;
            return view;
        }
        else {
            return [];
        }
    }

}

export { MessageReader }
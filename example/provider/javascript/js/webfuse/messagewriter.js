class MessageWriter {

    constructor(message_id, message_type) {
        this.data = [ ]
        this.write_u32(message_id)
        this.write_u8(message_type)
        this.encoder = new TextEncoder("utf-8");
    }

    write_u8(value) {
        this.data.push(value)
    }

    write_u32(value) {
        const buffer = new ArrayBuffer(4);
        const view = new DataView(buffer);
        view.setUint32(0, value);

        const data = new Uint8Array(buffer);
        this.data.push(...data);
    }

    write_i32(value) {
        const buffer = new ArrayBuffer(4);
        const view = new DataView(buffer);
        view.setInt32(0, value);

        const data = new Uint8Array(buffer);
        this.data.push(...data);
    }

    write_u64(value) {
        const buffer = new ArrayBuffer(8);
        const view = new DataView(buffer);
        view.setBigUint64(0, BigInt(value));

        const data = new Uint8Array(buffer);
        this.data.push(...data);
    }

    // value in milliseconds
    write_time(value) {
        const seconds = Math.floor(value / 1000);
        const millis = value % 1000;
        const nanos = millis * 1000 * 1000;

        this.write_u64(seconds);
        this.write_u32(nanos);
    }

    write_str(value) {
        const data = this.encoder.encode(value);
        this.write_u32(data.length);
        this.data.push(...data);
    }

    write_strings(list) {
        this.write_u32(list.length);
        for(const item of list) {
            this.write_str(item);
        }
    }

    write_bytes(value) {
        this.write_u32(value.length);
        this.data.push(...value);
    }

    get_data() {
        return new Uint8Array(this.data);
    }

}

export { MessageWriter }
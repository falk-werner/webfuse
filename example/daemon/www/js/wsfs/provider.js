/* eslint no-unused-vars: ["error", { "argsIgnorePattern": "^_" }] */

import { BadState } from "./bad_state.js";

export class Provider {

    async lookup(_parent, _name) {
        throw new BadState(BadState.NOT_IMPLEMENTED);
    }

    async getattr(_inode) {
        throw new BadState(BadState.NOT_IMPLEMENTED);
    }

    async readdir(_inode) {
        throw new BadState(BadState.NOT_IMPLEMENTED);
    }

    async open(_inode, _mode) {
        throw new BadState(BadState.NOT_IMPLEMENTED);
    }

    close(_inode, _handle, _mode) {
        // empty
    }

    async read(_inode, _handle, _offset, _length) {
        throw new BadState(BadState.NOT_IMPLEMENTED);
    }
}

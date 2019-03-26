export class FileMode {
    static get ACCESS_MODE() { return 0x003; }
    static get READONLY()    { return 0x000; }
    static get WRITEONLY()   { return 0x001; }
    static get READWRITE()   { return 0x002; }
    static get CREATE()      { return 0x040; }
    static get EXCLUSIVE()   { return 0x080; }
    static get TRUNKATE()    { return 0x200; }
    static get APPEND()      { return 0x400; }    
}
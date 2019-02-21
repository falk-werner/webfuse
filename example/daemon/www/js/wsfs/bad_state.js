export class BadState extends Error {
    static get BAD()  { return 1; }

    static get NOT_IMPLEMENTED()  { return 2; }
    static get TIMEOUT()         { return 3; }
    static get FORMAT()          { return 4; }

    static get NO_ENTRY()  { return 101; }
    static get NO_ACCESS() { return 102; }

    constructor(code) {
        super("Bad State");
        this.code = code;
    }
}
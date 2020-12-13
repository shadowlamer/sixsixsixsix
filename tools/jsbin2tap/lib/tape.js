const TAPE_SIZE = 65535;
const HEADER_SIZE = 19;

const fieldType = {
    BYTE:   'byte',
    UINT:   'uint',
    STRING: 'string'
}

const blockType = {
    HEADER: 0x00,
    DATA:   0xff
}

const headerType = {
    PROGRAM:    0,
    NUM_ARRAY:  1,
    CHAR_ARRAY: 2,
    BYTES:      3
}

const tapeStructure = {
    block: {
        length: {type: fieldType.UINT},
        type: {type: fieldType.BYTE},
        header: {
            type: {type: fieldType.BYTE},
            name: {size: 10, type: fieldType.STRING},
            dataLength: {type: fieldType.UINT},
            params: {
                start: {type: fieldType.UINT},
                extra: {type: fieldType.UINT},
            }
        },
        data: {
            length: {type: fieldType.UINT}
        }
    }
}

class Tape {
    constructor() {
        this.tape = Buffer.alloc(TAPE_SIZE);
        this.pointer = 0;
        this.resetCRC();
    }

    getPointer() {
        return this.pointer;
    }

    rewind(val = 0) {
        this.pointer = val;
    }

    writeField(field, val) {
        switch (field.type) {
            case fieldType.BYTE:
                this.tape.writeUInt8(val, this.pointer);
                this.calcCRC(val);
                this.pointer++;
                break;
            case fieldType.UINT:
                this.tape.writeUInt16LE(val, this.pointer);
                this.calcCRC(this.tape.readUInt8(this.pointer));
                this.calcCRC(this.tape.readUInt8(this.pointer + 1));
                this.pointer += 2;
                break;
            case fieldType.STRING:
                this.tape.write(val.padEnd(field.size, ' '), this.pointer, field.size, 'ascii');
                for (let i = 0; i < field.size; i++) {
                    this.calcCRC(this.tape.readUInt8(this.pointer + i));
                }
                this.pointer += field.size;
                break;
        }
    }

    resetCRC(crc = 0x00) {
        this.crc = crc;
    }

    writeCRC() {
        this.tape.writeUInt8(this.crc, this.pointer);
        this.pointer++;1
    }

    calcCRC(byte) {
        this.crc ^= byte;
    }

    writeHeader(name, length, type, start, extra = 32768) {
        this.writeField(tapeStructure.block.length, HEADER_SIZE);
        this.writeField(tapeStructure.block.type, blockType.HEADER);
        this.resetCRC();
        this.writeField(tapeStructure.block.header.type, type);
        this.writeField(tapeStructure.block.header.name, name);
        this.writeField(tapeStructure.block.header.dataLength, length);
        this.writeField(tapeStructure.block.header.params.start, start);
        this.writeField(tapeStructure.block.header.params.extra, extra);
        this.writeCRC();
    }

    writeBytes(data) {
        this.writeField(tapeStructure.block.data.length, data.length + 2);
        this.resetCRC();
        this.writeField(tapeStructure.block.type, blockType.DATA);
        for (let i = 0; i < data.length; i++) {
            let byte = data.readUInt8(i);
            this.tape.writeUInt8(byte, this.pointer + i);
            this.calcCRC(byte);
        }
        this.pointer += data.length;
        this.writeCRC();
    }

    writeBytesBlock(data, name, start) {
        this.writeHeader(name, data.length, headerType.BYTES, start);
        this.writeBytes(data);
    }

    getBuffer() {
        return this.tape.slice(0, this.pointer);
    }
}

module.exports = new Tape();
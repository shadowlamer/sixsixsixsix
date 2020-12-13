#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const options = require('../lib/options');
const tape = require('../lib/tape');
const pjson = require('../package.json');

const READ_BUF_SIZE = 65536;

if (options.get('version')) {
    showVersion();
    process.exit(0);
}

if (options.get('help') || 0 === options.getInput().length) {
    showUsage();
    process.exit(0);
}

const outputFileName = options.get('output');
const startAddress = options.get('address');
const append = options.get('append');
const inputFileNames = options.getInput();


const outputFile = fs.openSync(outputFileName, append ? 'a' : 'w')
inputFileNames.forEach(inputFileName => {
    const blockName = getBlockName(inputFileName);
    const inputFile = fs.openSync(inputFileName, 'r');
    const readBuf = Buffer.alloc(READ_BUF_SIZE);

    const numBytesRead = fs.readSync(inputFile, readBuf, 0, readBuf.length, 0);
    tape.rewind();
    tape.writeBytesBlock(readBuf.slice(0, numBytesRead), blockName, startAddress);
    fs.writeSync(outputFile, tape.getBuffer());
    fs.closeSync(inputFile);
});
fs.closeSync(outputFile);

function getBlockName(name) {
    return path.basename(name, path.extname(name));
}

function showVersion () {
    console.log(`${pjson.name} ${pjson.version}`);
}

function showUsage () {
    showVersion();
    console.log(`Usage: ${pjson.name} [options] file.bin [file2.bin ...]`);
    options.info();
}

#!/usr/bin/env node

const options = require('../lib/options');
const fs = require('fs');
const pjson = require('../package.json');


const SCR_WIDTH = 32;
const SCR_HEIGHT = 192;

const spriteWidth = Number(options.get('width'));
const spriteHeight = Number(options.get('height'));
const prefix = options.get('prefix').toUpperCase();
const objName = options.get('name').toUpperCase();
const outFile = options.get('outfile');

let result = [];

if (options.get('help') || 0 === options.getInput().length) {
    showUsage();
    process.exit(0);
}

options.getInput().forEach(filename => {
        result.push(...processFile(filename));
    }
);

out =
`#ifndef ${prefix}_${objName}_H
#define ${prefix}_${objName}_H
#define ${objName}_NUM ${result.length}
#define ${objName}_WIDTH ${spriteWidth}
#define ${objName}_HEIGHT ${spriteHeight}
#define ${objName}_SIZE ${spriteWidth * spriteHeight}  
static const char ${objName.toLowerCase()}[${objName}_NUM][${objName}_SIZE] = {
${result.map(sprite => `  {${[...sprite].join(", ")}}`).join(",\n")}
};
#endif // ${prefix}_${objName}_H
`;

fs.writeFileSync(outFile, out);

function processFile(filename) {
    console.log(`Processing ${filename}`);
    const data = fs.readFileSync(filename);
    let sprites = [];
    for (let y = 0; y < SCR_HEIGHT; y += spriteHeight) {
        for (let x = 0; x < SCR_WIDTH; x += spriteWidth) {
            sprites.push(extractSprite(data, x,y, spriteWidth, spriteHeight));
        }
    }
    return sprites.filter(notEmpty);
}

/**
 * Get byte from screen buffer by screen coordinates
 * @param data screen buffer
 * @param x
 * @param y
 * @returns {number|*}
 */
function get(data, x, y) {
    if (x >= SCR_WIDTH || x < 0 || y >= SCR_HEIGHT || y < 0) return 0;
    const scrIndex = ((y & 0xc0) << 5) | ((y & 0x07) << 8) | ((y & 0x38) << 2) | (x & 0x1f);
    return data[scrIndex];
}

function extractSprite(data, x, y, width, height) {
    const spriteData = Buffer.alloc(width * height);
    let p = 0;
    for (let h = 0; h < height; h++) {
        for (let w = 0; w < width; w++) {
            spriteData[p] = get(data, x + w, y + h);
            p++;
        }
    }
    return spriteData;
}

function notEmpty(data) {
    return data.reduce((a, v) => (a || v));
}

function showUsage () {
    console.log(`Usage: ${pjson.name} [options] file.svg [file2.svg ...]`);
    options.info();
}

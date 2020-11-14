#!/usr/bin/env node

const options = require('../lib/options');
const xml2js = require('xml2js');
const parser = new xml2js.Parser();
const fs = require('fs');
const pjson = require('../package.json');

const MAX_ANGLE = 9;
const MIN_ANGLE = -9;
const DEFAULT_ANGLE = 0;
const AHEAD = 5;

const outDir = options.get('directory')
const guardPrefix = options.get('prefix')

if (options.get('help') || 0 === options.getInput().length) {
    showUsage();
    process.exit(0);
}

options.getInput().forEach(filename => processFile(filename));

function processFile(filename) {
    let xml_data = fs.readFileSync(filename, "utf8");
    parser.parseString(xml_data, function(error, result) {
        if(error === null) {
            if (result.svg) {
                processSvg(result.svg);
            }
        }
        else {
            console.log(error);
        }
    });
}

function processSvg(svg) {
    let pixelWidth = svg.$.width;
    let pixelHeight = svg.$.height;
    let viewportWidth = svg.$.viewBox.split(' ')[2];
    let viewportHeight = svg.$.viewBox.split(' ')[3];
    let qx = pixelWidth / viewportWidth;
    let qy = pixelHeight / viewportHeight;
    if (svg.g) {
        svg.g.forEach(g => processG(g, qx, qy, pixelWidth, pixelHeight));
    }
}

function processG(g, qx, qy, maxx, maxy) {
    let tx = parseFloat(g.$.transform.match(/translate\(([0-9\-.])+,([0-9\-.]+)\)/)[1]);
    let ty = parseFloat(g.$.transform.match(/translate\(([0-9\-.])+,([0-9\-.]+)\)/)[2]);
    if (g.path) {
        g.path.forEach(path => processPath(path, qx, qy, tx, ty, maxx, maxy));
    }
}

function processPath(path, qx, qy, tx, ty, maxx, maxy) {
    const name = path.$.id || 'out';
    const pathElements = path.$.d.split(' ');
    let ox = parseFloat(pathElements[1].split(',')[0]) + tx;
    let oy = parseFloat(pathElements[1].split(',')[1]) + ty;

    const deltas = pathElements.slice(Math.max(pathElements.indexOf('l') + 1, 2)).map(p => {
        let pp = p.split(',');
        return({
            dx: parseFloat(pp[0]),
            dy: parseFloat(pp[1])
        })
    }).filter(d => !isNaN(d.dx + d.dy));

    deltas.unshift({dx:0, dy:0}); //add first point

    let point = {
        x: ox * qx,
        y: oy * qy
    };

    const points = deltas.map(p => {
        let newPoint = {
            x: point.x + p.dx * qx,
            y: point.y + p.dy * qy
        };
        point = newPoint;
        return newPoint;
    });

    let triplex = [{}, {}, {}];

    let filteredAangle = undefined;
    const FILTER = 5;
    triplex = points.slice(points.length - 3);
    const trackPoints = points.map(p => {
        let angle = find_angle(triplex[0], triplex[1], triplex[2]) || DEFAULT_ANGLE;
        let normalizedAngle = angle *150;
        if (normalizedAngle > MAX_ANGLE) normalizedAngle = MAX_ANGLE;
        if (normalizedAngle < MIN_ANGLE) normalizedAngle = MIN_ANGLE;
        filteredAangle = (filteredAangle === undefined) ? normalizedAngle : (filteredAangle * (FILTER - 1) + normalizedAngle) / FILTER;
        triplex[0] = triplex[1];
        triplex[1] = triplex[2];
        triplex[2] = p;
        return {
        x: Math.round(p.x),
        y: Math.round(maxy - p.y),
        t: Math.round(-filteredAangle)
    }
    });
    correctAhead(trackPoints, AHEAD);
    out(trackPoints, name);
}

function find_angle(A,B,C) {
    let BA= {x: B.x - A.x, y: B.y - A.y};
    let CA= {x: C.x - A.x, y: C.y - A.y};  // vector subtraction, to get vector between points
    let dot = BA.x * CA.x + BA.y * CA.y;
    let pcross = BA.x * CA.y - BA.y * CA.x;
    return Math.atan2(pcross, dot);
}

function out(trackPoints, name) {
    const outData =
`#ifndef ${guardPrefix}_${name.toUpperCase()}_H
#define ${guardPrefix}_${name.toUpperCase()}_H
#define ${name.toUpperCase()}_SIZE ${trackPoints.length}
static const track_element_t ${name.toLowerCase()}[${name.toUpperCase()}_SIZE] = {
${trackPoints.map(p => `    {.x=${p.x}, .y=${p.y}, .t=${p.t} }`).join(',\n') + ','}
};
#endif //${guardPrefix}_${name.toUpperCase()}_H`;
    fs.writeFileSync(outDir + '/' + name + '.h', outData);
}

function showUsage () {
    console.log(`Usage: ${pjson.name} [options] file.svg [file2.svg ...]`);
    options.info();
}

function correctAhead(points, ahead) {
    points.forEach((point, index, points) => {
        point.t = points[index + ahead] ? points[index + ahead].t : 0;
    });
}

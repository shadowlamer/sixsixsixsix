const minimist = require('minimist')

const options = [
    {
        description: 'out file',
        long: 'outfile',
        short: 'o',
        default: 'sprites.h'
    },
    {
        description: 'array name',
        long: 'name',
        short: 'n',
        default: 'sprites'
    },
    {
        description: 'guard prefix',
        long: 'prefix',
        short: 'p',
        default: '_'
    },
    {
        description: 'sprite width in bytes',
        long: 'width',
        short: 'w',
        default: 2
    },
    {
        description: 'sprite height in bytes',
        long: 'height',
        short: 'h',
        default: 16
    },
    {
        description: 'print usage information',
        long: 'help',
        short: '?',
        default: false
    }
]

const argv = minimist(process.argv.slice(2), {
    string: options
        .filter(o => (o.default !== true && o.default !== false))
        .map(o => (o.long)),
    boolean: options
        .filter(o => (o.default === true || o.default === false))
        .map(o => (o.long)),
    alias: options.reduce((map, o) => {
        map[o.short] = o.long;
        return map;
    }, {}),
    default: options.reduce((map, o) => {
        map[o.short] = o.default;
        return map;
    }, {})
})

class Options {
    constructor() {
        this.options = options;
    }

    get(opt) {
        return argv[opt];
    }

    getInput() {
        return argv['_'];
    }

    info() {
        console.log('Options:')
        options.forEach(option => {
            console.log(`\t-${option.short.padEnd(2)}| --${option.long.padEnd(20)}\t${option.description} [${option.default}]`);
        })
    }
}

module.exports = new Options();
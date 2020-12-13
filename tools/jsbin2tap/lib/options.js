const minimist = require('minimist')

const options = [
    {
        description: 'output TAP file',
        long: 'output',
        short: 'o',
        default: 'out.tap'
    },
    {
        description: 'start address of binary file',
        long: 'address',
        short: 'a',
        default: 32768
    },
    {
        description: 'append tap at end of file',
        long: 'append',
        short: 'p',
        default: false
    },
    {
        description: 'print usage information',
        long: 'help',
        short: 'h',
        default: false
    },
    {
        description: 'print version information',
        long: 'version',
        short: 'v',
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
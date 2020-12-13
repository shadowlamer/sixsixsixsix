# jsbin2tap

Adds binary data to ZX Spectrum tape files.

### Usage

#### Without installation
```
npm start -- [options] input.bin [input2.bin ...]
```

#### With installation
```
sudo npm install -g jsbin2tap
jsbin2tap [options] input.bin [input2.bin ...]
```

#### As npm module
```
npm install --save jsbin2tap
```
...and then from JS...
```
const fs = require('fs');
const tape = require('jsbin2tap');
...
tape.writeBytesBlock(buf, blockName, startAddress);
fs.writeSync(outputFile, tape.getBuffer());
```

### Options

|Short | Long                  | description                 | Default |
|------|-----------------------|-----------------------------|---------|
|	-o | --output              |output TAP file              |out.tap  |
|	-a | --address             |start address of binary file |32768    |
|	-p | --append              |append tap at end of file    |false    |
|	-h | --help                |print usage information      |false    |
|	-v | --version             |print version information    |false    |
s
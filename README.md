# shared-buffer

A minimal node.js package that provides an ArrayBuffer that can be used across processes for interprocess-communication.

***

## Installation

```bash
  npm install shared-buffer --save
```

## Usage

```typescript
  /**
  * Allocate a new shared buffer.
  * @param key Any unique number to identify the memory segment we're allocating. Must be an integer.
  * @param size The size of the segment in bytes. Must be an integer.
  * @param initialize (optional) If true, will initialize a new segment, if false, will acquire an already existing one. 
  */
  declare function createSharedBuffer(key: number, size: number, initialize?: boolean): ArrayBuffer;
  
  /**
  * Explicitly free a shared buffer.
  * @param buffer The shared buffer to free.
  */
  declare function detachSharedBuffer(buffer: ArrayBuffer): void;
```

## Example

```js
  
  const cluster = require('cluster');
  const { createSharedBuffer, detachSharedBuffer } = require('shared-buffer');
  
  const SEGMENT_ID = 1024;
  const SEGMENT_ELEMENTS = 10;
  const SEGMENT_SIZE = SEGMENT_ELEMENTS * Float64Array.BYTES_PER_ELEMENT;
  
  if (cluster.isMaster) {
    const sharedBuffer = createSharedBuffer(SEGMENT_ID, SEGMENT_SIZE, true);
    const sharedArray = new Float64Array(sharedBuffer);
    for(let i = 0; i < SEGMENT_ELEMENTS; i++)
      sharedArray[i] = 1/i;
    cluster.fork();
  } else {
    const sharedBuffer = createSharedBuffer(SEGMENT_ID, SEGMENT_SIZE);
    const sharedArray = new Float64Array(sharedBuffer);
    console.log(sharedArray);
    // the buffer will be automatically detached, but it can be done explicitly
    detachSharedBuffer(sharedBuffer); 
    process.exit(0);
  }

/**
 * Output:
 * Float64Array [
 *   Infinity,
 *   1,
 *   0.5,
 *   0.3333333333333333,
 *   0.25,
 *   0.2,
 *   0.16666666666666666,
 *   0.14285714285714285,
 *   0.125,
 *   0.1111111111111111 ]
 */
```

## Compatibility

POSIX only for now, sorry.

## License (MIT)

Copyright 2017, Abdullah Ali

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
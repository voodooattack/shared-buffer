const cluster = require('cluster');
const { createSharedBuffer, detachSharedBuffer } = require('../index.js');

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
  let sharedBuffer = createSharedBuffer(SEGMENT_ID, SEGMENT_SIZE);
  let sharedArray = new Float64Array(sharedBuffer);
  console.log(sharedArray);
  detachSharedBuffer(sharedBuffer);
  sharedArray = null;
  sharedBuffer = null;
  gc();
  process.exit(0);
}

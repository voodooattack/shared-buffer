const cluster = require('cluster');
const { createSharedBuffer, detachSharedBuffer } = require('../index.js');

const SEGMENT_ID = 1024;
const SEGMENT_ELEMENTS = 10;
const SEGMENT_SIZE = SEGMENT_ELEMENTS * Float64Array.BYTES_PER_ELEMENT;

if (cluster.isMaster) {
  console.log(`MASTER: Creating shared buffer with ID ${SEGMENT_ID} and size ${SEGMENT_SIZE} bytes`);
  const sharedBuffer = createSharedBuffer(SEGMENT_ID, SEGMENT_SIZE, true);
  if (!sharedBuffer) {
    console.error(`MASTER: Failed to create shared buffer with ID ${SEGMENT_ID}`);
    process.exit(1);
  }
  console.log(`MASTER: Shared buffer created with ID ${SEGMENT_ID} and size ${SEGMENT_SIZE} bytes`);
  const sharedArray = new Float64Array(sharedBuffer);
  for(let i = 0; i < SEGMENT_ELEMENTS; i++)
    sharedArray[i] = 1/i;
  cluster.fork();
} else {
  console.log(`WORKER: Accessing shared buffer with ID ${SEGMENT_ID} and size ${SEGMENT_SIZE} bytes`);
  let sharedBuffer = createSharedBuffer(SEGMENT_ID, SEGMENT_SIZE);
  if (!sharedBuffer) {
    console.error(`WORKER: Failed to create or access shared buffer with ID ${SEGMENT_ID}`);
    process.exit(1);
  }   
  console.log(`WORKER: Shared buffer accessed with ID ${SEGMENT_ID} and size ${SEGMENT_SIZE} bytes`);
  let sharedArray = new Float64Array(sharedBuffer);
  console.log(`WORKER: Shared buffer values: ${sharedArray.join(', ')}`);
  detachSharedBuffer(sharedBuffer);
  gc();
  process.exit(0);
}

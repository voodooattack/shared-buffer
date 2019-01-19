declare module "shared-buffer" {
  /**
   * Allocate a new shared buffer.
   * @param key Any unique number to identify the memory segment we're allocating. Must be an integer.
   * @param size The size of the segment in bytes. Must be an integer.
   * @param initialize (optional) If true, will initialize a new segment, if false, will acquire an already existing one.
   */
  export function createSharedBuffer(key: number, size: number, initialize?: boolean): ArrayBuffer;
  /**
   * Explicitly free a shared buffer.
   * @param buffer The shared buffer to free.
   */
  export function detachSharedBuffer(buffer: ArrayBuffer): void;
}

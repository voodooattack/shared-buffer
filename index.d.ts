declare namespace "shared-buffer" {
  export function createSharedBuffer(key: number, size: number, initialize?: boolean): ArrayBuffer;
  export function detachSharedBuffer(buffer: ArrayBuffer): void;
}

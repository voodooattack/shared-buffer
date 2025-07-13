#include "v8-array-buffer.h"
#include <iostream>
#include <nan.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

NAN_METHOD(createSharedBuffer) {
  v8::Isolate *isolate = info.GetIsolate();
  Nan::HandleScope scope;

  if (!info[0]->IsUint32()) {
    Nan::ThrowError("Key must be an integer");
    return;
  } else if (!info[1]->IsUint32()) {
    Nan::ThrowError("Size must be an integer");
    return;
  }

  key_t key = info[0]->Uint32Value(isolate->GetCurrentContext()).FromJust();
  size_t size = info[1]->Uint32Value(isolate->GetCurrentContext()).FromJust();
  bool initZero = info[2]->BooleanValue(isolate);

  if (size == 0) {
    Nan::ThrowError("Size must be greater than 0");
    return;
  }

  key_t shmId = shmget(key, size, initZero ? IPC_CREAT | 0666 : 0666);

  if (shmId < 0) {
    Nan::ThrowError(strerror(errno));
    return;
  }

  char *data = (char *)shmat(shmId, NULL, 0);

  if (data == (char *)-1) {
    Nan::ThrowError(strerror(errno));
    return;
  }

  if (initZero)
    memset(data, 0, size);

  auto backingStore = v8::SharedArrayBuffer::NewBackingStore(
      data, size,
      [](void *data, size_t length, void *deleter_data) {
        // This callback is called when the buffer is garbage collected.
        shmdt(data);
      },
      nullptr);

  auto arrayBuffer =
      v8::SharedArrayBuffer::New(isolate, std::move(backingStore));

  arrayBuffer->SetInternalField(0, Nan::New<v8::External>((void *)(intptr_t)shmId));

  info.GetReturnValue().Set(arrayBuffer);
}

NAN_METHOD(detachSharedBuffer) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (!info[0]->IsSharedArrayBuffer()) {
    Nan::ThrowError("Argument must be a SharedArrayBuffer");
    return;
  }

  v8::Local<v8::SharedArrayBuffer> arrayBuffer =
      info[0].As<v8::SharedArrayBuffer>();

  if (!arrayBuffer->GetBackingStore()) {
    Nan::ThrowError("ArrayBuffer is already detached or invalid");
    return;
  }

  auto shmId =
      (key_t)(intptr_t)arrayBuffer->GetInternalField(0).As<v8::External>()->Value();

  if (shmId < 0) {
    Nan::ThrowError("Invalid SharedArrayBuffer");
    return;
  }

  if (shmId == 0) {
    Nan::ThrowError("ArrayBuffer is already detached or not a shared memory segment");
    return;
  }

  arrayBuffer->SetInternalField(0, Nan::New<v8::External>((void *)(intptr_t)0));
  arrayBuffer->GetBackingStore().reset((v8::BackingStore*)nullptr);
}

NAN_MODULE_INIT(Initialize) {
  NAN_EXPORT(target, createSharedBuffer);
  NAN_EXPORT(target, detachSharedBuffer);
}

NODE_MODULE(sharedbuffer, Initialize)

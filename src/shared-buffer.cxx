#include <nan.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void weakBufferCallback(const v8::WeakCallbackInfo<char> &info) {
  char *data = info.GetParameter();
  int result = shmdt(data);
}

NAN_METHOD(createSharedBuffer) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (!info[0]->IsUint32()) {
    Nan::ThrowError("key must be an integer");
    return;
  } else if (!info[1]->IsUint32()) {
    Nan::ThrowError("size must be an integer");
    return;
  }

  key_t key = info[0]->Uint32Value();
  size_t size = info[1]->Uint32Value();
  bool initialize = info[2]->BooleanValue();

  key_t shmId = shmget(key, size, initialize ? IPC_CREAT | 0666 : 0666);

  if (shmId < 0) {
    Nan::ThrowError(strerror(errno));
    return;
  }

  char * data = (char *)shmat(shmId, NULL, 0);

  if (data == (char *)-1) {
    Nan::ThrowError(strerror(errno));
    return;
  }

  if (initialize)
  memset(data, 0, size);

  v8::Persistent<v8::ArrayBuffer> buffer(isolate, v8::ArrayBuffer::New(isolate, (void*)data, size));

  buffer.SetWeak(data, &weakBufferCallback, v8::WeakCallbackType::kParameter);

  info.GetReturnValue().Set(Nan::New<v8::ArrayBuffer>(buffer));
}

NAN_METHOD(detachSharedBuffer) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (!info[0]->IsArrayBuffer()) {
    Nan::ThrowError("argument must be an ArrayBuffer");
    return;
  }

  v8::Local<v8::ArrayBuffer> arrayBuffer = info[0].As<v8::ArrayBuffer>();

  if (!arrayBuffer->IsNeuterable()) {
    Nan::ThrowError("ArrayBuffer is not a shared-buffer, or has already been detached");
    return;
  }

  const void *data = arrayBuffer->GetContents().Data();

  int result = shmdt(data);
  if (result == -1) {
    Nan::ThrowError(strerror(errno));
    return;
  } else {
    arrayBuffer->Neuter();
  }
}

NAN_MODULE_INIT(Initialize) {
  NAN_EXPORT(target, createSharedBuffer);
  NAN_EXPORT(target, detachSharedBuffer);
}

NODE_MODULE(sharedbuffer, Initialize)

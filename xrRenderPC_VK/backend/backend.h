#ifndef BACKEND_BACKEND_H_
#define BACKEND_BACKEND_H_

#include "backend/streams.h"

class BackEnd
{
public:
    BackEnd();
    ~BackEnd() = default;

    void OnDeviceCreate();
    void OnDeviceDestroy();

    void OnFrameBegin();
    void OnFrameEnd();

private:
    void CreateIndexCache();

    DataStream<VertexStream> vertex_stream_;
    DataStream<IndexStream>  index_stream_;
    DataStream<IndexStream>  index_cache_;
};

extern BackEnd backend;

#endif // BACKEND_BACKEND_H_

#ifndef MEDIA_DEMUXER_DEMUXER_STREAM_PROVIDER_H_
#define MEDIA_DEMUXER_DEMUXER_STREAM_PROVIDER_H_

#include "demuxer_stream.h"

namespace media{
    class DemuxerStreamProvider{
    public:
        DemuxerStreamProvider(){};
        virtual ~DemuxerStreamProvider(){};
        virtual DemuxerStream* GetDemuxerStream(DemuxerStream::Type type) = 0;
    };
} // namespace media


#endif
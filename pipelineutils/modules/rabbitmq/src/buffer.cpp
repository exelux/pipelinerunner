#include "buffer.h"
#include <cassert>

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

Buffer::Buffer(size_t size)
    : _data(size, 0)
    , _use(0)
{
}

size_t Buffer::write(const char *data, size_t size)
{
    if (_use == _data.size())
        return 0;

    const auto available = _data.size() - _use;
    const auto bytes_to_write = std::min(size, available);

    memcpy(_data.data() + _use, data, bytes_to_write);
    _use += bytes_to_write;
    return bytes_to_write;
}

void Buffer::resize(size_t size)
{
    _data.resize(size);
}

void Buffer::drain()
{
    _use = 0;
}

size_t Buffer::dataSize() const
{
    return _use;
}

size_t Buffer::availableSize() const
{
    return _data.size() - _use;
}

size_t Buffer::size() const
{
    return _data.size();
}

const char *Buffer::data() const
{
    return _data.data();
}

void Buffer::shiftLeft(size_t count)
{
    if (count == 0 || _use == 0)
        return;

    assert(count <= _use);

    _use -= count;
    if (_use > 0)
        std::memmove(_data.data(), _data.data() + count, _use);
}

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils

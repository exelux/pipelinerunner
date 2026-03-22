#pragma once

#include <cstring>
#include <vector>

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

class Buffer
{
public:
    Buffer(size_t size);

    size_t write(const char* data, size_t size);

    void resize(size_t size);

    void drain();

    size_t dataSize() const;
    size_t availableSize() const;
    size_t size() const;
    const char* data() const;

    void shiftLeft(size_t count);

private:
    std::vector<char> _data;
    size_t _use = 0;
};

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils

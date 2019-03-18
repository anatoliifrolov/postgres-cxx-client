#pragma once

#include <gmock/gmock.h>
#include <postgres/internal/IChannel.h>

namespace postgres::internal {

class ChannelMock : public IChannel {
public:
    MOCK_METHOD1(quit, void(int));
    MOCK_METHOD1(send, std::tuple<bool, Worker*>(Job));
    MOCK_METHOD1(receive, void(Slot&));
    MOCK_METHOD1(recycle, void(Worker&));
    MOCK_METHOD0(drop, void());
};

}  // namespace postgres::internal

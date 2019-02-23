#include <postgres/internal/Worker.h>

#include <postgres/internal/Channel.h>
#include <postgres/internal/Context.h>
#include <postgres/Error.h>

namespace postgres {
namespace internal {

Worker::Worker(Context const& ctx, Channel& ch)
    : ctx_{&ctx}, ch_{&ch} {
}

Worker::Worker(Worker&& other) noexcept = default;

Worker& Worker::operator=(Worker&& other) noexcept = default;

Worker::~Worker() noexcept {
    thread_.join();
}

void Worker::run() {
    conn_ = ctx_->uri.empty() ? Connection{ctx_->cfg} : Connection{ctx_->uri};
    for (auto const& prep : ctx_->prep) {
        conn_.exec(prep).check();
    }
    _POSTGRES_CXX_ASSERT(conn_.isOk(), conn_.message());

    thread_ = std::thread([this] {
        while (true) {
            auto job = ch_->receive();
            if (job) {
                job = ch_->receiveQuery();
            }
            job(conn_);
        }
    });
}

}  // namespace internal
}  // namespace postgres

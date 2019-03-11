#include <postgres/Status.h>
#include <postgres/Error.h>

namespace postgres {

Status::Status(PGresult* const handle)
    : handle_{handle, PQclear} {
}

Status::Status(Status&& other) noexcept = default;

Status& Status::operator=(Status&& other) noexcept = default;

Status::~Status() noexcept = default;

void Status::check() const {
    _POSTGRES_CXX_ASSERT(RuntimeError,
                         isOk(),
                         "fail to execute statement: " << describe() << ": " << message());
}

bool Status::isOk() const {
    switch (type()) {
        case PGRES_COMMAND_OK:
        case PGRES_TUPLES_OK:
        case PGRES_SINGLE_TUPLE:
        case PGRES_NONFATAL_ERROR: {
            return true;
        }
        default: {
            break;
        }
    }
    return false;
}

bool Status::isDone() const {
    return native() == nullptr;
}

bool Status::isEmpty() const {
    return size() == 0;
}

int Status::size() const {
    return PQntuples(native());
}

int Status::effect() const {
    std::string const s = PQcmdTuples(native());
    return s.empty() ? 0 : std::stoi(s);
}

const char* Status::message() const {
    return PQresultErrorMessage(native());
}

const char* Status::describe() const {
    return PQresStatus(type());
}

ExecStatusType Status::type() const {
    return PQresultStatus(native());
}

PGresult* Status::native() const {
    return handle_.get();
}

}  // namespace postgres

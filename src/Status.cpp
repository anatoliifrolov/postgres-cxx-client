#include <postgres/Status.h>
#include <postgres/Error.h>

namespace postgres {

Status::Status(PGresult* const handle)
    : handle_{handle, PQclear} {
}

Status::Status(Status&& other) = default;

Status& Status::operator=(Status&& other) = default;

Status::~Status() = default;

bool Status::isOk() const {
    switch (status()) {
        case ExecStatusType::PGRES_COMMAND_OK:
        case ExecStatusType::PGRES_TUPLES_OK:
        case ExecStatusType::PGRES_SINGLE_TUPLE:
        case ExecStatusType::PGRES_NONFATAL_ERROR:
            return true;
        default:
            return false;
    }
}

bool Status::isDone() const {
    return !native();
}

void Status::validate() const {
    _POSTGRES_CXX_ASSERT(isOk(), errorMessage());
}

Status::operator bool() const {
    return isOk();
}

ExecStatusType Status::status() const {
    return PQresultStatus(native());
}

const char* Status::statusName() const {
    return PQresStatus(status());
}

const char* Status::errorMessage() const {
    return PQresultErrorMessage(native());
}

int Status::size() const {
    return PQntuples(native());
}

int Status::affected() const {
    const std::string n = PQcmdTuples(native());
    return n.empty() ? 0 : std::stoi(n);
}

bool Status::empty() const {
    return size() == 0;
}

PGresult* Status::native() const {
    return handle_.get();
}

}  // namespace postgres

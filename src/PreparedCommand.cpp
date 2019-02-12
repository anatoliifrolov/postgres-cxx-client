#include <postgres/PreparedCommand.h>

namespace postgres {

PreparedCommand::PreparedCommand(PreparedCommand&& other) noexcept = default;

PreparedCommand& PreparedCommand::operator=(PreparedCommand&& other) noexcept = default;

PreparedCommand::~PreparedCommand() = default;

}  // namespace postgres

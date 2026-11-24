#include <uipc/common/exception.h>
#include <uipc/common/format.h>

namespace uipc
{
Exception::Exception(std::string_view msg)
    : std::runtime_error{std::string{msg}}
{
}
}  // namespace uipc

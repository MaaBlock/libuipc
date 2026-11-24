#pragma once
#include <uipc/common/string.h>
#include <uipc/common/dllexport.h>
#include <stdexcept>

namespace uipc
{
class UIPC_CORE_API Exception : public std::runtime_error
{
  public:
    Exception(std::string_view msg);
};
}  // namespace uipc

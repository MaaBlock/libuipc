#include <uipc/common/abort.h>
#include <cstdlib>
namespace uipc
{
void abort() noexcept
{
    std::abort();  // call std::abort to terminate the program
}
}  // namespace uipc

#include <uipc/common/demangle.h>
#if !defined(_MSC_VER) && defined(__has_include)
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#define UIPC_HAS_CXXABI 1
#endif
#endif

namespace uipc
{
std::string demangle(const std::string& mangled_name)
{
#if defined(_MSC_VER)
    std::string out = mangled_name;
    for(const char* prefix : {"class ", "struct ", "enum ", "union "})
    {
        const std::size_t len = std::char_traits<char>::length(prefix);
        if(out.rfind(prefix, 0) == 0)
        {
            out.erase(0, len);
            break;
        }
    }
    return out;
#elif defined(UIPC_HAS_CXXABI)
    int status = 0;
    std::unique_ptr<char, decltype(&std::free)> decoded(
        abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status),
        &std::free);
    return (status == 0 && decoded) ? std::string(decoded.get()) : mangled_name;
#else
    return mangled_name;
#endif
}
}  // namespace uipc

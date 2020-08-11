#include <ivorium_config.hpp>

//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;

#if IV_CONFIG_FS_ENABLED
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

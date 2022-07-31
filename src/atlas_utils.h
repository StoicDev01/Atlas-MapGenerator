#include "vector"
#include "filesystem"

namespace atlas{

    inline std::filesystem::directory_iterator iterate_templates_folder(){
        return std::filesystem::directory_iterator(std::filesystem::path("./data/world templates"));
    };

}
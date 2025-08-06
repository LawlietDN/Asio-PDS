#include <fstream>
#include <AsioPDS/node.h>

std::filesystem::path Node::userConfigurationDirectory()
{
    #ifdef _WIN32
    const char* base = std::getenv("APPDATA");           
    if (!base) throw std::runtime_error("APPDATA not set");
    return std::filesystem::path(base) / "pds";                       
#else
    const char* base = std::getenv("HOME");              
    if (!base) throw std::runtime_error("HOME not set");
    return std::filesystem::path(base) / ".pds";                     
#endif

}
std::filesystem::path Node::uuidFilePath() { return userConfigurationDirectory() / "node.uuid"; }

boost::uuids::uuid Node::loadOrCreateNodeUuid()
{
    std::filesystem::path dir  = userConfigurationDirectory();
    std::filesystem::path file = uuidFilePath();

    if (!std::filesystem::exists(dir)) std::filesystem::create_directories(dir);

    if (std::filesystem::exists(file))
    {
        std::ifstream in(file);
        boost::uuids::uuid id;
        in >> id;                         
        if (in) return id;
    }

    boost::uuids::uuid id = boost::uuids::random_generator()();
    std::ofstream(file) << id;            
    return id;
}

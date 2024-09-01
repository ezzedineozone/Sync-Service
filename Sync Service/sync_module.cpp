#include "sync_module.h"
//SyncModule();
//SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction);
//SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, std::string frequency);
//SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, const SyncInfo& info);


SyncModule::SyncModule() : info (SyncInfo(name)) {
    this->name = "";
    this->source = fs::path();
    this->destination = fs::path();
    this->type = "";
    this->direction = "";
}

SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, const SyncInfo& info) : SyncModule() {
    if (fs::exists(source)) {
        if (fs::exists(destination))
        {
            this->name = name;
            this->source = source;
            this->destination = destination;
            this->type = type;
            this->direction = direction;
            this->info = info;
        }
        else {
            std::cout << "Destination is invalid\n";
        }
    }
    else {
        std::cout << "source is invalid\n";
    }
};
SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, std::string frequency) : SyncModule(name, source, destination, type, direction, SyncInfo(name, frequency)) {};
SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction) : SyncModule(name, source, destination, type, direction, SyncInfo(name)) {};

SyncModule::SyncModule(const SyncModule& module) : SyncModule(module.name, module.source, module.destination, module.type, module.direction, module.info) {};

SyncModule::SyncModule(const nlohmann::json& j)
{
    this->name = j["name"].template get<std::string>();
    this->source = fs::path(j["source"].template get<std::string>());
    this->destination = fs::path(j["destination"].template get<std::string>());
    this->type = j["type"].template get<std::string>();
    this->direction = j["direction"].template get<std::string>();
    this->info = SyncInfo(j["info"]);
}






fs::path SyncModule::get_source() {
    return source;
}

fs::path SyncModule::get_destination() {
    return destination;
}

std::string SyncModule::get_name() {
    return this->name;
}

std::wstring SyncModule::get_source_wstr() {
    return source.wstring();
}

std::string SyncModule::to_string() {
    std::ostringstream oss;
    oss << "SyncModule:\n"
        << "  Name: " << name << "\n"
        << "  Source: " << source.string() << "\n"
        << "  Destination: " << destination.string() << "\n"
        << "  Type: " << type << "\n"
        << "  Direction: " << direction << "\n"
        << "  Info: " << info.to_json().dump(4);
    return oss.str();
}

std::wstring SyncModule::get_destination_wstr() {
    return destination.wstring();
}

std::string SyncModule::get_type() {
    return type;
}

std::string SyncModule::get_direction() {
    return direction;
}

nlohmann::json SyncModule::to_json()
{
    nlohmann::json j;
    j["name"] = this->name;
    j["source"] = this->source.string();
    j["destination"] = this->destination.string();
    j["type"] = this->type;
    j["direction"] = this->direction;
    j["info"] = this->info.to_json();
    return j;
}

void SyncModule::set_name(std::string name) {
    this->name = name;
}

void SyncModule::set_source(fs::path path) {
    this->source = path;
}

void SyncModule::set_destination(fs::path path) {
    this->destination = path;
}

void SyncModule::set_type(std::string type) {
    this->type = type;
}

void SyncModule::set_direction(std::string direction) {
    this->direction = direction;
}

bool SyncModule::operator==(const SyncModule other)
{
    return this->name == other.name && this->source == other.source && this->destination == other.destination && this->type == other.type && this->direction == other.direction;
}

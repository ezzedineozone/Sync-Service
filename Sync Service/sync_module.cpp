#include "sync_module.h"
#include <iostream>
#include "sync_info.h"
SyncModule::SyncModule() : info("") {
    this->set_destination(fs::path());
    this->set_source(fs::path());
    this->set_type(std::string());
    this->set_direction(std::string());
}
SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction) : SyncModule(name, source, destination, type, direction, "" ){};
SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, std::string frequency) : info(name, frequency) {

    if(fs::exists(source))
        if (fs::exists(destination))
        {
            this->set_name(name);
            this->set_destination(destination);
            this->set_source(source);
            this->set_type(type);
            this->set_direction(direction);
        }
        else
        {
            std::cout << "destination entered isn't valid\n";
            *this = SyncModule();
        }
    else {
        std::cout << "source entered isn't valid\n";
        *this = SyncModule();
    }

}
SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, SyncInfo info) : info (info), name(name), source(source), destination(destination), type(type), direction(direction) {

};
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

std::wstring SyncModule::get_destination_wstr() {
    return destination.wstring();
}

std::string SyncModule::get_type() {
    return type;
}

std::string SyncModule::get_direction() {
    return direction;
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

#include "sync_module.h"
#include <iostream>
#include "sync_info.h"
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

SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, const SyncInfo& info) : name(name), source(source), destination(destination), type(type), direction(direction), info(info) {};
SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, std::string frequency) : SyncModule(name, source, destination, type, direction, SyncInfo(name, frequency)) {};
SyncModule::SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction) : SyncModule(name, source, destination, type, direction, SyncInfo(name)) {};

SyncModule::SyncModule(const SyncModule& module) : SyncModule(module.name, module.source, module.destination, module.type, module.direction, module.info) {};







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

#pragma once

#include <matjson.hpp>
#include "../stuff.hpp"

struct Parameter {
    std::string name;
    std::string description;
    std::string type;
    bool primitive;

    bool operator==(const Parameter &other) const {
        return name == other.name
            && description == other.description
            && type == other.type
            && primitive == other.primitive;
    }
};

template<>
struct matjson::Serialize<Parameter> {
    static geode::Result<Parameter> fromJson(matjson::Value const& j) {
        return geode::Ok(Parameter{
            .name = j["name"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr(""),
            .type = j["type"].asString().or_(j["$ref"].asString()).unwrap(),
            .primitive = j.contains("type")
        });
    }
};

struct Method {
    std::string name;
    std::string description;
    std::vector<Parameter> parameters;

    bool operator==(const Method &other) const {
        return name == other.name
            && description == other.description
            && parameters == other.parameters;
    }
};

template<>
struct matjson::Serialize<Method> {
    static geode::Result<Method> fromJson(matjson::Value const& j) {
        return geode::Ok(Method{
            .name = j["name"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr(""),
            .parameters = deserializeJsonArray<Parameter>(j["parameters"].asArray().unwrapOr(std::vector<matjson::Value>{}))
        });
    }
};

struct Domain {
    std::string domain;
    std::string description;
    std::vector<Method> methods;

    bool operator==(const Domain &other) const {
        return domain == other.domain
            && description == other.description
            && methods == other.methods;
    }    
};

template<>
struct matjson::Serialize<Domain> {
    static geode::Result<Domain> fromJson(matjson::Value const& j) {
        return geode::Ok(Domain{
            .domain = j["domain"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr(""),
            .methods = deserializeJsonArray<Method>(j["commands"].asArray().unwrap())
        });
    }
};

struct ProtocolsMetadata {
    struct _version {
        std::string major;
        std::string minor;
    } version;
    std::vector<Domain> domains;
};

template<>
struct matjson::Serialize<ProtocolsMetadata> {
    static geode::Result<ProtocolsMetadata> fromJson(matjson::Value const& j) {
        return geode::Ok(ProtocolsMetadata{
            .version = {
                .major = j["version"]["major"].asString().unwrap(),
                .minor = j["version"]["minor"].asString().unwrap()
            },
            .domains = deserializeJsonArray<Domain>(j["domains"].asArray().unwrap())
        }
        );
    }
};
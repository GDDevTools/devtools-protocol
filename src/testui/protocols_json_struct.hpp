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


struct Type_ {
    std::string id;
    std::string description;
    std::string type;
    std::vector<Parameter> properties;

    bool operator==(const Type_ &other) const {
        return id == other.id
            && description == other.description
            && type == other.type
            && properties == other.properties;
    }
};

template<>
struct matjson::Serialize<Type_> {
    static geode::Result<Type_> fromJson(matjson::Value const& j) {
        return geode::Ok(Type_{
            .id = j["id"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr(""),
            .type = j["type"].asString().unwrap(),
            .properties = deserializeJsonArray<Parameter>(j["properties"].asArray().unwrapOr(std::vector<matjson::Value>{}))
        });
    }
};

struct Event {
    std::string name;
    std::string description;
    std::vector<Parameter> parameters;

    bool operator==(const Event &other) const {
        return name == other.name
            && description == other.description
            && parameters == other.parameters;
    }
};

template<>
struct matjson::Serialize<Event> {
    static geode::Result<Event> fromJson(matjson::Value const& j) {
        return geode::Ok(Event{
            .name = j["name"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr(""),
            .parameters = deserializeJsonArray<Parameter>(j["parameters"].asArray().unwrapOr(std::vector<matjson::Value>{}))
        });
    }
};

struct Method {
    std::string name;
    std::string description;
    std::vector<Parameter> parameters;
    std::vector<Parameter> returns;

    bool operator==(const Method &other) const {
        return name == other.name
            && description == other.description
            && parameters == other.parameters
            && returns == other.returns;
    }
};
template<>
struct matjson::Serialize<Method> {
    static geode::Result<Method> fromJson(matjson::Value const& j) {
       return geode::Ok(Method{
            .name = j["name"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr(""),
            .parameters = deserializeJsonArray<Parameter>(j["parameters"].asArray().unwrapOr(std::vector<matjson::Value>{})),
            .returns = deserializeJsonArray<Parameter>(j["returns"].asArray().unwrapOr(std::vector<matjson::Value>{}))
        });
    }
};

struct Domain {
    std::string domain;
    std::string description;
    std::vector<Method> methods;
    std::vector<Type_> types;
    std::vector<Event> events;

    bool operator==(const Domain &other) const {
        return domain == other.domain
            && description == other.description
            && methods == other.methods;
    }    
};

using matthewArray = std::vector<matjson::Value>;

template<>
struct matjson::Serialize<Domain> {
    static geode::Result<Domain> fromJson(matjson::Value const& j) {
        return geode::Ok(Domain{
            .domain = j["domain"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr(""),
            .methods = deserializeJsonArray<Method>(j["commands"].asArray().unwrapOr(matthewArray{})),
            .types = deserializeJsonArray<Type_>(j["types"].asArray().unwrapOr(matthewArray{})),
            .events = deserializeJsonArray<Event>(j["events"].asArray().unwrapOr(matthewArray{})),
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
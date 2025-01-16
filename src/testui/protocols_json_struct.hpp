#include <matjson.hpp>

struct Domain {
    std::string domain;
    std::string description;
};

template<>
struct matjson::Serialize<Domain> {
    static geode::Result<Domain> fromJson(matjson::Value const& j) {
        return geode::Ok(Domain{
            .domain = j["domain"].asString().unwrap(),
            .description = j["description"].asString().unwrapOr("")
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
        std::vector<Domain> domains;
        auto& domainsjs = j["domains"].asArray().unwrap();
        domains.reserve(domainsjs.size());
        for (auto& i : domainsjs) {
            domains.push_back(matjson::Serialize<Domain>::fromJson(i).unwrap());
        };
        return geode::Ok(ProtocolsMetadata{
            .version = {
                .major = j["version"]["major"].asString().unwrap(),
                .minor = j["version"]["minor"].asString().unwrap()
            },
            .domains = domains
        }
        );
    }
};
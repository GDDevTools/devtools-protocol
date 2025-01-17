#include "../WS.hpp"
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <matjson.hpp>
#include <tuple>

matjson::Value GJLevel2Json(GJGameLevel* level) {
  return matjson::makeObject({
    {"name", std::string(level->m_levelName)},
    {"author", std::string(level->m_creatorName)}
  });
}
$domainMethod(getLocalLevel) {
  auto ret = EditorIDs::getLevelByID(params["id"].asInt().unwrap());
  if (ret == nullptr) {
    return geode::Err(std::make_tuple(-32001, "No level with the specified ID found. Maybe search for online levels instead?"));
  }
  return geode::Ok(GJLevel2Json(ret));
}


$execute {
  auto p = Protocol::get();
  p->registerFunction("Levels.getLocalLevel", getLocalLevel);
}

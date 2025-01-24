#include "../WS.hpp"
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <matjson.hpp>
matjson::Value GJLevel2Json(GJGameLevel* level);

bool LevelDomainDisabled = true;

inline void fireLevelEvent(std::string eventName, matjson::Value const& content = {}) {
  if (!LevelDomainDisabled) fireEvent("Level."+eventName, content);
};

#include <Geode/modify/GameLevelManager.hpp>
struct noaka : public geode::Modify<noaka, GameLevelManager> {
  GJGameLevel* createNewLevel() {
    auto ret = GameLevelManager::createNewLevel();
    fireLevelEvent("levelCreated", {
      {"level", GJLevel2Json(ret)}
    });
    return ret;
  }
  void deleteLevel(GJGameLevel* e) {
    GameLevelManager::deleteLevel(e);
    fireLevelEvent("levelDeleted", {
      {"level", GJLevel2Json(e)}
    });
  }  
};


matjson::Value GJLevel2Json(GJGameLevel* level) {
  auto ret = matjson::makeObject({
    {"name", std::string(level->m_levelName)},
    {"author", std::string(level->m_creatorName)},
    {"type", std::string(level->m_isUploaded ? "online" : "local")},
    {"id", (int)level->m_levelID},
  });
  if (level->m_isUploaded) ret["downloads"] = level->m_downloads;

  return ret;
}

matjson::Value GJList2Json(GJLevelList* list, bool includeLevel) {
  auto ret = matjson::makeObject({
    {"name", std::string(list->m_listName)},
    {"author", std::string(list->m_creatorName)},
    {"type", std::string(list->m_uploaded ? "online" : "local")},
    {"id", (int)list->m_listID},
  });
  if (list->m_uploaded) ret["downloads"] = list->m_downloads;
  #ifndef GEODE_IS_MACOS
  if (includeLevel) {
    std::vector<matjson::Value> joe;
    for (auto* level : geode::cocos::CCArrayExt<GJGameLevel*>(list->getListLevelsArray(nullptr))) {
      joe.push_back(GJLevel2Json(level));
    };
    ret["levels"] = joe;
  }
  #endif
  return ret;
}

$domainMethod(getLocalLevel) {
  auto ret = EditorIDs::getLevelByID(params["id"].asInt().unwrap());
  if (ret == nullptr) {
    return errors::invalidParameter("No level with the specified ID found. Maybe search for online levels instead?");
  }
  return geode::Ok(GJLevel2Json(ret));
}
$domainMethod(getSavedLevel) {
  auto ret = GameLevelManager::get()->getSavedLevel(params["id"].asInt().unwrap());
  if (ret == nullptr) {
    return errors::invalidParameter("No level with the specified ID was saved. Maybe try downloading it or search for local levels instead?");
  }
  return geode::Ok(GJLevel2Json(ret));
}

$domainMethod(getLocalList) {
  auto ret = EditorIDs::getListByID(params["id"].asInt().unwrap());
  if (ret == nullptr) {
    return errors::invalidParameter("No level list with the specified ID found. Maybe search for online list instead?");
  }
  return geode::Ok(GJList2Json(ret, params["includeLevel"].asBool().unwrapOr(false)));
}
$domainMethod(getSavedList) {
  #ifndef GEODE_IS_ANDROID
  return errors::internalError("Only works on Android :(");
  #else
  auto ret = GameLevelManager::get()->getSavedLevelList(params["id"].asInt().unwrap());
  if (ret == nullptr) {
    return errors::invalidParameter("No level with the specified ID was saved. Maybe try downloading it or search for local levels instead?");
  }
  return geode::Ok(GJList2Json(ret, params["includeLevel"].asBool().unwrapOr(false)));
  #endif
}

$domainMethod(createLevel) {
  GJGameLevel* level = GameLevelManager::get()->createNewLevel();
  level->m_levelName = params["name"].asString().unwrap();
  level->m_levelDesc = params["description"].asString().unwrapOr("");
  return geode::Ok(GJLevel2Json(level));
}
$domainAsyncMethod(deleteLevel) {
  //??????
  GJGameLevel* level = static_cast<GJGameLevel*>(
    LocalLevelManager::get()->getAllLevelsInDict()->objectForKey(params["id"].asInt().unwrap())
  );
  if (level==nullptr) return geode::Task<Protocol::FunctionReturnType>::immediate(geode::Ok(matjson::Value::object()));
  return geode::Task<Protocol::FunctionReturnType>::runWithCallback([level](auto finish, auto prog, auto hasBeenCancelled) {
    geode::createQuickPopup(
      "Delete Level",
      "Are you sure you want to <cr>delete</c> <cj>"+level->m_levelName+"</c>?\n<cy>This action cannot be undone.</c>",   // content
      "No", "Yes",      // buttons
      [finish](auto, bool btn2) {
        if (btn2) {
          finish(geode::Ok(matjson::Value::object()));
        } else {
          finish(errors::userInterventionError("User cancelled deleting level."));
        }
      }
    );
  });
}

$domainMethod(disableLevel) {
  LevelDomainDisabled = true;
  return geode::Ok(matjson::Value::object());
};
$domainMethod(enableLevel) {
  LevelDomainDisabled = false;
  return geode::Ok(matjson::Value::object());
};

$execute {
  auto p = Protocol::get();
  p->registerFunction("Level.disable", disableLevel);
  p->registerFunction("Level.enable", enableLevel);
  p->registerFunction("Level.getLocalLevel", getLocalLevel, {"id"});
  p->registerFunction("Level.getSavedLevel", getSavedLevel, {"id"});
  p->registerFunction("Level.getLocalList", getLocalList, {"id"});
  p->registerFunction("Level.getSavedList", getSavedList, {"id"});
}

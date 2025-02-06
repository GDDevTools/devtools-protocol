#include "../WS.hpp"
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <matjson.hpp>
#include "Geode/loader/Loader.hpp"
#include "b64.hpp"
matjson::Value GJLevel2Json(GJGameLevel* level);

inline void fireLevelEvent(std::string eventName, matjson::Value const& content = {}) {
  fireEvent("Level."+eventName, content);
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
    {"description", base64_decode(std::string(level->m_levelDesc))},
    {"author", std::string(level->m_creatorName)},
    {"type", std::string(level->m_isUploaded ? "online" : "local")},
    {"id", level->m_isUploaded ? (int)level->m_levelID : EditorIDs::getID(level)},
  });
  if (level->m_isUploaded) ret["downloads"] = level->m_downloads;

  return ret;
}

matjson::Value GJList2Json(GJLevelList* list, bool includeLevel) {
  auto ret = matjson::makeObject({
    {"name", std::string(list->m_listName)},
    {"author", std::string(list->m_creatorName)},
    {"type", std::string(list->m_uploaded ? "online" : "local")},
    {"id", list->m_uploaded ? (int)list->m_listID : EditorIDs::getID(list)},
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

geode::Result<GJGameLevel*, Protocol::FunctionErrorReturnType> getLevelObj(matjson::Value params) {
  GJGameLevel* ret = nullptr;
  std::string type = params["type"].asString().unwrap();
  if (type == "local") {
    ret = EditorIDs::getLevelByID(params["id"].asInt().unwrap());
    if (ret == nullptr) {
      return geode::Err(errors::invalidParameter("No level with the specified ID found. Maybe search for online levels instead?").unwrapErr());
    }
  }
  else if (type == "saved") {
    ret = GameLevelManager::get()->getSavedLevel(params["id"].asInt().unwrap());
    if (ret == nullptr) {
      return geode::Err(errors::invalidParameter("No level with the specified ID was saved. Maybe try downloading it or search for local levels instead?").unwrapErr());
    }
  } else {
    return geode::Err(errors::invalidParameter(fmt::format("what is a \"{}\" level?", type)).unwrapErr());
  };
  return geode::Ok(ret);
}

$domainMethod(getLevel) {
  return geode::Ok(
    matjson::makeObject({
      {"level", GJLevel2Json(GEODE_UNWRAP(getLevelObj(params)))}
    })
  );
}

$domainMethod(getList) {
  std::string type = params["type"].asString().unwrap();
  GJLevelList* ret = nullptr;
  if (type == "local") {
    ret = EditorIDs::getListByID(params["id"].asInt().unwrap());
    if (ret == nullptr) {
      return errors::invalidParameter("No level list with the specified ID found. Maybe search for online list instead?");
    }
  }
  else if (type == "saved") { 
    #ifndef GEODE_IS_ANDROID
    return errors::internalError("Only works on Android :(");
    #else
    auto ret = GameLevelManager::get()->getSavedLevelList(params["id"].asInt().unwrap());
    if (ret == nullptr) {
      return errors::invalidParameter("No level with the specified ID was saved. Maybe try downloading it or search for local levels instead?");
    }
    #endif
  } else {
    return errors::invalidParameter(fmt::format("what is a \"{}\" level?", type));
  }
  return geode::Ok(matjson::makeObject({
    {"list", GJList2Json(ret, params["includeLevel"].asBool().unwrapOr(false))}
  }));
}

$domainMethod(createLevel) {
  GJGameLevel* level = GameLevelManager::get()->createNewLevel();
  level->m_levelName = params["name"].asString().unwrap();
  level->m_levelDesc = base64_encode(params["description"].asString().unwrapOr(""));
  return geode::Ok(matjson::makeObject({
    {"level",GJLevel2Json(level)}
  }));
}
$domainAsyncMethodUndepended(deleteSavedLevel) {
  //??????
  GJGameLevel* level = GameLevelManager::get()->getSavedLevel(params["id"].asInt().unwrap());
  if (level==nullptr) {
    finish(emptyResponse());
    return;
  }
  geode::createQuickPopup(
    "Delete Level",
    fmt::format("Are you sure you want to <cr>delete</c> <cj>{}</c>?",level->m_levelName.c_str()), // content
    "No", "Yes",      // buttons
    [finish,level](auto, bool btn2) {
      if (btn2) {
        GameLevelManager::get()->deleteLevel(level);
        finish(emptyResponse());
      } else {
        finish(errors::userInterventionError("User cancelled deleting level."));
      }
    }
  );
}
$domainAsyncMethodUndepended(deleteLocalLevel) {
  //??????
  GJGameLevel* level = EditorIDs::getLevelByID(params["id"].asInt().unwrap());
  if (level==nullptr) {
    finish(emptyResponse());
    return;
  }
  geode::createQuickPopup(
    "Delete Level",
    fmt::format(
      "Are you sure you want to <cr>delete</c> <cj>{}</c>?\n"
      "<cy>This action cannot be undone.</c>",
      level->m_levelName.c_str()
    ),   // content
    "No", "Yes",      // buttons
    [finish,level](auto, bool btn2) {
      if (btn2) {
        GameLevelManager::get()->deleteLevel(level);
        finish(emptyResponse());
      } else {
        finish(errors::userInterventionError("User cancelled deleting level."));
      }
    }
  );
}
$domainAsyncMethod(deleteLevel) {
  std::string type = params["type"].asString().unwrap();
  geode::queueInMainThread([params,finish,type]{
    matjson::Value g = params;
    if (type == "local") {deleteLocalLevel(g, finish);}
    else if (type == "saved") {deleteSavedLevel(g, finish);}
    else {
      finish(errors::invalidParameter(fmt::format("what is a \"{}\" level?", type)));
    }
  });
}

$domainMethod(updateLevel) {
  auto level = EditorIDs::getLevelByID(params["id"].asInt().unwrap());
  if (level == nullptr) {
    return geode::Err(errors::invalidParameter("No level with the specified ID found. Remember, this only works for local levels!").unwrapErr());
  }
  auto meta = params["info"];

  if (meta.contains("name")) {
    level->m_levelName = meta["name"].asString().unwrap();
  }
  if (meta.contains("description")) {
    level->m_levelDesc = base64_encode(meta["description"].asString().unwrap());
  }

  return geode::Ok(matjson::makeObject({
    {"level", GJLevel2Json(level)}
  }));
}

$domainMethod(disableLevel) {
  info.setDomainEnabled("Level", false);
  return emptyResponse();
};
$domainMethod(enableLevel) {
  info.setDomainEnabled("Level", false);
  return emptyResponse();
};


$on_mod(Loaded) {
  auto p = Protocol::get();
  
  p->registerFunction("Level.disable", disableLevel);
  p->registerFunction("Level.enable", enableLevel);
  p->registerFunction("Level.getLevel", getLevel, {"id", "type"});
  p->registerFunction("Level.getList", getList, {"id", "type"});
  p->registerFunction("Level.createLevel", createLevel, {"name"});
  p->registerFunction("Level.deleteLevel", deleteLevel, {"id", "type"});
  p->registerFunction("Level.updateLevel", updateLevel, {"id", "info"});
}

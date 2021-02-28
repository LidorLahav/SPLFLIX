#include "../include/Session.h"
#include "../include/json.hpp"
#include <fstream>
#include "../include/Watchable.h"
#include "../include/User.h"

using json = nlohmann::json;

Session::Session(const std::string &configFilePath) : content(), actionsLog(),userMap(), activeUser() {
    contentInitializer(configFilePath);
    activeUser = new LengthRecommenderUser("default");
    userMap["default"] = activeUser;
}

Session::Session(const Session &other) : content(),actionsLog(),userMap(),activeUser() {
    copy(other);
}

Session::Session(Session &&other) : content(),actionsLog(),userMap(),activeUser() {
    steal(other);
}

Session &Session::operator=(const Session &other) {
    if (&other != this) {
        clean();
        copy(other);
    }
    return *this;
}

Session &Session::operator=(Session &&other) {
    if (&other != this) {
        clean();
        steal(other);
    }
    return *this;
}

Session::~Session() { clean(); }

void Session::start() {
    std::cout << "SPLFLIX is now on!" << std::endl;

    std::string actionToDo;
    BaseAction* action = nullptr;
    for (std::cin >> actionToDo; actionToDo != "exit"; std::cin >> actionToDo) {
        if (actionToDo == "createuser")
            action = new CreateUser();
        else if (actionToDo == "changeuser")
            action = new ChangeActiveUser();
        else if (actionToDo == "deleteuser")
            action = new DeleteUser();
        else if (actionToDo == "dupuser")
            action = new DuplicateUser();
        else if (actionToDo == "content")
            action = new PrintContentList();
        else if (actionToDo == "watchhist")
            action = new PrintWatchHistory();
        else if (actionToDo == "watch")
            action = new Watch();
        else if (actionToDo == "log")
            action = new PrintActionsLog();
        else
            std::cout << "invalid action" << std::endl;

        if (action != nullptr)
            action->act(*this);
    }
    action = new Exit();
    action->act(*this);
}

void Session::contentInitializer(const std::string &configFilePath) {
    std::fstream filePath(configFilePath);
    json pFile = json::parse(filePath);
    json movies = pFile["movies"];
    json tv_series = pFile["tv_series"];

    long id = 1;
    for (auto& movie : movies.items()) {
        std::string name = movie.value()["name"];
        int length = movie.value()["length"];

        std::vector<std::string> tags;
        for (auto& tag : movie.value()["tags"])
            tags.push_back(tag);

        auto* moviePointer = new Movie(id,name,length,tags);
        content.push_back(moviePointer);
        id++;
    }
    for (auto& series : tv_series.items()) {
        std::string name = series.value()["name"];
        int length = series.value()["episode_length"];

        std::vector<std::string> tags;
        for (auto& tag : series.value()["tags"])
            tags.push_back(tag);

        int seasonNumber = 1;
        for(auto& season: series.value()["seasons"]) {
            for (int i = 1; i <= season; i++) {
                auto *episodePointer = new Episode(id, name, length, seasonNumber, i, tags);  // למחוק????????
                content.push_back(episodePointer);

                if (season != series.value()["seasons"] && i != season)
                    episodePointer->setNextEpisodeId(id+1);

                id++;
            }
            seasonNumber++;
        }
    }
}

bool Session::nameExists(const std::string &name) const {
    return (userMap.find(name) != userMap.end() );
}

std::unordered_map<std::string,User*>& Session::getUserMap() {
    return userMap;
}

void Session::addActionToLog(BaseAction *const action) {
    this->actionsLog.push_back(action);
}

void Session::setActiveUser(const std::string & name) {
    activeUser = userMap.find(name)->second;
}

const std::vector<Watchable*>& Session::getContent() const {
    return content;
}

std::string Session::getActiveUserName() const {
    return activeUser->getName();
}

User* Session::getActiveUser() const {
    return activeUser;
}

const std::vector<BaseAction *> &Session::getActionsLog() const {
    return actionsLog;
}

void Session::copy(const Session &other) {
    for (int i = 0; i < (int) other.content.size() ; ++i) {
        Watchable* video = other.content[i]->deepCopy();
        content.push_back(video);
    }
    for (int i = 0; i < (int) other.actionsLog.size() ; ++i) {
        auto* action = other.actionsLog[i]->deepCopy();
        actionsLog.push_back(action);
    }
    for (auto& userPair : other.userMap) {
        std::string otherName = userPair.first;
        User* otherUser = userPair.second;
        User* user = otherUser -> deepCopy(*this, otherName);
        userMap[otherName] = user;

        if (otherName == other.getActiveUser() -> getName())
            activeUser = user;
    }
}

void Session::clean() {
    for (int i = 0; i < (int) content.size() ; ++i) {
        delete content[i];
        content[i] = nullptr;
    }
    for (int i = 0; i < (int) actionsLog.size() ; ++i) {
        delete actionsLog[i];
        actionsLog[i] = nullptr;
    }
    for (auto& userPair : userMap) {
        delete userPair.second;
        userPair.second = nullptr;
    }
    content.clear();
    actionsLog.clear();
    userMap.clear();
    activeUser = nullptr;
}

void Session::steal(Session &other) {
    copy(other);
    other.clean();
}
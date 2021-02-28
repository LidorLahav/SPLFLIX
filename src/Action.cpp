#include "../include/Action.h"
#include "../include/Session.h"
#include "../include/User.h"
#include "../include/Watchable.h"

// ************************Implementation Of BaseAction************************ //
BaseAction::BaseAction() : errorMsg(""), status(PENDING) {}

BaseAction::~BaseAction()=default;

ActionStatus BaseAction::getStatus() const { return status; }

void BaseAction::complete() { status = COMPLETED; }

void BaseAction::error(const std::string &errorMsg) {
    status = ERROR;
    this->errorMsg = errorMsg;
    std::cout << "ERROR - " + errorMsg << std::endl;
}

std::string BaseAction::getErrorMsg() const { return errorMsg; }

std::string BaseAction::statusToString() const {
    if (status == COMPLETED)
        return "COMPLETED";
    else if (status == ERROR)
        return "ERROR: " + errorMsg;
    else
        return "PENDING";
}

void BaseAction::setStatus(ActionStatus status) { this->status = status; }

// ************************Implementation Of CreateUser************************ //
void CreateUser::act(Session &sess) {
    std::string name;
    std::cin >> name;
    std::string algorithm;
    std::cin >> algorithm;

    auto& userMap = sess.getUserMap();
    User* newUser = nullptr;
    if (sess.nameExists(name))
        error("the new user name is already taken");
    else if (algorithm == "len")
        newUser = new LengthRecommenderUser(name);
    else if (algorithm == "rer")
        newUser = new RerunRecommenderUser(name);
    else if (algorithm == "gen")
        newUser = new GenreRecommenderUser(name);
    else
        error("invalid algorithm code");

    if (newUser != nullptr) {
        userMap[name] = newUser;
        complete();
    }
    sess.addActionToLog(this);
}

std::string CreateUser::toString() const { return "CreateUser " + statusToString(); }

BaseAction *CreateUser::deepCopy() {
    auto* action = new CreateUser();
    action->setStatus(getStatus());
    return action;
}

// ************************Implementation Of ChangeActiveUser************************ //
void ChangeActiveUser::act(Session &sess) {
    std::string name;
    std::cin >> name;

    if (!sess.nameExists(name))
        error("the user name doesn't exist");
    else {
        sess.setActiveUser(name);
        complete();
    }
    sess.addActionToLog(this);
}

std::string ChangeActiveUser::toString() const { return "ChangeActiveUser " + statusToString(); }

BaseAction *ChangeActiveUser::deepCopy() {
    auto* action = new ChangeActiveUser();
    action->setStatus(getStatus());
    return action;
}

// ************************Implementation Of DeleteUser************************ //
void DeleteUser::act(Session &sess) {
    std::string name;
    std::cin >> name;

    if (!sess.nameExists(name))
        error("the user name doesn't exist");
    else {
        delete sess.getUserMap()[name];
        auto iter = sess.getUserMap().find(name);
        sess.getUserMap().erase(iter);
        complete();
    }
    sess.addActionToLog(this);
}

std::string DeleteUser::toString() const { return "DeleteUser " + statusToString(); }

BaseAction *DeleteUser::deepCopy() {
    auto* action = new DeleteUser();
    action->setStatus(getStatus());
    return action;
}

// ************************Implementation Of DuplicateUser************************ //
void DuplicateUser::act(Session &sess) {
    std::string originalName;
    std::cin >> originalName;
    std::string newName;
    std::cin >> newName;

    if (!sess.nameExists(originalName))
        error("the user name doesn't exist");
    else if (sess.nameExists(newName))
        error("user name already exist");
    else {
        auto& userMap = sess.getUserMap();
        User* originalUser = (userMap.find(originalName))->second;
        User* newUser = originalUser->clone(newName);

        userMap[newName] = newUser;
        complete();
    }
    sess.addActionToLog(this);
}

std::string DuplicateUser::toString() const { return "DuplicateUser " + statusToString(); }

BaseAction *DuplicateUser::deepCopy() {
    auto* action = new DuplicateUser();
    action->setStatus(getStatus());
    return action;
}

// ************************Implementation Of PrintContentList************************ //
void PrintContentList::act(Session &sess) {
    int counter = 0;
    for (auto iter = sess.getContent().begin(); iter < sess.getContent().end(); iter++) {
        counter++;
        std::string id = std::to_string(counter);
        auto* video = (*iter);
        std::string title = video->toString();
        std::string length = std::to_string(video->getLength());
        std::string tags = video->tagsToString();

        std::cout << id << ". " << title << " " << length << " " << "minutes [" << tags << "]" << std::endl;
    }
    complete();
    sess.addActionToLog(this);
}

std::string PrintContentList::toString() const { return "PrintContentList " + statusToString(); }

BaseAction *PrintContentList::deepCopy() {
    auto* action = new PrintContentList();
    action->setStatus(getStatus());
    return action;
}

// ************************Implementation Of PrintWatchHistory************************ //
void PrintWatchHistory::act(Session &sess) {
    const User *activeUser = sess.getActiveUser();
    std::cout << "Watch history for " << activeUser->getName() << std::endl;

    int counter = 0;
    for (auto iter = activeUser->get_history().begin(); iter < activeUser->get_history().end(); iter++) {
        counter++;
        std::string id = std::to_string(counter);
        auto *video = (*iter);
        std::string title = video->toString();

        std::cout << id << ". " << title << std::endl;
    }
    complete();
    sess.addActionToLog(this);
}

std::string PrintWatchHistory::toString() const { return "PrintWatchHistory " + statusToString(); }

BaseAction *PrintWatchHistory::deepCopy() {
    auto* action = new PrintWatchHistory();
    action->setStatus(getStatus());
    return action;
}

// ************************Im&& video_ptr != nullptrplementation Of Watch************************ //
void Watch::act(Session &sess) {
    long id;
    std::cin >> id;
    auto content = sess.getContent();
    auto video_ptr = content[id-1];
    auto user_ptr = sess.getActiveUser();
    bool first = true;
    char answer='n';

    do {
        std::cout << "Watching " << video_ptr->toString() << std::endl;
        if (first == true) {
            complete();
            sess.addActionToLog(this);
        }
        else {
            Watch* action = new Watch();
            action->complete();
            sess.addActionToLog(action);
        }

        first = false;
        user_ptr -> addToHistory(video_ptr);
        user_ptr -> prepare(*video_ptr);

        if (video_ptr->nextEpisode())
            video_ptr = content[(video_ptr->getId())];
        else
            video_ptr = video_ptr->getNextWatchable(sess);


        if (video_ptr != nullptr){
            std::cout<<"We recommend watching "<<video_ptr->toString()<<" continue watching? [y/n]"<<std::endl;
            std::cin >> answer;
        }
        else
            break;
    }
    while (answer == 'y');
}

std::string Watch::toString() const {
    return "Watch " + statusToString();
}

BaseAction *Watch::deepCopy() {
    auto* action = new Watch();
    action->setStatus(getStatus());
    return action;
}

// ************************Implementation Of PrintActionsLog************************ //
void PrintActionsLog::act(Session &sess) {
    for (int i = (int)sess.getActionsLog().size() - 1; i>=0 ; i--)
        std::cout << sess.getActionsLog()[i] -> toString() <<std::endl;

    complete();
    sess.addActionToLog(this);
}

std::string PrintActionsLog::toString() const {
    return "PrintActionsLog " + statusToString();
}

BaseAction *PrintActionsLog::deepCopy() {
    auto* action = new PrintActionsLog();
    action->setStatus(getStatus());
    return action;
}

// ************************Implementation Of Exit************************ //
void Exit::act(Session &sess) {
    complete();
    sess.addActionToLog(this);
}

std::string Exit::toString() const {
    return "Exit " + statusToString();
}

BaseAction *Exit::deepCopy() {
    auto* action = new Exit();
    action->setStatus(getStatus());
    return action;
}
#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "../include/Action.h"

class User;
class Watchable;


class Session{
public:
    Session(const std::string &configFilePath);
    Session(const Session& other);
    Session(Session&& other);
    Session& operator=(const Session& other);
    Session&operator=(Session&& other);
    ~Session();
    void start();
    bool nameExists(const std::string& name) const;
    std::unordered_map< std::string,User*>& getUserMap();
    void addActionToLog(BaseAction *const action);
    void setActiveUser(const std::string & name);
    const std::vector<Watchable*>& getContent() const;
    std::string getActiveUserName() const;
    User* getActiveUser () const;
    const std::vector<BaseAction *> &getActionsLog() const;

private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    void contentInitializer(const std::string &configFilePath);
    void copy(const Session& other);
    void clean();
    void steal(Session& other);

protected:
};
#endif
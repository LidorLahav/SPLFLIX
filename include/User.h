#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    User(const User& other);
    virtual ~User();
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    const std::vector<Watchable*>& get_history() const;
    bool videoInHistory(Watchable& video);
    virtual User* clone(std::string& name)=0;
    virtual void prepare(Watchable& video)=0;
    void addToHistory(Watchable* video);
    virtual User* deepCopy(Session& sess, std::string otherName)=0;

protected:
    std::vector<Watchable*> history;
    void historyClone(User* cloned);

private:
    std::string name;
};

class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string &name);
    virtual Watchable *getRecommendation(Session &s);
    virtual LengthRecommenderUser *clone(std::string &name);
    void prepare(Watchable &video);
    virtual User* deepCopy(Session& sess, std::string otherName);

private:
    int totalLength;
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual RerunRecommenderUser* clone(std::string& name);
    void prepare(Watchable &video);
    virtual User* deepCopy(Session& sess, std::string otherName);

private:
    int index;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual GenreRecommenderUser* clone(std::string& name);
    void prepare(Watchable &video);
    virtual User* deepCopy(Session& sess, std::string otherName);

private:
    std::vector<std::pair<int, std::string>> tagsHistory;
};

#endif
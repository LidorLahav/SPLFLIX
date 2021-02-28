#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"

// ************************Implementation Of User************************ //
User::User(const std::string& name) : history(0), name(name) {
}

User::User(const User &other) : history(), name(other.name) {
    for (int i = 0; i < (int)history.size(); ++i)
        history.push_back(other.history[i]->deepCopy());
}

User::~User() = default;

std::string User::getName() const {
    return name;
}
const std::vector<Watchable*>& User::get_history() const {
    return history;
}

void User::historyClone(User* cloned) {
    std::vector<Watchable*>::iterator videoIter;
    for (videoIter = history.begin(); videoIter < history.end() ; videoIter++)
        cloned->history.push_back(*videoIter);
}

bool User::videoInHistory(Watchable& video) {
    bool videoInHistory = false;
    for (auto historyItem = history.begin(); historyItem < history.end() && !videoInHistory; historyItem++) {
        if (*historyItem == &video)
            videoInHistory = true;
    }
    return videoInHistory;
}

void User::addToHistory(Watchable* video) {
    history.push_back(video);
}



// ************************Implementation Of LengthRecommenderUser************************ //
LengthRecommenderUser::LengthRecommenderUser(const std::string &name) : User(name), totalLength(0) {
}

Watchable* LengthRecommenderUser::getRecommendation(Session &s)
{
    Watchable* recommendation = nullptr;
    float averageLength = totalLength/history.size(); // נותנים ערך ל toalLength ב watch

    // Explanation of the following loop:
    // 1) s.getContent[i] are Watchable* in session's content vector
    // 2) define them as 'content'
    // 3) if user didn't watched the Watchable item (*content)
    // 4) if it's the first unwatched Watchable
    // 5) assign that Watchable by a pointer as 'recommendation'
    // 6) else, assign 'content' as recommendation only if it's length is closer to the average length
    for (int i=0; i < (int)s.getContent().size(); i++) {
        Watchable *content = s.getContent()[i];
        if (!videoInHistory(*content)) {
            if (!recommendation)
                recommendation = s.getContent()[i];
            else if (std::abs(content->getLength() - averageLength) <
                     std::abs(recommendation->getLength() - averageLength))
                recommendation = content;
        }
    }
    return recommendation;
}

LengthRecommenderUser* LengthRecommenderUser::clone(std::string &name) {
    auto* cloned = new LengthRecommenderUser(name);
    historyClone(cloned);
    return cloned;
}

void LengthRecommenderUser::prepare(Watchable &video) {
    totalLength += video.getLength();
}

User *LengthRecommenderUser::deepCopy(Session &sess, std::string otherName) {
    auto* user = new LengthRecommenderUser(otherName);
    for (int i = 0; i < (int)history.size() ; ++i) {
        auto content = sess.getContent();
        long index = history[i]->getId() - 1;
        Watchable* video = content[index];
        user->addToHistory(video);
    }
    return user;
}

// ************************Implementation Of RerunRecommenderUser************************ //
RerunRecommenderUser::RerunRecommenderUser(const std::string& name)  : User(name), index(-1) {
 }

Watchable* RerunRecommenderUser::getRecommendation(Session& s) {
    Watchable* recommendation = nullptr;
    if (history.size() > 0) {
        index= (index+1)%(history.size());
        recommendation = history[index];
    }

    return recommendation;


}

RerunRecommenderUser* RerunRecommenderUser::clone(std::string &name) {
    auto cloned = new RerunRecommenderUser(name);
    historyClone(cloned);
    return cloned;
}

void RerunRecommenderUser::prepare(Watchable &video) {};

User *RerunRecommenderUser::deepCopy(Session &sess, std::string otherName) {
    auto* user = new RerunRecommenderUser(otherName);
    for (int i = 0; i < (int)history.size() ; ++i) {
        auto content = sess.getContent();
        long index = history[i]->getId() - 1;
        Watchable* video = content[index];
        user->addToHistory(video);
    }
    return user;
}

// ************************Implementation Of GenreRecommenderUser************************ //
GenreRecommenderUser::GenreRecommenderUser(const std::string& name)  : User(name), tagsHistory() {
}

Watchable* GenreRecommenderUser::getRecommendation(Session& s) {
    Watchable* recommendation(nullptr);

    // Explanation of following loop:
    // 1) get<1>(tagsHistory[i]) are tags in tagsHistory which are sorted by their # of appearances
    // 2) s.getContent[j] are Watchable pointers in session's content list
    // 3) the following is skipped unless if user didn't watched the Watchable items: *s.getContent[j]
    // 4) (*s.getContent[j]).getTags[k] are the tags of the Watchables (tags are inside a vector<string>)
    // 5) if the tags in tagHistory matches the Watchable tags
    // 6)recommend the Watchable (by a pointer)
    for (int i=0; i < (int)tagsHistory.size() && !recommendation; i++)
        for (int j=0; j < (int)s.getContent().size() && !recommendation; j++)
            if (!videoInHistory(*s.getContent()[j]))
                for (int k=0; k < (int)(*s.getContent()[j]).getTags().size() && !recommendation; k++)
                    if (std::get<1>(tagsHistory[i]) == (*s.getContent()[j]).getTags()[k] )
                        recommendation = s.getContent()[j];

    return recommendation;
}

GenreRecommenderUser* GenreRecommenderUser::clone(std::string &name) {
    auto cloned = new GenreRecommenderUser(name);
    historyClone(cloned);
    return cloned;
}

void GenreRecommenderUser::prepare(Watchable &video) {
    int tagsHistorySize = tagsHistory.size();
    for (int i = 0; i < (int)video.getTags().size(); ++i) {
        bool foundTag(false);
        for (int j = 0; j < tagsHistorySize && !foundTag; ++j)
            if (video.getTags()[i] == std::get<1>(tagsHistory[j])) {
                std::get<0>(tagsHistory[j])--;
                foundTag = true;
            }
        if (!foundTag)
            tagsHistory.push_back(std::pair<int, std::string>(-1, video.getTags()[i]));
    }

    for (int i=1; i < (int)tagsHistory.size(); i++ ) {
        auto key = tagsHistory[i];
        int j = i-1;

        while (j>=0 && (std::get<0>(tagsHistory[j]) > std::get<0>(key) ||  (std::get<0>(tagsHistory[j]) == std::get<0>(key) && std::get<1>(tagsHistory[j]) > std::get<1>(key)))) {
            tagsHistory[j+1] = tagsHistory[j];
            j--;
        }
        tagsHistory[j+1] = key;
    }
}

User *GenreRecommenderUser::deepCopy(Session &sess, std::string otherName) {
    auto* user = new GenreRecommenderUser(otherName);
    for (int i = 0; i < (int)history.size() ; ++i) {
        auto content = sess.getContent();
        long index = history[i]->getId() - 1;
        Watchable* video = content[index];
        user->addToHistory(video);
    }
    return user;
}
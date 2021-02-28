#include "../include/Watchable.h"
#include "../include/User.h"
#include "../include/Session.h"

// ************************Implementation Of Watchable************************ //
Watchable::Watchable(long id, int length, const std::vector<std::string>& tags)
        : id(id), length(length), tags(tags) {
}

Watchable::~Watchable()=default;

int Watchable::getLength() const {
    return length;
}

const std::vector<std::string>& Watchable::getTags() const {
    return tags;
}

const std::string Watchable::tagsToString() const {
    std::string tagsString("");

    auto& tags = getTags();
    for (int i=0; i < (int) tags.size() - 1; i++)
        tagsString += tags[i] + ",";

    tagsString += tags[tags.size() - 1];
    return tagsString;
}

const long Watchable::getId() const {
    return id;
}

// ************************Implementation Of Movie************************ //
Movie::Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags)
        : Watchable(id, length, tags), name(name) {}

std::string Movie::toString() const {
    return name;
}

Watchable* Movie::getNextWatchable(Session& s) const {
    return s.getActiveUser()->getRecommendation(s);
}

bool Movie::nextEpisode() const {
    return false;
}

Watchable *Movie::deepCopy() const {
    auto* copiedVideo = new Movie(getId(),name,getLength(),getTags());
    return copiedVideo;
}

// ************************Implementation Of Episode************************ //
Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags)
        : Watchable(id,length, tags), seriesName(seriesName), season(season), episode(episode), nextEpisodeId(-1) {}

std::string Episode::toString() const {
    return seriesName + " " + "S" + std::to_string(season) + "E" + std::to_string(episode);
}

Watchable* Episode::getNextWatchable(Session& s) const {
    return (*s.getActiveUser()).getRecommendation(s);
}

void Episode::setNextEpisodeId(long nextEpisodeId) {
    Episode::nextEpisodeId = nextEpisodeId;
}

bool Episode::nextEpisode() const {
    return (nextEpisodeId != -1);
}

Watchable *Episode::deepCopy() const {
    auto* copiedVideo = new Episode(getId(),seriesName,getLength(),season,episode,getTags());
    return copiedVideo;
}
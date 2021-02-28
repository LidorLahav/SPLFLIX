#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>


class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual ~Watchable();
    int getLength() const;
    const std::string tagsToString() const;
    virtual std::string toString() const = 0;
    virtual Watchable* getNextWatchable(Session&) const = 0;
    const std::vector<std::string>& getTags() const;
    virtual bool nextEpisode() const=0;
    const long getId() const;
    virtual Watchable* deepCopy() const = 0;

private:
    const long id;
    int length;
    std::vector<std::string> tags;

protected:
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual bool nextEpisode() const override;
    virtual Watchable* deepCopy() const ;

private:
    std::string name;
};

class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    void setNextEpisodeId(long nextEpisodeId);
    virtual bool nextEpisode() const override;
    virtual Watchable* deepCopy() const;

private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif
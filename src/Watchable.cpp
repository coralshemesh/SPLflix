//
// Created by coralshem on 11/19/19.
//

#include "../include/Watchable.h"
#include <string>
#include <vector>
#include "../include/User.h"
#include "../include/Session.h"

using namespace std;

//abstract classWatchable
Watchable::Watchable(long id, int length, const vector<string> &tags): id(id), length(length), tags(tags){} //constructor

Watchable ::~Watchable() = default;//distructor

long Watchable::getId() const { return this->id;}

int Watchable::getLength() const { return this->length;}

vector<string> Watchable::getTags() const { return this->tags;}



//Movie class
Movie::Movie(long id, const string &name, int length, const vector<string> &tags): Watchable(id,length,tags), name(name) {}

Watchable* Movie::clone() {
    return new Movie(*this);
}

Watchable* Movie::getNextWatchable(Session &s) const {
    return s.getActiveUser().getRecommendation(s);
}

string Movie::getName() const { return this->name;}

string Movie::toString() const {
        long id = this->getId();
        string name = this->getName();
        size_t length = this->getLength();
        vector<string> tag = this->getTags();
        string str = to_string(id) + ". " + name + " " + to_string(length) + " minutes [";
        for (size_t i = 0; i<tag.size(); i++){
            str = str + tag[i] + " ";
        }
            str = str + "]";
    return str;
}

string Movie::stringToWatch() { //tostring for watch method
    return  this->getName();
}

//Episode class
Episode::Episode(long id, const string& seriesName,int length, int season, int episode ,const vector<string>& tags):  Watchable(id, length,tags), seriesName(seriesName), season(season), episode(episode), nextEpisodeId(id+1){}

Watchable* Episode::clone() {
    return new Episode(*this);
}

Watchable* Episode::getNextWatchable(Session &s) const {
    vector<Watchable*> history = s.getActiveUser().get_history();
    //check if there are more episodes in this series the user just watched
    //if the series name of this episode (the last in history vector) and the episode with the next id is the same bring the next episode
    if ((s.getContentList()[history[history.size()-1]->getId()-1]->getName()).compare(s.getContentList()[(history[history.size()-1]->getId())]->getName()) ==0){
        return s.getContentList()[nextEpisodeId-1];
    }

    return s.getActiveUser().getRecommendation(s);
}

string Episode::getName() const { return this->seriesName;}

string Episode::toString() const {
    long id = this->getId();
    string name = this->getName();
    int length = this->getLength();
    int episode = this->getEpisode();
    int season = this->getSeason();
    vector<string> tag = this->getTags();
    string str = to_string(id) + ". " + name + " "+ "S" + to_string(season) +"E" + to_string(episode) +  " " + to_string(length) + " minutes [";
    for (size_t i = 0; i<tag.size(); i++){
        str = str + tag[i] + " ";
    }
    str=str+ "]";
    return str;
}

string Episode::stringToWatch() {  //tostring for watch method
    string name = this->getName();
    int episode = this->getEpisode();
    int season = this->getSeason();
    string str = name + " "+ "S" + to_string(season) +"E" + to_string(episode);
    return str;
}

int Episode::getEpisode() const { return this->episode;}

int Episode::getSeason() const { return this->season;}

//
// Created by coralshem on 11/19/19.
//

#include "../include/User.h"
#include "../include/Session.h"
#include <vector>
#include "../include/Watchable.h"
#include <cstdlib>
#include <algorithm>
#include <map>


using namespace std;

//User class
User::User(const string &name) : history(),name(name), index (0), average(0) { } //constructor

User::~User() {} //destructor

string User::getName() const { return this->name;}

int User::getIndex() { return  this->index;}

void User::setIndex() {this->index++;};

vector<Watchable*> User::get_history() const { return this->history;}

vector<Watchable*> User::getWatchHistory() { return this->history;} //non-const method to add a watchable object


void User::addHistory(Watchable* w) {
    this->history.push_back(w);
    int tmp = 0;
    for(size_t i = 0; i < history.size();i++){
        tmp = tmp + (history[i]->getLength());
    }
    average = tmp/(history.size()); //update the average length for the recommendation
}

int User::getAverage() { return this->average;}

void User::setHistory(Session &s) {
    for (size_t i = 0; i < history.size(); i++){
        int index = history[i]->getId()-1;
        history[i]=s.getContentList()[index];
    }
}

//LengthRecommenderUser class
LengthRecommenderUser::LengthRecommenderUser(const std::string& name): User(name) {}

User* LengthRecommenderUser::clone() {
    return new LengthRecommenderUser(*this);
}

Watchable* LengthRecommenderUser::getRecommendation(Session &s) {
    int minDiff = 10000; // min difference between the average length of contents in the history and the contents in the content list
    size_t minContentIndex = 0; // the content with the minimum difference from the average
    for (size_t i =0; i < s.getContentList().size(); i++){
        if ((find(history.begin(), history.end(), s.getContentList()[i]) == history.end())){
            if((abs(this->getAverage() - s.getContentList()[i]->getLength()) )< minDiff){
                minDiff = abs(this->getAverage() - s.getContentList()[i]->getLength());
                minContentIndex = i;
            }
        }
    }
    return s.getContentList()[minContentIndex];
}

void LengthRecommenderUser::getDuplicate(string name , Session& s) {
    LengthRecommenderUser *LdupUser = new LengthRecommenderUser(name);
    size_t index = 0;
    while (index < history.size()) { //copy the watch history
        LdupUser->history.push_back(this->get_history()[index]);
        index++;
    }
    s.inserUserMap(name,LdupUser);
}

//RerunRecommenderUser class
RerunRecommenderUser::RerunRecommenderUser(const string& name): User(name){}

User* RerunRecommenderUser::clone() {
    return new RerunRecommenderUser(*this);
}

Watchable* RerunRecommenderUser::getRecommendation(Session &s) {
    this->setIndex();
    return this->history[(this->getIndex()-1) % this->history.size()];
}

void RerunRecommenderUser::getDuplicate(string name, Session& s) {
    RerunRecommenderUser *RdupUser = new RerunRecommenderUser(name);
    size_t index = 0;
    while (index < this->history.size()) { //copy the watch history
        RdupUser->history.push_back(this->get_history()[index]);
        index++;
    }
    s.inserUserMap(name,RdupUser);
}

//GenreRecommenderUser class
GenreRecommenderUser::GenreRecommenderUser(const std::string& name) : User(name){}


User* GenreRecommenderUser::clone() {
    return new GenreRecommenderUser(*this);
}

Watchable* GenreRecommenderUser::getRecommendation(Session &s) {
    map<string,int> tagsMap;
    //arrange all value's in the map and update the frequency of each tag
    for (size_t i = 0; i< history.size(); i++ ){ //check for each Watchable in the history vector
        for (size_t j = 0; j < history[i]->getTags().size(); j++){ //check which tags a watchable have and insert to the map
            map<string,int >::iterator tagIt = tagsMap.find(history[i]->getTags()[j]);
            if (tagIt == tagsMap.end()){
                tagsMap.insert({history[i]->getTags()[j], 1});
            }
            else{
                tagIt->second = tagIt->second +1; //update the frequency of a tag
            }
        }
    }

    // find the most popular tag
    multimap<int,string> tagsPopular;
    for (auto &pair : tagsMap){ //swap the value and key for each pair in the map and get a sorted by tags frequency multimap
        tagsPopular.insert(make_pair(pair.second,pair.first));
    }
    // push the tags into a vector according the popularity
    vector<string> orderedTags;
    for ( auto it = tagsPopular.rbegin(); it != tagsPopular.rend(); it++){
        orderedTags.push_back(it->second);
    }

    //find the first content with the most popular tag, if the user watched it already return the next content
    size_t tagsIndex=0;
    while (tagsIndex < orderedTags.size()){
        for (size_t i = 0; i < s.getContentList().size(); i++){
            for(size_t j =0; j<s.getContentList()[i]->getTags().size(); j++){
                if (s.getContentList()[i]->getTags()[j].compare(orderedTags[tagsIndex]) == 0){
                    if ((find(history.begin(), history.end(), s.getContentList()[i]) == history.end())){
                        return s.getContentList()[i];
                    }
                }
            }
        }
        tagsIndex++;
    }
    return nullptr;
}



void GenreRecommenderUser::getDuplicate(string name, Session &s) {
    GenreRecommenderUser *GdupUser = new GenreRecommenderUser(name);
    size_t index = 0;
    while (index < history.size()) { //copy the watch history
        GdupUser->history.push_back(history[index]);
        index++;
    }
    s.inserUserMap(name,GdupUser);
}


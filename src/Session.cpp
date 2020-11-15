//
// Created by coralshem on 11/19/19.
//

#include "../include/Session.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/Action.h"
#include "../include/json.hpp"
#include "../include/User.h"
#include "../include/Watchable.h"

using namespace nlohmann;
using namespace std;

Session::Session(const std::string &configFilePath) : content(), actionsLog(), userMap(), activeUser(), inputVec(), exitFlag() {
    //initialize a default user
    string defaultUserName = "default";
    LengthRecommenderUser *def = new LengthRecommenderUser(defaultUserName); // create a default user
    this->inserUserMap(defaultUserName,def);
    this->setActiveUser(*def);
    this->exitFlag = true;
    //read from json file
    ifstream i(configFilePath, ifstream::binary);
    json myFile;
    i >> myFile;
    //update the content vector
    //movies
    int index = 0;
    while (myFile["movies"][index] != nullptr){
        string name = myFile["movies"][index]["name"];
        int length = myFile["movies"][index]["length"];
        vector<string> tags;
        int tagIndex = 0;
        while (myFile["movies"][index]["tags"][tagIndex] != nullptr){
            tags.push_back(myFile["movies"][index]["tags"][tagIndex]);
            tagIndex++;
        }
        this->content.push_back(new Movie(index+1,name,length,tags));
        index++;
    }
    //TV series
    int SeriesIndex = 0;
    while (myFile["tv_series"][SeriesIndex] != nullptr){
        string name = myFile["tv_series"][SeriesIndex]["name"];
        int episode_length = myFile["tv_series"][SeriesIndex]["episode_length"];
        vector<string> tags;
        int tagIndex = 0;
        while (myFile["tv_series"][SeriesIndex]["tags"][tagIndex] != nullptr){
            tags.push_back(myFile["tv_series"][SeriesIndex]["tags"][tagIndex]);
            tagIndex++;
        }
        int seasonsIndex = 0;
        while (myFile["tv_series"][SeriesIndex]["seasons"][seasonsIndex] != nullptr){

            for(size_t i =1; i <=myFile["tv_series"][SeriesIndex]["seasons"][seasonsIndex]; i++ ){
                this->content.push_back(new Episode(index+1,name,episode_length,seasonsIndex+1,i,tags));
                index++;
            }
            seasonsIndex++;
        }

        SeriesIndex++;
    }
}

Session::~Session() {//destructor
    for (size_t i=0;i<actionsLog.size();i++){//deleting the actionslog
        delete (actionsLog[i]);
    }
    for (auto &it:userMap){//deleting all the users
        delete(it.second);
    }
    for(size_t i=0;i<content.size();i++){
            delete(content[i]);
    }
}

//copy constructor
Session::Session (const Session &s): content(), actionsLog(), userMap(), activeUser(), inputVec(), exitFlag() {
    if (this != &s) {
        for (size_t j = 0; j < s.content.size(); j++) {
            this->content.push_back(s.content[j]->clone());
        }
        for (size_t i = 0; i < s.actionsLog.size(); i++) {
            this->actionsLog.push_back(s.actionsLog[i]->clone());
        }
        auto it = s.userMap.begin();
        while (it != s.userMap.end()) {
            User *u = it->second->clone();
            u->setHistory(*this);  // update the history of the user
            this->userMap.insert({u->getName(), u});
            it++;
        }

        this->activeUser = this->userMap.at(s.activeUser->getName());
        this->exitFlag = true;
    }
}

Session::Session(Session &&s) : content(s.content), actionsLog(s.actionsLog), userMap(s.userMap), activeUser(s.activeUser), inputVec(s.inputVec), exitFlag(true) {//move constructor
    if (this != &s) {
        s.inputVec.clear();
        s.userMap.clear();
        s.content.clear();
        s.actionsLog.clear();
        s.activeUser = nullptr;
    }
}

Session& Session::operator=(const Session & s) {//copy assignment operator
    if(this == &s){//if they are the same, do nothing
        return *this;
    } else{
        for(size_t i=0;i<content.size();i++){
            delete(content[i]);
        }
        for (size_t i=0;i<actionsLog.size();i++){//deleting the actionslog
            delete (actionsLog[i]);
        }
        for (auto &it:userMap){//deleting all the users
            delete(it.second);
        }
        inputVec.clear();
        userMap.clear();
        actionsLog.clear();
        content.clear();
        activeUser= nullptr;

        //copying from the input session to this session
        for (size_t j = 0; j < s.content.size(); j++) {
            this->content.push_back(s.content[j]->clone());
        }
        for (size_t i = 0; i < s.actionsLog.size(); i++) {
            this->actionsLog.push_back(s.actionsLog[i]->clone());
        }
        auto it = s.userMap.begin();
        while (it != s.userMap.end()) {
            User *u = it->second->clone();
            u->setHistory(*this); // update the history of the user
            this->userMap.insert({u->getName(), u});
            it++;
        }
        this->activeUser = this->userMap.at(s.activeUser->getName());
        this->exitFlag = true;
        return *this;
    }

}

 Session& Session::operator=( Session & s) {//move assignment operator
    //deleting the current session pointers
    for (size_t i = 0; i < content.size(); i++) {
        delete (content[i]);
    }
    for (size_t i = 0; i < actionsLog.size(); i++) {//deleting the actionslog
        delete (actionsLog[i]);
    }
    for (auto &it:userMap) {//deleting all the users
        delete (it.second);
    }
    userMap.clear();
    inputVec.clear();
     actionsLog.clear();
     content.clear();

    //copying from the input session to this session
    this->content= s.content;
    this->actionsLog=s.actionsLog;
    this->userMap = s.userMap;
    this->activeUser = this->userMap.at(s.activeUser->getName());
    this->exitFlag = true;

    //deleting the information from the input session ("stealing it")
    s.content.clear();
    s.actionsLog.clear();
    s.userMap.clear();
    s.inputVec.clear();
    s.activeUser = nullptr;

     return *this;
}


void Session::start() {
    printf("SPLFLIX is now on!");
    string input3;
    while (exitFlag == true) {

        getline(cin , input3);

        //split the string and push it into a vector
        istringstream s(input3);

        do {
            string word;
            s >> word;
            this->inputVec.push_back(word);
        } while (s);

        if (inputVec[0].compare("createuser") == 0) {
            CreateUser *newUser = new CreateUser();
            newUser->act(*this);
        } else if (inputVec[0].compare("changeuser") == 0) {
            ChangeActiveUser *ActiveUser = new ChangeActiveUser();
            ActiveUser->act(*this);
        } else if (inputVec[0].compare("deleteuser") == 0) {
            DeleteUser *UserDeleted = new DeleteUser();
            UserDeleted->act(*this);
        } else if (inputVec[0].compare("dupuser") == 0) {
            DuplicateUser *DupUser = new DuplicateUser();
            DupUser->act(*this);
        } else if (inputVec[0].compare("content") == 0) {
            PrintContentList *pContent = new PrintContentList();
            pContent->act(*this);
        } else if (inputVec[0].compare("watchhist") == 0) {
            PrintWatchHistory *wHistory = new PrintWatchHistory();
            wHistory->act(*this);
        } else if (inputVec[0].compare("watch") == 0) {
            Watch *watch = new Watch();
            watch->act(*this);
        } else if (inputVec[0].compare("log") == 0) {
            PrintActionsLog *aLog = new PrintActionsLog();
            aLog->act(*this);
        } else if (inputVec[0].compare("exit") == 0){
            this->exitFlag = false;
            Exit *exit= new Exit();
            exit->act(*this);
        }
        else cout << "invalid action" << endl;

        this->inputVec.clear(); //clear all the strings in the input vector

    }
}

vector<string> Session::getInput (){ return inputVec; }

vector<BaseAction*> &Session::getActionLog() { return this->actionsLog;}


unordered_map<string, User*> &Session::getUserMap() { return this->userMap;}

//check if user is found in the UserMap
string Session::checKey(unordered_map<string,User*> UserMap, string key ) {
    if (UserMap.find(key) == UserMap.end()) return "not found";
    return "found";
}

vector<Watchable*> &Session::getContentList() { return this->content;}

User& Session::getUser(string userName) { //find a user in the map according to his name
    User *user;
    for (auto &i : this->getUserMap()){
        if (i.first.compare(userName) == 0)
            user = i.second ;
    }
    return *user;
}

User& Session::getActiveUser() {return *activeUser;}

void Session::setActiveUser(User &user) {
    this->activeUser = &user;
}

void Session::setInputVec(string str) {
    this->inputVec[1] = str;
}

void Session::inserUserMap(std::string s, User *u) {
    userMap.insert({s,u});
}

void Session::eraseFromUserMap(std::string s) {
    userMap.erase(s);
}

void Session::addToActionLog(BaseAction *action) {
    actionsLog.push_back(action);
}

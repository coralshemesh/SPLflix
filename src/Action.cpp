#include "../include/Action.h"
#include <string>
#include <iostream>
#include "../include/Session.h"
#include "../include/User.h"
#include "../include/Watchable.h"
#include <algorithm>
#include <sstream>
 using namespace std;


// BaseAction class
BaseAction::BaseAction() : errorMsg("Error"), status(PENDING) {} //constructor
BaseAction::BaseAction(BaseAction &action) : errorMsg(action.errorMsg), status(action.status) {} //copy constructor
ActionStatus BaseAction::getStatus() const { return status;}
void BaseAction::complete() {status = COMPLETED;}
void BaseAction::error(const std::string &errorMsg) { status = ERROR; this->errorMsg = errorMsg; cout << errorMsg<<endl;}
std::string BaseAction::getErrorMsg() const { return errorMsg;}


//CreateUser class
CreateUser::CreateUser(){}

void CreateUser::act(Session& sess){
    vector<string> sessVec = sess.getInput(); // the input we get from the user
    //create a new user according to the algorithm the user chose
    if (sessVec[2].compare( "len")  == 0){
        LengthRecommenderUser *uLen = new LengthRecommenderUser(sessVec[1]);
        sess.inserUserMap(sessVec[1], uLen);
        this->complete();
    }
    else if ( sessVec[2].compare("rer")==0 ){
        RerunRecommenderUser *uRer = new RerunRecommenderUser(sessVec[1]);
        sess.inserUserMap(sessVec[1], uRer);
        this->complete();
    }
    else if (sessVec[2].compare("gen")==0){
        GenreRecommenderUser *uGen = new GenreRecommenderUser(sessVec[1]);
        sess.inserUserMap(sessVec[1], uGen);
        this->complete();
    }
    else{
        this->error("invalid algorithm");
    }
    sess.addToActionLog(this);
}

string CreateUser::toString() const {
    if (this->getStatus() == ERROR){
        return " createuser ERROR " + this->getErrorMsg();
    }
    else if (this ->getStatus() == COMPLETED){
        return " createuser COMPLETED";
    }
    return " createuser PENDING";
}

BaseAction* CreateUser::clone() {
    return new CreateUser(*this);
}

// DeleteUser class
DeleteUser::DeleteUser() {}

void DeleteUser::act(Session &sess) {
    vector<string> sessVec = sess.getInput();
    if (sess.checKey(sess.getUserMap(),sessVec[1]).compare("found")==0) { //if this user exist
        unordered_map<string,User*>::const_iterator got=sess.getUserMap().find(sessVec[1]);
        delete(got->second);
        sess.eraseFromUserMap(sessVec[1]);
        this->complete();
    }
    else {
        this->error("user not found");
    }
    sess.addToActionLog(this);
}

string DeleteUser::toString() const {
    if (this->getStatus() == ERROR){
        return " deleteuser ERROR " + this->getErrorMsg();
    }
    else if (this ->getStatus() == COMPLETED){
        return " deleteuser COMPLETED";
    }
    return " deleteuser PENDING";
}

BaseAction* DeleteUser::clone() {
    return new DeleteUser(*this);
}

//DuplicateUser class
DuplicateUser::DuplicateUser() {} //constructor

void DuplicateUser::act(Session &sess) {
    vector<string> sessVec = sess.getInput();
    if (sess.checKey(sess.getUserMap(),sessVec[1]).compare("not found")==0){
        this->error("user not found");

    }
    else if (sess.checKey(sess.getUserMap(),sessVec[2]).compare("found")==0){
        this->error("there is already a user in this name");
    }
    else{
        sess.getActiveUser().getDuplicate(sessVec[2], sess);
    }
    sess.addToActionLog(this);
}

string DuplicateUser::toString() const {
    if (this->getStatus() == ERROR){
        return " duplicateuser ERROR " + this->getErrorMsg();
    }
    if (this->getStatus() == COMPLETED){
        return " duplicateuser COMPLETED";
    }
    return " duplicateuser PENDING";
}


BaseAction* DuplicateUser::clone() {
    return new DuplicateUser(*this);
}

//PrintContentList class
PrintContentList::PrintContentList() {}

void PrintContentList::act(Session &sess) {
    for(size_t index =0; index < sess.getContentList().size(); index++ ){
        cout << sess.getContentList()[index]->toString()<< endl;
    }
    this->complete();
    sess.addToActionLog(this);
}


string PrintContentList::toString() const {
    if (this->getStatus() == COMPLETED){
        return " printcontentlist COMPLETED";
    }
    return " printcontentlist PENDING";
}

BaseAction* PrintContentList::clone() {
    return new PrintContentList(*this);
}

//printActionsLog class
PrintActionsLog::PrintActionsLog() {} //constructor

void PrintActionsLog::act(Session &sess) {
    vector<BaseAction*> actionsLog = sess.getActionLog();
    if(actionsLog[0]!= nullptr) {
        for (int i = actionsLog.size() - 1; i >= 0; i--) {
                cout << actionsLog[i]->toString() << endl;
        }
    }
    this->complete();
    sess.addToActionLog(this);
}


string PrintActionsLog::toString() const {
    if (this->getStatus() == ERROR){
        return " printactionlog ERROR " + this->getErrorMsg();
    }
    if (this->getStatus() == COMPLETED){
        return " printactionlog COMPLETED";
    }
    return " printactionlog PENDING";
}

BaseAction* PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}


//ChangeActiveUser class
ChangeActiveUser::ChangeActiveUser() {}

void ChangeActiveUser::act(Session &sess) {
    vector<string> sessVec=sess.getInput();
    unordered_map<string,User*>::const_iterator got=sess.getUserMap().find(sessVec[1]);
    if(got==sess.getUserMap().end()){
        this->error("The user name does not exist");
        sess.addToActionLog(this);
    }
    else{
        User *user = got->second;
        sess.setActiveUser(*user);
        this->complete();
        sess.addToActionLog(this);
    }

}

string ChangeActiveUser::toString() const {
    if (this->getStatus() == ERROR){
        return "changeactiveuser ERROR " + this->getErrorMsg();
    }
    if (this->getStatus() == COMPLETED){
        return " changeactiveuser COMPLETED";
    }
    return " changeactiveuser PENDING";
}

BaseAction* ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}


//PrintWatchHistory: class
PrintWatchHistory::PrintWatchHistory() {}

void PrintWatchHistory::act(Session &sess) {
    vector<Watchable*> history;
    history=sess.getActiveUser().get_history();
    string name = sess.getActiveUser().getName();
    cout << "Watch history for " << name << endl;
    if(!history.empty()) {
        for (size_t i = 0; i < history.size();) {
            string output = history[i]->stringToWatch();
            i++;
            cout << i << ". " + output << endl;
        }
    }
    this->complete();
    sess.addToActionLog(this);
}


string PrintWatchHistory::toString() const {
    if (this->getStatus() == ERROR){
        return " printwatchhistory ERROR " + this->getErrorMsg();
    }
    if (this->getStatus() == COMPLETED){
        return " printwatchhistory COMPLETED";
    }
    return " printwatchhistory PEDING";
}

BaseAction* PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);
}


//Watch class
Watch::Watch() {}

void Watch::act(Session &sess) {
    vector<string> inputVec = sess.getInput();
    string s = inputVec[1];
    stringstream temp(s);
    size_t i = 0;
    temp >> i;
    if (i < sess.getContentList().size() - 1) {
        string name = sess.getContentList()[i - 1]->getName();
        cout << "Watching " << sess.getContentList()[i - 1]->stringToWatch() << endl;
        sess.getActiveUser().addHistory(
                sess.getContentList()[i - 1]); //insert the movie/episode to the user's history list
        this->complete();
        sess.addToActionLog(this);
        string nr = sess.getActiveUser().getWatchHistory()[sess.getActiveUser().getWatchHistory().size() -
                                                           1]->getNextWatchable(
                sess)->stringToWatch();//next recommendation
        cout << "We recommend watching " + nr + ", continue watching? [y/n]" << endl;
        char input;
        scanf("%c", &input);
        while (getchar() != '\n');
        if (input == 'y') {
            int nextIndex = sess.getActiveUser().getWatchHistory()[sess.getActiveUser().getWatchHistory().size() -
                                                                   1]->getNextWatchable(sess)->getId();
            string sIndex = to_string(
                    nextIndex);//the input vector is a string vector so we change it from int to string
            sess.setInputVec(sIndex);
            Watch *newWatch = new Watch();
            newWatch->act(sess);
        } else {}
    }
    else{
        this->error("index out of bound");
    }
}

string Watch::toString() const {
    if (this->getStatus() == ERROR){
        return " watch ERROR " + this->getErrorMsg();
    }
    if(this->getStatus() == COMPLETED){
        return " watch COMPLETED";
    }
    return " watch PENDING";
}


BaseAction* Watch::clone() {
    return new Watch(*this);
}


//Exit class
Exit::Exit() {}

void Exit::act(Session &sess) {
    this->complete();
    sess.addToActionLog(this);
}

string Exit::toString() const {
    if (this->getStatus() == ERROR){
        return " exit ERROR " + this->getErrorMsg();
    }
    if(this->getStatus() == COMPLETED){
        return " exit COMPLETED";
    }
    return " exit PENDING";
}


BaseAction* Exit::clone() {
    return new Exit(*this);
}

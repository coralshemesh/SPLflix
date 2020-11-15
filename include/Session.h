#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);
    ~Session();
    Session (const Session &);
    Session(Session&&);//move constructor
    Session& operator=(const Session &);//copy assignment operator
    Session& operator=(Session &);//move assignment operator
    void start();
    std::vector<std::string> getInput ();
    std::vector<BaseAction*> &getActionLog();
    std::unordered_map<std::string, User*> &getUserMap();
    std::vector<Watchable*> &getContentList();
    std::string checKey(std::unordered_map<std::string, User*>, std::string);
    User& getUser(std::string);
    User& getActiveUser();
    void setActiveUser (User &user);
    void setInputVec (std::string);
    void inserUserMap(std::string, User*);
    void eraseFromUserMap(std::string);
    void addToActionLog(BaseAction*);
private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    std::vector<std::string> inputVec; // contains the input from the user . example- "[createuser, yossi, len]"
    bool exitFlag;
};
#endif

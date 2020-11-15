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
    virtual ~User();
    virtual User* clone()=0;
    virtual Watchable* getRecommendation(Session& s) = 0;
    virtual void getDuplicate (std::string, Session&) =0; //visitor pattern - for duplicate method
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    std::vector<Watchable*> getWatchHistory();
    void addHistory(Watchable*);
    int getIndex();
    void setIndex();
    int getAverage(); //returns the average length value
    void setHistory(Session&); // change the history pointers in the copy constructor
protected:
    std::vector<Watchable*> history;
private:
    std::string name;
    int index; //for rer recommendation
    int average; // for length recommendation

};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    virtual ~LengthRecommenderUser(){}
    virtual Watchable* getRecommendation(Session& s);
    virtual void getDuplicate (std::string, Session&);
    virtual User* clone() ;
private:
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    virtual ~RerunRecommenderUser(){}
    virtual Watchable* getRecommendation(Session& s);
    virtual void getDuplicate (std::string, Session&);
    virtual User* clone() ;
private:
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    virtual ~GenreRecommenderUser(){}
    virtual Watchable* getRecommendation(Session& s);
    virtual void getDuplicate (std::string, Session&);
    virtual User* clone() ;
private:
};

#endif

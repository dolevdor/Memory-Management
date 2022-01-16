#ifndef STUDIO_H_
#define STUDIO_H_

#include <vector>
#include <string>
#include "Workout.h"
#include "Trainer.h"
#include "Action.h"
#include "Studio.h"

extern Studio* backup;

class Studio{
public:
    Studio();
    // Rule of 5
    virtual ~Studio(); // Destructor
    Studio(Studio const &other); // Copy Constructor
    Studio& operator=(const Studio &other); // Assignment Operator
    Studio(Studio && other); // Move Constructor
    Studio& operator=(Studio&& other); // Move Assignment Operator
    Studio(const std::string &configFilePath);
    void start();
    int getNumOfTrainers() const;
    Trainer* getTrainer(int tid);
    BaseAction* buildAction (string& command);
    const std::vector<BaseAction*>& getActionsLog() const; // Return a reference to the history of actions
    std::vector<Workout>& getWorkoutOptions();
    void addLog (BaseAction& s); // The function receives a string which is an input from the user, and adds it to the log along
    // with completed or error, that is stored in a different array

private:
    int a; // C++ bug
    int custNum = 0;
    std::vector<Workout> workout_options;
    std::vector<BaseAction*> actionsLog;
    bool open;
    std::vector<Trainer*> trainers;

};

#endif
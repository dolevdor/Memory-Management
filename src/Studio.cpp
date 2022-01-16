
#include <iostream>
#include <fstream>
#include "../include/Studio.h"
#include <string>
#include <sstream>
#include "../include/Action.h"
#include "../include/Customer.h"

using namespace std;

Studio::Studio(){};

Studio::~Studio() { // Destructor
    for (size_t i = 0; i < actionsLog.size(); i++){
        if(actionsLog[i])
            delete actionsLog[i];
    }

    for (size_t i = 0; i < trainers.size(); i++){
        if (trainers[i])
            delete trainers[i];
    }

    open = false;
}

Studio::Studio(Studio const &other) { // Copy Constructor
    for (size_t i = 0; i < other.workout_options.size(); i++)
        workout_options.push_back(other.workout_options[i]);;

    for (size_t i = 0; i < other.actionsLog.size(); i++) {
        actionsLog.push_back(other.actionsLog[i]->clone());
    }

    open = other.open;

    for (size_t i = 0; i < other.trainers.size(); i++){
        trainers.push_back(new Trainer(*other.trainers[i]));
    }
}

Studio& Studio::operator=(const Studio &other){// copy assignment opertator
    if(this!=&other) {
        //clears studio's fields
        workout_options.clear();

        for (size_t i = 0; i < actionsLog.size(); i++) {
           actionsLog[i] = nullptr;
        }
        actionsLog.clear();

        for (size_t i = 0; i < trainers.size(); i++) {
            trainers[i] = nullptr;
        }
        trainers.clear();

        open = other.open;

        //puts new arguments to studio
        for (size_t i = 0; i < other.workout_options.size(); i++)
            workout_options.push_back(other.workout_options[i]);

        for (size_t i = 0; i < other.actionsLog.size(); i++) {
            actionsLog.push_back(other.actionsLog[i]->clone());
        }
        for (size_t i = 0; i < other.trainers.size(); i++) {
            trainers.push_back(new Trainer(*(other.trainers[i])));
        }
    }
    return *this;
}
//Move Constructor
Studio::Studio(Studio && other):
                                open(other.open), workout_options(other.workout_options){

    //deep copy of actionsLog
    for (BaseAction *action: other.actionsLog) {
        actionsLog.push_back(action);
    }

    //deep copy of trainers
    for (Trainer *trainer: other.trainers) {
        trainers.push_back(trainer);
    }

};

// Move Assignment Operator
Studio& Studio::operator=(Studio&& other){
    if(this!=&other) {
        //clears this studio's actionsLog
        for(size_t i=0; i < actionsLog.size(); i++){
            if(actionsLog[i] != nullptr){
                delete actionsLog[i];
                actionsLog[i] = nullptr;
            }
        }
        actionsLog.clear();

        //clears this studio's trainers vector
        for(size_t i=0; i < trainers.size(); i++){
            if(trainers [i] != nullptr){
                delete trainers[i];
                trainers[i] = nullptr;
            }
        }
        trainers.clear();

        workout_options.clear();

        for (size_t i = 0; i < other.workout_options.size(); i++)
            workout_options.push_back(other.workout_options[i]);

        open = other.open;

        other.workout_options.clear();

        //deep copy of actionsLog and release other's vetor
        for (BaseAction *action: other.actionsLog) {
            actionsLog.push_back(action);
            action = nullptr;
        }
        other.actionsLog.clear();

        //deep copy of trainers and release other's vetor
        for (Trainer *trainer: other.trainers) {
            trainers.push_back(trainer);
            trainer = nullptr;
        }
        other.trainers.clear();
    }
    return *this;
};

Studio::Studio(const std::string &configFilePath) {

    ifstream configFile;
    configFile.open(configFilePath);

    if (configFile.is_open()) { // Reading from config file
        string line;
        int counter = 0;
        int trainerNum = 0;
        int workoutId = 0;
        int currId=0;
        while (getline(configFile, line)) {
            if (line != "" && line[0] != '#') { // If the line is relevant to read from
                counter++; // line number

                stringstream lineStream(line);
                string token;

                if (counter == 1) { // The line that represents the number of trainers
                    trainerNum = stoi(line);
                } else if (counter == 2) { // The line that represents the capacity of the trainers
                    while (getline(lineStream, token, ',')) { // While capacity line isn't done
                        trainers.push_back(
                                new Trainer(stoi(token))); // Adds a pointer to a trainer to the trainers vector
                    }
                } else { // These lines represent the workouts details
                    int inCount = 1; // Segments the line to the different parts
                    int price;
                    string name;
                    WorkoutType type;

                    while (getline(lineStream, token, ',')) { // Reads the workout details
                        if (inCount == 1) // Reading the workout name
                            name = token;
                        else if (inCount == 2) { // Reading the workout type
                            if (token.compare(" Anaerobic") == 0)
                                type = ANAEROBIC;
                            else if (token.compare(" Mixed") == 0)
                                type = MIXED;
                            else
                                type = CARDIO;
                        } else if (inCount == 3) { // Reading the price
                            price = stoi(token);
                            inCount = 1;
                            workout_options.push_back(*(new Workout(currId, name, price,type))); // Adds the Workout to the workout_options vector
                            currId++;
                        }
                        inCount++;
                    }

                }

            }
        }
    }
};

void Studio::start(){
    open = true;
    std::cout << "Studio is now open! \n";

    while(open){
//        char* command = new char[500];
//        string convert(command);
//        string* s = &convert;
//        cin.getline(command,500);
        string command;
        getline(cin,command);
        BaseAction* Bact = buildAction(command);
        Bact->act(*this);
        if (dynamic_cast <CloseAll *>(Bact)) // Checks if Bact is instance of CloseAll
            this->~Studio();
    }
};

int Studio::getNumOfTrainers() const{
    return trainers.size();
};
Trainer* Studio::getTrainer(int tid){
    if (tid >= 0 && tid <= trainers.size()-1)
        return trainers[tid];
    else
        return nullptr;
};

BaseAction* Studio::buildAction (string& command){

    if ((command.substr(0,4)).compare("open") == 0){
//        char tId = command[5];
//        const char *cid= tId.c_str();
//        char ch = command[5];
//        string str = "" + ch;
//        char c = str[0];
//        int id = c;

        int id = stoi(command.substr(5));

        vector<Customer*> custmrVec;
        OpenTrainer *OT = new OpenTrainer(id, custmrVec);

        for(size_t i=7; i < command.length() ;i++){
            string name;
            string cType;

            while (command[i]!=(',')){
                name = name + command[i];
                i++;
            }
            i++;
            while (command[i]!=(' ') && i<command.length()){
                cType = cType + (command[i]);
                i++;
            }

            if(cType.compare("swt")==0){
                SweatyCustomer *swtC = new SweatyCustomer(name ,custNum);
                OT->addCustomer(swtC);
            }
            if(cType.compare("chp")==0){
                CheapCustomer *chpC = new CheapCustomer(name ,custNum);
                OT->addCustomer(chpC);
            }
            if(cType.compare("mcl")==0){
                HeavyMuscleCustomer *mclC = new HeavyMuscleCustomer(name ,custNum);
                OT->addCustomer(mclC);
            }
            if(cType.compare("fbd")==0){
                FullBodyCustomer *fbdC = new FullBodyCustomer(name ,custNum);
                OT->addCustomer(fbdC);
            }
            custNum++;
        }
        actionsLog.push_back(OT);

        return OT;
    };

    if (command.substr(0,5).compare("order") == 0){
        int id = stoi(command.substr(5));

        Order *OD = new Order(id);
        actionsLog.push_back(OD);
        return OD;
    };

    if (command.substr(0,4).compare("move") == 0){
        int srcId = stoi(command.substr(5));
        int dstId = stoi(command.substr(7));
        int cId = stoi(command.substr(9));
        MoveCustomer *MO = new MoveCustomer(srcId,dstId,cId);
        actionsLog.push_back(MO);
        return MO;

    };
    if (command.substr(0,8).compare("closeall") == 0){
        CloseAll *CA = new CloseAll();
        //actionsLog.push_back(CA);

        return CA;
    };
    if (command.substr(0,5).compare("close") == 0){
        string trainerId = "";
        for (size_t i=6; i<command.length(); i++) {
            trainerId = trainerId + command[i];
        }
        int trainId = stoi(trainerId);
        Close *CL = new Close(trainId);
        actionsLog.push_back(CL);
        return CL;
    };
    if (command.substr(0,6).compare("status") == 0){
        string trainerId = "";
        for (size_t i=6; i<command.length(); i++) {
            trainerId = trainerId + command[i];
        }
        int trainId = stoi(trainerId);

        PrintTrainerStatus *PTS = new PrintTrainerStatus(trainId);
        actionsLog.push_back(PTS);
        return PTS;
    };
    if (command.substr(0,15).compare("workout_options") == 0){
        PrintWorkoutOptions *PWO = new PrintWorkoutOptions;
        actionsLog.push_back(PWO);
        return PWO;
    };
    if (command.substr(0,3).compare("log") == 0){
        PrintActionsLog *PAL = new PrintActionsLog;
        actionsLog.push_back(PAL);
        return PAL;
    }
    if(command.substr(0,7).compare("restore")==0) {
        RestoreStudio *RS = new RestoreStudio;
        actionsLog.push_back(RS);
        return RS;
    }
    else{
       // (command.substr(0,6).compare("backup")){
        BackupStudio *BU = new BackupStudio;
        actionsLog.push_back(BU);
        return BU;
    };
};

const std::vector<BaseAction*>& Studio::getActionsLog() const{
    return actionsLog;
};
std::vector<Workout>& Studio::getWorkoutOptions(){
    return workout_options;
};

void Studio::addLog (BaseAction& s){
    actionsLog.push_back(&s);
}
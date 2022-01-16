#include "../include/Action.h"
#include "../include/Studio.h"
using namespace std;

BaseAction::BaseAction(){};
// BaseAction* BaseAction::clone() const{};
BaseAction::~BaseAction(){};
ActionStatus BaseAction::getStatus() const{
    return status;
};
void BaseAction::act(Studio& studio){};
// std::string BaseAction::toString() const{};
void BaseAction::complete(){
    status = COMPLETED;
    errorMsg = "Completed";
};
void BaseAction::error(std::string errorMsg){
    status = ERROR;
    this->errorMsg = "Error: " + errorMsg;
};
std::string BaseAction::getErrorMsg() const{
    return errorMsg;
};

// All toString actions are used to print the user's action that were logged, along with completed/error

//constructor
OpenTrainer::OpenTrainer(int id, std::vector<Customer *> &customersList): trainerId(id){
    copy(id,vector<Customer*>());
};

//copy constructor
OpenTrainer::OpenTrainer(const OpenTrainer &other) : trainerId(other.trainerId) {
    copy(trainerId, other.customers);
};

//copy assignment operator
void OpenTrainer::operator=(const OpenTrainer &other){
    if(this != &other){
        for (size_t i = 0; i < customers.size(); i++){
            customers[i] = nullptr;
        }
        copy(other.trainerId, other.customers);
    }
};
//move constructor
OpenTrainer::OpenTrainer(OpenTrainer &&other):trainerId(other.trainerId) {
    for(size_t i=0; i<customers.size();i++)
        customers[i]=nullptr;

    copy(other.trainerId, other.customers);

    // Release other's resources
    for(size_t i=0; i<other.customers.size();i++)
        other.customers[i]=nullptr;
};

//destructor
OpenTrainer::~OpenTrainer() {clear();}

//this function helps the destructor
void OpenTrainer::clear(){
    for(size_t i=0; i<customers.size();i++){
        if(customers[i]){//if this place is not nullptr
            customers[i] = nullptr;
        }
    }
};

BaseAction* OpenTrainer::clone() const{
    return new OpenTrainer(*this);
};

void OpenTrainer::copy(const int otherId, const vector<Customer *> otherCustomers) {
    //deep copy
    customers.clear();
    for (size_t i = 0; i < otherCustomers.size(); i++) {
        if (otherCustomers[i]) {//if this place is not nullptr
            customers.push_back(otherCustomers[i]->clone());
        }
    };
}
void OpenTrainer::act(Studio &studio){
    Trainer* myTrain = studio.getTrainer(trainerId);
    if (!myTrain || (myTrain->isOpen())){
        error("Workout session does not exist or is already open");
        cout << "Error: Workout session does not exist or is already open\n";
    }
    //changes the customer list to a one that trainer capable to accept
    if (customers.size() > myTrain->getCapacity()){
        myTrain-> openTrainer(); // Trainer is open
        for (size_t i=0; i<myTrain->getCapacity(); i++)
        {
            myTrain->addCustomer(customers[i]);
        }
        complete();
    }
    else{
        myTrain-> openTrainer(); // Trainer is open
        for (size_t i=0; i<customers.size(); i++)
        {
            myTrain->addCustomer(customers[i]);
        }
        complete();
    }


};
std::string OpenTrainer::toString() const{
    string s = "open " + to_string(trainerId);
    for (size_t i=0; i<customers.size(); i++){
        s = s + " " + customers[i]->getName() + "," + customers[i]->toString();
    }
    s = s+ " " +getErrorMsg();
    return s;

};
void OpenTrainer:: addCustomer(Customer *customer){
    customers.push_back(customer);
    complete();
}

Order::Order(int id): trainerId(id){};
void Order::act(Studio &studio){
    Trainer* t1 = studio.getTrainer(trainerId);
    vector <Customer*> myCust = t1->getCustomers();

    if(!t1->isOpen() || t1->getCapacity() < myCust.size() ){
        error("Trainer does not exist or is not open\n");
        cout << (getErrorMsg());
    }
    else {
        vector < Workout > myWorksoptions = studio.getWorkoutOptions();
        for (size_t i = 0; i < myCust.size(); i++) {  // Calling trainer's order function with each customer
            t1->order(myCust[i]->getId(), myCust[i]->order(myWorksoptions), myWorksoptions);
        }
        complete();
    }

    for(size_t i= 0; i<t1->getOrders().size();i++){ //run over all the trainer's orders
        cout<<t1->getCustomer(t1->getOrders()[i].first)->getName()<<" Is Doing "
        <<t1->getOrders()[i].second.getName()<<"\n";
    }
};
std::string Order::toString() const{
    return ("order " + to_string(trainerId) + " " + getErrorMsg());
};

BaseAction* Order::clone() const{
    return new Order(*this);
};

MoveCustomer::MoveCustomer(int src, int dst, int customerId): srcTrainer(src), dstTrainer(dst), id(customerId){};
void MoveCustomer::act(Studio &studio) {
    Trainer *srcT = studio.getTrainer(srcTrainer);
    Trainer *dstT = studio.getTrainer(dstTrainer);
    Customer *toMove = srcT->getCustomer(id);

    if (srcT == nullptr || dstT == nullptr || !srcT->isOpen() || !dstT->isOpen()
        || toMove == nullptr || dstT->getCustomers().size() == dstT->getCapacity()) {
        cout << "Error: Cannot move customer\n";
        error("Cannot move customer");
    } else {
        dstT->addCustomer(toMove);
        vector<int> custOrders;
        vector<int> cust;
        for (size_t i = 0; i < srcT->getOrders().size(); i++) {
            if (srcT->getOrders()[i].first == toMove->getId()) {
                custOrders.push_back(srcT->getOrders()[i].second.getId()); // Collect all customer's workout id's
            }
        }
        dstT->order(toMove->getId(), custOrders, studio.getWorkoutOptions());
        srcT->removeCustomer(id);

        if (srcT->getCustomers().size() == 0)
            Close(srcTrainer).act(studio);
    }
}

std::string MoveCustomer::toString() const{
    return ("move " + to_string(srcTrainer) + " " + to_string(dstTrainer) + " " + to_string(id) + " " + getErrorMsg());
};
BaseAction* MoveCustomer::clone() const{
    return new MoveCustomer(*this);
};

Close::Close(int id) : trainerId(id) {};
void Close::act(Studio &studio) {
    Trainer *myTrain = studio.getTrainer(trainerId);
    if (!myTrain || !(myTrain->isOpen())) {
        error("Trainer does not exist or is not open");
        cout << "Trainer does not exist or is not open";
    } else {
        int salary = myTrain->getSalary();
        myTrain->closeTrainer();
        std::cout << "Trainer " << trainerId << " closed. Salary " << salary << "NIS" << "\n";
        complete();
    }
};

std::string Close::toString() const{
    return ("close " + to_string(trainerId) + " " + getErrorMsg());
};
BaseAction* Close::clone() const{
    return new Close(*this);
};


CloseAll::CloseAll(){};
void CloseAll::act(Studio &studio){
    int trainersNum = studio.getNumOfTrainers();
    for (size_t i=0; i<trainersNum; i++) // Closing all trainers that are open
    {
        if (studio.getTrainer(i) && studio.getTrainer(i)->isOpen()){ // If trainer exists & is open
            Close(i).act(studio);
        }
    }
    complete();
};
std::string CloseAll::toString() const{
    return ("closeall " + getErrorMsg());
};
BaseAction* CloseAll::clone() const{
    return new CloseAll(*this);
};

PrintWorkoutOptions::PrintWorkoutOptions(){};
void PrintWorkoutOptions::act(Studio &studio){
    std::vector<Workout> workoutoptions = studio.getWorkoutOptions();
    int size = workoutoptions.size();
    for (size_t i=0; i<size; i++){
        string tempName = workoutoptions[i].getName();
        int tempPrice = workoutoptions[i].getPrice();
        WorkoutType tempType = workoutoptions[i].getType();
        string tempPrintType;
        if (tempType == 0)
            tempPrintType = "Anaerobic";
        else if (tempType == 1)
            tempPrintType = "Mixed";
        else if (tempType == 2)
            tempPrintType = "Cardio";

        cout<< tempName << ", " << tempPrintType << ", " << tempPrice<<"\n";

    }
    complete();
};
std::string PrintWorkoutOptions::toString() const{
    return ("workout_options " + getErrorMsg());
};
BaseAction* PrintWorkoutOptions::clone() const{
    return new PrintWorkoutOptions(*this);
};


PrintTrainerStatus::PrintTrainerStatus(int id): trainerId(id){};
void PrintTrainerStatus::act(Studio &studio){
    Trainer* myTrain = studio.getTrainer(trainerId);
    if (!myTrain->isOpen()) // if the Trainer is closed
        cout << "Trainer " << trainerId << " status: closed \n";
    else{
        cout << "Trainer " << trainerId << " status: open \n";
        cout << "Customers: \n";
        vector<Customer*> myCust = myTrain->getCustomers();
        int tempId;
        string tempName;
        for (size_t i=0; i<myCust.size(); i++){ // Printing all the trainer's customers with ids and names
            tempId = myCust[i]->getId();
            tempName = myCust[i]->getName();
            cout<<tempId<<" "<<tempName<<"\n";
        }
        vector<OrderPair> myOrd = myTrain->getOrders();
        int tempId1;
        int tempPrice;
        string workoutName;
        cout << "Orders:\n";
        for (size_t i=0; i<myOrd.size(); i++){ // Printing all the trainer's workouts with name, price and id
            tempId = myOrd[i].first;
            tempPrice = myOrd[i].second.getPrice();
            workoutName = myOrd[i].second.getName();
            cout<<workoutName<<" "<<tempPrice<<"NIS "<< tempId <<"\n";
        }
        cout<<"Current Trainer's Salary: "<<myTrain->getSalary()<<"NIS\n";
    }
    complete();
};
std::string PrintTrainerStatus::toString() const{
    return ("status " + to_string(trainerId) + " " + getErrorMsg());
};
BaseAction* PrintTrainerStatus::clone() const{
    return new PrintTrainerStatus(*this);
};


PrintActionsLog::PrintActionsLog(){};
void PrintActionsLog::act(Studio &studio){
    for (size_t i=0; i<studio.getActionsLog().size()-1; i++){ // Printing all user's logs
        cout<<studio.getActionsLog()[i]->toString()<<"\n";

    }
    complete();
};
std::string PrintActionsLog::toString() const{
    return ("log " + getErrorMsg());
};
BaseAction* PrintActionsLog::clone() const{
    return new PrintActionsLog(*this);
};


BackupStudio::BackupStudio(){};
void BackupStudio::act(Studio &studio){
    backup = new Studio(studio);
    complete();
};
std::string BackupStudio::toString() const{
    return ("backup " + getErrorMsg());};

BaseAction* BackupStudio::clone() const{
    return new BackupStudio(*this);
};

RestoreStudio::RestoreStudio(){};

void RestoreStudio::act(Studio &studio){
    if(backup== nullptr) {
        error("No backup available") ;
        cout << ("No backup available");
    }
    studio = *backup;
    complete();
};
std::string RestoreStudio::toString() const{
    return ("restore " + getErrorMsg());
};

BaseAction* RestoreStudio::clone() const{
    return new RestoreStudio(*this);
};
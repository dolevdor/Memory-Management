#include "../include/Customer.h"
#include "../include/Trainer.h"
#include "../include/Studio.h"
#include "../include/Action.h"
#include "../include/Workout.h"
#include <algorithm>
using namespace std;

Customer::Customer(std::string c_name, int c_id):name(c_name), id(c_id){};

string Customer::getName() const{
    return name;
};
int Customer::getId() const{
    return id;
};

SweatyCustomer::SweatyCustomer(std::string name, int id): Customer(name, id){};

vector<int> SweatyCustomer::order(const std::vector<Workout> &workout_options){
    vector<int> myOrder;
    for (size_t i=0; i<workout_options.size(); i++){
        Workout tempWorkout = workout_options[i];
        WorkoutType tempType = tempWorkout.getType(); // ANAEROBIC, MIXED, CARDIO
        if (tempType == 2){ // This costumer orders all the cardio activities (type 2) from first to last
            myOrder.push_back(tempWorkout.getId());
        }
    }
    return myOrder;
};
string SweatyCustomer::toString() const{
    return "swt";
};

Customer* SweatyCustomer::clone() const{
    return new SweatyCustomer(*this);
};

CheapCustomer::CheapCustomer(std::string name, int id): Customer(name, id){};

vector<int> CheapCustomer::order(const std::vector<Workout> &workout_options){
    vector<int> myOrder;
    int min = workout_options[0].getPrice();
    int id = -1;
    for (size_t i=1; i<workout_options.size(); i++){ // Looking for cheapest activity
        Workout tempWorkout = workout_options[i];
        if(tempWorkout.getPrice() < min){
            min = tempWorkout.getPrice();
            id = i;
        }
    }
    myOrder.push_back(id); // This costumer only orders 1 activity, the cheapest one, no matter the type
    return myOrder;
};
string CheapCustomer::toString() const{
    return "chp";
};

Customer* CheapCustomer::clone() const{
    return new CheapCustomer(*this);
};

HeavyMuscleCustomer::HeavyMuscleCustomer(std::string name, int id): Customer(name, id){};

vector<int> HeavyMuscleCustomer::order(const std::vector<Workout> &workout_options){

    vector<int> myOrder;
    vector<Workout> anaerobicActions;

    for (size_t i=0; i<workout_options.size(); i++){ // Creating a vector of Anaerobic exercises only
        Workout tempWorkout = workout_options[i];
        if (tempWorkout.getType() == 0)
            anaerobicActions.push_back(workout_options[i]);
    };

    vector<int> prices;
    for(size_t i=0; i< anaerobicActions.size(); i++){
        prices.push_back(anaerobicActions[i].getPrice());
    }

    stable_sort(prices.begin(), prices.end());

    vector<int> takenId;
    for(size_t a: prices){
        takenId.push_back(0);
    }

    //sorting the anaerobicActions vector
    for(size_t i=prices.size()-1; i >= 0;i--){
        for(size_t j=0; j<anaerobicActions.size();j++) {
            if (anaerobicActions[j].getPrice() == prices[i] && takenId[j]==0) {
            myOrder.push_back(anaerobicActions[j].getId());
            takenId[j]=1;
            }
        }
        if(i==0)
            break; //for avoid infinity loop
    }


    return myOrder;
}

string HeavyMuscleCustomer::toString() const{
    return "mcl";
};

Customer* HeavyMuscleCustomer::clone() const{
    return new HeavyMuscleCustomer(*this);
};

FullBodyCustomer::FullBodyCustomer(std::string name, int id): Customer(name, id){};

vector<int> FullBodyCustomer::order(const std::vector<Workout> &workout_options){
    vector<int> myOrder;

    int cheapestCardioPrice;
    int cheapestCardioId;
    int expensiveMixPrice;
    int expensiveMixId;
    int cheapestAnaerobicPrice;
    int cheapestAnaerobicId;

    bool firstCardio = false;
    bool firstMixed = false;
    bool firstAnaerobic = false;

    for (size_t i=0; i<workout_options.size(); i++){
        Workout tempWorkout = workout_options[i];
        // The loop finds the cheapest Cardio Workout, if there is one
        if (tempWorkout.getType() == 2){
            if (!firstCardio){
                firstCardio = true;
                cheapestCardioId = tempWorkout.getId();
                cheapestCardioPrice = tempWorkout.getPrice();
            }
            else{
                if (tempWorkout.getPrice() < cheapestCardioPrice){
                    cheapestCardioPrice = tempWorkout.getPrice();
                    cheapestCardioId = tempWorkout.getId();
                }
            }
        }

        // The loop finds the most expensive Mixed Type Workout, if there is one
        if (tempWorkout.getType() == 1){
            if (!firstMixed){
                firstMixed = true;
                expensiveMixId = tempWorkout.getId();
                expensiveMixPrice = tempWorkout.getPrice();
            }
            else{
                if (tempWorkout.getPrice() > expensiveMixPrice){
                    expensiveMixPrice = tempWorkout.getPrice();
                    expensiveMixId = tempWorkout.getId();
                }
            }
        }

        // The loop finds the most expensive Mixed Type Workout, if there is one
        if (tempWorkout.getType() == 0){
            if (!firstAnaerobic){
                firstAnaerobic = true;
                cheapestAnaerobicId = tempWorkout.getId();
                cheapestAnaerobicPrice = tempWorkout.getPrice();
            }
            else{
                if (tempWorkout.getPrice() < cheapestAnaerobicPrice){
                    cheapestAnaerobicPrice = tempWorkout.getPrice();
                    cheapestAnaerobicId = tempWorkout.getId();
                }
            }
        }
    }
    if (firstCardio)
        myOrder.push_back(cheapestCardioId);
    if(firstMixed)
        myOrder.push_back(expensiveMixId);
    if(firstAnaerobic)
        myOrder.push_back(cheapestAnaerobicId);

    return myOrder;
};
string FullBodyCustomer::toString() const{
    return "fbd";
};

Customer* FullBodyCustomer::clone() const{
    return new FullBodyCustomer(*this);
};
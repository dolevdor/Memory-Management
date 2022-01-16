#include "../include/Trainer.h"
#include <vector>
#include "../include/Studio.h"
#include "../include/Workout.h"
#include <utility>


Trainer::Trainer(int t_capacity): capacity(t_capacity), open(false), customersList(), orderList(){};

Trainer::~Trainer() { // Destructor
    for (size_t i = 0; i < customersList.size(); i++){ // Deleting trainer's customers list
        if(customersList[i])
            customersList[i] = nullptr;
    }
}
Trainer::Trainer(Trainer const &other){ // Copy Constructor
    capacity = other.capacity;
    open = other.open;
    salary = other.salary;
    for (size_t i = 0; i < other.orderList.size(); i++)
        orderList.push_back(other.orderList[i]);
    for (size_t i = 0; i < other.customersList.size(); i++)
        customersList.push_back(other.customersList[i]->clone());
};

Trainer& Trainer:: operator=(const Trainer &other){ // Assignment Operator
    capacity = other.capacity;
    open = other.open;
    salary = other.salary;
    // Clears the customerList vector of this trainer
    for (size_t i = 0; i < other.customersList.size(); i++){
        customersList[i] = nullptr;
    }
    customersList.clear();
    for (size_t i = 0; i < other.customersList.size(); i++){
        customersList[i] = other.customersList[i]->clone();
    }
    orderList.clear();
    for (size_t i = 0; i < other.orderList.size(); i++) {
        orderList.push_back(OrderPair(other.orderList[i].first, other.orderList[i].second));
    }
    return *this;
}

Trainer::Trainer(Trainer && other){  // Move Constructor
    capacity = other.capacity;
    open = other.open;
    salary = other.salary;
    for (size_t i = 0; i < other.customersList.size(); i++){ //
        customersList.push_back(other.customersList[i]);
        other.customersList[i]= nullptr;
    }
    for (size_t i = 0; i < other.orderList.size(); i++){
        orderList.push_back(other.orderList[i]);
    }
};

Trainer& Trainer:: operator=(Trainer&& other){  // Move Assignment Operator
    for (size_t i = 0; i < customersList.size(); i++){ // Deleting trainer's customers list
        if(customersList[i])
            customersList[i] = nullptr;
    }
    customersList.clear();
    orderList.clear();

    capacity = other.capacity;
    open = other.open;
    salary = other.salary;

    for (size_t i = 0; i < other.orderList.size(); i++) {
        orderList.push_back(other.orderList[i]);
    }

    for (size_t i = 0; i < other.customersList.size(); i++) {
        customersList.push_back(other.customersList[i]);
        other.customersList[i] = nullptr;
    }
    return *this;
};

int Trainer::getCapacity() const{
    return capacity;
};
void Trainer::addCustomer(Customer* customer){
    customersList.push_back(customer);

}
void Trainer::removeCustomer(int id) {
    if (customersList.empty())//if there is no customer to remove
        cout<< "This trainer doesn't have any customers!\n";
    else {
        vector<Customer *> newCustList; //Declaring a new customers list to be the same, but without the deleted one
        vector<OrderPair> newOrdList;  //Declaring a new orders list to be the same, but without the deleted one
        //search for this customer in the customerList
        for (size_t i = 0; i < customersList.size(); i++) { // Creating the new customer list
            if (customersList[i]->getId() != id) {
                newCustList.push_back(customersList[i]);
            }
        }
        customersList.clear();
        for (size_t i = 0; i < newCustList.size(); i++) { // Creating the new customer list
            customersList.push_back(newCustList[i]);
            newCustList[i] = nullptr;
        }
        newCustList.clear();

        salary = 0;

        for (size_t i = 0; i < orderList.size(); i++) { // Creating the new order list
            if (orderList[i].first != id) {
                newOrdList.push_back(orderList[i]);
            }
        }

        orderList.clear();
        for (size_t i = 0; i < newOrdList.size(); i++) { // Creating the new order list
            orderList.push_back(newOrdList[i]);
            salary = salary + newOrdList[i].second.getPrice(); //update the new salary
        }
        newOrdList.clear();
    }
}
Customer* Trainer::getCustomer(int id){
    if (customersList.empty())
        return nullptr;
    //the loop search for the customer with the given id
    for (size_t i = 0; i < customersList.size(); i++) {
        if (customersList[i]->getId() == id)
            return customersList[i];
    }
    return nullptr;

};
std::vector<Customer*>& Trainer::getCustomers(){
    return customersList;};
std::vector<OrderPair>& Trainer::getOrders(){
    return orderList;
};
void Trainer::order(const int customer_id, const std::vector<int> workout_ids, const std::vector<Workout>& workout_options){
    //this function gets customer id and his workout wishes, and all the options for workout and update "orderList" and "salary"
    for (size_t workId: workout_ids){//goes over all customer's workouts
        OrderPair myPair(customer_id, workout_options[workId]);//creates order-pair for the customer
        orderList.push_back(myPair);
        salary += workout_options[workId].getPrice(); //Trainer's salary growth with each order
    }
};
void Trainer::openTrainer(){
    open=true;
};

void Trainer::closeTrainer(){
    open = false;
    orderList.clear();
    for(size_t i=0; i<customersList.size();i++)
        customersList[i]= nullptr;
    customersList.clear();
};
int Trainer::getSalary(){
    return salary;
};

bool Trainer::isOpen(){
    return open;
};

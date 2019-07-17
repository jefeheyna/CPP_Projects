/**
 * BankData.cpp
 * Name:Jeffrey Hejna
 * Person number:50044619
 *
 * All definitions of functions for the class BankData and the description of all methods are contained in here.
 * Feel free to add more function definitions to this, but you must include at least the required methods.
 * You may not use any additional headers.
 */

// Do not modify/add/remove includes in this file.
#include "BankData.hpp"

/**
 * Add a customer to the stored customers if no customer with the same id is present.
 * Initialize the customer balance to the value provided.
 *
 * @param customer_id - id to assign the new customer.
 * @param balance - starting balance for the customer.
 * @return  true - if customer was successfully added.
 *         false - if the customer was previously added.
 */
bool BankData::add_customer(const size_t& customer_id, const double& balance) {
    if(customer_exists(customer_id)){return false;}
    Customer* newcustomer = new Customer(customer_id,balance);
    if(balance<0){
        newcustomer->_overdrawn = true;
    }
    std::list<size_t> copy_times;
    if(mycustomers.size()+1>mycustomers.capacity()){
        for (int i = 0; i < mycustomers.size(); ++i) {
            CSE250::dTransactionNode* temp = mycustomers.at(i)._pending_txs;
            while(temp!= nullptr){
                copy_times.push_back(temp->_tx._timestamp);
                temp=temp->_next;
            }

        }
    }
    mycustomers.push_back(*newcustomer);
    int counter=0;
    for (auto it=copy_times.begin(); it != copy_times.end(); ++it){ //http://www.cplusplus.com/reference/list/list/begin/
        CSE250::dTransactionNode* temp = mycustomers.at(counter)._pending_txs;
        while(temp!= nullptr){
            mycustomers.at(counter)._pending_txs->_tx._timestamp = *it;
            temp = temp->_next;
        }
        ++counter;

    }
    return true;
}

/**
 * Add a list of customers to the stored customers with the ids within this list.
 * You should initialize the customers with an account balance of 0.
 * All values in the list should be processed, even if a customer already exists.
 *
 * @param customer_list - list of customer ids to add.
 * @return  true - if all customers were successfully added.
 *         false - if some customer was previously added (including if there are duplicates in the list itself).
 */
bool BankData::add_customer(const CSE250::dCustomerIDNode* customer_list) {


    if(customer_list == nullptr){
        return true;
    }

    std::vector<CSE250::dCustomerIDNode> duplicates;
    if(customer_list->_next == nullptr){
        if(customer_exists(customer_list->_customer_id)){
            duplicates.push_back(*customer_list);
        }
        else{
            add_customer(customer_list->_customer_id,0);
            //mycustomers.push_back(Customer(customer_list->_customer_id,0));
        }
    }

    while(customer_list!= nullptr){
        if(customer_exists(customer_list->_customer_id)){
            duplicates.push_back(*customer_list);
        }
        else{
            add_customer(customer_list->_customer_id,0);
            //mycustomers.push_back(Customer(customer_list->_customer_id,0));
        }
        customer_list = customer_list->_next;
    }


    return duplicates.empty();


}

/**
 * Remove a customer from the stored customers.
 *
 * @param customer_id - id of customer to remove.
 * @return  true - if customer was successfully removed.
 *         false - if the customer was not present.
 */
bool BankData::remove_customer(const size_t& customer_id) {
    if(customer_exists(customer_id)){
        for (int i = 0; i < mycustomers.size(); ++i) {
            if(customer_id == mycustomers.at(i)._customer_id){
                //Customer* temp = &mycustomers.at(i);
                mycustomers.erase(mycustomers.begin()+i);
                //delete temp;
                return true;
            }
        }
    }
    return false;
}

/**
 * Remove a list of customer to the stored customers with an account balance of 0.
 * All values in the list should be processed, even if an ID is for a customer that
 * was already removed or doesn't exist.
 *
 * @param customer_list - list of customer ids to remove.
 * @return  true - if all customers were successfully removed.
 *         false - if some customer was not removed (including if there are duplicates in the list itself).
 */
bool BankData::remove_customer(const CSE250::dCustomerIDNode* customer_list) {
    //empty list
    if(customer_list == nullptr){
        return true;
    }
    //only one element
    if(customer_list->_next == nullptr && customer_list->_prev == nullptr){
        if(customer_exists(customer_list->_customer_id)){
            mycustomers.erase(mycustomers.begin());
            return true;
        }
        return false;
    }


    int numdeleted=0;
    int counter=0;
    while(customer_list!= nullptr){
        for (int i = 0; i < mycustomers.size(); ++i) {
            if(mycustomers.at(i)._customer_id == customer_list->_customer_id){
                mycustomers.erase(mycustomers.begin()+i);
                ++numdeleted;
            }
        }
        ++counter;
        customer_list = customer_list->_next;
    }

    return numdeleted == counter;
}

/**
 * Add a transaction for to the transaction list for the specified customer.
 *
 * You may assume that each timestamp will be unique between each call to process transactions.
 * You may not assume that the timestamps will be given in any particular order.
 *
 * @param customer_id - id of the customer to add the transaction under.
 * @param timestamp - timestamp the transaction should be created with.
 * @param amount - amount the transaction will change their account balance
 * @return  true - if the transaction was successfully added to the customer's account.
 *         false - if the customer didn't exist.
 */
bool BankData::add_transaction(const size_t& customer_id, const size_t& timestamp, const double& amount) {
    if(!customer_exists(customer_id)){return false;}
    for (int i = 0; i < mycustomers.size(); ++i) {
        if(mycustomers.at(i)._customer_id == customer_id){
            mycustomers.at(i).add_txs(Transaction(timestamp,amount));
            return true;
        }
    }
    return true;
}

/**
 * Void the transaction with the specified customer id and timestamp
 * (i.e., remove it from the customer's transaction list).
 *
 * You may assume that each timestamp will be unique between each call to process transactions.
 *
 * @param customer_id - id of the customer the transaction is under.
 * @param timestamp - timestamp the transaction was created with.
 * @return  true - if the transaction was successfully added to the customer's account.
 *         false - if the customer didn't exist or there is no transaction with the given timestamp
 *                 for that customer.
 */
bool BankData::void_transaction(const size_t& customer_id, const size_t& timestamp) {
    for (int i = 0; i <mycustomers.size() ; ++i) {
        if(customer_id == mycustomers.at(i)._customer_id){
            CSE250::dTransactionNode* temp = mycustomers.at(i)._pending_txs;
            while(temp!= nullptr){
                if(temp->_tx._timestamp == timestamp){
                     //only one
                    if(temp->_next == nullptr && temp->_prev == nullptr){
                        delete temp;
                        mycustomers.at(i)._pending_txs = nullptr;
                        return true;
                    }
                    //if its the head
                    if(temp->_prev == nullptr && temp->_next != nullptr){
                        mycustomers.at(i)._pending_txs = mycustomers.at(i)._pending_txs->_next;
                        mycustomers.at(i)._pending_txs->_prev = nullptr;
                        delete temp;
                        return true;
                    }
                    //if its the tail
                    if(temp->_next == nullptr && temp->_prev!= nullptr){
                        temp->_prev->_next = nullptr;
                        delete temp;
                        return true;
                    }
                    temp->_prev->_next = temp->_next;
                    temp->_next->_prev = temp->_prev;
                    delete temp;
                    return true;

                }
                temp = temp->_next;
            }
        }

    }
    return false;
}

/**
 * Process each transaction for each customer.
 * At the end, all transaction lists should be empty and account balances should reflect all changes
 * dictated by the respective lists of transactions.
 * Transactions should be processed in increasing order of timestamp (within the Customer's pending transactions).
 * Create a list of all customers that had overdrawn their account.
 * @return double linked list of all customer IDs that overdrew their account.
 */
CSE250::dCustomerIDNode* BankData::process_transactions() {
    CSE250::dCustomerIDNode* overdrawn_list = nullptr;
    CSE250::dCustomerIDNode* temp = overdrawn_list;
    for (int i = 0; i < mycustomers.size(); ++i) {
        if(mycustomers.at(i)._pending_txs == nullptr){
            continue;
        }
        bool before = mycustomers.at(i)._overdrawn;
        process_transactions(mycustomers.at(i));
        bool after = mycustomers.at(i)._overdrawn;
        if(before == false && after == true){
            if(overdrawn_list == nullptr){
                overdrawn_list = new CSE250::dCustomerIDNode(mycustomers.at(i)._customer_id);
                temp = overdrawn_list;
            }
            else{
                CSE250::dCustomerIDNode* addedcustomer = new CSE250::dCustomerIDNode(mycustomers.at(i)._customer_id);
                temp->_next = addedcustomer;
                addedcustomer->_prev = temp;
                temp = temp->_next;
            }
        }

    }

    return overdrawn_list;
}


/**
 * Process each transaction for a single customer.
 * Transactions should be processed in increasing timestamp order.
 * While processing the transactions, if the customer's balance becomes negative,
 * then the _overdrawn flag should be set to true.
 *
 * @param customer - customer object for which to process transactions.
 */
void BankData::process_transactions(Customer& customer) {
    CSE250::dTransactionNode* temp = customer._pending_txs;

    //customer pending_trans is empty
    if(temp == nullptr){
        return;
    } //only one element
    else if(temp->_next == nullptr && temp->_prev == nullptr){

        customer._account_balance+=customer._pending_txs->_tx._tx_amt;

        if(customer._account_balance<0 && customer._overdrawn == false){
            customer._overdrawn=true;
        }

        delete temp;
        customer._pending_txs = nullptr;


    }
    else {
        while (temp != nullptr) {
            customer._account_balance += temp->_tx._tx_amt;
            if(customer._account_balance<0 && customer._overdrawn == false){
                customer._overdrawn = true;
            }
            temp = temp->_next;
            //delete temp->_prev;
        }
    }
    if (customer._pending_txs != nullptr){
        temp = customer._pending_txs;
        while(customer._pending_txs!= nullptr){
            customer._pending_txs = customer._pending_txs->_next;
            delete temp;
            temp = customer._pending_txs;
        }
    }

}

/**
 * Tell whether or not a customer with the specified ID exists in the stored customer data.
 *
 * @param customer_id -  id of the customer requested.
 * @return  true - if the customer requested has a stored record.
 *         false - otherwise.
 */
bool BankData::customer_exists(const size_t& customer_id) {
    if(mycustomers.empty()){return false;}
    for (int i = 0; i < mycustomers.size(); ++i) {
        if(customer_id == mycustomers.at(i)._customer_id){
            return true;
        }
    }
    return false;
}

/**
 * Tell whether or not the ID belongs to a valid customer and
 * the timestamp belongs to a valid Transaction under that customer.
 *
 * @param customer_id -  id of the customer requested.
 * @param timestamp - timestamp the transaction was created with.
 * @return  true - if the customer requested has a stored record.
 *         false - otherwise.
 */
bool BankData::transaction_exists(const size_t& customer_id, const size_t& timestamp) {
    if(mycustomers.empty()){return false;}
    for (int i = 0; i < mycustomers.size(); ++i) {
        if(customer_id == mycustomers.at(i)._customer_id){
            return mycustomers.at(i).findTrans(timestamp);
        }
    }
    return false;
}

/***********************************************************************************************/
/* The following definitions cannot be completed until you decide how you will store your data.*/
/***********************************************************************************************/

/**
 * Get access to the customer data associated with the requested ID.
 * You should assume that the ID belongs to a valid customer.
 * @param customer_id - id of the customer requested.
 * @return constant reference to the Customer object associated with the id.
 */
const Customer& BankData::get_customer_data(const size_t& customer_id) {
    Customer* temp = new Customer(0,0);
    for (int i = 0; i < mycustomers.size(); ++i) {
        if(mycustomers.at(i)._customer_id == customer_id){
            temp = &mycustomers.at(i);
            break;
        }
    }
    const Customer* the_customer = temp;

    return *the_customer;

}

/**
 * Get access to the transaction data associated with the requested ID and timestamp.
 * You should assume that the ID belongs to a valid customer and the timestamp belongs to a valid Transaction.
 * @param customer_id - id of the customer requested.
 * @param timestamp - timestamp the transaction was created with.
 * @return constant reference to the Transaction object associated with the id and timestamp.
 */
const Transaction& BankData::get_customer_transaction(const size_t& customer_id, const size_t& timestamp) {
    Transaction* placeholder = new Transaction(0,0);
    for (int i = 0; i < mycustomers.size(); ++i) {
        if(mycustomers.at(i)._customer_id == customer_id){
            CSE250::dTransactionNode* temp = mycustomers.at(i)._pending_txs;
            while(temp != nullptr){
                if(temp->_tx._timestamp == timestamp){
                    placeholder = &temp->_tx;
                    break;
                }
                temp = temp->_next;
            }
        }
    }
    const Transaction* the_transaction = placeholder;


    return *the_transaction;
}

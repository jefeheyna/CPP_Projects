/**
 * Customer.hpp
 * Name:Jeffrey Hejna
 * Person number:50044619
 *
 * This file contains the definition of the Customer class.
 * Add any fields you wish, but do not change the name of or remove any
 * of the required fields that are provided by default.
 * You may not use any additional headers.
 */

#ifndef A3_CUSTOMER_HPP
#define A3_CUSTOMER_HPP

// Do not modify/add/remove includes in this file.
#include "Transaction.hpp"
#include <vector>
#include <list>
#include <algorithm>

class Customer {
private:
    /**
     * Default constructor with no parameters is made private so that no transactions are created without
     * providing a value for the _customer_id.
     */
    Customer();

public:
    // Used to store the customer ID.
    size_t _customer_id;

    // Used to store the customer's account balance.
    double _account_balance;

    // List of pending transactions.
    CSE250::dTransactionNode* _pending_txs;

    // Flag to determine if account was overdrawn.
    bool _overdrawn;

    /**
     * Constructor for Customer requires a customer id to be provided.
     * By default, if no starting balance is given, the customer is created with a starting balance of 0.
     * As the member variable _cust_id is const, you must provide this at construction as it cannot be changed later.
     *
     * @param id - the unique customer id number assigned to this customer.
     * @param starting_balance - the dollar amount to start the account at.
     */
    Customer(const size_t& id, const double& starting_balance = 0) : _customer_id(id), _account_balance(starting_balance),
                                                                     _pending_txs(nullptr), _overdrawn(false) { }

    // Feel free to add more functionality here as needed.



    bool findTrans(size_t time){
        if(_pending_txs == nullptr){
            return false;
        }
        CSE250::dTransactionNode* temp = _pending_txs;
        while(temp->_next!= nullptr){
            if(temp->_tx._timestamp == time){
                return true;
            }
            temp = temp->_next;
        }
        return temp->_tx._timestamp == time;
    }
    void add_txs(Transaction trans){
        if(_pending_txs == nullptr){
            _pending_txs = new CSE250::dTransactionNode(trans._timestamp,trans._tx_amt);
            return;
        }
        CSE250::dTransactionNode* temp = _pending_txs;
        CSE250::dTransactionNode* added_trans = new CSE250::dTransactionNode(trans._timestamp,trans._tx_amt);

        //only one element cases
        if(temp->_next == nullptr && temp->_prev == nullptr && added_trans->_tx._timestamp<= temp->_tx._timestamp){
            _pending_txs->_prev = added_trans;
            added_trans->_next = _pending_txs;
            _pending_txs = added_trans;
            return;
        }
        if(temp->_next == nullptr && temp->_prev == nullptr && added_trans->_tx._timestamp > temp->_tx._timestamp){
            _pending_txs->_next = added_trans;
            added_trans->_prev = _pending_txs;
            return;
        }

        while(temp!= nullptr){
            //adding at the head
            if(temp->_prev == nullptr && added_trans->_tx._timestamp<= temp->_tx._timestamp){
                added_trans->_next = _pending_txs;
                temp->_prev = added_trans;
                _pending_txs = added_trans;
                return;
            }
            //adding at tail
            if(temp->_next == nullptr && added_trans->_tx._timestamp> temp->_tx._timestamp){
                temp->_next = added_trans;
                added_trans->_prev = temp;
                return;
            }
            //any other spot
            if(added_trans->_tx._timestamp<= temp->_tx._timestamp){
                temp->_prev->_next = added_trans;
                added_trans->_prev = temp->_prev;
                added_trans->_next = temp;
                temp->_prev = added_trans;
                return;
            }
            temp = temp->_next;
        }

    }

    ~Customer(){   //http://faculty.lasierra.edu/~ehwang/courses/cptg122/Notes/Linked%20List%20Destructor.pdf
        CSE250::dTransactionNode* temp = _pending_txs;
        while(_pending_txs!= nullptr){
            _pending_txs = _pending_txs->_next;
            delete temp;
            temp = _pending_txs;
        }
    }

};

namespace CSE250 {
    struct dCustomerIDNode;
}

struct CSE250::dCustomerIDNode {
    // Transaction stored within the node.
    const size_t _customer_id;

    // Pointer to previous node in list.
    dCustomerIDNode* _prev;

    // Pointer to next node in list.
    dCustomerIDNode* _next;

    /**
     * Constructor for a transaction node requires the default information to create a transaction.
     *
     * @param id - the id of a customer of interest.
     */
    dCustomerIDNode(const size_t& id) : _customer_id(id), _prev(nullptr), _next(nullptr) { }

private:
    /**
     * Default constructor with no parameters is made private so that no node is created without
     * providing a value for the _customer_id. This avoids accidentally creating a node without
     * providing a specific id.
     */
    dCustomerIDNode();
};


#endif //A3_CUSTOMER_HPP

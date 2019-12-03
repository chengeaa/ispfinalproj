#include <iostream>
#include <assert.h>
#include <memory>
#include <limits>
#include <cmath>
#include <vector>
#include <string>
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::cout;
using std::cin;
using std::endl;

class Address {
    private:
        float i, j;
    public:
        Address(float i, float j): i(i), j(j) {};
        string as_string(){
            string output = "";
            output.append("(");
            output.append(std::to_string(i));
            output.append(",");
            output.append(std::to_string(j));
            output.append(")");
            return output;
        }
        void print(){
            cout << "(" << i << ", " << j << ")" << endl;
        }
        float l1(Address other){
            return abs(i - other.i) + abs(j - other.j);
        }
        float l2(Address other){
            float di = i - other.i;
            float dj = j - other.j;
            return sqrt(di*di + dj*dj);
        }

};

class AddressList {
    protected:
        vector<Address> addresses;
    public:
        AddressList(){
            addresses = {};
        }
        void add_address(Address a){
            addresses.push_back(a);

        }
        int size(){
            return addresses.size();
        }
        void print(){
            for (Address a: addresses){
                a.print();
            }
        }
        float length(string norm){
            // returns total length of path if added addresses  traversed in order, based on specified norm
            float total = 0;
            Address last = addresses[0];
            for (int i = 1; i < addresses.size(); i++){
                Address curr = addresses[i];
                if (norm == "l1"){
                    total += last.l1(curr);
                } else {
                    total += last.l2(curr);
                }
                last = curr;
            }
            return total;
        }
        int index_closest_to(Address a, string norm){
            //Assume a is not represented in the list
            //(ie), no 0 distances allowed
            int closest = 0;
            float mindist = std::numeric_limits<float>::max();
            for (int i = 0; i < addresses.size(); i++){
                Address curr = addresses[i];
                float thisdist;
                //calculate target node to current node distance 
                //based on specified norm
                if(norm == "l1"){
                    thisdist = a.l1(curr);
                } else {
                    thisdist = a.l2(curr);
                }
                //set new min if this is closer
                if(thisdist < mindist){
                    closest = i;
                    mindist = thisdist;
                }
            }
            return closest;
        }
        
        vector<Address> greedy_route(string norm){
            vector<Address> stops = addresses;
            vector<Address> result; 
            Address we_are_here = Address(0,0);
            result.push_back(we_are_here);
            //iterate through 'stops', append and erase as we go
            while(stops.size() > 0){
                int closest_address_index = index_closest_to(we_are_here, norm);
                result.push_back(stops[closest_address_index]);
                stops.erase(stops.begin() + closest_address_index);
            }
            return result; 
        }

};

class Route : public AddressList {
    private:
        Address depot = Address(0,0);
    public:
        Route() : AddressList(){
            AddressList::add_address(depot);
            AddressList::add_address(depot);
        }

        void add_address(Address newaddress){
            for (auto a: addresses) {
                if (newaddress.l1(a)  == 0.0){ //choice of metric doesn't matter here
                    cout << "Address not added; identical to current member" << endl;
                    cout << "For " << a.as_string() << "," << newaddress.as_string() << endl;
                    return;
                } 
            }
            addresses.pop_back();
            AddressList::add_address(newaddress);
            AddressList::add_address(depot);
        }
        Route greedy_route(string norm){
            vector<Address> stops = addresses;
            Route result;
            
            //add start to result, remove it from stop list
            stops.erase(stops.begin());//since result is a Route, already has depot

            //turn stops vector into non-endpoint subvector
            stops.erase(stops.begin() + stops.size());

            Address we_are_here = depot;
            for(auto stop:stops){
                stop.print();
            }
            
            cout << "pruning step:" <<endl;
            //iterate through 'stops', append and erase as we go
            while(stops.size() > 0){
                int closest_address_index = index_closest_to(we_are_here, norm);
                we_are_here = stops[closest_address_index]; 
                we_are_here.print() ;
                result.add_address(we_are_here);
                stops.erase(stops.begin() + closest_address_index);

            }
            return result; 
        }

        string as_string(){
            string output = "";
            for (Address a: addresses) {
                output.append(a.as_string());
            }
            return output;
        }
        
        
};

void testfunc(){
    Route deliveries;
    deliveries.add_address( Address(0,5) );
    deliveries.add_address( Address(5,0) );
    deliveries.add_address( Address(5,5) );
    cout << "Travel in order: " << deliveries.length("l1") << "\n";
    cout << deliveries.as_string() << endl;
    assert( deliveries.size()==5 );
    cout <<endl;
    cout << "now for the greedy route:" << endl << endl;
    Route route = deliveries.greedy_route("l1");
    assert( route.size()==5 );
    float len = route.length("l1");
    cout << "Square route: " << route.as_string()
    << "\n has length " << len << "\n";
}

int main() {
        /*
        Address a1 = Address(1, 1);
        Address a2 = Address(2, 2);
        Address a3 = Address(3, 3);
        */
        /*
        cout << "L1: " << a1.l1(a2) << endl;
        cout << "L2: " << a1.l2(a2) << endl;
        AddressList al = AddressList();
        al.add_address(a1);
        al.add_address(a2);
        al.add_address(a3);
        cout << "L1 length:  " << al.length("l1") << endl;
        cout << "L2 length:  " << al.length("l2") << endl;
        al.print();

        Address a4 = Address(2, 2.2);
        cout << al.index_closest_to(a4, "l1") << endl;
        cout << al.index_closest_to(a4, "l2") << endl;
        */
        /*
        Route r = Route();
        r.add_address(a1);
        r.add_address(a2);
        r.add_address(a3);
        r.print();
        */
    testfunc();
    return 0;
}




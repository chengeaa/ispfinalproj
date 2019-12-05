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
using std::cerr;
using std::cin;
using std::endl;

class Address {
    private:
        float i, j;
        int last_date;
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
            cout << "(" << i << ", " << j << ") ";
        }
        float distance(Address other){
            // chose to implement distance using L2 norm to match example output
            float di = i - other.i;
            float dj = j - other.j;
            return sqrt(di*di + dj*dj);
        }
        int get_last_date(){
            return last_date;
        }
};

class AddressList {
    protected:
        vector<Address> addresses;
    public:
        AddressList(){
            addresses = {};
        }
        AddressList(vector<Address> source){
            addresses = source;
        }
        void add_address(Address newaddress){
            // adds address a to vector 'addresses' if and only if 
            // l1 norm of a to all addresses in addresses is not 0
            for (auto a: addresses) {
                if (newaddress.distance(a)  == 0.0){ 
                    cout << "Address not added; identical to current member" << endl;
                    cout << "For " << a.as_string() << "," << newaddress.as_string() << endl;
                    return;
                } 
            }
            addresses.push_back(newaddress);
        }
        int size(){
            return addresses.size();
        }
        void print(){
            for (Address a: addresses){
                a.print();
            }
            cout << endl;
        }
        float length(){
            // returns total length of path if added addresses traversed in order
            float total = 0;
            Address last = addresses[0];
            for (int i = 1; i < addresses.size(); i++){
                Address curr = addresses[i];
                total += last.distance(curr);
                last = curr;
            }
            return total;
        }
        int index_closest_to(Address a){
            //Assume a is not represented in the list
            //ie, no 0 distances allowed
            int closest = 0;
            float min_dist = std::numeric_limits<float>::max();
            for (int i = 0; i < addresses.size(); i++){
                Address curr = addresses[i];
                float this_dist;
                this_dist = a.distance(curr);
                //set new min if this is closer
                if(this_dist < min_dist){
                    closest = i;
                    min_dist = this_dist;
                }
            }
            cout << "closest element to ";
            a.print();
            cout << "in ";
            print();
            cout << "is ";
            addresses[closest].print();
            cout << endl;
            return closest;
        }
        
        vector<Address> greedy_route(){
            vector<Address> result; 
            Address we_are_here = addresses[0]; // assume depot is first item in address list;
            // terrible idea abstraction-wise, but the spec says to, so...
            result.push_back(we_are_here);
            addresses.erase(addresses.begin());
            //iterate through 'addresses', append and erase as we go
            while(addresses.size() > 0){
                int closest_address_index = index_closest_to(we_are_here);
                we_are_here = addresses[closest_address_index];
                result.push_back(we_are_here);
                addresses.erase(addresses.begin() + closest_address_index);
            }
            return result; 
        }

};

class Route : public AddressList {
    private:
        Address depot = Address(0,0);
    public:
        Route() : AddressList(){
            addresses = {
                depot, depot
            };
        }
        Route(vector<Address> source) : AddressList(source){}

        void add_address(Address newaddress){
            addresses.pop_back();
            AddressList :: add_address(newaddress);
            addresses.push_back(depot);
        }

        Route greedy_route(){
            addresses.pop_back();
            AddressList subvector = AddressList(addresses);
            cout << "subvector: ";
            subvector.print();
            cout << "greedy output: " << endl;
            vector<Address> greedy_output = subvector.greedy_route();
            // construct greedy route output
            greedy_output.push_back(depot);
            return Route(greedy_output);
        }

        /*
        Route greedy_route(){
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
                int closest_address_index = index_closest_to(we_are_here);
                we_are_here = stops[closest_address_index]; 
                we_are_here.print() ;
                result.add_address(we_are_here);
                stops.erase(stops.begin() + closest_address_index);

            }
            return result; 
        }
        */

        string as_string(){
            string output = "";
            for (Address a: addresses) {
                output.append(a.as_string());
            }
            return output;
        }
        
        
};

void closest_index_test(){
    AddressList deliveries;
    deliveries.add_address( Address(5,5) );
    deliveries.add_address( Address(0,5) );
    deliveries.print();
    cout << deliveries.index_closest_to( Address(5,0) ) << endl;
}
void exampletest(){
    Route deliveries;
    deliveries.add_address( Address(0,5) );
    deliveries.add_address( Address(5,0) );
    deliveries.add_address( Address(5,5) );
    cerr << "Travel in order: " << deliveries.length() << "\n";
    cout << deliveries.as_string() << endl;
    assert( deliveries.size()==5 );
    cout <<endl;
    cout << "now for the greedy route:" << endl << endl;
    auto route = deliveries.greedy_route();
    assert( route.size()==5 );
    auto len = route.length();
    cerr << "Square route: " << route.as_string()
    << "\n has length " << len << "\n";
}

void distance_test(){
    Address one(1., 1.), two(2., 2.);
    cerr << "Distance: " << one.distance(two) << "\n";
}
int main() {
    exampletest();
    return 0;
}




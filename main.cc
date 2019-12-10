#include <iostream>
#include <thread>
#include <chrono>
#include <time.h>
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
        void clear(){
            addresses = {};
        }
        void add_address(Address newaddress){
            // adds address a to vector 'addresses' if and only if 
            // l1 norm of a to all addresses in addresses is not 0
            if (in(newaddress)){ 
                //cout << "Address not added; identical to current member" << endl;
                //cout << "For " <<  newaddress.as_string() << endl;
                return;
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
            return closest;
        }
        
        vector<Address> greedy_route(){
            vector<Address> result; 
            Address we_are_here = addresses[0]; // assume depot is first item in address list;
            // throw the following error otherwise
            if (we_are_here.distance(Address(0,0)) != 0){
                throw "First address in address list should be depot (origin)";
            }
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

        bool in(Address newaddress){
            for(Address a: addresses){
                if(newaddress.distance(a) == 0){
                    return true;
                }
            }
            return false;
        }

        bool anyin(int start, int end){
            for(Address a: addresses){
                for(int i = start; i <= end; i++){
                    if(addresses[i].distance(a) == 0){
                        return true;
                    }
                }
            }
            return false;
        }

        bool anyin_subsection(vector<Address> addresslist, int start, int end){
            for(int i = start; i <= end; i++){
                for(Address a: addresslist){
                    if(addresses[i].distance(a) == 0){
                        return true;
                    }
                }
            }
            return false;
        }

        void insert(Address a, int p){
            addresses.insert(addresses.begin() + p, a);
        }

        void erase(int p){
            addresses.erase(addresses.begin() + p);
        }

        void erase(int start, int end){
            for (int i = end; i >= start; i--){
                addresses.erase(addresses.begin() + i);
            }
        }

        Address pick_random(){
            return addresses[rand() % size()];
        }

        vector<Address> my_addresses(){
            return addresses;
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

        void clear(){
            addresses = {depot, depot};
        }

        void add_address(Address newaddress){
            addresses.pop_back();
            AddressList :: add_address(newaddress);
            addresses.push_back(depot);
        }

        Route greedy_route(){
            // returns a new route that is greedily optimal
            // construct subvector of all but last stop (depot)
            addresses.pop_back(); 
            AddressList subvector = AddressList(addresses);
            addresses.push_back(depot);
            // constuct and return greedy search output, including depot at end
            vector<Address> greedy_output = subvector.greedy_route();
            greedy_output.push_back(depot);
            return Route(greedy_output);
        }

        void reverse(int index1, int index2){
            // reverses route segment in place
            vector<Address> temp;
            for(int i = index2; i >= index1; i--){
                temp.push_back(addresses[i]);
            }
            for(int j = index1; j <= index2; j++){
                addresses[j] = temp[j - index1];
            }
        }
 
        void opt2(){
            // loop over subarray not consisting of depots;
            // cannot reverse depot segments
            float min_length = length();
            float curr_length;
            for(int n = 1; n < size() - 1; n++){
                for(int m = 1; m < n; m++){
                    reverse(m, n);
                    curr_length = length();
                    if (curr_length < min_length){
                        min_length = curr_length;
                    } else {
                        reverse(m, n); // basically if curr_length is longer, 
                        // undo the reverse
                    }
                }

            }
        }

        void multi_opt2(Route &other_route){
            // assume only 2 routes optimizing for (mention pairwise+ search in writeup?)
            // minimization criteria will be total length of routes
            // the nested loops extend op2 by considering 4 possible routes:
            // pure swap (..., i, ..., j, ...) (..., m, ..., n...)
            // swap, reverse 1 (..., j, ..., i, ...) (..., m, ..., n...)
            // swap, reverse 2 (..., i, ..., j, ...) (..., n, ..., m...)
            // swap, reverse both (..., i, ..., j, ...) (..., m, ..., n...)
            // iterate over all subsets of the route not containing endpoints
            int min_m, min_j, min_n, min_i;
            bool min_swap_1, min_swap_2;
            float min_total = length() + other_route.length();
            float pure_length, rev1_length, rev2_length, rev_both_length;

            min_m = -1;

            for (int n = 1; n < size() - 1; n++){
                for(int m = 1; m < n; m++){
                   //iterate over all subsets of other route not containing endpoints
                   for (int i = 1; i < other_route.size() - 1; i++){
                       for (int j = 1; j < i; j++){
                           //try_swap(other_route, m, j, n, i, false, false); //pure swap
                           //try_swap(other_route, m, j, n, i, true, false); //swap, reverse 1
                           //try_swap(other_route, m, j, n, i, false, true); //swap, reverse 2
                           //try_swap(other_route, m, j, n, i, true, true); //swap, reverse both

                           pure_length = try_swap(other_route, m, j, n, i, false, false); //pure swap
                           rev1_length = try_swap(other_route, m, j, n, i, true, false); //swap, reverse 1
                           rev2_length = try_swap(other_route, m, j, n, i, false, true); //swap, reverse 2
                           rev_both_length = try_swap(other_route, m, j, n, i, true, true); //swap, reverse both

                           if(pure_length < min_total){
                               min_m = m; min_j = j; min_n = n; min_i = i;
                               min_swap_1 = false; min_swap_2 = false;
                               min_total = pure_length;
                           }
                           if(rev1_length < min_total){
                               min_m = m; min_j = j; min_n = n; min_i = i;
                               min_swap_1 = true; min_swap_2 = false;
                               min_total = pure_length;
                           }
                           if(rev2_length < min_total){
                               min_m = m; min_j = j; min_n = n; min_i = i;
                               min_swap_1 = false; min_swap_2 = true;
                               min_total = pure_length;
                           }
                           if(rev_both_length < min_total){
                               min_m = m; min_j = j; min_n = n; min_i = i;
                               min_swap_1 = true; min_swap_2 = true;
                               min_total = pure_length;
                           }
                       }
                   }
                }
            }
            if (min_m > 0){
                swap(other_route, min_m, min_j, min_n, min_i, min_swap_1, min_swap_2);
            }
        }

        void multi_opt2(Route &other_route, vector<Address> fixed_sites){
            // assume only 2 routes optimizing for (mention pairwise+ search in writeup?)
            // minimization criteria will be total length of routes
            // the nested loops extend op2 by considering 4 possible routes:
            // pure swap (..., i, ..., j, ...) (..., m, ..., n...)
            // swap, reverse 1 (..., j, ..., i, ...) (..., m, ..., n...)
            // swap, reverse 2 (..., i, ..., j, ...) (..., n, ..., m...)
            // swap, reverse both (..., i, ..., j, ...) (..., m, ..., n...)
            // iterate over all subsets of the route not containing endpoints

            //store parameters associated with min length swap result
            int min_m, min_j, min_n, min_i;
            bool min_swap_1, min_swap_2;
            float min_total = length() + other_route.length();
            float pure_length, rev1_length, rev2_length, rev_both_length;
            min_m = -1;
            for (int n = 1; n < size() - 1; n++){
                for(int m = 1; m < n; m++){
                   //iterate over all subsets of other route not containing endpoints
                   for (int i = 1; i < other_route.size() - 1; i++){
                       for (int j = 1; j < i; j++){
                           if (anyin_subsection(fixed_sites, m, n) or other_route.anyin_subsection(fixed_sites, j, i)){
                               continue;
                           } else {
                               pure_length = try_swap(other_route, m, j, n, i, false, false); //pure swap
                               rev1_length = try_swap(other_route, m, j, n, i, true, false); //swap, reverse 1
                               rev2_length = try_swap(other_route, m, j, n, i, false, true); //swap, reverse 2
                               rev_both_length = try_swap(other_route, m, j, n, i, true, true); //swap, reverse both

                               if(pure_length < min_total){
                                   min_m = m; min_j = j; min_n = n; min_i = i;
                                   min_swap_1 = false; min_swap_2 = false;
                                   min_total = pure_length;
                               }
                               if(rev1_length < min_total){
                                   min_m = m; min_j = j; min_n = n; min_i = i;
                                   min_swap_1 = true; min_swap_2 = false;
                                   min_total = pure_length;
                               }
                               if(rev2_length < min_total){
                                   min_m = m; min_j = j; min_n = n; min_i = i;
                                   min_swap_1 = false; min_swap_2 = true;
                                   min_total = pure_length;
                               }
                               if(rev_both_length < min_total){
                                   min_m = m; min_j = j; min_n = n; min_i = i;
                                   min_swap_1 = true; min_swap_2 = true;
                                   min_total = pure_length;
                               }
                           }
                       }
                   }
                }
            }

            if (min_m > 0){
                swap(other_route, min_m, min_j, min_n, min_i, min_swap_1, min_swap_2);
            }
        }

        float try_swap(Route &other_route, int my_start, int other_start, 
            int my_end, int other_end, bool reverse_me, bool reverse_other) {
            // test swap, calculates total length from swap, unswap, return length

            float total_curr_length;

            swap(other_route, my_start, other_start, my_end, other_end, reverse_me, reverse_other);
            total_curr_length = length() + other_route.length();
            unswap(other_route, my_start, other_start, my_end, other_end, reverse_me, reverse_other);
            return total_curr_length;

        }
        void swap(Route &other_route, int my_start, int other_start, 
            int my_end, int other_end, bool reverse_me, bool reverse_other) {
            if(my_start == 0 or other_start == 0 or my_end == size() or other_end == other_route.size()){
                cout << my_start << other_start << my_end << other_end << endl;
                cout << "aaaaa" << endl;
                throw "cannot swap links containing depot";
            }
            vector<Address> temp;
            for(int i = my_start; i <= my_end; i++){
                temp.push_back(addresses[i]);
            }

            vector<Address> other_temp;
            for(int i = other_start; i <= other_end; i++){
                other_temp.push_back(other_route.addresses[i]);
            }

            erase(my_start, my_end);
            other_route.erase(other_start, other_end);

            for(int i = 0; i < other_temp.size(); i++){
                insert(other_temp[i], my_start + i);
            }

            for(int i = 0; i < temp.size(); i++){
                other_route.insert(temp[i], other_start + i);
            }
        }

        void unswap(Route &other_route, int my_start, int other_start, 
            int my_end, int other_end, bool reverse_me, bool reverse_other) {
            // using the exact same parameters for the original swap, reverse that effect
            int my_diff =  my_end - my_start;
            int other_diff = other_end - other_start;
            swap(other_route, my_start, other_start, my_start + other_diff, other_start + my_diff, reverse_me, reverse_other);
        }

        string as_string(){
            string output = "";
            for (Address a: addresses) {
                output.append(a.as_string());
            }
            return output;
        }
        
        
};

void evaluate(Route route1, Route route2){
    cout << "route 1: " ;
    route1.print();
    //cout << "route 1 length: " << route1.length() << endl;
    cout << "route 2: " ;
    route2.print();
    //cout << "route 2 length: " << route2.length() << endl;
    cout << "total length : " << route1.length() + route2.length();
    cout << endl;
}

void closest_index_test(){
    AddressList deliveries;
    deliveries.add_address( Address(5,5) );
    deliveries.add_address( Address(0,5) );
    deliveries.print();
    cout << deliveries.index_closest_to( Address(5,0) ) << endl;
}

void opt2test(){
    Route deliveries;
    deliveries.add_address( Address(0,5) );
    deliveries.add_address( Address(5,0) );
    deliveries.add_address( Address(5,5) );
    cout << "Inorder route: " << deliveries.as_string() << 
    " has length " << deliveries.length() << endl;
    assert( deliveries.size()==5 );
    auto route = deliveries.greedy_route();
    assert( route.size()==5 );
    auto len = route.length();
    cerr << "Square route: " << route.as_string()
    << " has length " << len << "\n";
    deliveries.opt2();
    route.opt2();
    cerr << "From given: " << deliveries.as_string() << " has length " << deliveries.length() << endl;
    cerr << "From greedy: " << route.as_string() << " has length " << route.length() << endl;
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

void reverse_test(){
    Route deliveries;
    deliveries.add_address( Address(0,5) );
    deliveries.add_address( Address(5,0) );
    deliveries.add_address( Address(5,5) );
    for(int i = 1; i < 4; i++){
        for(int j = 1; j < i; j++){
            deliveries.print();
            deliveries.reverse(j, i);
            deliveries.print();
            deliveries.reverse(j,i);
        }
    }
}

void multi_opt2_test(){
    Route deliveries1, deliveries2;

    deliveries1.add_address(Address(0, 2));
    deliveries1.add_address(Address(2, 3));
    deliveries1.add_address(Address(3, 2));
    deliveries1.add_address(Address(2, 0));

    deliveries2.add_address(Address(1, 3));
    deliveries2.add_address(Address(1, 2));
    deliveries2.add_address(Address(2, 1));
    deliveries2.add_address(Address(3, 1));
    
    cout << "initial route 1: " << endl;
    deliveries1.print();
    cout << "length: " << deliveries1.length() << endl;
    cout << "initial route 2: " << endl;
    deliveries2.print();
    cout << "length: " << deliveries2.length() << endl;
    //optimization step
    deliveries1.multi_opt2(deliveries2);
    cout << "final delivery 1: " << endl;
    deliveries1.print();
    cout << "length: " << deliveries1.length() << endl;
    cout << "final delivery 2: " << endl;
    deliveries2.print();
    cout << "length: " << deliveries2.length() << endl;
}

void amazon_prime_test(){
    Route deliveries1, deliveries2, deliveries3, deliveries4, deliveries5, deliveries6;
    
    cout << "input routes" << endl;
    deliveries1.add_address(Address(0, 2));
    deliveries1.add_address(Address(2, 3));
    deliveries1.add_address(Address(3, 2));
    deliveries1.add_address(Address(2, 0));

    deliveries2.add_address(Address(1, 3));
    deliveries2.add_address(Address(1, 2));
    deliveries2.add_address(Address(2, 1));
    deliveries2.add_address(Address(3, 1));
    
    cout << "initial route 1: " << endl;
    deliveries1.print();
    cout << "length: " << deliveries1.length() << endl;
    cout << "initial route 2: " << endl;
    deliveries2.print();
    cout << "length: " << deliveries2.length() << endl;
    //optimization step
    deliveries1.multi_opt2(deliveries2);
    cout << endl;
    cout << "mTSP, no prime" << endl;
    cout << "final delivery 1: " << endl;
    deliveries1.print();
    cout << "length: " << deliveries1.length() << endl;
    cout << "final delivery 2: " << endl;
    deliveries2.print();
    cout << "length: " << deliveries2.length() << endl;

    deliveries3.add_address(Address(0, 2));
    deliveries3.add_address(Address(2, 3));
    deliveries3.add_address(Address(3, 2));
    deliveries3.add_address(Address(2, 0));

    deliveries4.add_address(Address(1, 3));
    deliveries4.add_address(Address(1, 2));
    deliveries4.add_address(Address(2, 1));
    deliveries4.add_address(Address(3, 1));
    vector<Address> fixed = {Address(0, 2)};
    deliveries3.multi_opt2(deliveries4, fixed);
    cout << endl;

    cout << "mTSP, with prime (includes mTSP solution address)" << endl;
    cout << "final delivery 3: " << endl;
    deliveries3.print();
    cout << "length: " << deliveries3.length() << endl;
    cout << "final delivery 4: " << endl;
    deliveries4.print();
    cout << "length: " << deliveries4.length() << endl;

    deliveries5.add_address(Address(0, 2));
    deliveries5.add_address(Address(2, 3));
    deliveries5.add_address(Address(3, 2));
    deliveries5.add_address(Address(2, 0));

    deliveries6.add_address(Address(1, 3));
    deliveries6.add_address(Address(1, 2));
    deliveries6.add_address(Address(2, 1));
    deliveries6.add_address(Address(3, 1));
    fixed = {Address(2, 0)};
    deliveries5.multi_opt2(deliveries6, fixed);
    cout << endl;

    cout << "mTSP, with prime (does not include mTSP solution address)" << endl;
    cout << "final delivery 5: " << endl;
    deliveries5.print();
    cout << "length: " << deliveries5.length() << endl;
    cout << "final delivery 6: " << endl;
    deliveries6.print();
    cout << "length: " << deliveries6.length() << endl;
}

void anyin_subsection_test(){
    Route deliveries1, deliveries2;

    deliveries1.add_address(Address(0, 2));
    deliveries1.add_address(Address(2, 3));
    deliveries1.add_address(Address(3, 2));
    deliveries1.add_address(Address(2, 0));

    deliveries2.add_address(Address(1, 3));
    deliveries2.add_address(Address(1, 2));
    deliveries2.add_address(Address(2, 1));
    deliveries2.add_address(Address(3, 1));

    vector<Address> fixed = {Address(0, 2)};
    cout << deliveries1.anyin_subsection(fixed, 0, 3) << endl;

}

void try_swap_test(){
    Route deliveries1, deliveries2;

    deliveries1.add_address(Address(0, 2));
    deliveries1.add_address(Address(2, 3));
    deliveries1.add_address(Address(3, 2));
    deliveries1.add_address(Address(2, 0));

    deliveries2.add_address(Address(1, 3));
    deliveries2.add_address(Address(1, 2));
    deliveries2.add_address(Address(2, 1));
    deliveries2.add_address(Address(3, 1));

    deliveries1.try_swap(deliveries2, 2, 2, 3, 3, false, false);
    cout << "final delivery 1: " << endl;
    deliveries1.print();
    cout << "final delivery 2: " << endl;
    deliveries2.print();

}

void swap_test(){
    Route deliveries1, deliveries2;

    deliveries1.add_address(Address(0, 2));
    deliveries1.add_address(Address(2, 3));
    deliveries1.add_address(Address(3, 2));
    deliveries1.add_address(Address(2, 0));

    deliveries2.add_address(Address(1, 3));
    deliveries2.add_address(Address(1, 2));
    deliveries2.add_address(Address(2, 1));
    deliveries2.add_address(Address(3, 1));

    cout << "original routes" << endl;
    deliveries1.print();
    deliveries2.print();
    cout << "swap links 1 and 2 of each" << endl;
    deliveries1.swap(deliveries2, 1, 1, 2, 2, false, false);
    deliveries1.print();
    deliveries2.print();
    //deliveries1.swap(deliveries2, 1, 1, 2, 2, false, false);
    deliveries1.unswap(deliveries2, 1, 1, 2, 2, false, false);
    cout << "unswap" << endl;
    deliveries1.print();
    deliveries2.print();
    cout << "swap links 1 and 2 from 1, 2 and 4 from second" << endl;
    deliveries1.swap(deliveries2, 1, 2, 2, 4, false, false);
    deliveries1.print();
    deliveries2.print();
    //deliveries1.swap(deliveries2, 1, 2, 3, 3, false, false);
    deliveries1.unswap(deliveries2, 1, 2, 2, 4, false, false);
    cout << "unswap" << endl;
    deliveries1.print();
    deliveries2.print();
    cout << "swap links 1 and 2 from 1, 2 and 4 from second, reverse both" << endl;
    deliveries1.swap(deliveries2, 1, 2, 2, 4, true, true);
    deliveries1.print();
    deliveries2.print();
    //deliveries1.swap(deliveries2, 1, 2, 3, 3, true, true);
    deliveries1.unswap(deliveries2, 1, 2, 2, 4, true, true);
    cout << "unswap" << endl;
    deliveries1.print();
    deliveries2.print();
    //cout << "swap links 0 and 2 from 1, 2 and 4 from second" << endl;
    //deliveries1.swap(deliveries2, 0, 2, 2, 4, false, false); 
}

void prime_ratio_test(){
    srand(137);
      
    int num_addresses = 20;
    int max_length = 20;
    Route route_a, route_b;
    AddressList addresses = {};

    for(int i = 0; i < num_addresses; i++){
        Address newaddress = Address(rand() % max_length, rand() % max_length);
        addresses.add_address(newaddress);
        if(rand() % 2 == 0){ //choose random route to add to
           route_a.add_address(newaddress);
        } else {
           route_b.add_address(newaddress);
        }
    }
    cout << endl;
    cout << "original routes" << endl;
    evaluate(route_a, route_b);


    for (int i = 0; i < addresses.size(); i++){
        AddressList available_for_prime = addresses;
        Route route1, route2;
        AddressList primes;
        route1 = route_a; route2 = route_b;

        cout << "initial routes" << endl;
        evaluate(route1, route2);

        for (int j = 0; j < i; j++){
            Address random_address = available_for_prime.pick_random();
            primes.add_address(random_address);
            available_for_prime.erase(available_for_prime.index_closest_to(random_address));
        }
        cout << "With " << i << " out of " << addresses.size() << " prime addresses, " << endl;
        primes.print();
        cout << " my routes are " << endl;
        route1.multi_opt2(route2, primes.my_addresses());
        evaluate(route1, route2);
    }
}

void prime_ratio_output(int num_addresses, int max_length){
    //num_adddresses is number of addresses in route
    //max_length is the maximum allowed value for coordinates

    srand (time(NULL));
    Route route_a, route_b;
    AddressList addresses = {};

    for(int i = 0; i < num_addresses; i++){
        Address newaddress = Address(rand() % max_length, rand() % max_length);
        addresses.add_address(newaddress);
        if(rand() % 2 == 0){ //choose random route to add to
           route_a.add_address(newaddress);
        } else {
           route_b.add_address(newaddress);
        }
    }
    cout << endl;
    cout << "unmodified length: " << route_a.length() + route_b.length() << endl;
    cout << "nprimes, " << "total addresses, " << "length, " << "difference" << endl;
    //evaluate(route_a, route_b);
    

    for (int i = 0; i < addresses.size(); i++){
        AddressList available_for_prime = addresses;
        Route route1, route2;
        AddressList primes;
        route1 = route_a; route2 = route_b;

        for (int j = 0; j < i; j++){
            Address random_address = available_for_prime.pick_random();
            primes.add_address(random_address);
            available_for_prime.erase(available_for_prime.index_closest_to(random_address));
        }
        //cout << "With " << i << " out of " << addresses.size() << " prime addresses, ";
        cout << i << "," << addresses.size() << "," ;
        //cout << " my routes are " << endl;
        route1.multi_opt2(route2, primes.my_addresses());
        //evaluate(route1, route2);
        cout << route1.length() + route2.length() << "," << route_a.length() + route_b.length() - route1.length() - route2.length() << endl;
    }
}

float dynamic_test1(){
    // in which we simply add new prime and nonprime addresses to list 1 each day
    // leaving list 2 to do what it will with the mopt2 output (like a queue)
    srand(time(NULL));

    int days = 50;
      
    int num_addresses = 50;
    int max_length = 20; // basically determines graph bound box 
    int prime_chance = 3; // x for 1/x where 1/x is the chance that an address is prime

    Route route_a, route_b;
    AddressList addresses; // all addresses in total graph
    AddressList primes;
    float sum_initial, sum_difference;

    for (int i = 0; i < days; i++){
        AddressList available_for_prime = route_a; //only addresses in route a can be prime

        float initial_total, final_total;
        for (int j = 0; j < rand() % num_addresses ; j++){ //tack on some new addresses
            Address newaddress = Address(rand() % max_length, rand() % max_length);
            if(rand() % 2 == 0){
                route_a.add_address(newaddress);
                if(rand() % prime_chance == 0){
                    primes.add_address(newaddress);
                }
            } else {
                route_b.add_address(newaddress);
            }
            //Address random_address = available_for_prime.pick_random();
            //primes.add_address(random_address);
            //available_for_prime.erase(available_for_prime.index_closest_to(random_address));
        }

        
        //cout << "day " << i << endl;
        //cout << "initial routes" << endl;
        //evaluate(route_a, route_b);
        initial_total = route_a.length() + route_b.length();
        //cout << "my primes are " << endl;
        //primes.print();
        //cout << " my routes are " << endl;
        route_a.multi_opt2(route_b, primes.my_addresses());
        //evaluate(route_a, route_b);
        final_total  = route_a.length() + route_b.length();

        sum_initial = sum_initial + initial_total;
        sum_difference = sum_difference + (final_total - initial_total);

        //at end of the day, clear routes
        route_a.clear();
        primes.clear();
    }
    //cout << "average percent difference: " << sum_difference/sum_initial * 100 << endl;
    return sum_difference/sum_initial * 100;
}

float dynamic_test2(){
    // in which I add all addresses in route_2 to route_1 on the next day and run mopt2
    srand(time(NULL));

    int days = 50;
      
    int num_addresses = 50;
    int max_length = 20; // basically determines graph bound box 
    int prime_chance = 3; // x for 1/x where 1/x is the chance that an address is prime in route1

    Route route_a, route_b;
    AddressList addresses; // all addresses in total graph
    AddressList primes;
    float sum_initial, sum_difference;

    for (int i = 0; i < days; i++){

        float initial_total, final_total;
        for (int j = 0; j < rand() % num_addresses ; j++){ //tack on new primes into route1, nonprimes into route2
            Address newaddress = Address(rand() % max_length, rand() % max_length);
            if(rand() % prime_chance * 2 == 0){
                route_a.add_address(newaddress);
                primes.add_address(newaddress);
            } else {
                route_b.add_address(newaddress);
            }
        }

        
        //cout << "day " << i << endl;
        //cout << "initial routes" << endl;
        //evaluate(route_a, route_b);
        initial_total = route_a.length() + route_b.length();
        //cout << "my primes are " << endl;
        //primes.print();
        //cout << " my routes are " << endl;
        route_a.multi_opt2(route_b, primes.my_addresses());
        //evaluate(route_a, route_b);
        final_total  = route_a.length() + route_b.length();

        sum_initial = sum_initial + initial_total;
        sum_difference = sum_difference + (final_total - initial_total);

        //at end of the day, clear routes
        route_a.clear();
        primes.clear();
        route_a = route_b;
        route_b.clear();
    }
    //cout << "average percent difference: " << sum_difference/sum_initial * 100 << endl;
    return sum_difference/sum_initial * 100;
}

float dynamic_test3(){
    // in which I add all addresses in route_2 to route_1 on the next day and run opt2
    srand(time(NULL));

    int days = 50;
      
    int num_addresses = 50;
    int max_length = 20; // basically determines graph bound box 
    int prime_chance = 3; // x for 1/x where 1/x is the chance that an address is prime in route1

    Route route_a, route_b;
    AddressList addresses; // all addresses in total graph
    AddressList primes;
    float sum_initial, sum_difference;

    for (int i = 0; i < days; i++){

        float initial_total, final_total;
        for (int j = 0; j < rand() % num_addresses ; j++){ //tack on new primes into route1, nonprimes into route2
            Address newaddress = Address(rand() % max_length, rand() % max_length);
            if(rand() % prime_chance * 2 == 0){
                route_a.add_address(newaddress);
                primes.add_address(newaddress);
            } else {
                route_b.add_address(newaddress);
            }
        }

        
        //cout << "day " << i << endl;
        //cout << "initial routes" << endl;
        //evaluate(route_a, route_b);
        initial_total = route_a.length() + route_b.length();
        //cout << "my primes are " << endl;
        //primes.print();
        //cout << " my routes are " << endl;
        route_a.opt2();
        //evaluate(route_a, route_b);
        final_total  = route_a.length() + route_b.length();

        sum_initial = sum_initial + initial_total;
        sum_difference = sum_difference + (final_total - initial_total);

        //at end of the day, clear routes
        route_a.clear();
        primes.clear();
        route_a = route_b;
        route_b.clear();
    }
    //cout << "average percent difference: " << sum_difference/sum_initial * 100 << endl;
    return sum_difference/sum_initial * 100;
}

void rand_test(){
    srand (time(NULL));
    cout << rand() % 100;
}


int main(int argc, char** argv) {
    //int a, b;
    //prime_ratio_output(atoi(argv[1]), atoi(argv[2]));
    //prime_ratio_test();
    vector<float> outputs;
    float mean, std;
    for(int i = 0; i < 20; i++){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        outputs.push_back(dynamic_test3());
    }
    float N = outputs.size();

    for(float x: outputs){
        mean = mean + x;
    }
    mean = mean/N;

    for(float x: outputs){
        std = std + pow((x - mean), 2);
    }
    cout << std << endl;
    std = pow(1./(N - 1.) * std, .5);
    cout << mean << "," << std << endl;
    return 0;
}




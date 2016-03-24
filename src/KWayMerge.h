#include<vector>
#include <queue>
#include<tuple>
#include<exception>

using namespace std;

typedef pair<T , *vector<T> > listhead;

template<typename T>
class KWayMerge {
    void kway_merge(vector< vector<T> >& sorted_lists, vector<T>& result_to_fill) {
        if ( !result_to_fill.empty() ) {
            throw runtime_error("Result list must be empty");
        }
        //determine result size
        size_t result_size = 0;
        for ( size_t i; i < sorted_lists.size(); i++ )
            result_size += sorted_lists[i].size();
        //reserve appropriate capacity to speed up the push_back 's    
        result_to_fill.reserve( result_size );
        //Store each non-empty list in a priority_queue with the back (highest element) of each list as the key.
        //sort in descending order (not as desired) and reverse result in the end. Easier to implement using vectors
        //Expect the sorted_lists in ascending order
        priority_queue< listhead > pq;
        for ( int i = 0; i < sorted_lists.size() ) {
            if ( sorted_lists[i].size() > 0 ) {
                pq.emplace( listhead( sorted_lists[i].back(), &sorted_lists[i] ) );
            }
        }
        //Take out the highest among all elements and add it to the result list
        while ( !pq.empty() ) {
            listhead lh = pq.top();
            pq.pop();
            
            T cur = lh.first;
            *vector<T> cur_list = lh.second;
            
            //insert into result
            result_to_fill.push_back( cur );
            
            //chop off the highest element from the current list
            cur_list.pop_back();
            
            //reinsert into priority_queue ( equals a decreasekey + list mod )
            if ( cur_list.size() > 0 ) {
                lh.first = cur_list.back();
                pq.push( lh );
            }
        }
        
        //Finally reverse the result to achieve ascending order
        T temp;     //swap variable
        result_size = result_to_fill.size()
        for ( size_t index = 0; index < result_size / 2; index++ ) {
            temp = result_to_fill[ index ];
            result_to_fill[ index ] = result_to_fill[ result_size - index ];
            result_to_fill[ result_size - index ] = temp;
        }
    }
};

#include <vector>
#include <queue>
#include <tuple>
#include <exception>
#include <algorithm>
#include <iostream>

using namespace std;

template<typename T>
class KWayMerge {
	typedef pair<T, vector<T>*> listhead;

public:
	void kway_merge(vector<vector<T>> &sorted_lists,
			vector<T> &result_to_fill) {
		//determine result size
		size_t result_size = 0;
		for (size_t i; i < sorted_lists.size(); i++) {
			result_size += sorted_lists[i].size();
		}

		if (result_to_fill.size() != result_size) {
			cout << result_to_fill.size() << " != " << result_size << endl;
			throw runtime_error("Result list must have result size");
		}

		//Store each non-empty list in a priority_queue with the back (highest element) of each list as the key.
		//sort in descending order (not as desired) and reverse result in the end. Easier to implement using vectors
		//Expect the sorted_lists in ascending order
		priority_queue<listhead> pq;
		for (size_t i = 0; i < sorted_lists.size(); i++) {
			if (sorted_lists[i].size() > 0) {
				pq.emplace(listhead(sorted_lists[i].back(), &sorted_lists[i]));
			}
		}

		int result_index = result_size - 1;

		//Take out the highest among all elements and add it to the result list
		while (!pq.empty()) {
			listhead lh = pq.top();
			pq.pop();

			T cur = lh.first;
			vector<T> *cur_list = lh.second;

			//insert into result
			result_to_fill[result_index--] = cur;

			//chop off the highest element from the current list
			cur_list->pop_back();

			//reinsert into priority_queue ( equals a decreasekey + list mod )
			if (cur_list->size() > 0) {
				lh.first = cur_list->back();
				pq.push(lh);
			}
		}
	}
};

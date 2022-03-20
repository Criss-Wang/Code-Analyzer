#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>

#include "intertable.h"
#include "../query_evaluator_exceptions.h"

namespace pql_table {

	InterTable::InterTable(pql::Synonym& synonym, std::vector<int>& int_list) {
	  header_ = std::vector<std::string>({ synonym.GetName() });
		for (int val : int_list) {
			rows_.push_back(std::vector<int>({ val }));
		}
	}

	InterTable::InterTable(std::vector<std::string>& header, std::vector<std::vector<int>>& rows) {
	  header_ = header;
		rows_ = rows;
	}

	int InterTable::GetColNum() {
		return header_.size();
	}

	int InterTable::GetRowNum() {
			return rows_.size();
	}

	int InterTable::FindSynCol(std::string& syn_name) {
		for (int index = 0; index < header_.size(); index++) {
			if (header_[index] == syn_name) {
				return index;
			}
		}

		return -1;
	}

	InterTable InterTable::GetColsByIndices(std::vector<int>& valid_col_nums) {
	  std::vector<std::string> new_header;
		std::vector<std::vector<int>> new_rows;

		//intialize new_rows to have the same size as rows_ with each new_row being an empty vector
		for (int index = 0; index < rows_.size(); index++) {
		  new_rows.push_back(std::vector<int>());
		}

		for (int& col_num : valid_col_nums) {
			new_header.push_back(header_[col_num]);

			for (int index = 0; index < rows_.size(); index++) {
				new_rows[index].push_back(rows_[index][col_num]);
			}
		}

		return InterTable(new_header, new_rows).Deduplicate();
	}

  InterTable InterTable::Deduplicate() {
	  std::unordered_set<std::vector<int>, hash_vector_fn> s(rows_.begin(), rows_.end());
		std::vector<std::vector<int>> new_rows(s.begin(), s.end());
		std::vector<std::string> new_header(header_);
		return InterTable(new_header, new_rows);
	}

	InterTable InterTable::Merge(InterTable& t1) {
		std::vector<std::string> new_header(header_);
		
		for (auto& syn : t1.header_) {
			new_header.push_back(syn);
		}

		//apply a cross product between current rows and t1 rows
		std::vector<std::vector<int>> new_rows;

		std::for_each(this->rows_.begin(), this->rows_.end(),
		  [&](auto& front_row) {
				std::for_each(t1.rows_.begin(), t1.rows_.end(),
				  [&](auto& back_row) {
						std::vector<int> row_to_be_insert;

						for (int& ele : front_row) {
						  row_to_be_insert.push_back(ele);
						}

						for (int& ele : back_row) {
						  row_to_be_insert.push_back(ele);
						}

						new_rows.push_back(row_to_be_insert);
					});
			}
		);

		return InterTable(new_header, new_rows);
	}
	
	InterTable InterTable::Filter(Predicate& pred) {
		std::vector<std::string> new_header(header_);
		std::vector<std::vector<int>> new_rows;
		int first_syn_col_index = FindSynCol(pred.first_syn_);
		int second_syn_col_index = FindSynCol(pred.second_syn_);
		std::unordered_set<std::pair<int, int>, hash_pair_fn> 
				pair_domain(pred.allowed_pairs_.begin(), pred.allowed_pairs_.end());

		for (int index = 0; index < rows_.size(); index++) {
			std::pair<int, int> cur_pair(rows_[index][first_syn_col_index], rows_[index][second_syn_col_index]);
			std::unordered_set<std::pair<int, int>>::iterator iter;
			iter = pair_domain.find(cur_pair);

			if (iter != pair_domain.end()) {
			  new_rows.push_back(rows_[index]);
			}
		}

		if (new_rows.empty()) {
		  throw pql_exceptions::EmptyTableException();
		}

		return InterTable(new_header, new_rows);
	}
		
	std::unordered_map<int, std::vector<int>> GenerateIndexMap(std::vector<std::vector<int>>& rows, int col_index) {
		std::unordered_map<int, std::vector<int>> row_domain;

		for (int index = 0; index < rows.size(); index++) {
		  std::unordered_map<int, std::vector<int>>::iterator iter;
		  iter = row_domain.find(rows[index][col_index]);

  		if (iter == row_domain.end()) {
				//the element does not exist in the map
				std::vector<int> v({ index });
				row_domain.insert({ rows[index][col_index], v });
	  	}	else {
				iter->second.push_back(index);
	  	}
		}

		return row_domain;
	}

	InterTable InterTable::MergeAndFilter(InterTable& t1, Predicate& pred) {
		std::vector<std::string> new_header(header_);

		for (auto& syn : t1.header_) {
			new_header.push_back(syn);
		}

		std::vector<std::vector<int>> new_rows;
		int first_syn_col_index = FindSynCol(pred.first_syn_);
		int second_syn_col_index = t1.FindSynCol(pred.second_syn_);

		//The maps store element to a list of row indices that contain the element
		std::unordered_map<int, std::vector<int>> first_syn_domain = GenerateIndexMap(rows_, first_syn_col_index);
		std::unordered_map<int, std::vector<int>> second_syn_domain = GenerateIndexMap(t1.rows_, second_syn_col_index);

		for (auto& ele_pair : pred.allowed_pairs_) {
		  std::unordered_map<int, std::vector<int>>::iterator it1;
		  it1 = first_syn_domain.find(ele_pair.first);
		  if (it1 == first_syn_domain.end()) {
				continue;
		  }

		  std::unordered_map<int, std::vector<int>>::iterator it2;
		  it2 = second_syn_domain.find(ele_pair.second);
		  if (it2 == second_syn_domain.end()) {
				continue;
		  }

		  for (int first_index : it1->second) {
				for (int second_index : it2->second) {
				  std::vector<int> row_to_be_insert;
						
				  for (auto& ele : rows_[first_index]) {
						row_to_be_insert.push_back(ele);
				  }

				  for (auto& ele : t1.rows_[second_index]) {
						row_to_be_insert.push_back(ele);
				  }
						
				  new_rows.push_back(row_to_be_insert);
				}
		  }
		}

		if (new_rows.empty()) {
				throw pql_exceptions::EmptyTableException();
		}

		return InterTable(new_header, new_rows);
	}

	bool InterTable::equal(InterTable& t) {
		if (GetColNum() != t.GetColNum() || GetRowNum() != t.GetRowNum()) {
			return false;
		}

		//check header equality
		for (int index = 0; index < GetColNum(); index++) {
			if (header_[index] != t.header_[index]) {
				return false;
			}
		}

		std::multiset<std::vector<int>> s1(rows_.begin(), rows_.end());
		std::multiset<std::vector<int>> s2(t.rows_.begin(), t.rows_.end());

		return s1 == s2;
	}
}

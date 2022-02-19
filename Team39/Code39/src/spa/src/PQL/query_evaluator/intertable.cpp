#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>

#include "intertable.h"

namespace pql_table {

	InterTable::InterTable(pql::Synonym& synonym, std::vector<int>& int_list) {
	  header_ = std::vector<std::string>({ synonym.GetName() });
		for (int val : int_list) {
			element to_be_insert;
			to_be_insert.val = val;
			rows_.push_back(std::vector<element>({ to_be_insert }));
		}
	}

	InterTable::InterTable(pql::Synonym& synonym, std::vector<std::string>& str_list) {
    header_ = std::vector<std::string>({ synonym.GetName() });
		for (std::string str : str_list) {
			element to_be_insert;
			to_be_insert.name = str;
			rows_.push_back(std::vector<element>({ to_be_insert }));
		}
	}

	InterTable::InterTable(std::vector<std::string>& header, std::vector<std::vector<element>>& rows) {
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

	std::vector<element> InterTable::GetColByName(std::string& name) {
	  std::vector<std::string> header({ name });
		int index = FindSynCol(name);
		std::vector<std::vector<element>> rows;

		for (auto& row : rows_) {
		  std::vector<element> row_to_be_insert({ row[index] });
			rows.push_back(row_to_be_insert);
		}

		InterTable table(header, rows);
		table = table.Deduplicate();

		std::vector<element> res;

		for (auto& row : table.rows_) {
				res.push_back(row[0]);
		}

		return res;
	}

	void InterTable::DeleteRow(int row_index) {
		rows_.erase(rows_.begin() + row_index);
	}

	void InterTable::DeleteCol(int col_index) {
		header_.erase(header_.begin() + col_index);
				
		std::for_each(rows_.begin(), rows_.end(),
		  [&](auto& row) {
			  row.erase(row.begin() + col_index);
			}
		);
	}

  InterTable InterTable::Deduplicate() {
	  std::unordered_set<std::vector<element>, hash_vector_fn> s(rows_.begin(), rows_.end());
		std::vector<std::vector<element>> new_rows(s.begin(), s.end());
		std::vector<std::string> new_header(header_);
		return InterTable(new_header, new_rows);
	}

	InterTable InterTable::Merge(InterTable& t1) {
		std::vector<std::string> new_header(header_);
		
		for (auto& syn : t1.header_) {
			new_header.push_back(syn);
		}

		//apply a cross product between current rows and t1 rows
		std::vector<std::vector<element>> new_rows;

		std::for_each(this->rows_.begin(), this->rows_.end(),
		  [&](auto& front_row) {
				std::for_each(t1.rows_.begin(), t1.rows_.end(),
				  [&](auto& back_row) {
						std::vector<element> row_to_be_insert;

						for (element& ele : front_row) {
						  row_to_be_insert.push_back(ele);
						}

						for (element& ele : back_row) {
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
		std::vector<std::vector<element>> new_rows;
		int first_syn_col_index = FindSynCol(pred.first_syn_);
		int second_syn_col_index = FindSynCol(pred.second_syn_);
		std::unordered_set<std::pair<element, element>, hash_pair_fn> 
				pair_domain(pred.allowed_pairs_.begin(), pred.allowed_pairs_.end());

		for (int index = 0; index < rows_.size(); index++) {
			std::pair<element, element> cur_pair(rows_[index][first_syn_col_index], rows_[index][second_syn_col_index]);
			std::unordered_set < std::pair<element, element>, int>::iterator iter;
			iter = pair_domain.find(cur_pair);

			if (iter != pair_domain.end()) {
			  new_rows.push_back(rows_[index]);
			}
		}

		return InterTable(new_header, new_rows);
	}

	InterTable InterTable::MergeAndFilter(InterTable& t1, Predicate& pred) {
			std::vector<std::string> new_header(header_);

			for (auto& syn : t1.header_) {
					new_header.push_back(syn);
			}

		std::vector<std::vector<element>> new_rows;
		int first_syn_col_index = FindSynCol(pred.first_syn_);
		int second_syn_col_index = t1.FindSynCol(pred.second_syn_);

		//The maps store element to a list of row indices that contain the element
		std::unordered_map<element, std::vector<int>, hash_fn> first_syn_domain;
		std::unordered_map<element, std::vector<int>, hash_fn> second_syn_domain;

		for (int index = 0; index < rows_.size(); index++) {
		  std::unordered_map<element, std::vector<int>>::iterator iter;
		  iter = first_syn_domain.find(rows_[index][first_syn_col_index]);

		  if (iter == first_syn_domain.end()) {
				//the element does not exist in the map
				std::vector<int> v({ index });
				first_syn_domain.insert({ rows_[index][first_syn_col_index], v });
		  } else {
				iter->second.push_back(index);
		  }
		}

		for (int index = 0; index < t1.rows_.size(); index++) {
		  std::unordered_map<element, std::vector<int>>::iterator iter;
		  iter = second_syn_domain.find(t1.rows_[index][second_syn_col_index]);

		  if (iter == second_syn_domain.end()) {
				//the element does not exist in the map
				std::vector<int> v({ index });
				second_syn_domain.insert({ t1.rows_[index][second_syn_col_index], v });
		  }
		  else {
				iter->second.push_back(index);
		  }
		}

		for (auto& ele_pair : pred.allowed_pairs_) {
		  std::unordered_map<element, std::vector<int>>::iterator it1;
		  it1 = first_syn_domain.find(ele_pair.first);
		  if (it1 == first_syn_domain.end()) {
				continue;
		  }

		  std::unordered_map<element, std::vector<int>>::iterator it2;
		  it2 = second_syn_domain.find(ele_pair.second);
		  if (it2 == second_syn_domain.end()) {
				continue;
		  }

		  for (int first_index : it1->second) {
				for (int second_index : it2->second) {
				  std::vector<element> row_to_be_insert;
						
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

		std::multiset<std::vector<element>> s1(rows_.begin(), rows_.end());
		std::multiset<std::vector<element>> s2(t.rows_.begin(), t.rows_.end());

		return s1 == s2;
	}
}

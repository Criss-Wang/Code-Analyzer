#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "InterTable.h"

namespace pql_table {

	InterTable::InterTable(pql::Synonym& synonym, std::vector<int>& int_list) {
	  header_ = std::vector<pql::Synonym>(synonym);
		for (int val : int_list) {
			element to_be_insert;
			to_be_insert.val = val;
		  rows_.push_back(std::vector<element>(to_be_insert));
		}
	}

	InterTable::InterTable(pql::Synonym& synonym, std::vector<std::string>& str_list) {
	  header_ = std::vector<pql::Synonym>(synonym);
		for (std::string str : str_list) {
			element to_be_insert;
			to_be_insert.name = str;
			rows_.push_back(std::vector<element>(to_be_insert));
		}
	}

	int InterTable::GetColNum() {
		return header_.size();
	}

	int InterTable::FindSynCol(pql::Synonym& syn) {
		//Here we assume that the syn will always be in the header
		for (int index = 0; index < header_.size(); index++) {
			if (header_[index].GetName() == syn.GetName()) {
				return index;
			}
		}
	}

	void InterTable::DeleteRow(int row_index) {
		rows_.erase(row_index);
	}

	void InterTable::DeleteCol(int col_index) {
		header_.erase(col_index);
				
		std::for_each(rows_.begin(), rows_.end(),
		  [](auto& row) {
			  row.erase(col_index);
			}
		);
	}

	void InterTable::Deduplicate() {
	  std::unordered_set<std::vector<element>> s(rows_.begin(), rows_.end());
		rows_.assign(s.begin(), s.end());
	}

	void InterTable::Merge(InterTable& t1) {
		//add the header of t1 to the current header
		for (auto& syn : t1.header_) {
			header_.push_back(syn);
		}

		//apply a cross product between current rows and t1 rows
		std::vector<std::vector<element>> new_rows;

		std::for_each(this->rows_.begin(), this->rows_.end(),
		  [](auto& front_row) {
				std::for_each(t1.rows_.begin(), t1.rows_end(),
				  [](auto& back_row) {
						std::vector<element> row_to_be_insert;

						for (element& ele : front_row) {
								row_to_be_insert.push_back(ele)
						}

						for (element& ele : back_row) {
						  row_to_be_insert.push_back(ele)
						}

						new_rows.push_back(row_to_be_insert);
					});
			}
		);

		rows_ = new_rows;
	}

	void InterTable::Filter(Predicate& pred) {
		std::vector<std::vector<element>> new_rows;
		int first_syn_col_index = FindSynCol(pred.first_syn);
		int second_syn_col_index = FindSynCol(pred.second_syn);
		std::unordered_map<element, int, hash_fn> first_syn_domain;
		std::unordered_map<element, int, hash_fn> second_syn_domain;

		for (int index = 0; index < rows_.size(); index++) {
			first_syn_domain.insert({ rows_[index][first_syn_col_index], index });
			second_syn_domain.insert({ rows_[index][second_syn_col_index], index });
		}

		for (auto& ele_pair : pred.allowed_pairs) {
				std::unordered_map<element, int>::iterator it1;
				it1 = first_syn_domain.find(ele_pair.first);
				if (it1 == first_syn_domain.end()) {
						continue;
				}

				std::unordered_map<element, int>::iterator it2;
				it2 = second_syn_domain.find(ele_pair.second);
				if (it2 == second_syn_domain.end()) {
						continue;
				}

				std::vector<element> row_to_be_insert;
				for (element& ele : rows_[it1->second]) {
						row_to_be_insert.push_back(ele);
				}
				new_rows.push_back(row_to_be_insert);
		}

		rows_ = new_rows;
	}

	void MergeAndFilter(InterTable& t1, Predicate& pred) {
			std::vector<std::vector<element>> new_rows;
			int first_syn_col_index = FindSynCol(pred.first_syn);
			int second_syn_col_index = t1.FindSynCol(pred.second_syn);
			std::unordered_map<element, int, hash_fn> first_syn_domain;
			std::unordered_map<element, int, hash_fn> second_syn_domain;

			for (int index = 0; index < rows_.size(); index++) {
				first_syn_domain.insert({ rows_[index][first_syn_col_index], index });
			}

			for (int index = 0; index < t1.rows_.size(); index++) {
				second_syn_domain.insert({ t1.rows_[index][second_syn_col_index], index });
			}


			for (auto& ele_pair : pred.allowed_pairs) {
					std::unordered_map<element, int>::iterator it1;
					it1 = first_syn_domain.find(ele_pair.first);
					if (it1 == first_syn_domain.end()) {
							continue;
					}

					std::unordered_map<element, int>::iterator it2;
					it2 = second_syn_domain.find(ele_pair.second);
					if (it2 == second_syn_domain.end()) {
							continue;
					}

					std::vector<element> row_to_be_insert;
					for (element& ele : rows_[it1->second]) {
							row_to_be_insert.push_back(ele);
					}

					for (element& ele : rows_[it2->second]) {
							row_to_be_insert.push_back(ele);
					}

					new_rows.push_back(row_to_be_insert);
			}

			rows_ = new_rows;
	}
}

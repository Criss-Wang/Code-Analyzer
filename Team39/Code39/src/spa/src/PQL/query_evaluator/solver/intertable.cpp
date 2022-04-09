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

	InterTable::InterTable(std::vector<std::string>&& header, std::vector<std::vector<int>>&& rows) {
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

	//Return the columns that the index is inside valid_col_nums
	InterTable InterTable::GetColsByIndices(std::vector<int>& valid_col_nums) {
	  std::vector<std::string> new_header;
		new_header.reserve(GetColNum());
		std::vector<std::vector<int>> new_rows;
		new_rows.reserve(GetRowNum());

		//intialize new_rows to have the same size as rows_ with each new_row being an empty vector
		for (int index = 0; index < rows_.size(); index++) {
		  std::vector<int> new_row;
		  new_row.reserve(GetColNum());
		  new_rows.push_back(move(new_row));
		}

		for (int& col_num : valid_col_nums) {
			new_header.push_back(header_[col_num]);

			for (int index = 0; index < rows_.size(); index++) {
				new_rows[index].push_back(rows_[index][col_num]);
			}
		}

		return InterTable(move(new_header), move(new_rows)).Deduplicate();
	}

  InterTable InterTable::Deduplicate() {
	  std::unordered_set<std::vector<int>, hash_vector_fn> s(rows_.begin(), rows_.end());
		std::vector<std::vector<int>> new_rows(s.begin(), s.end());
		std::vector<std::string> new_header(header_);
		return InterTable(move(new_header), move(new_rows));
	}

	std::vector<std::string> GetHeaderFromTwoTables(InterTable& t1, InterTable& t2, int col_num) {
    std::vector<std::string> header;
		header.reserve(col_num);
		
		header.insert(header.end(), t1.header_.begin(), t1.header_.end());
		header.insert(header.end(), t2.header_.begin(), t2.header_.end());

		return header;
	}

	std::vector<int> MergeTwoRows(std::vector<int>& front_row, std::vector<int>& back_row, int col_num) {
    std::vector<int> row_to_be_insert;
		row_to_be_insert.reserve(col_num);

		row_to_be_insert.insert(row_to_be_insert.end(), front_row.begin(), front_row.end());
		row_to_be_insert.insert(row_to_be_insert.end(), back_row.begin(), back_row.end());

		return row_to_be_insert;
	}

	InterTable InterTable::Merge(InterTable& t1) {
	  int res_row_num = this->GetRowNum() * t1.GetRowNum();
		int res_col_num = this->GetColNum() + t1.GetColNum();
		
		std::vector<std::string> new_header = move(GetHeaderFromTwoTables(*this, t1, res_col_num));
		
		//apply a cross product between current rows and t1 rows
		std::vector<std::vector<int>> new_rows;
		new_rows.reserve(res_row_num);

		std::for_each(this->rows_.begin(), this->rows_.end(),
		  [&](auto& front_row) {
				std::for_each(t1.rows_.begin(), t1.rows_.end(),
				  [&](auto& back_row) {
						new_rows.push_back(move(MergeTwoRows(front_row, back_row, res_col_num)));
					});
			}
		);

		return InterTable(move(new_header), move(new_rows));
	}
	
	InterTable InterTable::Filter(Predicate& pred) {
		int max_row_num = GetRowNum();

		std::vector<std::string> new_header(header_);

		std::vector<std::vector<int>> new_rows;
		new_rows.reserve(max_row_num);

		int first_syn_col_index = FindSynCol(pred.first_syn_);
		int second_syn_col_index = FindSynCol(pred.second_syn_);
		std::unordered_set<std::pair<int, int>, hash_pair_fn> 
				pair_domain(pred.allowed_pairs_.begin(), pred.allowed_pairs_.end());

		for (auto& row : rows_) {
			std::pair<int, int> cur_pair = std::make_pair(row[first_syn_col_index], row[second_syn_col_index]);

			if (pair_domain.find(cur_pair) != pair_domain.end()) {
			  new_rows.push_back(row);
			}
		}

		if (new_rows.empty()) {
		  throw pql_exceptions::EmptyTableException();
		}

		return InterTable(move(new_header), move(new_rows));
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
	  int res_row_num = this->GetRowNum() * t1.GetRowNum();
		int res_col_num = this->GetColNum() + t1.GetColNum();

	  std::vector<std::string> new_header = move(GetHeaderFromTwoTables(*this, t1, res_col_num));

		std::vector<std::vector<int>> new_rows;
		new_rows.reserve(res_row_num);

		int first_syn_col_index = FindSynCol(pred.first_syn_);
		int second_syn_col_index = t1.FindSynCol(pred.second_syn_);

		//The maps store element to a list of row indices that contain the element
		std::unordered_map<int, std::vector<int>> first_syn_domain = move(GenerateIndexMap(rows_, first_syn_col_index));
		std::unordered_map<int, std::vector<int>> second_syn_domain = move(GenerateIndexMap(t1.rows_, second_syn_col_index));

		for (auto& ele_pair : pred.allowed_pairs_) {
		  if (first_syn_domain.find(ele_pair.first) == first_syn_domain.end()
					|| second_syn_domain.find(ele_pair.second) == second_syn_domain.end()) {
			  continue;
			}

			//only merge the rows that have pair inside allowed_pairs
		  for (int first_index : first_syn_domain[ele_pair.first]) {
				for (int second_index : second_syn_domain[ele_pair.second]) {
					new_rows.push_back(move(MergeTwoRows(this->rows_[first_index], t1.rows_[second_index], res_col_num)));
				}
		  }
		}

		if (new_rows.empty()) {
				throw pql_exceptions::EmptyTableException();
		}

		return InterTable(move(new_header), move(new_rows));
	}

	bool InterTable::equal(InterTable t) {
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

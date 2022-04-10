#pragma once

enum class EntityIdentifier {
	kProc,
	kStmtLst,
	kStmt,
	kRead,
	kPrint,
	kAssign,
	kWhile,
	kCall,
	kIf,
	kVariable,
	kConstant,
  kWildcard,
  kIdent,
  kStmtNumber
};

struct hash_pair_fn {
  std::size_t operator() (const std::pair<int, int>& p) const {
    std::size_t h1 = std::hash<int>{}(p.first);
    std::size_t h2 = std::hash<int>{}(p.second);

    return h1 ^ h2;
  }
};
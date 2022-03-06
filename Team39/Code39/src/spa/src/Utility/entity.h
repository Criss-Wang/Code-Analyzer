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

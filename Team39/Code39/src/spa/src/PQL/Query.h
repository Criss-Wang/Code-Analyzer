

class Query {
public:
	Query();
	~Query();

	enum class EntityType {
		STMT,
		READ,
		PRINT,
		CALL,
		WHILE,
		IF,
		ASSIGN,
		VARIABLE,
		CONSTANT,
		PROC,
	};

	EntityType getSelectType();
};

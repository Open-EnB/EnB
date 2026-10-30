// Earth & Beyond client (reconstruction): app\shared\boolpars.cpp. Notes at the end of the file.
#include <string.h>
#include <stdio.h>
#include "boolpars.h"
#include "unknown_debug.h"

static void *BoolContext;  // NAME 1: 0xBE34D8, set only around BoolExpressionClass::Evaluate

#define SKIP_WHITE(p)	while (**(p) == ' ' || **(p) == '\t') (*(p))++	// NAME 2: describes what it does


BoolValueStruct::~BoolValueStruct()
{
	if (Type == TYPE_STRING && String != NULL) {
		delete [] String;
		String = NULL;
	}
	if (Type == TYPE_FUNCTION && Function != NULL) {
		delete Function;
		Function = NULL;
	}
}


bool BoolValueStruct::Get_Bool() const
{
	switch (Type) {
		case TYPE_BOOL:
			return Bool;
		case TYPE_INT:
		case TYPE_STRING:
			return Int != 0;
		case TYPE_FLOAT:
			return Float != 0.0f;
		case TYPE_FUNCTION:
			if (Function != NULL) {
				return Function->Evaluate().Get_Bool();
			}
			break;
	}
	return false;
}


bool BoolValueStruct::Get_Int(int &value) const
{
	if (Type == TYPE_INT) {
		value = Int;
		return true;
	}
	if (Type == TYPE_FUNCTION && Function != NULL) {
		return Function->Evaluate().Get_Int(value);
	}
	return false;
}


bool BoolValueStruct::Get_Float(float &value) const
{
	if (Type == TYPE_FLOAT) {
		value = Float;
		return true;
	}
	if (Type == TYPE_FUNCTION && Function != NULL) {
		return Function->Evaluate().Get_Float(value);
	}
	return false;
}


bool BoolValueStruct::Get_String(char *&value) const
{
	if (Type == TYPE_STRING) {
		value = String;
		return true;
	}
	if (Type == TYPE_FUNCTION && Function != NULL) {
		return Function->Evaluate().Get_String(value);
	}
	return false;
}


bool BoolValueStruct::Get_Function(BoolFunctionNodeClass *&value) const
{
	if (Type == TYPE_FUNCTION) {
		value = Function;
		return true;
	}
	return false;
}


BoolExpressionClass::~BoolExpressionClass()
{
	if (TreeRoot != NULL) {
		delete TreeRoot;
		TreeRoot = NULL;
	}
}























































































































bool BoolExpressionClass::Evaluate(void *context)
{
	G_ASSERT(TreeRoot != NULL);
	BoolContext = context;
	bool result = TreeRoot->Evaluate().Get_Bool();
	BoolContext = NULL;
	return result;
}


bool BoolOperatorClass::Evaluate(BoolNodeClass *left, BoolNodeClass *right)
{
	switch (Op) {
		case OP_AND:
			if (!left->Evaluate().Get_Bool()) {
				return false;
			}
			return right->Evaluate().Get_Bool();
		case OP_OR:
			if (left->Evaluate().Get_Bool() == true) {
				return true;
			}
			return right->Evaluate().Get_Bool();
		case OP_NOT:
			return !left->Evaluate().Get_Bool();
	}
	return false;
}


BoolParserClass::BoolParserClass(BoolEvaluatorMap *callbacks)
{
	EvaluatorCallbacks = callbacks;
}


BoolParserClass::~BoolParserClass()
{
}




































void BoolParserClass::Set_Callbacks(BoolEvaluatorMap *callbacks)
{
	EvaluatorCallbacks = callbacks;
}

static char *Duplicate_Text(const char *text)	// NAME 2: describes what it does (not in the client: inlined into Parse)
{
	char *p = new char[strlen(text) + 1];
	G_ASSERT((p != NULL));
	if (p != NULL) {
		strcpy(p, text);
	}
	return p;
}


// NOT EXACT: inline budget, the client calls the second Parse_And 0x49A900 out of line (ours inlines it)
BoolExpressionClass *BoolParserClass::Parse(const char *text)
{
	if (text == NULL) {
		return NULL;
	}
	char *p = Duplicate_Text(text);
	if (p == NULL) {
		return NULL;
	}
	const char *cursor = p;
	BoolNodeClass *root = Parse_Or(&cursor);
	if (root == NULL) {
		delete [] p;
		return NULL;
	}
	SKIP_WHITE(&cursor);
	if (*cursor != '\n' && *cursor != '\0') {
		return NULL;
	}
	delete [] p;
	cursor = NULL;
	return new BoolExpressionClass(root);
}


BoolNodeClass *BoolParserClass::Parse_Or(const char **p)
{
	BoolNodeClass *left = Parse_And(p);
	if (left == NULL) {
		return NULL;
	}
	return Parse_Or_Rest(p, left);
}


BoolNodeClass *BoolParserClass::Parse_Or_Rest(const char **p, BoolNodeClass *left)
{
	SKIP_WHITE(p);
	if (strncmp(*p, "||", 2) != 0) {
		return left;
	}
	*p += 2;
	BoolNodeClass *right = Parse_And(p);
	if (right == NULL) {
		return NULL;
	}
	return Parse_Or_Rest(p, Create_Logic(BoolOperatorClass::OP_OR, left, right));
}


BoolNodeClass *BoolParserClass::Parse_And(const char **p)
{
	BoolNodeClass *left = Parse_Factor(p);
	if (left == NULL) {
		return NULL;
	}
	return Parse_And_Rest(p, left);
}


BoolNodeClass *BoolParserClass::Parse_And_Rest(const char **p, BoolNodeClass *left)
{
	SKIP_WHITE(p);
	if (strncmp(*p, "&&", 2) != 0) {
		return left;
	}
	*p += 2;
	BoolNodeClass *right = Parse_Factor(p);
	if (right == NULL) {
		return NULL;
	}
	return Parse_And_Rest(p, Create_Logic(BoolOperatorClass::OP_AND, left, right));
}


BoolNodeClass *BoolParserClass::Parse_Factor(const char **p)
{
	SKIP_WHITE(p);
	if (**p == '!') {
		(*p)++;
		BoolNodeClass *node = Parse_Factor(p);
		return Create_Logic(BoolOperatorClass::OP_NOT, node, NULL);
	}
	if (**p == '(') {
		(*p)++;
		BoolNodeClass *node = Parse_Or(p);
		SKIP_WHITE(p);
		if (**p != ')') {
			return NULL;
		}
		(*p)++;
		return node;
	}
	if (strnicmp(*p, "true", strlen("true")) == 0) {
		*p += strlen("true");
		return Create_Constant(true);
	}
	if (strnicmp(*p, "false", strlen("false")) == 0) {
		*p += strlen("false");
		return Create_Constant(false);
	}
	return Parse_Function(p);
}


BoolNodeClass *BoolParserClass::Parse_Function(const char **p)
{
	char name[128];
	BoolValueStruct **params;

	SKIP_WHITE(p);
	int length = 0;
	while (**p != ' ' && **p != '\t' && **p != '(' && **p != '\0') {
		name[length] = **p;
		(*p)++;
		length++;
	}
	name[length] = '\0';
	BoolEvaluatorClass *callback = Get_Callback(name);
	if (callback == NULL) {
		return NULL;
	}
	SKIP_WHITE(p);
	if (**p != '(') {
		return NULL;
	}
	(*p)++;
	int count = Parse_Params(p, params);
	if (count < 0) {
		return NULL;
	}
	SKIP_WHITE(p);
	if (**p != ')') {
		return NULL;
	}
	(*p)++;
	return Create_Function(callback, params, count);
}






































// NOT EXACT: shape, the loop's failure returns jump to the second 'return 0' tail in the client, the first in ours
int BoolParserClass::Parse_Params(const char **p, BoolValueStruct **&param_list)
{
	param_list = new BoolValueStruct *[4];
	G_ASSERT((param_list != NULL));
	if (param_list == NULL) {
		return 0;
	}
	int count = 0;
	SKIP_WHITE(p);
	if (**p == ')') {
		return 0;
	}
	while (true) {
		if (count >= 4) {
			return 0;
		}
		param_list[count] = Parse_Param(p);
		if (param_list[count] == NULL) {
			return 0;
		}
		count++;
		SKIP_WHITE(p);
		if (**p != ',') {
			return count;
		}
		(*p)++;
	}
}








































BoolValueStruct *BoolParserClass::Parse_Param(const char **p)
{
	float value;

	SKIP_WHITE(p);
	if (sscanf(*p, "%f", &value) == 1) {
		while (**p != ' ' && **p != '\t' && **p != ',' && **p != ')' && **p != '\0') {
			(*p)++;
		}
		int integer = (int)value;
		if ((float)integer == value) {
			return new BoolValueStruct(integer);
		}
		return new BoolValueStruct(value);
	}
	if (**p == '"') {
		(*p)++;
		const char *end = strchr(*p, '"');
		if (end != NULL) {
			int length = end - *p;
			char *str = new char[length + 1];
			G_ASSERT((str != NULL));
			if (str != NULL) {
				strncpy(str, *p, length);
				str[length] = '\0';
				*p += length + 1;
				return new BoolValueStruct(str);
			}
		}
		return NULL;
	}
	BoolNodeClass *node = Parse_Function(p);
	if (node != NULL) {
		return new BoolValueStruct(node->As_Function());
	}
	return NULL;
}


BoolNodeClass *BoolParserClass::Create_Logic(int op, BoolNodeClass *left, BoolNodeClass *right)
{
	return new BoolLogicNodeClass(op, left, right);
}


BoolNodeClass *BoolParserClass::Create_Constant(bool value)
{
	return new BoolFunctionNodeClass(value);
}


BoolNodeClass *BoolParserClass::Create_Function(BoolEvaluatorClass *callback, BoolValueStruct **params, int count)
{
	return new BoolFunctionNodeClass(callback, params, count);
}
















BoolEvaluatorClass *BoolParserClass::Get_Callback(char *name)
{
	G_ASSERT(EvaluatorCallbacks != NULL);
	strlwr(name);
	BoolEvaluatorMap::iterator it = EvaluatorCallbacks->find(std::string(name));
	if (it == EvaluatorCallbacks->end()) {
		return NULL;
	}
	return it->second;
}

/*
**	Reconstruction notes
**
**	client.exe 0x499750-0x49B5E0, then the node classes' inline virtuals and the table's instances to
**	about 0x49B9A0 (callback_manager.cpp follows at 0x49B9F0): the value's destructor 0x499750 and its
**	accessors Get_Bool 0x4997A0, Get_Int 0x499920, Get_Float 0x499A30, Get_String 0x499B40,
**	Get_Function 0x499C50; BoolExpressionClass's destructor 0x499C80 and Evaluate 0x499CB0 (the
**	assert on the client's line 222, the context in 0xBE34D8 while it runs); the operator 0x499E70
**	(a member object at +4 of the logic node, its own vtable 0xAEA958); the parser's constructor
**	0x49A390, destructors 0x49A3C0 / 0x49A3F0, Set_Callbacks 0x49A410, Parse 0x49A430 (line 303), the
**	recursive descent 0x49A650-0x49AF10 (lines 492 / 578), the node factories 0x49B170 / 0x49B3C0 /
**	0x49B590 and Get_Callback 0x49B5E0 (line 630; the name lower-cased, then looked up in
**	EvaluatorCallbacks, an SGI hash_map keyed by std::string, libs/sgistl, D87). Nothing in the client
**	calls the parser (no caller of the constructor, Parse or Evaluate). The '_Rest' functions are tail
**	recursive (VC6 turns them into loops and inlines them one level into Parse_And / Parse_Or, as the
**	client shows); whitespace is skipped by a macro (every copy inlined). Duplicate_Text is not in the
**	client: it is what keeps the parse's inline budget near the client's. NOT EXACT: Parse 0x49A430
**	(inline budget) and Parse_Params 0x49AE30 (shape).
*/

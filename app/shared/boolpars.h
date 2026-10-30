/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\boolpars.h (name assumed from boolpars.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\shared\boolpars.cpp')
**
**	A boolean expression parser: '&&', '||', '!', parentheses, 'true' / 'false' and named functions
**	with up to four arguments (numbers, quoted strings, nested functions), looked up by lower-case
**	name in an SGI hash_map of evaluators (libs/sgistl, D87). Nothing in the retail client calls it.
**	The client names TreeRoot and EvaluatorCallbacks (its asserts) and the arguments param_list, str
**	and p; the classes and the other names are guesses (D10, D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __BOOLPARS_H  // NAME 3 [boolpars.h]: header name assumed from the client's boolpars.cpp
#define __BOOLPARS_H

#include <string>
#include <hash_map.h>

class BoolNodeClass;
class BoolFunctionNodeClass;

struct BoolValueStruct  // NAME 2: describes what it holds
{
	enum {																					// NAME 2: describes what they select
		TYPE_BOOL,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_FUNCTION
	};

	BoolValueStruct(bool value) : Type(TYPE_BOOL) { Bool = value; }
	BoolValueStruct(int value) { Int = value; Type = TYPE_INT; }
	BoolValueStruct(float value) { Float = value; Type = TYPE_FLOAT; }
	BoolValueStruct(char *value) { String = value; Type = TYPE_STRING; }
	BoolValueStruct(BoolFunctionNodeClass *value) { Function = value; Type = TYPE_FUNCTION; }
	~BoolValueStruct();																	// 0x499750

	bool Get_Bool() const;																// 0x4997A0  // NAME 2: describes what it does
	bool Get_Int(int &value) const;														// 0x499920  // NAME 2: describes what it does
	bool Get_Float(float &value) const;													// 0x499A30  // NAME 2: describes what it does
	bool Get_String(char *&value) const;												// 0x499B40  // NAME 2: describes what it does
	bool Get_Function(BoolFunctionNodeClass *&value) const;							// 0x499C50  // NAME 2: describes what it does

	int Type;																				// +0x00  // NAME 2: describes what it holds
	union {
		bool Bool;																			// +0x04  // NAME 2: describes what it holds
		int Int;																			// NAME 2: describes what it holds
		float Float;																		// NAME 2: describes what it holds
		char *String;																		// NAME 2: describes what it holds
		BoolFunctionNodeClass *Function;												// NAME 2: describes what it holds
	};
};

class BoolEvaluatorClass  // NAME 2: what the callbacks do (the client's EvaluatorCallbacks)
{
public:
	virtual BoolValueStruct Evaluate(BoolValueStruct **params, int count) = 0;	// NAME 2: describes what it does
};

typedef std::hash_map<std::string, BoolEvaluatorClass *> BoolEvaluatorMap;		// NAME 2: describes what it holds

class BoolLogicNodeClass;

class BoolNodeClass  // NAME 2: describes what it is (vtable 0xAEA95C)
{
public:
	virtual ~BoolNodeClass() {}																// 0x49B390
	virtual BoolValueStruct Evaluate() = 0;												// NAME 2: describes what it does
	virtual BoolLogicNodeClass *As_Logic() { return NULL; }								// 0x49B370  // NAME 2: describes what it does
	virtual BoolFunctionNodeClass *As_Function() { return NULL; }						// 0x49B1F0  // NAME 2: describes what it does
};

class BoolOperatorClass  // NAME 2: describes what it is (vtable 0xAEA958)
{
public:
	enum {																					// NAME 2: describes what they select
		OP_AND,
		OP_OR,
		OP_NOT
	};

	BoolOperatorClass(int op) : Op(op) {}
	virtual ~BoolOperatorClass() {}														// 0x49B1C0 (deleting)

	bool Evaluate(BoolNodeClass *left, BoolNodeClass *right);						// 0x499E70  // NAME 2: describes what it does

	int Op;																					// +0x04  // NAME 2: describes what it holds
};

class BoolLogicNodeClass : public BoolNodeClass  // NAME 2: describes what it is (vtable 0xAEA944, 0x14 bytes)
{
public:
	BoolLogicNodeClass(int op, BoolNodeClass *left, BoolNodeClass *right) : Operator(op) { Left = left; Right = right; }
	virtual ~BoolLogicNodeClass()															// 0x49B2A0
	{
		if (Left != NULL) {
			delete Left;
			Left = NULL;
		}
		if (Right != NULL) {
			delete Right;
			Right = NULL;
		}
	}
	virtual BoolValueStruct Evaluate() { return BoolValueStruct(Operator.Evaluate(Left, Right)); }	// 0x49B230
	virtual BoolLogicNodeClass *As_Logic() { return this; }								// 0x49B210

	BoolOperatorClass Operator;														// +0x04  // NAME 2: describes what it holds
	BoolNodeClass *Left;																	// +0x0C  // NAME 2: describes what it holds
	BoolNodeClass *Right;																	// +0x10  // NAME 2: describes what it holds
};

class BoolFunctionNodeClass : public BoolNodeClass  // NAME 2: describes what it is (vtable 0xAEA970, 0x14 bytes)
{
public:
	BoolFunctionNodeClass(bool value) { Constant = value; Callback = NULL; Params = NULL; Count = 0; }
	BoolFunctionNodeClass(BoolEvaluatorClass *callback, BoolValueStruct **params, int count) : Callback(callback) { Constant = false; Params = params; Count = count; }
	virtual ~BoolFunctionNodeClass()														// 0x49B4A0
	{
		if (Params != NULL) {
			for (int index = 0; index < Count; index++) {
				delete Params[index];
			}
			delete [] Params;
			Params = NULL;
		}
		Count = 0;
		Callback = NULL;
	}
	virtual BoolValueStruct Evaluate()													// 0x49B420
	{
		if (Callback != NULL) {
			return Callback->Evaluate(Params, Count);
		}
		return BoolValueStruct(Constant);
	}
	virtual BoolFunctionNodeClass *As_Function() { return this; }						// 0x49B400

	bool Constant;																			// +0x04  // NAME 2: describes what it holds
	BoolEvaluatorClass *Callback;															// +0x08  // NAME 2: describes what it holds
	BoolValueStruct **Params;																// +0x0C  // NAME 2: describes what it holds
	int Count;																				// +0x10  // NAME 2: describes what it holds
};

class BoolExpressionClass  // NAME 2: describes what it is (4 bytes, no vtable)
{
public:
	BoolExpressionClass(BoolNodeClass *root) : TreeRoot(root) {}
	~BoolExpressionClass();																// 0x499C80 (no caller)

	bool Evaluate(void *context);														// 0x499CB0  // NAME 2: describes what it does

	BoolNodeClass *TreeRoot;																// +0x00, the client's name (its assert)
};

class BoolParserClass  // NAME 3: by convention (D24) from the client's file boolpars.cpp (vtable 0xAEA938)
{
public:
	BoolParserClass(BoolEvaluatorMap *callbacks);										// 0x49A390
	virtual ~BoolParserClass();															// 0x49A3F0
	void Set_Callbacks(BoolEvaluatorMap *callbacks);									// 0x49A410  // NAME 2: describes what it does
	virtual BoolExpressionClass *Parse(const char *text);								// 0x49A430  // NAME 2: describes what it does

protected:
	BoolNodeClass *Parse_Or(const char **p);											// 0x49A650  // NAME 2: describes what it does
	BoolNodeClass *Parse_Or_Rest(const char **p, BoolNodeClass *left);				// 0x49A7E0  // NAME 2: describes what it does
	BoolNodeClass *Parse_And(const char **p);											// 0x49A900  // NAME 2: describes what it does
	BoolNodeClass *Parse_And_Rest(const char **p, BoolNodeClass *left);				// 0x49A9A0  // NAME 2: describes what it does
	BoolNodeClass *Parse_Factor(const char **p);										// 0x49AA30  // NAME 2: describes what it does
	BoolNodeClass *Parse_Function(const char **p);										// 0x49ACF0  // NAME 2: describes what it does
	int Parse_Params(const char **p, BoolValueStruct **&param_list);				// 0x49AE30  // NAME 2: describes what it does
	BoolValueStruct *Parse_Param(const char **p);										// 0x49AF10  // NAME 2: describes what it does
	BoolNodeClass *Create_Logic(int op, BoolNodeClass *left, BoolNodeClass *right);	// 0x49B170  // NAME 2: describes what it does
	BoolNodeClass *Create_Constant(bool value);											// 0x49B3C0  // NAME 2: describes what it does
	BoolNodeClass *Create_Function(BoolEvaluatorClass *callback, BoolValueStruct **params, int count);	// 0x49B590  // NAME 2: describes what it does
	BoolEvaluatorClass *Get_Callback(char *name);										// 0x49B5E0  // NAME 2: describes what it does

	BoolEvaluatorMap *EvaluatorCallbacks;												// +0x04, the client's name (its assert)
};

#endif

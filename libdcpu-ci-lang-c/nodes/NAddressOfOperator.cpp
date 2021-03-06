/**

	File:			NAddressOfOperator.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NAddressOfOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NAddressOfOperator.h"

AsmBlock* NAddressOfOperator::compile(AsmGenerator& context)
{
	// Just return the address of the value.
	return this->expr.reference(context);
}

AsmBlock* NAddressOfOperator::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an address-of operator.");
}

IType& NAddressOfOperator::getExpressionType(AsmGenerator& context)
{
	// The address of operator has the type of it's expression as a pointer.
	IType& c = this->expr.getExpressionType(context);
	NType t = NType((NType&)c);
	t.pointerCount += 1;
	return t;
}
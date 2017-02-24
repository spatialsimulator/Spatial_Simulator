#ifndef ASTFUNCTION_H_
#define ASTFUNCTION_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <vector>

LIBSBML_CPP_NAMESPACE_USE

void rearrangeAST(ASTNode *ast);

void countAST(ASTNode *ast, unsigned int &numOfASTNode);

void parseAST(ASTNode *ast, reversePolishInfo *rpInfo, std::vector<variableInfo*> &varInfoList, int index_max);

void parseDependence(const ASTNode *ast, std::vector<variableInfo*> &dependence, std::vector<variableInfo*> &varInfoList);

#endif

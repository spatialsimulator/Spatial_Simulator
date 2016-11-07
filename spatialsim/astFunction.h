#ifndef ASTFUNCTION_H_
#define ASTFUNCTION_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <vector>

void rearrangeAST(libsbml::ASTNode *ast);

void countAST(libsbml::ASTNode *ast, int &numOfASTNode);

void parseAST(libsbml::ASTNode *ast, reversePolishInfo *rpInfo, std::vector<variableInfo*> &varInfoList, int index_max);

void parseDependence(const libsbml::ASTNode *ast, std::vector<variableInfo*> &dependence, std::vector<variableInfo*> &varInfoList);

#endif

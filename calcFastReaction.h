#ifndef CALCFASTREACTION_H_
#define CALCFASTREACTION_H_

void calcFastReaction(reactionInfo *rInfo, variableInfo *sInfo, int Xindex, int Yindex, int Zindex, double dt, int m, materialType mType, double stoichimetry);
void replaceOne(ASTNode *ast);

#endif

#ifndef SPATIALSIMULATOR_H_
#define SPATIALSIMULATOR_H_
#ifdef __cplusplus
extern "C" {
#endif

void spatialSimulator(optionList options);
bool isResolvedAll(vector<variableInfo*> &dependence);

#ifdef __cplusplus
}
#endif
#endif

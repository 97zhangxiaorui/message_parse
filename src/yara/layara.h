#ifndef __LAYARA_H__
#define __LAYARA_H__

#include <yara.h>

void InitYara();
bool CompilerRulesFromFile(YR_COMPILER* pCompiler, int nFiles, char** pFiles);
YR_RULES* LoadRules(char **pRuleFile, YR_RULES** pRules);
int YaraResult( YR_SCAN_CONTEXT* context, int message, void* message_data, void* user_data);
void ExitYara();
char* RuleVerification(unsigned char* pBytes, int nBytesLen, YR_RULES* pRules, void *pOutBuffer, int nBufferLen);
char *GetFingerByYara(u_char *pBytes, int nBytesLen);
#endif
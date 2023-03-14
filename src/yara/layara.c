#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "layara.h"

/*
*定义指纹buffer结构体
*/
typedef struct _FingerBuffer_
{
	char *pBuffer;
	int nBufferLen;
}FingerBuffer;

//定义yara规则
YR_RULES* gpRules;

/*
*初始化yara库
*/
void InitYara()
{
	char *pRuleFiles[] = {"../conf/yara/1.yara"};

	gpRules = NULL;
	yr_initialize();

	//获取规则
	LoadRules(pRuleFiles, &gpRules);
}

/*
*加载yara规则文件,仅处理没有编译过的yara文件
*/
bool CompilerRulesFromFile(YR_COMPILER* pCompiler, int nFiles, char** pFiles)
{
	for (int i = 0; i < nFiles - 1; i++)
	{
		FILE* pRuleFile = NULL;
		const char* ns = NULL;
		const char* pFileName = NULL;
		char* colon = NULL;
		int errors;
		printf("file1 :%s \n", pFiles[i]);

		if (access(pFiles[i], 0) != 0)
		{
			// A file with the name specified by the command-line argument wasn't
			// found, it may be because the name is prefixed with a namespace, so
			// lets try to find the colon that separates the namespace from the
			/// actual file name.
			colon = (char*) strchr(pFiles[i], ':');
		}

		// The namespace delimiter must be a colon not followed by a slash or
		// backslash.
		if (colon && *(colon + 1) != '\\' && *(colon + 1) != '/')
		{

			pFileName = colon + 1;
			*colon = '\0';
			ns = pFiles[i];
		}
		else
		{
			pFileName = pFiles[i];
			ns = NULL;
		}

		if (strcmp(pFileName, "-") == 0)
			pRuleFile = stdin;
		else
			pRuleFile = fopen(pFileName, "r");

		if (pRuleFile == NULL)
		{
			fprintf(stderr, "error: could not open file: %s\n", pFileName);
			return false;
		}

		errors = yr_compiler_add_file(pCompiler, pRuleFile, ns, pFileName);

		fclose(pRuleFile);

		if (errors > 0)
		return false;
	}
	return true;
}

/*
*通过yara规则文件获取yara规则
*/
YR_RULES* LoadRules(char **pRuleFile, YR_RULES** pRules)
{
	YR_COMPILER* pCompiler = NULL;
	YR_RULE* pRule = NULL;
	//char* file_name[] = {"1.yara"};

	if (yr_compiler_create(&pCompiler) != ERROR_SUCCESS)
	{
		printf("创建规则编译器失败，请检查\n");
		return NULL;
	}

	if (!CompilerRulesFromFile(pCompiler, 2, pRuleFile)){
		printf("从规则文件读取规则失败,请检查规则文件是否正确\n");
		return NULL;
	}

	if (yr_compiler_get_rules(pCompiler, pRules) != ERROR_SUCCESS)
	{
		yr_compiler_destroy(pCompiler);
		perror("编译规则失败，请检查规则是否正确\n");
		return EXIT_FAILURE;
	}

	yr_compiler_destroy(pCompiler); 

	//
	/*
	yr_rules_foreach(pRules, pRule)
	{
		printf("RULE NAME:%s \n", pRule->identifier);
	}
	*/

	return *pRules;
}

/*
*校验结果回调函数
*/
int YaraResult( YR_SCAN_CONTEXT* context, int message, void* message_data, void* user_data)
{	
	switch (message)
	{
		case CALLBACK_MSG_RULE_MATCHING:
			snprintf(((FingerBuffer*) user_data)->pBuffer, ((FingerBuffer*) user_data)->nBufferLen -1,"%s", ((YR_RULE*)message_data)->identifier);
			break;
		case CALLBACK_MSG_RULE_NOT_MATCHING:
			//printf("no match rule name : %s \n", ((YR_RULE*)message_data)->identifier);
			break;
	}
	return CALLBACK_CONTINUE;
}

/*
*释放yara库
*/
void ExitYara()
{
	yr_finalize();
}

/*
*规则校验
*/
char* RuleVerification(unsigned char* pBytes, int nBytesLen, YR_RULES* pRules, void *pOutBuffer, int nBufferLen)
{
	FingerBuffer Result;
	int nError = 0;

	Result.pBuffer = (char*)pOutBuffer; //指纹结构体
	Result.nBufferLen = nBufferLen;

	nError = yr_rules_scan_mem(pRules, pBytes, nBytesLen, 0, YaraResult, &Result, 0);
	if (nError != ERROR_SUCCESS)
	{
		fprintf(stderr, "test_save_load_rules: error: %d\n", nError);
		return NULL;
	}
	return pOutBuffer;
}

/*
*根据规则名字，查找资产指纹
*/
char * GetFIngerByRule(char *pDeviceFinger, char *pRuleName)
//char * GetFIngerByRule(char *pDeviceFinger, char *pRuleName)
{
	char *pTmpFinger = NULL;
	if ( pTmpFinger = GetFingerprintByName(pRuleName) ){
		pDeviceFinger = pTmpFinger;
		return pDeviceFinger;
	}
	return NULL;
}

/*
*根据规则文件，判断字节流指纹信息
*/
char *GetFingerByYara(u_char *pBytes, int nBytesLen)
{
	char TmpBuffer[128] = { 0 };

	if ( !gpRules ){
		printf("yara规则为空，不进行校验\n");
		return NULL;
	}

	RuleVerification(pBytes, nBytesLen, gpRules, TmpBuffer, sizeof(TmpBuffer));
	if ( strlen(TmpBuffer) > 0 )
	{
		//通过规则名字，获取指纹信息
		char *pTmpFinger = NULL;
		if ( pTmpFinger = GetFingerprintByName(TmpBuffer) ){
			return pTmpFinger;
		}
	}

	//
	return NULL;
}
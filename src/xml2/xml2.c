#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpointer.h>

#include "xml2.h"

xmlXPathObjectPtr GetNodeSet(xmlDocPtr pDoc,const xmlChar *pXpath)
{
        xmlXPathContextPtr pContext=NULL;//XPath上下文指针
        xmlXPathObjectPtr pResult=NULL; //XPath结果指针

        pContext = xmlXPathNewContext(pDoc);
        if(pDoc==NULL)
        {
                printf("pDoc is NULL\n");
                return NULL;
        }

        if(pXpath)
        {
                if (pContext == NULL)
                {
                        printf("context is NULL\n");
                        return NULL;
                }

                pResult = xmlXPathEvalExpression(pXpath, pContext);
                xmlXPathFreeContext(pContext); //释放上下文指针
                if (pResult == NULL)
                {
                        printf("xmlXPathEvalExpression return NULL\n");
                        return NULL;
                }

                if (xmlXPathNodeSetIsEmpty(pResult->nodesetval))
                {
                        xmlXPathFreeObject(pResult);
                        printf("nodeset is empty\n");
                        return NULL;
                }
        }

        return pResult;
}

xmlDocPtr ReadXmlFile(char *pFileName)
{
	xmlDocPtr pXmlDoc = NULL;
	//xmlNodePtr pRootNode = NULL;

	//printf("xmlFile name : %s \n", pFileName);

	if ( NULL != pFileName )
	{
		//必须加上，防止程序把元素前后的空白文本符号当作一个node
		xmlKeepBlanksDefault(0);

		//libxml只能解析UTF-8格式数据
		pXmlDoc = xmlReadFile (pFileName, "UTF-8", XML_PARSE_RECOVER);
		if ( NULL != pXmlDoc )
		{
			//printf("Open xml file %s sucessful!!\n", pFileName);
			return pXmlDoc;
		}
		else
		{
			printf("Open xml file failed, please check\n");
			return NULL;
		}
		return pXmlDoc;
	}
	else
	{
		printf("File name is null, open xml file failed , please check\n");
		return NULL;
	}
}

char * GetXmlNodeValueByXpath(xmlDocPtr pXmlDoc, char *pXmlPath, unsigned int nValueNuber, char *pValueBuf)
{
	if ( NULL == pXmlDoc || NULL == pXmlPath || NULL == pValueBuf )
	{
		printf("The param pxmlDoc or pXmlPath or pValueBuf of function GetXmlNodeValueByXpath is null, please check !!\n");
		return NULL;
	}

	xmlChar *pXpath = BAD_CAST(pXmlPath);
	xmlXPathObjectPtr pXmlPathObj = GetNodeSet(pXmlDoc, pXpath);
	if ( NULL != pXmlPathObj )
	{
		//xmlNodeSetPtr pNodeSet = pXmlPathObj->nodesetval;
		xmlNodeSetPtr pXmlNodeSet = pXmlPathObj->nodesetval;

		if ( pXmlNodeSet->nodeNr < nValueNuber )
		{
			printf("The param nValueNuber is greater than the number of values array, please check xml file and param !!");
			return NULL;
		}

		//do not check pValueBuf size
		if ( NULL != pXmlNodeSet->nodeTab[nValueNuber-1]->xmlChildrenNode )
		{
			int nContentLen = strlen(pXmlNodeSet->nodeTab[nValueNuber-1]->xmlChildrenNode->content);
			memcpy(pValueBuf, pXmlNodeSet->nodeTab[nValueNuber-1]->xmlChildrenNode->content, nContentLen);
		}
		xmlXPathFreeObject(pXmlPathObj);
		return pValueBuf;

	}
	else
	{
		printf("No find value by fuction GetNodeSet, please check xml path !!\n");
		return NULL;
	}
}

char * GetXmlNodeAttrValueByXpath(xmlDocPtr pXmlDoc, char *pXmlPath, unsigned int nValueNuber, char *pValueBuf)
{
	if ( NULL == pXmlDoc || NULL == pXmlPath || NULL == pValueBuf )
	{
		printf("The param pxmlDoc or pXmlPath or pValueBuf of function GetXmlNodeAttrValueByXpath is null, please check !!\n");
		return NULL;
	}

	xmlChar *pXpath = BAD_CAST(pXmlPath);
	xmlXPathObjectPtr pXmlPathObj = GetNodeSet(pXmlDoc, pXpath);
	if ( NULL != pXmlPathObj )
	{
		xmlNodeSetPtr pXmlNodeSet = pXmlPathObj->nodesetval;
		if ( pXmlNodeSet->nodeNr < nValueNuber )
		{
			printf("The param nValueNuber is greater than the number of values array, please check xml file and param !!");
			return NULL;
		}

		xmlAttrPtr pXmlAttr = pXmlPathObj->nodesetval->nodeTab[0]->properties;
		xmlAttrPtr pTmpXmlAttr = NULL;

		int nNum = 0;
		for ( ; nNum < nValueNuber, NULL != pXmlAttr; nNum++ )
		{
			pTmpXmlAttr = pXmlAttr;
			pXmlAttr = pXmlAttr->next;
		}
		//do not check pValueBuf size
		if ( NULL != pTmpXmlAttr )
		{
			strcpy(pValueBuf, pTmpXmlAttr->children->content);
		}
		xmlXPathFreeObject(pXmlPathObj);

		return pValueBuf;
	}
	else
	{
		printf("No find value by fuction getNodeset, please check xml path !!\n");
		return NULL;
	}

}

void DestroyXml(xmlDocPtr pXmlDoc)
{
	if ( NULL != pXmlDoc )
	{
		xmlFreeDoc (pXmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
		return;
	}
}
#ifndef __XML2_H_
#define __XML2_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpointer.h>

/*
*pFileName: file name
*return   : null or xml handle
*/
xmlDocPtr ReadXmlFile(char *pFileName);

/*
*pXmlDoc    : xml file handle
*pXmlPat    : xml node path
*nValueNuber: The number is key of the array values; begin 1,2...
*pValueBuf  : save value, must be initialized and large enough
*return     : node value or NULL
*/
char * GetXmlNodeValueByXpath(xmlDocPtr pXmlDoc, char *pXmlPath, unsigned int nValueNuber, char *pValueBuf);

/*
*pXmlDoc    : xml file handle
*pXmlPat    : xml node path
*nValueNuber: The number is key of the array attrs; begin 1,2...
*pValueBuf  : save value, must be initialized and large enough
*return     : node attr value or NULL
*/
char * GetXmlNodeAttrValueByXpath(xmlDocPtr pXmlDoc, char *pXmlPath, unsigned int nValueNuber, char *pValueBuf);

/*
*pXmlDoc: Xml file handle
*/
void DestroyXml(xmlDocPtr pXmlDoc);

/*
*
*/
xmlXPathObjectPtr GetNodeSet(xmlDocPtr pDoc,const xmlChar *pXpath);

#endif

#include <windows.h>
#include "docsDefine.h"
#include "removeStopWord.h"
#ifndef __LOAD__
#define __LOAD__
/**
* @auther LiYuqian IDC HUST WuHan
* @email  yuqianfly@gmail.com
*/

//输出docs信息
extern "C"
void printHostDocs(char *docName,HostDocs * docs){
	 printf("\n## Output: %s infor\n",docName);	
	 printf("** Notice: if size==0,the read failed.Please check the char_code\n");
	 printf("DocCount =%d\n",docs->DocCount); 
	 long total=0;
	 for(unsigned int i=0;i<docs->DocCount;i++)
	 {	
		total=total+docs->DocStreamSize[i];
		printf("docId= %d,size= %d words\n",i,docs->DocStreamSize[i]);
	    //printf("%ws\n",docs->DocStream[i]);
	 }
	 printf("total size=%ld words\n",total);
	 printf("\n");
}

//输出doc全部token信息
extern "C"
void printHostDocsTotalTokens(char *outputName,HostDocsTotalTokens * hdtTokens){
	 printf("## Output: %s \n",outputName);
	
	 for(int i=0;i<hdtTokens->threadsNum;i++)
		printf("s\n",hdtTokens->ThreadsTokens[i]);
	printf("\n");
}

extern "C"
void memcpyUS2W(wchar_t *dest, unsigned short *src)
{
   int i=0;
   while(src[i]){
	    i++;
		dest[i]=src[i];
	}
}

extern "C"
void writeDocsTotalTokens(char * outputName,char * outputFile,HostDocsTotalTokens *hdtTokens){
	FILE *fp;
	if((fp=fopen(outputFile,"w"))==NULL){
		  printf("Cannot open file: %s!",outputFile);
          getchar();
          exit(1);
	}
	int num=hdtTokens->threadsNum;
	fprintf(fp,"## Output: %s\n",outputName); 
	fprintf(fp,"total threads = %d\n",num);
	wchar_t buffer[MAX_TOKEN_PER＿THREAD];
	for(int i=0;i<num;i++){
	    memset(buffer,'\0',sizeof(wchar_t)*MAX_TOKEN_PER＿THREAD);	
		memcpyUS2W(buffer,hdtTokens->ThreadsTokens[i]);
		fprintf(fp,"thread %4d: %ws\n",i,(wchar_t*)hdtTokens->ThreadsTokens[i]); 
	}
		fclose(fp);
}

//加载docs到CPU内存: please notice the char code!!!!
extern "C"
HostDocs * loadBatchDocs(char * inputFold){
    printf("start load docs...\n");
	char *fmode=(char*)malloc(sizeof(char)*20);
	strcpy(fmode,inputFold);
	strcat(fmode,"*.txt");
	printf("fmode =%s\n",fmode);
   
	HostDocs *hdocs=(HostDocs*)malloc(sizeof(HostDocs));
	hdocs->DocCount=0;
	for(int i=0;i<DOC_BATCH_SIZE;i++)
	{	
	    hdocs->DocStream[i]=0;
	    hdocs->DocStreamSize[i]=0;
	}

	char *buffer=(char*)calloc(MAX_DOC_SIZE,sizeof(char));
	//int readBlock=1024,blockTimes=10;
 
	HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
	int docid=0;
	int wnum=MAX_DOC_SIZE/2;
	wchar_t *wstrTemp=(wchar_t*)malloc(sizeof(wchar_t)*wnum);
    setlocale(LC_ALL,".936");
	char *fname=(char*)malloc(sizeof(char)*40);
	if((hFind=FindFirstFileA(fmode,&FindFileData)) != INVALID_HANDLE_VALUE)
    {//文件路径自己更改               
        BOOL bFind = TRUE;    
        while(bFind)
        {                    	   
		   memset(fname,0,sizeof(char)*40);
		   //char buffer[1024];
		   strcpy(fname,inputFold);
		   strcat(fname,FindFileData.cFileName);
		   //printf("----------------\n");
		   printf("%s\n",fname);	
		   FILE *fp;
	       if((fp=fopen(fname,"r"))==NULL){
		       printf("Cannot open file: %s!",fname);
               getchar();
               exit(1);
	       }
		   memset(buffer,'\0',sizeof(char)*MAX_DOC_SIZE);
		   //while(fgets())
		   char buf[1024];		   
		   while(fgets(buf,1024,fp) != NULL){
		        //fputs(buf,stdout);
                strcat(buffer,buf);
		   }			  		   
		   Trim(buffer);	
		   //printf("%s\n",buffer);
		   memset(wstrTemp,'\0',sizeof(wchar_t)*wnum);
	       mbstowcs(wstrTemp,buffer,wnum);

		   int realNum=wcslen(wstrTemp);
           hdocs->DocStreamSize[docid]=realNum;
		   hdocs->DocStream[docid]=(unsigned short *)malloc(sizeof(unsigned short)*(realNum));
		   memset(hdocs->DocStream[docid],0,sizeof(unsigned short)*(realNum));
		   memcpy(hdocs->DocStream[docid],wstrTemp,sizeof(wchar_t)*realNum);
           docid++;
           fclose(fp);	
           
           bFind = FindNextFile(hFind,&FindFileData);
           
        }
    }	
	hdocs->DocCount=docid;		
    printf("End of load patch docs to CPU memory\n");
	free(wstrTemp);
	free(buffer);
	free(fmode);
	return hdocs;
}
#endif
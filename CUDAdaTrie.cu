/********************************************************************
*@version-0.1 
*@author Liyuqian-李雨前 yuqianfly@gmail.com 
*华中科技大学计算机学院 智能与分布式计算实验室
*注意: 除华中科技大学计算机学院智能与分布式计算外，
*任何个人、团队、研究结构、商业单位等不能对本算法进行专利申请或者撰写
*本算法的论文。
*任何个人、团队、研究结构、商业单位都可以对本算法进行使用、修改、扩展、传播。
*使用本算法不当造成的损失，责任有使用者自行负责。
* 
* 使用提示：
*     1。本词典详细构造没有公开，有疑问请与作者联系
*     2。随代码附带词典信息与代码具有很强的相关系性，任何不正确修改都可能导致
* 分词异常。
*     3。使用本代码过程中，带来的损失，作者一概不负责
*     4。在启动运行前，需要足够的堆栈空间，例如10240000bytes
*     5。优化版本和 非优化版本，在编译时，选择一个就行，不支持同时编译
*     6。三种分词可以选择对应不同的输出目录。
*     7。其他未尽描述的问题，请与作者联系。
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cutil_inline.h>
#include <cutil.h>
#include <string.h>
#include <locale.h>

#include "dLoadTrie.h"
#include "loadDocs.h"


#define WORD_SIZE  30
#define WWORD_NUM   15

#ifndef __GLOBALVAR__
#define __GLOBALVAR__

__device__  unsigned  char d_Status[318609];//全局变量
__device__   int          d_Check[318608];  //全局变量
__device__  unsigned int  d_Base[318608];  //全局变量
__device__  unsigned int  d_CharsHash[65535];

#endif
#if __DEVICE_EMULATION__
bool InitCUDA(void){return true;}
#else
bool InitCUDA(void)
{
	int count = 0;
	int i = 0;

	cudaGetDeviceCount(&count);
	if(count == 0) {
		fprintf(stderr, "There is no device.\n");
		return false;
	}

	for(i = 0; i < count; i++) {
		cudaDeviceProp prop;
		if(cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
			if(prop.major >= 1) {
				break;
			}
		}
	}
	if(i == count) {
		fprintf(stderr, "There is no device supporting CUDA.\n");
		return false;
	}
	cudaSetDevice(i);

	printf("CUDA initialized.\n");
	return true;
}
#endif

/************************************************************************/
/* 三种分词实现                                                              */
/************************************************************************/
/**正向全匹配分词*/
__device__ void gGetAllWords(unsigned short *w_chars,int posFrom,int posTo,unsigned short *output){   
	int outputIndex=0;
	int t=0,i=posFrom,start=posFrom,end=0,charHashCode=0;
	unsigned char stats='0';
 		
    int  baseValue = 0;
    int  checkValue = 0;
	for (; i <posTo; i++) {
     	end++;
		charHashCode = d_CharsHash[w_chars[i]];
        if( charHashCode<1 ) stats='0';
		else{
		    checkValue=baseValue;
		    baseValue = d_Base[checkValue] + charHashCode;
		    if (d_Check[baseValue] == checkValue || d_Check[baseValue] == -1)
			    stats= d_Status[baseValue];
		    else
			    stats='0';
		}

		switch (stats) {
			case '0':	
				i = start;
				start++;				
				end = 0;
				baseValue = 0;
				break;

			case '2':
				for(t=0;t<end;t++){
					 output[outputIndex++]=w_chars[t+start];
				}
                output[outputIndex++]=49;
				break;
			case '3':
				for(t=0;t<end;t++){
					output[outputIndex++]=w_chars[t+start];	
			    }
				 output[outputIndex++]=49;
				 i = start;
				 start++;
				 end = 0;
				 baseValue = 0;				
				 break;
			}//end of switch		
	}//end of for			
}


/**正向最大匹配分词*/
__device__ void gMaxFrontWords(unsigned short * w_chars,int posFrom,int posTo,unsigned short * output) {
	int outputIndex=0;

	int t=0,i=posFrom,start=posFrom,end=0,charHashCode=0;
	unsigned char stats='0';
 		
    int  tempEnd = 0;
    int  baseValue = 0;
    int  checkValue = 0;
    bool hasEnd = false;
    int wlen=posTo-posFrom;
	for(;i<posTo;i++){
    	end++;
		charHashCode = d_CharsHash[w_chars[i]];
		if( charHashCode<1 ) stats='0';
		else{
             checkValue=baseValue;
		     baseValue = d_Base[checkValue] + charHashCode;
		     if (d_Check[baseValue] == checkValue || d_Check[baseValue] == -1)
			    stats= d_Status[baseValue];
		     else
			    stats='0';
		}

		switch (stats) {
		case '0':
			if (hasEnd) {
				for(t=0;t<tempEnd;t++){
					output[outputIndex++]=w_chars[t+start];
				}
				output[outputIndex++]=49;
				hasEnd = false;
				baseValue = 0;
				start = start + tempEnd ;
				i = start-1;
				tempEnd = 0;
				end = 0;				
				break;
			} else {
				baseValue = 0;
				tempEnd = 0;
				i = start;
				start++;				
				end = 0;
			}
			break;
		case '2':
			tempEnd = end;
			hasEnd = true;
			break;
		case '3':
			for(t=0;t<end;t++){
				output[outputIndex++]=w_chars[t+start];			
			}
			output[outputIndex++]=49;//代表字符1           		
			hasEnd = false;
			baseValue = 0;
			tempEnd = 0;
			start = i ;
			end = 0;				
			break;
		}
		if (i == wlen - 1) {
			if (hasEnd) {
				for(t=0;t<tempEnd;t++){
					output[outputIndex++]=w_chars[t+start];
				}
				output[outputIndex++]=49;	
				hasEnd = false;
				baseValue = 0;
				start = start + tempEnd;
				i = start-1;
				tempEnd = 0;
				end = 0;
				break;
					
			}
		}
	}
}

/**正向最小匹配分词*/
__device__ void gMinFrontWords(unsigned short * w_chars,int posFrom,int posTo,unsigned short * output){
  	
    int outputIndex=0;
	int t=0,i=posFrom,start=posFrom,end=0,charHashCode=0;
	unsigned char stats='0';
 		
    int  baseValue = 0;
    int  checkValue = 0;

	for (; i < posTo; i++) {
		end++;
		charHashCode = d_CharsHash[w_chars[i]];       

        if( charHashCode<1 ) stats='0';
		else{
        checkValue=baseValue;
		baseValue = d_Base[checkValue] + charHashCode;
		if (d_Check[baseValue] == checkValue || d_Check[baseValue] == -1)
			stats= d_Status[baseValue];
		else
			stats='0';
		}
		switch (stats) {
			case '0':
				baseValue = 0;
				i = start;
				start++;
				end = 0;
				break;
			case '2':
				for(t=0;t<end;t++)	{
					output[outputIndex++]=w_chars[t+start];
				}
				output[outputIndex++]=49;
				baseValue = 0;
				start = i+1;
				end = 0;
				break;
			case '3':
				for(t=0;t<end;t++){
					output[outputIndex++]=w_chars[t+start];
				}
				output[outputIndex++]=49;
				baseValue = 0;
				start = i+1;
				end = 0;
				break;
			}
		}
}

/**内核入口函数
* 功能：进行批量文档分词
* 文档按线程数平均划分，每个文档对应一个block
*/
__global__  void gBatchSearchKernel(HostDocs * inputDocs,HostDocsTotalTokens *outputTokens){ 	
	int bid=blockIdx.x; //块全局id
	int tid=blockIdx.x*blockDim.x+threadIdx.x;//线程全局id
	int docsize=inputDocs->DocStreamSize[bid];//快对应文档大小
	int average=docsize/blockDim.x;//每个线程数据
	int start=threadIdx.x*average;//包括端点
	int end=start+average;//不包括端点
	//gGetAllWords(inputDocs->DocStream[bid],start,end,outputTokens->ThreadsTokens[tid]);
    //gMaxFrontWords(inputDocs->DocStream[bid],start,end,outputTokens->ThreadsTokens[tid]);
    gMinFrontWords(inputDocs->DocStream[bid],start,end,outputTokens->ThreadsTokens[tid]);
}

/**test load doc*/
__global__ void testLoad(HostDocs * inputDocs,unsigned short * writeadoc){   
	for(int i=0;i<100000;i++)   
	  writeadoc[i]=inputDocs->DocStream[1][i];
}


/**
这个版本功能：
根据加载的文档个数，启动相应的block数，每个block分配TREAD_PER_BLOCK线程
分词结果按照thread单位保存，即 block_num* TREAD_PER_BLOCK 个数组元素，
每个元素长度MAX_TOKEN_PER＿THREAD==100 即每个线程分词结果最大100个中文字
*/
void runCUDADATrie(char * inputFold,char * outputFold){  
	// make double trie
	if( h_initCUDADATrie())
	   printf("InitCUDADAtrie success.\n\n");
    else
	   printf("*** initCUDADATrie failed!\n\n");
	
	//从文件夹inputFold加载批量文档，测试用例，不要超过批量上限DOC_BATCH_SIZE==96
	 HostDocs *hdocs = loadBatchDocs(inputFold);
     printHostDocs("docs",hdocs);

	 printf("\nCopy docs to GPU...\n");
	 HostDocs *ddocs;
	 unsigned short **CPU_ARRAY;
	 CPU_ARRAY =(unsigned short **)malloc(sizeof(unsigned short*)*DOC_BATCH_SIZE);
	 memset(CPU_ARRAY,0,sizeof(unsigned short*)*DOC_BATCH_SIZE);

	 int docSize=0,docsNum=hdocs->DocCount;
	 for(int i=0;i<docsNum;i++){
		 docSize=hdocs->DocStreamSize[i];
         cutilSafeCall( cudaMalloc((void **)&CPU_ARRAY[i],sizeof(unsigned short)*docSize));
         cutilSafeCall( cudaMemset(CPU_ARRAY[i],0,sizeof(unsigned short)*(docSize)));
         cutilSafeCall( cudaMemcpy(CPU_ARRAY[i],hdocs->DocStream[i],sizeof(unsigned short)*docSize,cudaMemcpyHostToDevice));
	 }   
	cutilSafeCall(cudaMalloc( (void**)&ddocs,sizeof(HostDocs)));
	cutilSafeCall(cudaMemcpy(ddocs->DocStream,CPU_ARRAY,sizeof(unsigned short*)*DOC_BATCH_SIZE,cudaMemcpyHostToDevice));
	cutilSafeCall(cudaMemcpy(ddocs->DocStreamSize,hdocs->DocStreamSize,sizeof(unsigned short)*DOC_BATCH_SIZE,cudaMemcpyHostToDevice));
	printf("End of copy\n\n");
     
	//printHostDocs("d_docs test",bdocs);
    	 
	 //cpu端接受内核输出结果
	HostDocsTotalTokens *hDocAllTokens;
	int tokensTotalMemSize=TOTAL_THREADS_NUM*MAX_TOKEN_PER＿THREAD;//128*96*100
    hDocAllTokens=(HostDocsTotalTokens*)malloc(sizeof(HostDocsTotalTokens));
	hDocAllTokens->threadsNum=0;
	memset(hDocAllTokens->ThreadsTokens,0,sizeof(unsigned short)*tokensTotalMemSize);
	 
	 //内核输出结果
	HostDocsTotalTokens *dDocAllTokens;
    CUDA_SAFE_CALL(cudaMalloc( (void**)&dDocAllTokens,sizeof(HostDocsTotalTokens)));
	int tNum=docsNum*TREAD_PER_BLOCK;//全部线程数目2*128
	cutilSafeCall(cudaMemcpy( &dDocAllTokens->threadsNum,&tNum,sizeof(unsigned short),cudaMemcpyHostToDevice));
	cutilSafeCall(cudaMemset( dDocAllTokens->ThreadsTokens,0,sizeof(unsigned short)*tokensTotalMemSize));
	
	int blockNum=docsNum;//工作线程块数目
	int threadsPerBlock=TREAD_PER_BLOCK;//每个线程块里面的线程个数
    
	dim3 dimBlock(threadsPerBlock,1,1);
	dim3 dimGrid(blockNum,1);
    printf("start kernel...\n");
    unsigned int timer = 0;
    cutilCheckError( cutCreateTimer( &timer));
    cutilCheckError( cutStartTimer( timer));
//工作内核
/**test load code*/
/*
unsigned short *writeDoc;
size_t docMemSize=sizeof(unsigned short)*MAX_DOC_SIZE;
cutilSafeCall(cudaMalloc((void**)&writeDoc,docMemSize));
cutilSafeCall(cudaMemset(writeDoc,0,docMemSize));
	
unsigned short *readDoc;
readDoc=(unsigned short*)malloc(docMemSize);
memset(readDoc,0,docMemSize);

printf("init..\n");
for(int i=0;i<10;i++)
    printf("%4d: %wc\n",i,readDoc[i]);
*/

	gBatchSearchKernel<<<dimGrid,dimBlock>>>(ddocs,dDocAllTokens); 
//testLoad<<<1,1>>>(ddocs,writeDoc);
	cutilCheckMsg("Kernel execution failed\n");	
	cudaThreadSynchronize();
	
    cutilCheckError( cutStopTimer( timer));
    printf("Kernel processing time: %f (ms)\n", cutGetTimerValue( timer));
    cutilCheckError( cutDeleteTimer( timer));
    printf("end of kernel\n");

//test load code
/*  
cutilSafeCall(cudaMemcpy(readDoc,writeDoc,docMemSize,cudaMemcpyDeviceToHost));
printf("the contrent:\n");
for(int i=0;i<10;i++)
   printf("%4d : %wc\n",i,readDoc[i]);
printf("%ws\n",readDoc);
*/

	cutilSafeCall(cudaMemcpy(hDocAllTokens,dDocAllTokens,sizeof(HostDocsTotalTokens),cudaMemcpyDeviceToHost));
	writeDocsTotalTokens("keneal docs total tokens: minWords",outputFold,hDocAllTokens);

	//释放资源
    free(hdocs);
    free(hDocAllTokens);
	cutilSafeCall(cudaFree(ddocs));
	cutilSafeCall(cudaFree(dDocAllTokens));
    /*
	cutilSafeCall(cudaFree(d_Base));
	cutilSafeCall(cudaFree(d_Check));
	cutilSafeCall(cudaFree(d_Status));
	cutilSafeCall(cudaFree(d_CharsHash));
	*/
}

/*
int main(int argc, char* argv[])
{
	if(!InitCUDA()) {
		return 0;
	} 
   
	char *console="outputFiles/minWords_log_48_64.txt";
    freopen(console,"w",stdout); //输出重定向，输出数据将保存在out.txt文件中 
    time_t timep;
    time (&timep);
	printf("------------------------\n");
    printf("%s\n",ctime(&timep));
	char * inputFold="inputFiles/48/";
	char * outputFold="outputFiles/minWords_48_64.txt";
    runCUDADATrie(inputFold,outputFold);

	time (&timep);	
	printf("%s\n",ctime(&timep));
    printf("------------------------\n");
    fclose(stdout);//关闭文件 

	CUT_EXIT(argc, argv);
	
    fclose(stdout);//关闭文件 
	return 0;
}
*/
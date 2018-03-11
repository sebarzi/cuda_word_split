#include <cuda_runtime.h>
#include <cutil.h>
#include "InitDictionary.h"
/**
* @auther LiYuqian IDC HUST WuHan
* @email  yuqianfly@gmail.com
*/

/*IF global variable is define as
__device__  int check[318608]; 
__device__  int base[318608];
__device__  unsigned char status[318609];
__device__  int charsHash[65535];
 then can use kernel to initia.

 If define as
_device__   int * check; //全局变量
__device__  int * base;//全局变量
__device__  unsigned char * status
__device__ int  *charsHash
 then can use initialGloabalVariable_Point to initlia
*/
#ifndef __INITCUDADATRIE__
#define __INITCUDADATRIE__

void initialGloabalVariable_Point(int *h_base,
							int *h_check,unsigned char *h_status,
							int h_bsize,
							char *h_charsHash,int h_hsize){ 

	unsigned int  * d_base;
	         int  * d_check;
	unsigned char * d_status;
	unsigned int  * d_charsHash;

	extern  unsigned int  d_Base[318608];  //全局变量
    extern   int  d_Check[318608]; //全局变量
    extern  unsigned char  d_Status[318609];
	extern  unsigned int  d_CharsHash[65535];

    int baseMemSize=sizeof(unsigned int)*h_bsize;
	int statusMemSize=sizeof(unsigned char)*(h_bsize+1);

	//global base
    CUDA_SAFE_CALL(cudaMalloc( (void**)&d_base,baseMemSize));
	CUDA_SAFE_CALL(cudaMemset(d_base,0,baseMemSize));
	CUDA_SAFE_CALL(cudaMemcpy(d_base,h_base,baseMemSize,cudaMemcpyHostToDevice));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol(d_Base,&d_base,sizeof(unsigned int*),0,cudaMemcpyHostToDevice));
    
	//global check
    CUDA_SAFE_CALL(cudaMalloc( (void**)&d_check,baseMemSize));
	CUDA_SAFE_CALL(cudaMemset(d_check,0,baseMemSize));
	CUDA_SAFE_CALL( cudaMemcpy(d_check,h_check,baseMemSize,cudaMemcpyHostToDevice));
	CUDA_SAFE_CALL( cudaMemcpyToSymbol(d_Check,&d_check,sizeof(unsigned int*),0,cudaMemcpyHostToDevice));

    //global status
    CUDA_SAFE_CALL(cudaMalloc( (void**)&d_status,statusMemSize));
	CUDA_SAFE_CALL(cudaMemset(d_status,0,statusMemSize));
	CUDA_SAFE_CALL( cudaMemcpy(d_status,h_status,statusMemSize,cudaMemcpyHostToDevice));
	CUDA_SAFE_CALL( cudaMemcpyToSymbol(d_Status,&d_status,sizeof(unsigned int*),0,cudaMemcpyHostToDevice));

//host data copy to device ,then copy device address to global device variable
//so the sizeof(int*) is right,while sizeof(int)*bcsize is wrong

    //global charsHash
	CUDA_SAFE_CALL(cudaMalloc( (void**)&d_charsHash,sizeof(char)*(h_hsize+1)));
    CUDA_SAFE_CALL(cudaMemset(d_charsHash,'\0',sizeof(unsigned char)*(h_hsize+1)));		
	CUDA_SAFE_CALL(cudaMemcpy(d_charsHash,h_charsHash,sizeof(char)*(h_hsize),cudaMemcpyHostToDevice));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol(d_CharsHash,&d_charsHash,sizeof(char*),0,cudaMemcpyHostToDevice));

}


/**initial the global Variable method.*/
__global__ void copyTrieKernel(unsigned int *d_base, int *d_check,
							   unsigned char * d_status,unsigned int d_bsize,
							   unsigned int* d_charsHash,unsigned int d_hsize){
   
	extern  unsigned int  d_Base[318608];  //全局变量
    extern           int  d_Check[318608]; //全局变量
    extern  unsigned char  d_Status[318609];
	extern  unsigned int  d_CharsHash[65535];
	
	unsigned int i=0;
	for(i=0;i< d_bsize;i++)
	{
		d_Base[i]  = d_base[i];
	    d_Check[i] = d_check[i];
		d_Status[i]= d_status[i];
	}
	d_Status[d_bsize]='\0';
	
	for(i=0;i< d_hsize;i++)
		d_CharsHash[i]=d_charsHash[i];
}

void initialGloabalVariable_Copy(unsigned int *h_base,
		          int *h_check,
		unsigned char *h_status,
		unsigned int h_bsize,
		unsigned int *h_charsHash,
		unsigned int h_hsize){

    unsigned int  * d_base;
	         int  * d_check;
	unsigned char * d_status;
	unsigned int  * d_charsHash;

	//全局变量
	extern  unsigned int  d_Base[318608];  
    extern           int  d_Check[318608];
    extern  unsigned char  d_Status[318609];
	extern  unsigned int  d_CharsHash[65535];

    unsigned int baseMemSize=sizeof(unsigned int)*h_bsize;
	unsigned int statusMemSize=sizeof(unsigned char)*(h_bsize+1);
    unsigned int charsHashMemSize=sizeof(unsigned int)*65535;

	//global base
    CUDA_SAFE_CALL(cudaMalloc( (void**)&d_base,baseMemSize));
	CUDA_SAFE_CALL(cudaMemset(d_base,0,baseMemSize));
	CUDA_SAFE_CALL(cudaMemcpy(d_base,h_base,baseMemSize,cudaMemcpyHostToDevice));
  
	//global check
    CUDA_SAFE_CALL(cudaMalloc( (void**)&d_check,baseMemSize));
	CUDA_SAFE_CALL(cudaMemset(d_check,0,baseMemSize));
	CUDA_SAFE_CALL( cudaMemcpy(d_check,h_check,baseMemSize,cudaMemcpyHostToDevice));
	
    //global status
    CUDA_SAFE_CALL(cudaMalloc( (void**)&d_status,statusMemSize));
	CUDA_SAFE_CALL(cudaMemset(d_status,0,statusMemSize));
	CUDA_SAFE_CALL( cudaMemcpy(d_status,h_status,statusMemSize,cudaMemcpyHostToDevice));
	
    //global charsHash
	CUDA_SAFE_CALL(cudaMalloc( (void**)&d_charsHash,charsHashMemSize));
    CUDA_SAFE_CALL(cudaMemset(d_charsHash,'\0',charsHashMemSize));		
	CUDA_SAFE_CALL(cudaMemcpy(d_charsHash,h_charsHash,charsHashMemSize,cudaMemcpyHostToDevice));

	copyTrieKernel<<<1,1>>>(d_base,d_check,d_status,h_bsize,d_charsHash,h_hsize);
	
	CUDA_SAFE_CALL( cudaFree( d_base));
	CUDA_SAFE_CALL( cudaFree( d_check));
	CUDA_SAFE_CALL( cudaFree( d_status));
    CUDA_SAFE_CALL( cudaFree( d_charsHash));
}


bool h_initCUDADATrie(){
   	
	 unsigned int *h_base;
	          int *h_check;
     unsigned char *h_status; 
	 unsigned int *h_charsHash; 

	 InitDictionary initDic;	
     h_base = initDic.base;
	 h_check = initDic.check;
	 h_status = initDic.status;     //按位处理	
	 h_charsHash = initDic.charsHash;
   
	 initialGloabalVariable_Copy(h_base,h_check,h_status,318608,h_charsHash,65535);
     //printf("End of load double trie to GPU!\n\n");
	 return true;
	}
#endif
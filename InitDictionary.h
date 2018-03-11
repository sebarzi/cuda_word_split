#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
/**
* @auther LiYuqian IDC HUST WuHan
* @email  yuqianfly@gmail.com
*/

/**notice when run these program must need large stack */
#ifndef __DICTIONARY__
#define __DICTIONARY__
struct InitDictionary{
    public:
	// base: 数组用来存放单词的转换..其实就是一个DFA转换过程 
	unsigned int *base; //实际240094个词条,空间有效率 240094/318608=0.75357
	
	// check: 数组用来验证里面存储的是上一个状态的位置	 
    int *check;	
	
	// status: 用来判断一个单词的状态 1.为不成词.处于过度阶段 2.成词也可能是词语的一部分. 3.词语结束
	// example: 中 1 中华 2 中华人 1 中华人民 3	
	unsigned char *status;
	
	// * charsHash: 汉字编码数组.对4000多个常用汉字进行了重新编码.
	unsigned int *charsHash; //实际6143 6142/65535=0.093736  
		 
    public:
		   char  haspath[40];
           char arraysPath[40];
           char charEncoding[8];
           bool isInit;
    public:
	       void initArrays();
	       void initCharsHash();
	       void init();
	
InitDictionary(){	
	base=(unsigned int*)malloc(sizeof(unsigned int)*318608);
	memset(base,0,sizeof(unsigned int)*318608);
	check=(int*)malloc(sizeof(int)*318608);
	memset(check,0,sizeof(int)*318608);
    status=(unsigned char*)malloc(sizeof(unsigned char)*318609);
	memset(status,0,sizeof(unsigned char)*318609);
	charsHash=(unsigned int*)malloc(sizeof(unsigned int)*65535);
	memset(charsHash,0,sizeof(unsigned int)*65535);

    strcpy(haspath,"library/charHash.dic");
    strcpy(arraysPath,"library/arrays_modify2.dic");
	strcpy(charEncoding,"GBK");
	isInit = false;
	
	init();
	
  }
};

void InitDictionary::init() {
	if (!isInit) {
	    double start = (double)clock();
		printf("InitCharsHash...\n");	
	    initCharsHash();
		printf("InitArrays...\n");
		initArrays();
			//printf("End of initing\n");
		isInit = true;
		double end=(double)clock();
		printf("词典加载完成用时:%10.4lf 毫秒\n\n",end-start );			
	}
}

//  and arraysPath="library/arrays_modify2.dic" ;
void InitDictionary::initArrays(){	

	FILE *farrayp;
	if((farrayp=fopen(arraysPath,"r"))==NULL){
		printf("Cannot open file: %s!",arraysPath);
        getchar();
        exit(1);
	}
     
    int i=0;
	int num=0,bs_value=0,ck_value=0;
	unsigned char status_value='a';
    for(i=0;i<240094;i++) { 
	    fscanf(farrayp,"%d %d %d %c\n",&num,&bs_value,&ck_value,&status_value);	
		base[num]   = bs_value;
		check[num]  = ck_value;
		status[num] =status_value;		
	}
	status[318608]='\0';

	printf("total line: %d\n",i);
	if(fclose(farrayp)==0)
		printf("file: %s close is success!\n",arraysPath);
	else
		printf("*** !! file: %s close failed\n",arraysPath);

}

	/**
	 * 汉字编码词典的加载
	 * 一	19968	2
	 * 19968对应汉字 一的unicode编码值，在java中汉字一 看做一个字符 在c中等于两个字符
	 * 
	 */

void InitDictionary::initCharsHash(){	
	FILE *fhp;
	if((fhp=fopen(haspath,"r"))==NULL){
		printf("Cannot open file: %s!",haspath);
        getchar();
        exit(1);
	}
      
    unsigned int i=0,j=0,n=0;
    char aword[4];
    for(i=0;i<6143;i++) {   
	     fscanf(fhp,"%s %d %d \n",aword,&j,&n);              	
		 charsHash[j] = n;			    			
	}
	printf("total line: %d\n",i); 
    if(fclose(fhp)==0)
		printf("file：%s close ok!\n",haspath);
	else
		printf("*** !!! file：%s close failed!\n",haspath);
	
}
#endif 
/*
int main(int argc, char *argv[])
{   
	InitDictionary initDic; 	
    return 0;
}
*/
/**
* @auther LiYuqian IDC HUST WuHan
* @email  yuqianfly@gmail.com
* This code just for string operator which is common on CPU
* you can add your own method
*/

/**compare two string */
__device__  int d_strcmp ( const char* src, const char* dst )
{
    int ret = 0 ;
    while(!(ret = *(unsigned char *)src - *(unsigned char *)dst) && *dst)
	{
	  ++src; ++dst;
	}
   if ( ret < 0 )
      ret = -1 ;
   else if ( ret > 0 )
      ret = 1 ;
   return( ret );
}
/**strcat*/
__device__ void d_strcat(char * dst,char *src){
     while(*dst)
     dst++;                     // find end of dst //  
     while( *dst++ = *src++ ); // Copy src to end of dst //
}

/**wchar len*/
__device__ int d_wcslen(unsigned short *dst){	
	int num=0;
	while(*dst!=0){
		num++;
		dst++;
	}
	return num;
}

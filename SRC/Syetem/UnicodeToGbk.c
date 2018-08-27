
#include "unicodeTables.h"
#include "gbkTables.h"

//gbk->unicode
/*int GbkToUnicode(char *gbk_buf, unsigned short *unicode_buf, int max_unicode_buf_size)
{
    unsigned short word;
    unsigned char *gbk_ptr =(unsigned char *) gbk_buf;
    unsigned short *uni_ptr = unicode_buf;
	unsigned int uni_ind = 0, gbk_ind = 0, uni_num = 0;
	unsigned char ch;
    int word_pos;

    if( !gbk_buf || !unicode_buf )
        return -1;

    while(1)
    {
    	ch = *(gbk_ptr + gbk_ind);

		if(ch == 0x00)
			break;
		
        if( ch > 0x80 )
        {
			//将gbk编码的中文字符的两个字节组合成一个    unsigned short word;
				word = *(gbk_ptr + gbk_ind);
				word <<= 8;
				word += *(gbk_ptr + gbk_ind+1);
				gbk_ind += 2;

            word_pos = word - gbk_first_code;
            if(word >= gbk_first_code && word <= gbk_last_code  && (word_pos < unicode_buf_size))
            {
				*(uni_ptr + uni_ind) = unicodeTables[word_pos];
				uni_ind++;
				uni_num++;
            }
        }
		else
		{
			gbk_ind++;
			*(uni_ptr + uni_ind) = ch;
			uni_ind++;
			uni_num++;
        }
        
        if(uni_num > max_unicode_buf_size - 1)
			break;
    }

    return uni_num;
}*/

//unicode->gbk
int UnicodeToGbk(unsigned short *unicode_buf, char *gbk_buf, int max_gbk_buf_size)
{
  unsigned short word;
  unsigned short gbk_word;
  //unsigned char ch;
  unsigned char *gbk_ptr =(unsigned char *) gbk_buf;
  unsigned short *uni_ptr = unicode_buf;
  unsigned int uni_ind = 0, gbk_ind = 0, gbk_num = 0;
  int word_pos;

    if( !gbk_buf || !unicode_buf )
        return -1;

	while(1)
	{
		word = *(uni_ptr + uni_ind);
		uni_ind++;

		if(word == 0x0000)  //字符串结束符
			break;
		
		if(word < 0x80)  /*ASCII不用查表*/
		{
			*(gbk_ptr + gbk_ind) = (unsigned char)word;
			gbk_ind++;
		}
		else
		{
			word_pos = word - unicode_first_code;
			if(word >= unicode_first_code && word <= unicode_last_code && word_pos < gbk_buf_size)
			{
				gbk_word = gbkTables[word_pos];//gbk_word是gbk编码，但是为unsigned short类型，需要拆分成两个字节

				*(gbk_ptr + gbk_ind) = (unsigned char)(gbk_word >> 8);//提取高8位
				gbk_ind++;
				*(gbk_ptr + gbk_ind) = (unsigned char)(gbk_word >> 0);//提取低8位
				gbk_ind++;
				gbk_num +=2;//gbk字符加2个
			}
		}

		if(gbk_num > max_gbk_buf_size - 1)
			break;
	}

    return gbk_num;
}

/*int main()
{
	////1、gbk->unicode
	//char *strGbk ="我";
	//unsigned short strUnicode[5] ={0x00};
	//
	// int num = GbkToUnicode(strGbk,strUnicode,5);  
 //
 //   unsigned char* p = (unsigned char*)strUnicode;     
 //   for (int i = 0; i < num*2; i++)  
 //   {  
 //       printf("%0x", *p);  
 //       p++;  
 //   }//输出1162 
 //   printf("\n");  

	//2、unicode->gbk
	unsigned short strUnicode[2] = {0x6211,0x00};//加上0x00字符串结束符,终止函数内部的循环
	char strGbk[5]={0x00};

	int num = UnicodeToGbk(strUnicode,strGbk,5);

    unsigned char* p = (unsigned char*)strGbk;     
    for (int i = 0; i < num; i++)  
    {  
        printf("%0x", *p);  
        p++;  
    }//输出ced2 
    printf("\n");  
     return 0;
}
*/
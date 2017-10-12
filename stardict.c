#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>                                     
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/**
 * Date: 2015/11/24 11:25
 * 
 */

const int MAX_WORD=256;
const int MAX_KEYS=26;
const char *KEY[]={
   "a","b","c","d","e","f","g",
   "h","i","j","k","l","m","n",
   "o","p","q","r","s","t","u",
   "v","w","x","y","z"
};

FILE *pidx; /* idx文件指针 */
FILE *pdict; /* dict文件指针 */

/* 
 * 从idx文件中获取当前目标单词
 * data_poffset用来保存单词的data偏移位置信息
 * data_plength用来保存单词的data长度信息 
 */
int get_word(char* word_buf, unsigned int * data_poffset, unsigned int * data_plength)
{
   int len=0;
   int i;
   while( (word_buf[len++]=getc(pidx)) && (len<MAX_WORD) );

   char* poff=(char*)data_poffset;
   char* plen=(char*)data_plength;

   for(i=sizeof(unsigned int)-1; i>=0; i--)
      poff[i]=getc(pidx);

   for(i=sizeof(unsigned int)-1; i>=0; i--)
      plen[i]=getc(pidx);
   return --len;
}

/* 
 * 通过偏移位置offset和长度length
 * 来从dict文件中获取data内容，并返回data
 */
char* get_data(unsigned int offset, unsigned int length)
{
   char * data_buf=(char*)malloc(length+1);
   if(data_buf==NULL){
      fprintf(stderr,"Memory malloc error!\n");
      exit(1);
   }
   fseek(pdict, offset, SEEK_SET);
   fread(data_buf, length, 1, pdict);
   data_buf[length]='\0';

   return data_buf;
}

/* 
 * 显示data内容
 */
void display_data(char *data_buf, unsigned int data_length)
{
   fwrite(data_buf,data_length,1,stdout);
}

/*
 * 从idx文件中搜索由word指定的单词，并保存相应的偏移和长度信息。
 * 若搜索成功，返回1; 否则返回0
 */
int search_word(const char* word, unsigned int* data_poffset, unsigned int* data_plength)
{
   //int wd_len;
   char wd[MAX_WORD];
   for(get_word(wd, data_poffset, data_plength); !feof(pidx); 
         get_word(wd, data_poffset, data_plength)){
      if(strcasecmp(word,wd)==0) return 1;
   }
   return 0;  
}


/*
 * 从标准输入获取待查询的单词
 */
char* get_input(char *input_buf, int max_len)
{
   int len;
   fgets(input_buf, max_len, stdin);

   if( input_buf && (len=strlen(input_buf))>1){
      input_buf[len-1]='\0';
      return input_buf;
   }
   else return NULL;
}

/*
 * 缓存KEYS在idx中的偏移信息，以便加快search_word的搜索速度
 */
void cache_idx(long *idx_cache)
{
   int i;
   long *p=idx_cache;
   unsigned int unused;

   rewind(pidx);
   for(i=0; i<MAX_KEYS; i++){
      if( search_word(KEY[i], &unused, &unused)){
         p[i]=ftell(pidx);
      }else p[i]=0;
   }
}

/*
 * 定位由word指定的单词在idx文件中的大概偏移位置
 */
long locate_idx(char * word, long* idx_cache)
{
   int i=0;
   int pre=0;

   while( i<MAX_KEYS && KEY[i][0]<tolower(word[0]) ){
      pre=i;
      ++i;
   }
   return idx_cache[pre];
}


/*
 * 主要查询函数
 */
void consult(char *word, char *dict_name)
{
   char *data=NULL;
   long idx[MAX_KEYS];
   unsigned int offset, length;

   cache_idx(idx);

   fseek(pidx,locate_idx(word,idx),SEEK_SET);
   if(search_word(word,&offset,&length))
   {
      printf("\n\n[Dict: %s]\n\n", dict_name);
      data=get_data(offset,length);
      display_data(data,length);
      free(data);
      data=NULL;
   }
}

void search(char *dict_index ,char *dict_name, char *word)
{
   pidx=fopen(dict_index,"rb");
   //perror(dict_index);
   pdict=fopen(dict_name,"rb");
   //perror(dict_name);
   if(pidx==NULL || pdict==NULL) {
      printf("%s open dictionary failed.\n", dict_name);
      return;
   }

   consult(word,dict_name);

   fclose(pidx);
   fclose(pdict);
}

struct file_name_ext
{
   char file_name[40];
   char file_ext[6];
};

struct file_name_ext  pfne;
void get_file_name_ext(const char *file_name)
{
   memset(pfne.file_name, '\0', strlen(pfne.file_name));
   memset(pfne.file_ext, '\0', strlen(pfne.file_ext));

   if(strcmp(file_name, ".") == 0 || strcmp(file_name, "..")==0)
   {
      return;
   }

   char *p = rindex(file_name, '.');
   if( p != NULL && file_name < p )
   {
      strncpy(pfne.file_name, file_name, strlen(file_name) - strlen(p));
      strncpy(pfne.file_ext, p, strlen(p));
   }
}

char * get_dict(const char * file_name)
{
   get_file_name_ext(file_name);
   if(strcmp(pfne.file_ext, ".dict") == 0)
      return pfne.file_name;
   return NULL;
}

int index_of(const char *s, const char *sub)
{
   if(s == NULL || sub == NULL)
   {
      return -2;
   }

   char *p=NULL;
   if((p=strstr(s,sub)) != NULL)
   {
      return strlen(s)-strlen(p);
   }
   else
   {
      return -1;
   }
}

/*                                                         
 * 主函数
 */
int main(int argc, char **argv)
{
   char buf[120];
   memset(buf,'\0',120);

   if( argc != 2 )
   {
      printf("Usage: %s word\n", argv[0]);
      exit(1);
   }

   chdir("dicts");
   DIR *pdir;
   struct dirent *file;

   char buf_name[80];
   char buf_index[80];

   if(!(pdir = opendir(".")))
   {
      printf("can not opendir");
      return -1;
   }

   while((file = readdir(pdir)) != NULL)
   {
     
      char *p = get_dict(file->d_name);
      if(p != NULL)
      {
         memset(buf_name,'\0',strlen(buf_name));
         memset(buf_index,'\0',strlen(buf_index));

         strcat(buf_index,p);
         strcat(buf_index,".idx");

         strcat(buf_name,p);
         strcat(buf_name,".dict");

         search(buf_index, buf_name, argv[1]);
      }
   }
   return 0;
}

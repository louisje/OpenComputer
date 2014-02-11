#include "Assembler.h"                                          // 引用組譯器檔頭                
#include "Compiler.h"                                           // 引用編譯器檔頭                

#define TEST 0
#define C0C  1
#define AS0  2
#define VM0  3
#define M0   4

void argError(char *msg) {                                      // 處理參數錯誤的情況            
  printf("%s\n", msg);                                                                        
  exit(1);                                                                                    
}                                                                                             
                                                                                              
int main(int argc, char *argv[]) {                              // 主程式開始                    
  char cFile0[]="test.c0", *cFile=cFile0;                       //  預設程式檔為 test.c0         
  char asmFile0[]="test.asm0", *asmFile=asmFile0;               //  預設組合語言為test.asm0      
  char objFile0[]="test.obj0", *objFile=objFile0;               //  預設目的檔為 test.obj0       
  char osAsmFile0[]="os0.asm0", *osAsmFile=osAsmFile0;
  char osObjFile0[]="os0.obj0", *osObjFile=osObjFile0;
#if TARGET==TEST  
                                              // 如果編譯目標為 TEST           
//  ArrayTest();                                                  //   測試陣列物件                
//  system("pause");
//  HashTableTest();                                              //   測試雜湊表物件              
//  OpTableTest();                                                //   測試指令表物件              
    printf("cFile=%s asmFile=%s\n", cFile, asmFile);
    compile(cFile, asmFile);                                      //   測試編譯器                  
    system("pause");
    assemble(asmFile, objFile);                                   //   測試組譯器                  
    system("pause");
    execute(objFile, 0);                                          //   測試虛擬機器    

/*
  assemble(osAsmFile, osObjFile);
  system("pause");
  M0execute(osObjFile);
*/
  system("pause");
  checkMemory();                                                //   檢查記憶體使用狀況          
#elif TARGET==C0C                                               // 如果編譯目標為 C0C            
  if (argc == 3) {                                              //  如果有 3 個參數             
    cFile=argv[1]; asmFile=argv[2];                             //    設定參數                  
  } else                                                        //  否則                        
    argError("c0c <c0File> <asmFile>");                         //    提示程式執行方法          
  compile(cFile, asmFile);                                      //  開始編譯                    
#elif TARGET==AS0                                               // 如果編譯目標為 AS0            
  if (argc == 3) {                                              //  如果有 3 個參數             
    asmFile=argv[1]; objFile=argv[2];                           //    設定參數                  
  } else                                                        //  否則                        
    argError("as0 <asmFile> <objFile>");                        //    提示程式執行方法          
  assemble(asmFile, objFile);                                   //  開始組譯                    
#elif TARGET==VM0                                               // 如果編譯目標為 VM0            
  if (argc == 2)                                                //  如果有 2 個參數             
    objFile=argv[1];                                            //    設定參數                  
  else                                                          //  否則                        
    argError("vm0 <objFile>");                                  //    提示程式執行方法          
  execute(objFile, 0);                                          //  開始執行 (虛擬機)           
#elif TARGET==M0                                               // 如果編譯目標為 M0
  if (argc == 2)
    objFile = argv[1];
  else
    objFile = osObjFile0;
  M0execute(objFile);
#endif                                                                                        
  return 0;
}

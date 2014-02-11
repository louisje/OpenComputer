#include "Assembler.h"                                          // 引用組譯器檔頭                
#include "Compiler.h"                                           // 引用編譯器檔頭                

#define TEST        1
#define CC          2
#define CI          3
#define AS          4
#define VM          5
#define OS          6

void argError(char *msg) {                                      // 處理參數錯誤的情況            
  printf("%s\n", msg);                                                                        
  exit(1);                                                                                    
}                                                                                             
                                                                                              
int main(int argc, char *argv[]) {                              // 主程式開始                    
	char cFile1[]="test.c1", *cFile=cFile1;                       //  預設程式檔為 test.c1
	char asmFile1[]="test.asm1", *asmFile=asmFile1;               //  預設組合語言為 test.asm1
	char objFile1[]="test.obj1", *objFile=objFile1;               //  預設目的檔為 test.obj1
	char osAsmFile1[]="os0.asm1", *osAsmFile=osAsmFile1;
	char osObjFile1[]="os0.obj1", *osObjFile=osObjFile1;
#if TARGET==TEST  
//  LibTest();
//	ArrayTest();                                                  //   測試陣列物件
//	HashTableTest();                                              //   測試雜湊表物件
//	OpTableTest();                                                //   測試指令表物件
//  AsmTest();
//	VmTest();
//	ScannerTest("test.c1");
//	SymTableTest();
	ParserTest("test.c1");
//    OsTest();
#elif TARGET==CI
//	printf("interpret:cFile=%s\n", cFile);
//	interpret(cFile);
#elif TARGET==CC                                               // 如果編譯目標為 C0C            
  if (argc == 3) {                                              //  如果有 3 個參數             
    cFile=argv[1]; asmFile=argv[2];                             //    設定參數                  
  } else                                                        //  否則                        
		argError("cc1 <c0File> <asmFile>");                         //    提示程式執行方法          
  compile(cFile, asmFile);                                      //  開始編譯                    
#elif TARGET==AS                                               // 如果編譯目標為 AS0            
  if (argc == 3) {                                              //  如果有 3 個參數             
    asmFile=argv[1]; objFile=argv[2];                           //    設定參數                  
  } else                                                        //  否則                        
		argError("as1 <asmFile> <objFile>");                        //    提示程式執行方法          
  assemble(asmFile, objFile);                                   //  開始組譯                    
#elif TARGET==VM                                               // 如果編譯目標為 VM0            
    int irqTimes = 0;
	if (argc >= 2) {                                               //  如果有 2 個參數
		objFile=argv[1];                                            //    設定參數
		if (argc >=3) irqTimes=atoi(argv[2]);                     //    設定每幾個指令中斷一次 
	} else                                                          //  否則                        
		argError("vm1 <objFile> <irqTimes>");                                  //    提示程式執行方法
	execute(objFile, irqTimes);                                //  開始執行 (虛擬機)           
#elif TARGET==OS                                               // 如果編譯目標為 OS
  if (argc == 2)                                                //  如果有 2 個參數             
    objFile=argv[1];                                            //    設定參數                  
  else                                                          //  否則                        
		argError("os1 <objFile>");                                  //    提示程式執行方法          
	execute(objFile, 10);                                          //  開始執行 (虛擬機)           
#elif TARGET==CI                                               // 如果編譯目標為 M0
  if (argc == 2)
		interpret(cFile);
#endif                                                                                        
    system("pause");
  return 0;
}

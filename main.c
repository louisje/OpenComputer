#include "Assembler.h"                                          // �ޥβ�Ķ�����Y                
#include "Compiler.h"                                           // �ޥνsĶ�����Y                

#define TEST 0
#define C0C  1
#define AS0  2
#define VM0  3
#define M0   4

void argError(char *msg) {                                      // �B�z�Ѽƿ��~�����p            
  printf("%s\n", msg);                                                                        
  exit(1);                                                                                    
}                                                                                             
                                                                                              
int main(int argc, char *argv[]) {                              // �D�{���}�l                    
  char cFile0[]="test.c0", *cFile=cFile0;                       //  �w�]�{���ɬ� test.c0         
  char asmFile0[]="test.asm0", *asmFile=asmFile0;               //  �w�]�զX�y����test.asm0      
  char objFile0[]="test.obj0", *objFile=objFile0;               //  �w�]�ت��ɬ� test.obj0       
  char osAsmFile0[]="os0.asm0", *osAsmFile=osAsmFile0;
  char osObjFile0[]="os0.obj0", *osObjFile=osObjFile0;
#if TARGET==TEST  
                                              // �p�G�sĶ�ؼЬ� TEST           
//  ArrayTest();                                                  //   ���հ}�C����                
//  system("pause");
//  HashTableTest();                                              //   �����������              
//  OpTableTest();                                                //   ���ի��O����              
    printf("cFile=%s asmFile=%s\n", cFile, asmFile);
    compile(cFile, asmFile);                                      //   ���սsĶ��                  
    system("pause");
    assemble(asmFile, objFile);                                   //   ���ղ�Ķ��                  
    system("pause");
    execute(objFile, 0);                                          //   ���յ�������    

/*
  assemble(osAsmFile, osObjFile);
  system("pause");
  M0execute(osObjFile);
*/
  system("pause");
  checkMemory();                                                //   �ˬd�O����ϥΪ��p          
#elif TARGET==C0C                                               // �p�G�sĶ�ؼЬ� C0C            
  if (argc == 3) {                                              //  �p�G�� 3 �ӰѼ�             
    cFile=argv[1]; asmFile=argv[2];                             //    �]�w�Ѽ�                  
  } else                                                        //  �_�h                        
    argError("c0c <c0File> <asmFile>");                         //    ���ܵ{�������k          
  compile(cFile, asmFile);                                      //  �}�l�sĶ                    
#elif TARGET==AS0                                               // �p�G�sĶ�ؼЬ� AS0            
  if (argc == 3) {                                              //  �p�G�� 3 �ӰѼ�             
    asmFile=argv[1]; objFile=argv[2];                           //    �]�w�Ѽ�                  
  } else                                                        //  �_�h                        
    argError("as0 <asmFile> <objFile>");                        //    ���ܵ{�������k          
  assemble(asmFile, objFile);                                   //  �}�l��Ķ                    
#elif TARGET==VM0                                               // �p�G�sĶ�ؼЬ� VM0            
  if (argc == 2)                                                //  �p�G�� 2 �ӰѼ�             
    objFile=argv[1];                                            //    �]�w�Ѽ�                  
  else                                                          //  �_�h                        
    argError("vm0 <objFile>");                                  //    ���ܵ{�������k          
  execute(objFile, 0);                                          //  �}�l���� (������)           
#elif TARGET==M0                                               // �p�G�sĶ�ؼЬ� M0
  if (argc == 2)
    objFile = argv[1];
  else
    objFile = osObjFile0;
  M0execute(objFile);
#endif                                                                                        
  return 0;
}

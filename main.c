#include "Assembler.h"                                          // �ޥβ�Ķ�����Y                
#include "Compiler.h"                                           // �ޥνsĶ�����Y                

#define TEST        1
#define CC          2
#define CI          3
#define AS          4
#define VM          5
#define OS          6

void argError(char *msg) {                                      // �B�z�Ѽƿ��~�����p            
  printf("%s\n", msg);                                                                        
  exit(1);                                                                                    
}                                                                                             
                                                                                              
int main(int argc, char *argv[]) {                              // �D�{���}�l                    
	char cFile1[]="test.c1", *cFile=cFile1;                       //  �w�]�{���ɬ� test.c1
	char asmFile1[]="test.asm1", *asmFile=asmFile1;               //  �w�]�զX�y���� test.asm1
	char objFile1[]="test.obj1", *objFile=objFile1;               //  �w�]�ت��ɬ� test.obj1
	char osAsmFile1[]="os0.asm1", *osAsmFile=osAsmFile1;
	char osObjFile1[]="os0.obj1", *osObjFile=osObjFile1;
#if TARGET==TEST  
//  LibTest();
//	ArrayTest();                                                  //   ���հ}�C����
//	HashTableTest();                                              //   �����������
//	OpTableTest();                                                //   ���ի��O����
//  AsmTest();
//	VmTest();
//	ScannerTest("test.c1");
//	SymTableTest();
	ParserTest("test.c1");
//    OsTest();
#elif TARGET==CI
//	printf("interpret:cFile=%s\n", cFile);
//	interpret(cFile);
#elif TARGET==CC                                               // �p�G�sĶ�ؼЬ� C0C            
  if (argc == 3) {                                              //  �p�G�� 3 �ӰѼ�             
    cFile=argv[1]; asmFile=argv[2];                             //    �]�w�Ѽ�                  
  } else                                                        //  �_�h                        
		argError("cc1 <c0File> <asmFile>");                         //    ���ܵ{�������k          
  compile(cFile, asmFile);                                      //  �}�l�sĶ                    
#elif TARGET==AS                                               // �p�G�sĶ�ؼЬ� AS0            
  if (argc == 3) {                                              //  �p�G�� 3 �ӰѼ�             
    asmFile=argv[1]; objFile=argv[2];                           //    �]�w�Ѽ�                  
  } else                                                        //  �_�h                        
		argError("as1 <asmFile> <objFile>");                        //    ���ܵ{�������k          
  assemble(asmFile, objFile);                                   //  �}�l��Ķ                    
#elif TARGET==VM                                               // �p�G�sĶ�ؼЬ� VM0            
    int irqTimes = 0;
	if (argc >= 2) {                                               //  �p�G�� 2 �ӰѼ�
		objFile=argv[1];                                            //    �]�w�Ѽ�
		if (argc >=3) irqTimes=atoi(argv[2]);                     //    �]�w�C�X�ӫ��O���_�@�� 
	} else                                                          //  �_�h                        
		argError("vm1 <objFile> <irqTimes>");                                  //    ���ܵ{�������k
	execute(objFile, irqTimes);                                //  �}�l���� (������)           
#elif TARGET==OS                                               // �p�G�sĶ�ؼЬ� OS
  if (argc == 2)                                                //  �p�G�� 2 �ӰѼ�             
    objFile=argv[1];                                            //    �]�w�Ѽ�                  
  else                                                          //  �_�h                        
		argError("os1 <objFile>");                                  //    ���ܵ{�������k          
	execute(objFile, 10);                                          //  �}�l���� (������)           
#elif TARGET==CI                                               // �p�G�sĶ�ؼЬ� M0
  if (argc == 2)
		interpret(cFile);
#endif                                                                                        
    system("pause");
  return 0;
}

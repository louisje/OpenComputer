// Terminal Token
DEF(kIF,"if")           // ����r if 
DEF(kELSE ,"else")      // ����r else
DEF(kFOR ,"for")        // ����r for
DEF(kWHILE ,"while")    // ����r while
DEF(kRETURN ,"return")  // ����r return
DEF(kINT ,"int")        // �򥻫��A int 
DEF(kBYTE ,"byte")      // �򥻫��A byte
DEF(kCHAR ,"char")      // �򥻫��A char
DEF(kFLOAT ,"float")    // �򥻫��A float
DEF(kSTR ,"str")        // �򥻫��A str
DEF(kSTRUCT ,"struct")  // ����r struct 
DEF(sLBRACE ,"{")       // �Ÿ� { 
DEF(sRBRACE ,"}")       // �Ÿ� }
DEF(sLPAREN ,"(")       // �Ÿ� ( 
DEF(sRPAREN ,")")       // �Ÿ� ) 
DEF(sLSQUARE ,"[")      // �Ÿ� [
DEF(sRSQUARE ,"]")      // �Ÿ� ] 
DEF(sSEMI ,";")         // �Ÿ� ;
DEF(sCOMMA ,",")        // �Ÿ� ,
DEF(sARROW ,"->")       // �Ÿ� -> 
DEF(sDOT ,".")          // �Ÿ� .
DEF(sLT ,"<")           // �Ÿ� <
DEF(sGT ,">")           // �Ÿ� > 
DEF(sASSIGN ,"=")       // �Ÿ� =
DEF(sLE ,"<=")          // �Ÿ� <=
DEF(sGE ,">=")          // �Ÿ� >=
DEF(sEQ ,"==")          // �Ÿ� ==
DEF(sNE ,"!=")          // �Ÿ� !=
DEF(sLAND ,"&&")        // �Ÿ� &&
DEF(sLOR ,"||")         // �Ÿ� ||
DEF(sNOT ,"!")          // �Ÿ� !
DEF(sADD ,"+")          // �Ÿ� +
DEF(sSUB ,"-")          // �Ÿ� - 
DEF(sSTAR, "*")         // �Ÿ� *
DEF(sDIV ,"/")          // �Ÿ� / 
DEF(sMOD ,"%")          // �Ÿ� % 
DEF(sSHL ,">>")         // �Ÿ� >>
DEF(sSHR ,"<<")         // �Ÿ� <<
DEF(sBAND ,"&")         // �Ÿ� &
DEF(sBOR ,"|")          // �Ÿ� |
DEF(sDPLUS ,"++")       // �Ÿ� ++
DEF(sDMINUS ,"--")      // �Ÿ� -- 
DEF(kEND ,"_?END?_")    // �ɮײפ�Ÿ� 
DEF(CINT ,"CINT")       // ��Ʊ`�ơA�Ҧp�G 374 
DEF(CFLOAT ,"CFLOAT")   // �B�I�`�ơA�Ҧp�G 3.14
DEF(CSTR ,"CSTR")       // �r��`�ơA�Ҧp�G"Hello!" 
DEF(ID ,"ID")           // �ܼƦW�١GID = [A-Za-z_][0-9A-Za-z_]*
DEF(OP1 ,"OP1")         // �@���B��GOP1 = ++ | --
// DEF(OP2, "+|-|/|*|%|&|&&|^|<<|>>|<|>|<=|>=|==|!=|||||||")

// �ŧi�D�׺ݶ��ئr��GNonterminal Node
DEF(PROG ,"PROG")           // �{���GPROG = (STRUCT | METHOD | DECL ; )*
DEF(METHOD ,"METHOD")       // ��ơGMETHOD = TYPE ** ID(PARAM_LIST?) BLOCK
DEF(STRUCT ,"STRUCT")       // ���c�GSTRUCT = struct ID { DECL_LIST ; }
DEF(BLOCK ,"BLOCK")         // �϶��GBLOCK = { BASE* }
DEF(BASE ,"BASE")           // �򥻡GBASE
DEF(IF ,"IF")               // �P�_�GIF = if (EXP) BASE (else BASE)?
DEF(FOR ,"FOR")             // �j��GFOR = for (STMT ; EXP ; STMT) BASE
DEF(WHILE ,"WHILE")         // �j��GWHILE = while (EXP) BASE
DEF(STMT ,"STMT")           // ���z�GSTMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1 
DEF(VAR ,"VAR")             // �ܼơGVAR = ** ID ([ integer ])* (= EXP)? 
DEF(EXP ,"EXP")             // �B��GEXP = TERM (OP2 TERM)?
DEF(TERM ,"TERM")           // ���ءGTERM = ( EXP (OP2 EXP)? ) | CINT | CFLOAT | CSTR | PATH
DEF(PATH ,"PATH")           // ���|�GPATH = ATOM ((.|->) ATOM)*
DEF(ATOM ,"ATOM")           // �򤸡GATOM = ID (([ EXP ])* |( EXP_LIST? ))
DEF(CALL ,"CALL")           // �I�s�GID (( EXP_LIST? ))
DEF(ARRAY_MEMBER ,"ARRAY_MEMBER") // �}�C�����GID ([ EXP ])*
DEF(DECL ,"DECL")           // �ŧi�GDECL = TYPE VAR_LIST
DEF(PARAM ,"PARAM")         // �ѼơGPARAM = TYPE VAR
DEF(TYPE ,"TYPE")           // ���A (int|float|ID) 
DEF(DECL_LIST ,"DECL_LIST") // ��C�GDECL_LIST = DECL (; DECL)* 
DEF(PARAM_LIST ,"PARAM_LIST")//��C�GPARAM_LIST = PARAM (, PARAM)*
DEF(EXP_LIST ,"EXP_LIST")   // ��C�GEXP_LIST = EXP (, EXP)*
DEF(VAR_LIST ,"VAR_LIST")   // ��C�GVAR_LIST = VAR (, VAR)*
DEF(BTYPE, "BTYPE")         // �򥻫��A�G(int|float|...) 
DEF(ETYPE, "ETYPE")         // �򥻫��A�G(int|float|...) �[�W Structure ���A 
DEF(GLOBAL, "GLOBAL")       // ����϶��G.global 
// DEF(sCOLON ,":")
// DEF(sMUL ,"*") // �Ÿ� *

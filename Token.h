// Terminal Token
DEF(kIF,"if")           // 關鍵字 if 
DEF(kELSE ,"else")      // 關鍵字 else
DEF(kFOR ,"for")        // 關鍵字 for
DEF(kWHILE ,"while")    // 關鍵字 while
DEF(kRETURN ,"return")  // 關鍵字 return
DEF(kINT ,"int")        // 基本型態 int 
DEF(kBYTE ,"byte")      // 基本型態 byte
DEF(kCHAR ,"char")      // 基本型態 char
DEF(kFLOAT ,"float")    // 基本型態 float
DEF(kSTR ,"str")        // 基本型態 str
DEF(kSTRUCT ,"struct")  // 關鍵字 struct 
DEF(sLBRACE ,"{")       // 符號 { 
DEF(sRBRACE ,"}")       // 符號 }
DEF(sLPAREN ,"(")       // 符號 ( 
DEF(sRPAREN ,")")       // 符號 ) 
DEF(sLSQUARE ,"[")      // 符號 [
DEF(sRSQUARE ,"]")      // 符號 ] 
DEF(sSEMI ,";")         // 符號 ;
DEF(sCOMMA ,",")        // 符號 ,
DEF(sARROW ,"->")       // 符號 -> 
DEF(sDOT ,".")          // 符號 .
DEF(sLT ,"<")           // 符號 <
DEF(sGT ,">")           // 符號 > 
DEF(sASSIGN ,"=")       // 符號 =
DEF(sLE ,"<=")          // 符號 <=
DEF(sGE ,">=")          // 符號 >=
DEF(sEQ ,"==")          // 符號 ==
DEF(sNE ,"!=")          // 符號 !=
DEF(sLAND ,"&&")        // 符號 &&
DEF(sLOR ,"||")         // 符號 ||
DEF(sNOT ,"!")          // 符號 !
DEF(sADD ,"+")          // 符號 +
DEF(sSUB ,"-")          // 符號 - 
DEF(sSTAR, "*")         // 符號 *
DEF(sDIV ,"/")          // 符號 / 
DEF(sMOD ,"%")          // 符號 % 
DEF(sSHL ,">>")         // 符號 >>
DEF(sSHR ,"<<")         // 符號 <<
DEF(sBAND ,"&")         // 符號 &
DEF(sBOR ,"|")          // 符號 |
DEF(sDPLUS ,"++")       // 符號 ++
DEF(sDMINUS ,"--")      // 符號 -- 
DEF(kEND ,"_?END?_")    // 檔案終止符號 
DEF(CINT ,"CINT")       // 整數常數，例如： 374 
DEF(CFLOAT ,"CFLOAT")   // 浮點常數，例如： 3.14
DEF(CSTR ,"CSTR")       // 字串常數，例如："Hello!" 
DEF(ID ,"ID")           // 變數名稱：ID = [A-Za-z_][0-9A-Za-z_]*
DEF(OP1 ,"OP1")         // 一元運算：OP1 = ++ | --
// DEF(OP2, "+|-|/|*|%|&|&&|^|<<|>>|<|>|<=|>=|==|!=|||||||")

// 宣告非終端項目字串：Nonterminal Node
DEF(PROG ,"PROG")           // 程式：PROG = (STRUCT | METHOD | DECL ; )*
DEF(METHOD ,"METHOD")       // 函數：METHOD = TYPE ** ID(PARAM_LIST?) BLOCK
DEF(STRUCT ,"STRUCT")       // 結構：STRUCT = struct ID { DECL_LIST ; }
DEF(BLOCK ,"BLOCK")         // 區塊：BLOCK = { BASE* }
DEF(BASE ,"BASE")           // 基本：BASE
DEF(IF ,"IF")               // 判斷：IF = if (EXP) BASE (else BASE)?
DEF(FOR ,"FOR")             // 迴圈：FOR = for (STMT ; EXP ; STMT) BASE
DEF(WHILE ,"WHILE")         // 迴圈：WHILE = while (EXP) BASE
DEF(STMT ,"STMT")           // 陳述：STMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1 
DEF(VAR ,"VAR")             // 變數：VAR = ** ID ([ integer ])* (= EXP)? 
DEF(EXP ,"EXP")             // 運算：EXP = TERM (OP2 TERM)?
DEF(TERM ,"TERM")           // 項目：TERM = ( EXP (OP2 EXP)? ) | CINT | CFLOAT | CSTR | PATH
DEF(PATH ,"PATH")           // 路徑：PATH = ATOM ((.|->) ATOM)*
DEF(ATOM ,"ATOM")           // 基元：ATOM = ID (([ EXP ])* |( EXP_LIST? ))
DEF(CALL ,"CALL")           // 呼叫：ID (( EXP_LIST? ))
DEF(ARRAY_MEMBER ,"ARRAY_MEMBER") // 陣列成員：ID ([ EXP ])*
DEF(DECL ,"DECL")           // 宣告：DECL = TYPE VAR_LIST
DEF(PARAM ,"PARAM")         // 參數：PARAM = TYPE VAR
DEF(TYPE ,"TYPE")           // 型態 (int|float|ID) 
DEF(DECL_LIST ,"DECL_LIST") // 串列：DECL_LIST = DECL (; DECL)* 
DEF(PARAM_LIST ,"PARAM_LIST")//串列：PARAM_LIST = PARAM (, PARAM)*
DEF(EXP_LIST ,"EXP_LIST")   // 串列：EXP_LIST = EXP (, EXP)*
DEF(VAR_LIST ,"VAR_LIST")   // 串列：VAR_LIST = VAR (, VAR)*
DEF(BTYPE, "BTYPE")         // 基本型態：(int|float|...) 
DEF(ETYPE, "ETYPE")         // 基本型態：(int|float|...) 加上 Structure 型態 
DEF(GLOBAL, "GLOBAL")       // 全域區塊：.global 
// DEF(sCOLON ,":")
// DEF(sMUL ,"*") // 符號 *

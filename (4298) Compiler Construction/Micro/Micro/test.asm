LDA       R15,VARS
LDA       R14,STRS
WRST      +0(R14)
WRNL      
RDI       +0(R15)
WRST      +18(R14)
WRI       +0(R15)
WRNL      
HALT      
LABEL     VARS
SKIP      4
LABEL     STRS
STRING    "Enter a number:: "
STRING    ":"Your input:::" "
SKIP      34

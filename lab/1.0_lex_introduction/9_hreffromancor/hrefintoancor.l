%start HREF ANCOR

%%

\<a             BEGIN ANCOR;
<ANCOR>href=\"  BEGIN HREF;
<HREF>[^\"]*    {ECHO;}
<HREF>\"        BEGIN ANCOR;
<ANCOR>a\>      BEGIN 0;
.



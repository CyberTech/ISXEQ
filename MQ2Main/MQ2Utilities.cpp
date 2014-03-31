/*****************************************************************************
MQ2Main.dll: MacroQuest2's extension DLL for EverQuest
Copyright (C) 2002-2003 Plazmic, 2003-2005 Lax

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
******************************************************************************/
#if !defined(CINTERFACE)
#error /DCINTERFACE
#endif
#define DBG_SPEW

#include "MQ2Main.h"

// ***************************************************************************
// Function:    DebugSpew
// Description: Outputs text to debugger, usage is same as printf ;)
// ***************************************************************************
VOID DebugSpew(PCHAR szFormat, ...)
{
#ifdef DBG_SPEW
    if (gFilterDebug) return;
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    OutputDebugString(DebugHeader);
    OutputDebugString(szOutput);
    OutputDebugString("\n");
#endif
}

VOID WriteChatf(PCHAR szFormat, ...)
{
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    WriteChatColor(szOutput);
}

VOID DebugSpewAlways(PCHAR szFormat, ...)
{
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;

    OutputDebugString(DebugHeader);
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    OutputDebugString(szOutput);
    OutputDebugString("\n");

#ifdef DBG_SPEW
    if (gSpewToFile) {
        FILE *fOut = NULL;
        CHAR Filename[MAX_STRING] = {0};
        sprintf(Filename,"%s\\DebugSpew.log",gszLogPath);
        fOut = fopen(Filename,"at");
        if (!fOut) return;
#ifdef DBG_CHARNAME
        CHAR Name[256] = "Unknown";
        PCHARINFO pCharInfo = GetCharInfo();
        if (pCharInfo)
        {
            strcpy(Name,pCharInfo->Name);
        }
        fprintf(fOut,"%s - %s\r\n", Name, szOutput);
#else
        fprintf(fOut,"%s\r\n", szOutput);
#endif
        fclose(fOut);
    }
#endif
}

EQLIB_API VOID DebugSpewNoFile(PCHAR szFormat, ...)
{
#ifdef DBG_SPEW
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;

    OutputDebugString(DebugHeader);
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    OutputDebugString(szOutput);
    OutputDebugString("\n");
#endif
}

VOID StrReplaceSection(PCHAR szInsert,DWORD Length,PCHAR szNewString)
{
    DWORD NewLength=(DWORD)strlen(szNewString);
    memmove(&szInsert[NewLength],&szInsert[Length],strlen(&szInsert[Length])+1);
    strncpy(szInsert,szNewString,NewLength);
}

VOID ConvertCR(PCHAR Text)
{// not super-efficient but this is only being called at initialization currently.
    while (PCHAR Next=strstr(Text,"\\n"))
    {
        StrReplaceSection(Next,2,"\n");
    }
}

VOID Flavorator(PCHAR szLine)
{
    PCHAR pSpot;
    while(pSpot=strstr(szLine,"%e"))
    {
        StrReplaceSection(pSpot,2,szColorExpletive[rand()%nColorExpletive]);
    }
    while(pSpot=strstr(szLine,"%a"))
    {
        StrReplaceSection(pSpot,2,szColorAdjective[rand()%nColorAdjective]);
    }
    while(pSpot=strstr(szLine,"%y"))
    {
        StrReplaceSection(pSpot,2,szColorAdjectiveYou[rand()%nColorAdjectiveYou]);
    }
}

#ifndef ISXEQ
VOID SyntaxError(PCHAR szFormat, ...)
{
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    if (bLaxColor)
    {
        CHAR szColor[MAX_STRING]={0};
        strcpy(szColor,szColorSyntaxError[rand()%nColorSyntaxError]);
        if (szColor[0])
        {
            Flavorator(szColor);
            WriteChatColor(szColor);
        }
    }
    WriteChatColor(szOutput,CONCOLOR_YELLOW);
    strcpy(gszLastSyntaxError,szOutput);
}

VOID MacroError(PCHAR szFormat, ...)
{
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    if (bLaxColor)
    {
        CHAR szColor[MAX_STRING]={0};
        strcpy(szColor,szColorMacroError[rand()%nColorMacroError]);
        if (szColor[0])
        {
            Flavorator(szColor);
            WriteChatColor(szColor);
        }
    }
    WriteChatColor(szOutput,CONCOLOR_RED);
    if (bAllErrorsLog) MacroLog(NULL, "Macro Error");
    if (bAllErrorsLog) MacroLog(NULL, szOutput);
    strcpy(gszLastNormalError,szOutput);
    if (gMacroBlock)
    {
        if (bAllErrorsDumpStack || bAllErrorsFatal)
            DumpStack(0,0);
        if (bAllErrorsFatal)
            EndMacro((PSPAWNINFO)pCharSpawn,"");
    }
}

VOID FatalError(PCHAR szFormat, ...)
{
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    if (bLaxColor)
    {
        CHAR szColor[MAX_STRING]={0};
        strcpy(szColor,szColorFatalError[rand()%nColorFatalError]);
        if (szColor[0])
        {
            Flavorator(szColor);
            WriteChatColor(szColor);
        }
    }
    WriteChatColor(szOutput,CONCOLOR_RED);
    strcpy(gszLastNormalError,szOutput);
    if (bAllErrorsLog) MacroLog(NULL, "Fatal Error");
    if (bAllErrorsLog) MacroLog(NULL, szOutput);
    if (gMacroBlock)
    {
        DumpStack(0,0);
        EndMacro((PSPAWNINFO)pCharSpawn,"");
    }
}

VOID MQ2DataError(PCHAR szFormat, ...)
{
    CHAR szOutput[MAX_STRING] = {0};
    va_list vaList;
    va_start( vaList, szFormat );
    vsprintf(szOutput,szFormat, vaList);
    if (gFilterMQ2DataErrors)
        DebugSpew(szOutput);
    else
    {
        if (bLaxColor)
        {
            CHAR szColor[MAX_STRING]={0};
            strcpy(szColor,szColorMQ2DataError[rand()%nColorMQ2DataError]);
            if (szColor[0])
            {
                Flavorator(szColor);
                WriteChatColor(szColor);
            }
        }
        WriteChatColor(szOutput,CONCOLOR_RED);
    }
    strcpy(gszLastMQ2DataError,szOutput);
    if (bAllErrorsLog) MacroLog(NULL, "Data Error");
    if (bAllErrorsLog) MacroLog(NULL, szOutput);
    if (gMacroBlock)
    {
        if (bAllErrorsDumpStack || bAllErrorsFatal)
            DumpStack(0,0);
        if (bAllErrorsFatal)
            EndMacro((PSPAWNINFO)pCharSpawn,"");
    }
}
#endif

VOID FixStringTable()
{
    PEQSTRINGTABLE pTable=(PEQSTRINGTABLE)pStringTable;
    for (DWORD N=0 ; N<pTable->Count ; N++)
        if (PEQSTRING pStr=pTable->StringItems[N])
            if (PCHAR p=pStr->String)
            {
                while(*p)
                    p++;
                p--;
                while(*p==' ' && p!=pStr->String)
                {
                    *p=0;
                    p--;
                }
            }
}

#ifndef ISXEQ
// ***************************************************************************
// Function:    GetNextArg
// Description: Returns a pointer to the next argument
// ***************************************************************************
PSTR GetNextArg(PCSTR szLine, DWORD dwNumber, BOOL CSV, CHAR Separator)
{
    PCSTR szNext = szLine;
    BOOL CustomSep = FALSE;
    BOOL InQuotes = FALSE;
    if (Separator!=0) CustomSep=TRUE;

    while (
        ((!CustomSep) && (szNext[0] == ' '))
        || ((!CustomSep) && (szNext[0] == '\t'))
        || ((CustomSep) && (szNext[0]==Separator))
        || ((!CustomSep) && (CSV) && (szNext[0]==','))
        ) szNext++;

    if ((INT)dwNumber < 1) return (PSTR)szNext;
    for (dwNumber;dwNumber>0;dwNumber--) {
        while ((
            ((CustomSep) || (szNext[0] != ' '))
            && ((CustomSep) || (szNext[0] != '\t'))
            && ((!CustomSep) || (szNext[0]!=Separator))
            && ((CustomSep) || (!CSV) || (szNext[0]!=','))
            && (szNext[0] != 0)
            )
            || (InQuotes)
            ) {
                if ((szNext[0] == 0) && (InQuotes)) {
                    DebugSpew("GetNextArg - No matching quote, returning empty string");
                    return (PSTR)szNext;
                }
                if (szNext[0] == '"') InQuotes = !InQuotes;
                szNext++;
        }
        while (
            ((!CustomSep) && (szNext[0] == ' '))
            || ((!CustomSep) && (szNext[0] == '\t'))
            || ((CustomSep) && (szNext[0]==Separator))
            || ((!CustomSep) && (CSV) && (szNext[0]==','))
            ) szNext++;
    }
    return (PSTR)szNext;
}

// ***************************************************************************
// Function:    GetArg
// Description: Returns a pointer to the current argument in szDest
// ***************************************************************************
PSTR GetArg(PSTR szDest, PCSTR szSrc, DWORD dwNumber, BOOL LeaveQuotes, BOOL ToParen, BOOL CSV, CHAR Separator, BOOL AnyNonAlphaNum)
{
    DWORD i=0;
    DWORD j=0;
    BOOL CustomSep = FALSE;
    BOOL InQuotes = FALSE;
    PCSTR szTemp = szSrc;
    ZeroMemory(szDest,MAX_STRING);

    if (Separator!=0) CustomSep=TRUE;

    szTemp = GetNextArg(szTemp,dwNumber-1, CSV, Separator);

    while ((
        ((CustomSep) || (szTemp[i] != ' '))
        && ((CustomSep) || (szTemp[i] != '\t'))
        && ((CustomSep) || (!CSV) || (szTemp[i]!=','))
        && ((!CustomSep) || (szTemp[i]!=Separator))
        && ((!AnyNonAlphaNum) || (
        (szTemp[i]>='0' && szTemp[i]<='9') ||
        (szTemp[i]>='a' && szTemp[i]<='z') ||
        (szTemp[i]>='A' && szTemp[i]<='Z') ||
        (szTemp[i]=='_')
        ))
        && (szTemp[i] != 0)
        && ((!ToParen) || (szTemp[i] !=')'))
        )
        || (InQuotes)
        ) {
            if ((szTemp[i] == 0) && (InQuotes)) {
                DebugSpew("GetArg - No matching quote, returning entire string");
                DebugSpew("Source = %s",szSrc);
                DebugSpew("Dest = %s",szDest);
                return szDest;
            }
            if (szTemp[i] == '"') {
                InQuotes = !InQuotes;
                if (LeaveQuotes) {
                    szDest[j] = szTemp[i];
                    j++;
                }
            } else {
                szDest[j] = szTemp[i];
                j++;
            }
            i++;
    }
    if ((ToParen) && (szTemp[i]==')')) szDest[j]=')';
    //DebugSpew("GetArg - Arg%d from '%s' = '%s'",dwNumber,szTemp,szDest);

    return szDest;
}
#endif

PCHAR GetEQPath(PCHAR szBuffer)
{
    GetModuleFileName(NULL, szBuffer, MAX_STRING);
    PCHAR pSearch=0;
    if (pSearch=strstr(strlwr(szBuffer), "\\wineq\\"))
        *pSearch=0;
    else if (pSearch=strstr(strlwr(szBuffer), "\\testeqgame.exe"))
        *pSearch=0;
    else if (pSearch=strstr(szBuffer, "\\eqgame.exe"))
        *pSearch=0;
    return szBuffer;
}

VOID ConvertItemTags(CXStr &cxstr, BOOL Tag)
{
    __asm{
        push ecx;
        push eax;
        push [Tag];
        push [cxstr];
        call [EQADDR_CONVERTITEMTAGS];
        pop ecx;
        pop ecx;
        pop eax;
        pop ecx;
    };
}

// YES THIS NEEDS TO BE PCXSTR * 
VOID AppendCXStr(PCXSTR *cxstr, PCHAR text) 
{ 
    CXStr *Str=(CXStr*)cxstr;
    (*Str)+=text;
    cxstr=(PCXSTR*)Str;
    //cxstr+=text;
} 

// YES THIS NEEDS TO BE PCXSTR * 
VOID SetCXStr(PCXSTR *cxstr, PCHAR text) 
{ 
    //cxstr=text;
    CXStr *Str=(CXStr*)cxstr;
    (*Str)=text;
    cxstr=(PCXSTR*)Str;
} 

DWORD GetCXStr(PCXSTR pCXStr, PCHAR szBuffer, DWORD maxlen)
{
    if (!szBuffer)
        return 0;
    szBuffer[0]=0;
    if (!pCXStr || !maxlen){return 0;}
    if (pCXStr->Encoding==0)
    {
        if (pCXStr->Length<maxlen)
        {
            strcpy(szBuffer,pCXStr->Text);
            return pCXStr->Length;
        }
        else
        {
            strncpy(szBuffer,pCXStr->Text,maxlen);
            szBuffer[maxlen-1]=0;
            return maxlen;
        }
    }
    else
    { // unicode
        // this is kind of ghetto but it works for english
        DWORD i=0;
        DWORD o=0;
        maxlen--;
        while(pCXStr->Text[i] && o<maxlen)
        {
            szBuffer[o++]=pCXStr->Text[i];
            i+=2;
        }
        szBuffer[o]=0;
        return o;
    }
}
/**/

#define InsertColor(text,color) sprintf(text,"<c \"#%06X\">",color);TotalColors++; 
#define InsertStopColor(text)   sprintf(text,"</c>");TotalColors--; 

VOID StripMQChat(PCHAR in, PCHAR out)
{
    //DebugSpew("StripMQChat(%s)",in);
    int i = 0;
    int o = 0;
    while(in[i])
    {
        if (in[i]=='\a')
        {
            i++;
            if(in[i]=='-')
            { // skip 1 after -
                i++;
            }
            else if (in[i]=='#')
            { // skip 6 after #
                i+=6;
            }
        }
        else if (in[i]=='\n')
        {
        }
        else
            out[o++]=in[i];
        i++;
    }
    out[o]=0;
    //DebugSpew("StripMQChat=>(%s)",out);
}

DWORD MQToSTML(PCHAR in, PCHAR out, DWORD maxlen, DWORD ColorOverride)
{
    //DebugSpew("MQToSTML(%s)",in);
    // 1234567890123
    // <c "#123456">
    if (maxlen>14)
        maxlen-=14; // huh...
    DWORD pchar_in_string_position = 0;
    DWORD pchar_out_string_position = 0;
    BOOL bFirstColor=false;
    BOOL bNBSpace=false;
    ColorOverride&=0xFFFFFF;
    int CurrentColor=ColorOverride;
    int TotalColors=0; // this MUST be signed.
    pchar_out_string_position+=InsertColor(&out[pchar_out_string_position],CurrentColor);

    while(in[pchar_in_string_position]!=0 && pchar_out_string_position<maxlen)
    {
        if (in[pchar_in_string_position]==' ')
        {
            if (bNBSpace) {
                out[pchar_out_string_position++]='&';
                out[pchar_out_string_position++]='N';
                out[pchar_out_string_position++]='B';
                out[pchar_out_string_position++]='S';
                out[pchar_out_string_position++]='P';
                out[pchar_out_string_position++]=';';
            } else {
                out[pchar_out_string_position++]=' ';
            }
            bNBSpace=1;
        }
        else
        {
            bNBSpace=0;
            switch(in[pchar_in_string_position])
            {
            case '\a':
                // HANDLE COLOR
                bFirstColor=true;
                pchar_in_string_position++;
                if (in[pchar_in_string_position]=='x')
                {
                    CurrentColor=-1;
                    pchar_out_string_position+=InsertStopColor(&out[pchar_out_string_position]);
                }
                else
                    if (in[pchar_in_string_position]=='#')
                    {
                        pchar_in_string_position++;
                        char temp[7];
                        for (int x = 0 ; x < 6 ; x++)
                        {
                            temp[x]=in[pchar_in_string_position++];
                        }
                        pchar_in_string_position--;
                        temp[6]=0;
                        CurrentColor=-1;
                        pchar_out_string_position+=sprintf(&out[pchar_out_string_position],"<c \"#%s\">",&temp[0]);
                        TotalColors++;
                    }
                    else
                    {
                        bool Dark=false;
                        if (in[pchar_in_string_position]=='-')
                        {
                            Dark=true;    
                            pchar_in_string_position++;
                        }
                        int LastColor=CurrentColor;
                        switch(in[pchar_in_string_position])
                        {
                        case 'y': // yellow (green/red)
                            if (Dark)
                                CurrentColor=0x999900;
                            else
                                CurrentColor=0xFFFF00;
                            break;
                        case 'o': // orange (green/red)
                            if (Dark)
                                CurrentColor=0x996600;
                            else
                                CurrentColor=0xFF9900;
                            break;
                        case 'g': // green   (green)
                            if (Dark)
                                CurrentColor=0x009900;
                            else
                                CurrentColor=0x00FF00;
                            break;
                        case 'u': // blue   (blue)
                            if (Dark)
                                CurrentColor=0x000099;
                            else
                                CurrentColor=0x0000FF;
                            break;
                        case 'r': // red     (red)
                            if (Dark)
                                CurrentColor=0x990000;
                            else
                                CurrentColor=0xFF0000;
                            break;
                        case 't': // teal (blue/green)
                            if (Dark)
                                CurrentColor=0x009999;
                            else
                                CurrentColor=0x00FFFF;
                            break;
                        case 'b': // black   (none)
                            CurrentColor=0x000000;
                            break;
                        case 'm': // magenta (blue/red)
                            if (Dark)
                                CurrentColor=0x990099;
                            else
                                CurrentColor=0xFF00FF;
                            break;
                        case 'p': // purple (blue/red)
                            if (Dark)
                                CurrentColor=0x660099;
                            else
                                CurrentColor=0x9900FF;
                            break;
                        case 'w': // white   (all)
                            if (Dark)
                                CurrentColor=0x999999;
                            else
                                CurrentColor=0xFFFFFF;
                            break;
                        }
                        if (CurrentColor!=LastColor)
                        {
                            pchar_out_string_position+=InsertColor(&out[pchar_out_string_position],CurrentColor);
                        }
                    }
                    break;
            case '&':
                out[pchar_out_string_position++]='&';
                out[pchar_out_string_position++]='A';
                out[pchar_out_string_position++]='M';
                out[pchar_out_string_position++]='P';
                out[pchar_out_string_position++]=';';
                break;
            case '%':
                out[pchar_out_string_position++]='&';
                out[pchar_out_string_position++]='P';
                out[pchar_out_string_position++]='C';
                out[pchar_out_string_position++]='T';
                out[pchar_out_string_position++]=';';
                break;
            case '<':
                out[pchar_out_string_position++]='&';
                out[pchar_out_string_position++]='L';
                out[pchar_out_string_position++]='T';
                out[pchar_out_string_position++]=';';
                break;
            case '>':
                out[pchar_out_string_position++]='&';
                out[pchar_out_string_position++]='G';
                out[pchar_out_string_position++]='T';
                out[pchar_out_string_position++]=';';
                break;
            case '"':
                out[pchar_out_string_position++]='&';
                out[pchar_out_string_position++]='Q';
                out[pchar_out_string_position++]='U';
                out[pchar_out_string_position++]='O';
                out[pchar_out_string_position++]='T';
                out[pchar_out_string_position++]=';';
                break;
            case '\n':
                out[pchar_out_string_position++]='<';
                out[pchar_out_string_position++]='B';
                out[pchar_out_string_position++]='R';
                out[pchar_out_string_position++]='>';
                break;
            default:
                out[pchar_out_string_position++]=in[pchar_in_string_position];
                break;
            }
        }
        pchar_in_string_position++;
    }
    for (TotalColors ; TotalColors>0 ;)
    {
        pchar_out_string_position+=InsertStopColor(&out[pchar_out_string_position]);
    }
    out[pchar_out_string_position++]=0;
    return pchar_out_string_position;
}

PCHAR GetFilenameFromFullPath(PCHAR Filename)
{
    while (strstr(Filename,"\\")) Filename=strstr(Filename,"\\")+1;
    return Filename;
}

PCHAR GetSubFromLine(PMACROBLOCK pLine, PCHAR szSub)
{
    while (pLine!=NULL) {
        if (!strnicmp(pLine->Line,"sub ",4)) {
            strcpy(szSub,pLine->Line+4);
            return szSub;
        }
        pLine = pLine->pPrev;
    }
    strcpy(szSub,"NULL");
    return szSub;
}

BOOL CompareTimes(PCHAR RealTime, PCHAR ExpectedTime)
{
    //Match everything except seconds
    //Format is: WWW MMM DD hh:mm:ss YYYY
    //           0123456789012345678901234
    //                     1         2
    if (!strnicmp(RealTime,ExpectedTime,17) &&
        !strnicmp(RealTime+19,ExpectedTime+19,5))
        return TRUE;
    return FALSE;
}

VOID AddFilter(PCHAR szFilter, int Length, PBOOL pEnabled)
{
    PFILTER New = (PFILTER)malloc(sizeof(FILTER));
    if (!New) return;
    if (Length==-1) Length=strlen(szFilter);
    strcpy(New->FilterText,szFilter);
    New->Length=Length;
    New->pEnabled=pEnabled;
    New->pNext=gpFilters;
    gpFilters=New;
}

VOID DefaultFilters(VOID)
{
    AddFilter("You have become better at ",                                             26, &gFilterSkillsIncrease);
    AddFilter("You lacked the skills to fashion the items together.",                   -1, &gFilterSkillsAll);
    AddFilter("You have fashioned the items together to create something new!",         -1, &gFilterSkillsAll);
    AddFilter("You have fashioned the items together to create an alternate product.",  -1, &gFilterSkillsAll);
    AddFilter("You can no longer advance your skill from making this item.",            -1, &gFilterSkillsAll);
    AddFilter("You no longer have a target.",                                           -1, &gFilterTarget);
    AddFilter("You give ",                                                              9,  &gFilterMoney);
    AddFilter("You receive ",                                                           12, &gFilterMoney);
    AddFilter("You are encumbered",                                                     17, &gFilterEncumber);
    AddFilter("You are no longer encumbered",                                           27, &gFilterEncumber);
    AddFilter("You are low on drink",                                                   19, &gFilterFood);
    AddFilter("You are low on food",                                                    18, &gFilterFood);
    AddFilter("You are out of drink",                                                   19, &gFilterFood);
    AddFilter("You are out of food",                                                    18, &gFilterFood);
    AddFilter("You and your mount are thirsty.",                                        -1, &gFilterFood);
    AddFilter("You and your mount are hungry.",                                         -1, &gFilterFood);
    AddFilter("You are hungry",                                                         13, &gFilterFood);
    AddFilter("You are thirsty",                                                        14, &gFilterFood);
    AddFilter("You take a bite out of",                                                 22, &gFilterFood);
    AddFilter("You take a bite of",                                                     18, &gFilterFood);
    AddFilter("You take a drink from",                                                  21, &gFilterFood);
    AddFilter("Ahhh. That was tasty.",                                                  -1, &gFilterFood);
    AddFilter("Ahhh. That was refreshing.",                                             -1, &gFilterFood);
    AddFilter("Chomp, chomp, chomp...",                                                 22, &gFilterFood);
    AddFilter("Glug, glug, glug...",                                                    19, &gFilterFood);
    AddFilter("You could not possibly eat any more, you would explode!",                -1, &gFilterFood);
    AddFilter("You could not possibly drink any more, you would explode!",              -1, &gFilterFood);
    AddFilter("You could not possibly consume more alcohol or become more intoxicated!",-1, &gFilterFood);
}

PCHAR ConvertHotkeyNameToKeyName(PCHAR szName)
{
    if (!stricmp(szName,"EQUALSIGN")) strcpy(szName,"=");
    if (!stricmp(szName,"SEMICOLON")) strcpy(szName,";");
    if (!stricmp(szName,"LEFTBRACKET")) strcpy(szName,"[");
    return szName;
}

// ***************************************************************************
// Function:    GetFullZone
// Description: Returns a full zone name from a short name
// ***************************************************************************
PCHAR GetFullZone(DWORD ZoneID)
{
    if (!ppWorldData | !pWorldData)
		return NULL;
    if(ZoneID > MAX_ZONES)
        ZoneID &= 0x7FFF;
    if(ZoneID > MAX_ZONES || ZoneID < 0)
        return "UNKNOWN_ZONE";
    PZONELIST pZone = ((PWORLDDATA)pWorldData)->ZoneArray[ZoneID];
   if(pZone)
	   return pZone->LongName;
   else
		return "UNKNOWN_ZONE";
}
// ***************************************************************************
// Function:    GetShortZone
// Description: Returns a short zone name from a ZoneID
// ***************************************************************************
PCHAR GetShortZone(DWORD ZoneID)
{
    if (!ppWorldData | !pWorldData) return NULL;
    if(ZoneID > MAX_ZONES)
        ZoneID &= 0x7FFF;
    if(ZoneID > MAX_ZONES || ZoneID < 0)
        return "UNKNOWN_ZONE";
    PZONELIST pZone = ((PWORLDDATA)pWorldData)->ZoneArray[ZoneID];
	if(pZone)
		return pZone->ShortName;
	else
		return "UNKNOWN_ZONE";
}
// ***************************************************************************
// Function:    GetZoneID
// Description: Returns a ZoneID from a short zone name
// ***************************************************************************

DWORD GetZoneID(PCHAR ZoneShortName)
{
    PZONELIST pZone = NULL;
    if (!ppWorldData | !pWorldData)
		return -1;
    for (int nIndex=0; nIndex < MAX_ZONES; nIndex++) {
        pZone = ((PWORLDDATA)pWorldData)->ZoneArray[nIndex];
        if(pZone) {
            if (!_stricmp(pZone->ShortName,ZoneShortName)) {
                return nIndex;
            }
		}
    }
    return -1; 
}

// ***************************************************************************
// Function:    GetGameTime
// Description: Returns Current Game Time
// ***************************************************************************
VOID GetGameTime(int* Hour, int* Minute, int* Night)
{
    int eqHour = 0;
    int eqMinute = 0;
    if (!ppWorldData | !pWorldData) return;
    eqHour=((PWORLDDATA)pWorldData)->Hour - 1; // Midnight = 1 in EQ time
    eqMinute=((PWORLDDATA)pWorldData)->Minute;
    if (Hour) *Hour = eqHour;
    if (Minute) *Minute = eqMinute;
    if (Night) *Night = ((eqHour < 7) || (eqHour > 18));//?TRUE:FALSE; // already handled by operators
}

// ***************************************************************************
// Function:    GetGameDate
// Description: Returns Current Game Time
// ***************************************************************************
VOID GetGameDate(int* Month, int* Day, int* Year)
{
    if (!ppWorldData | !pWorldData) return;
    if (Month) *Month=((PWORLDDATA)pWorldData)->Month;
    if (Day) *Day=((PWORLDDATA)pWorldData)->Day;
    if (Year) *Year=((PWORLDDATA)pWorldData)->Year;
}

int GetLanguageIDByName( PCHAR SzName )
{
    if (!_stricmp(SzName, "Common")) return 1;
    if (!_stricmp(SzName, "Common Tongue")) return 1;
    if (!_stricmp(SzName, "Barbarian")) return 2;
    if (!_stricmp(SzName, "Erudian")) return 3;
    if (!_stricmp(SzName, "Elvish")) return 4;
    if (!_stricmp(SzName, "Dark Elvish")) return 5;
    if (!_stricmp(SzName, "Dwarvish")) return 6;
    if (!_stricmp(SzName, "Troll")) return 7;
    if (!_stricmp(SzName, "Ogre")) return 8;
    if (!_stricmp(SzName, "Gnomish")) return 9;
    if (!_stricmp(SzName, "Halfling")) return 10;
    if (!_stricmp(SzName, "Thieves Cant")) return 11;
    if (!_stricmp(SzName, "Old Erudian")) return 12;
    if (!_stricmp(SzName, "Elder Elvish")) return 13;
    if (!_stricmp(SzName, "Froglok")) return 14;
    if (!_stricmp(SzName, "Goblin")) return 15;
    if (!_stricmp(SzName, "Gnoll")) return 16;
    if (!_stricmp(SzName, "Combine Tongue")) return 17;
    if (!_stricmp(SzName, "Elder Tier'Dal")) return 18;
    if (!_stricmp(SzName, "Lizardman")) return 19;
    if (!_stricmp(SzName, "Orcish")) return 20;
    if (!_stricmp(SzName, "Faerie")) return 21;
    if (!_stricmp(SzName, "Dragon")) return 22;
    if (!_stricmp(SzName, "Elder Dragon")) return 23;
    if (!_stricmp(SzName, "Dark Speech")) return 24;
    if (!_stricmp(SzName, "Vah Shir")) return 25;
    return -1;
}

int GetCurrencyIDByName(PCHAR szName)
{
	if (!_stricmp(szName, "DOUBLOONS")) return ALTCURRENCY_DOUBLOONS;  // 0XA
	if (!_stricmp(szName, "ORUX")) return ALTCURRENCY_ORUX; //0XB
	if (!_stricmp(szName, "PHOSPHENES")) return ALTCURRENCY_PHOSPHENES; //0XC
	if (!_stricmp(szName, "PHOSPHITES")) return ALTCURRENCY_PHOSPHITES; //0XD
	if (!_stricmp(szName, "FAYCITUM")) return ALTCURRENCY_FAYCITES; //0XE
	if (!_stricmp(szName, "CHRONOBINES")) return ALTCURRENCY_CHRONOBINES; //0XF
	if (!_stricmp(szName, "SILVER TOKENS")) return ALTCURRENCY_SILVERTOKENS; //0X10
	if (!_stricmp(szName, "GOLD TOKENS")) return ALTCURRENCY_GOLDTOKENS; //0X11
	if (!_stricmp(szName, "MCKENZIE'S SPECIAL BREW")) return ALTCURRENCY_MCKENZIE; //0X12
	if (!_stricmp(szName, "BAYLE MARKS")) return ALTCURRENCY_BAYLE; //0X13   
	if (!_stricmp(szName, "TOKENS OF RECLAMATION")) return ALTCURRENCY_RECLAMATION; //0X14
	if (!_stricmp(szName, "BRELLIUM TOKENS")) return ALTCURRENCY_BRELLIUM; //0X15
	if (!_stricmp(szName, "DREAM MOTES")) return ALTCURRENCY_MOTES; //0X16
	if (!_stricmp(szName, "REBELLION CHITS")) return ALTCURRENCY_REBELLIONCHITS; //0X17
	if (!_stricmp(szName, "DIAMOND COINS")) return ALTCURRENCY_DIAMONDCOINS; //0X18
	if (!_stricmp(szName, "BRONZE FIATS")) return ALTCURRENCY_BRONZEFIATS; //0X19
	if (!_stricmp(szName, "EXPEDIENT DELIVERY VOUCHERS")) return ALTCURRENCY_VOUCHER; //0x1a
	if (!_stricmp(szName, "VELIUM SHARDS")) return ALTCURRENCY_VELIUMSHARDS; //0X1b
	if (!_stricmp(szName, "CRYSTALLIZED FEAR")) return ALTCURRENCY_CRYSTALLIZEDFEAR; //0X1c
	if (!_stricmp(szName, "SHADOWSTONES")) return ALTCURRENCY_SHADOWSTONES; //0X1d
	if (!_stricmp(szName, "DREADSTONES")) return ALTCURRENCY_DREADSTONES; //0X1e
	if (!_stricmp(szName, "Marks of Valor")) return ALTCURRENCY_MARKSOFVALOR; //0X1F
	if (!_stricmp(szName, "Marks of Heroism")) return ALTCURRENCY_MEDALSOFHEROISM; //0X20   
	return -1;
}
//CHAR szUnknownSpell[MAX_STRING] = "Unknown Spell";
// ^^ this shit is unnecessary, static strings are already stored at a location in the
// file itself. Changed the function to return "Unknown Spell". - Lax
PCHAR GetSpellNameByID(DWORD dwSpellID)
{
    if (ppSpellMgr) {
		PSPELL pSpell = &(*((PSPELLMGR)pSpellMgr)->Spells[dwSpellID]);
		if (pSpell && pSpell->Name && pSpell->Name[0]!='\0') {
			return pSpell->Name;
		}
	}
    return "Unknown Spell";
}

PSPELL GetSpellByName(PCHAR szName)
{
    // PSPELL GetSpellByName(PCHAR NameOrID)
    // This function now accepts SpellID as an argument as well as SpellName
    PSPELL pSpell = NULL;
    if (ppSpellMgr == NULL) return NULL;
    if (szName[0]>='0' && szName[0]<='9')
    {
        return GetSpellByID(atoi(szName));
    }
    for (DWORD dwSpellID = 0; dwSpellID < TOTAL_SPELL_COUNT; dwSpellID++) {
        pSpell = &(*((PSPELLMGR)pSpellMgr)->Spells[dwSpellID]);
        if ((pSpell->ID > 0) && (pSpell->ID < TOTAL_SPELL_COUNT))
        {
            if (pSpell->Name != NULL) 
            {
                if (!_stricmp(szName, pSpell->Name)) {
                    return pSpell;
                }
            }
        }
    }
    return NULL;
}

DWORD GetSpellDuration(PSPELL pSpell, PSPAWNINFO pSpawn) 
{ 
    switch (pSpell->DurationType) { 
                case 0:          
                    return 0;        
                case 1:        
                case 6: 
                    return unsigned long(min(ceil(double(pSpawn->Level) / 2), pSpell->DurationValue1)); 
                case 3: 
                case 4:              
                case 11: 
                case 12:            
                    if (pSpell->DurationValue1) { 
                        return (pSpell->DurationValue1); 
                    } else {        
                        return (pSpell->DurationType*10); 
                    } 
                case 2: 
                    return unsigned long(min(ceil(double(pSpawn->Level) * 0.6), pSpell->DurationValue1)); 
                case 5:                  
                    return 3; 
                case 7:              
                    return min(pSpawn->Level, pSpell->DurationValue1 ? pSpell->DurationValue1 : pSpawn->Level); 
                case 8: 
                    return min(unsigned int(pSpawn->Level) + 10, pSpell->DurationValue1); 
                case 9:          
                    return min(unsigned int(pSpawn->Level) * 2 + 10, pSpell->DurationValue1); 
                case 10:        
                    return min(unsigned int(pSpawn->Level) * 3 + 10, pSpell->DurationValue1); 
                case 13:
                    return pSpell->DurationValue1 * 6 / 10;
                case 50: 
                    return 0xFFFFFFFF; 
                case 3600: 
                    return 6000; 
                default: 
                    return 0xFFFFFFFE; 
    } 
} 

DWORD GetDeityTeamByID(DWORD DeityID) {
    switch (DeityID) {
        case DEITY_ErollisiMarr:
        case DEITY_MithanielMarr:
        case DEITY_RodcetNife:
        case DEITY_Quellious:
        case DEITY_Tunare:
            return 1;
        case DEITY_BrellSerilis:
        case DEITY_Bristlebane:
        case DEITY_Karana:
        case DEITY_Prexus:
        case DEITY_SolusekRo:
        case DEITY_TheTribunal:
        case DEITY_Veeshan:
            return 2;
        case DEITY_Bertoxxulous:
        case DEITY_CazicThule:
        case DEITY_Innoruuk:
        case DEITY_RallosZek:
            return 3;
        default:
            return 0;
    }
}

PCHAR GetGuildByID(DWORD GuildID)
{
    if(PGUILD pGuild = pGuildList->GuildList[GuildID % pGuildList->HashValue])
    {
        while(pGuild->ID != GuildID)
        {
            pGuild = pGuild->pNext;

            if(!pGuild)
                return 0;
        }

        if(pGuild->pGuildData->Name[0])
            return pGuild->pGuildData->Name;        
    }

    return 0;
}

DWORD GetGuildIDByName(PCHAR szGuild)
{
    DWORD n;
    for(n = 0; n < pGuildList->HashValue - 1; n++) {
        if(PGUILD pGuild = pGuildList->GuildList[n]) {
            while (pGuild) {
                if(!stricmp(pGuild->pGuildData->Name, szGuild))
                    return pGuild->ID;

                pGuild = pGuild->pNext;
            }
        }
    }

    for(n = 0; n < pGuildList->HashValue - 1; n++) {
        if(PGUILD pGuild = pGuildList->GuildList[n]) {
            while (pGuild) {
                if(!strnicmp(pGuild->pGuildData->Name, szGuild, strlen(szGuild)))
                    return pGuild->ID;

                pGuild = pGuild->pNext;
            }
        }
    }

    return 0xFFFF;
}

PCHAR GetLightForSpawn(PSPAWNINFO pSpawn)
{
    BYTE Light = pSpawn->Light;
    if (Light>LIGHT_COUNT) Light=0;
    return szLights[Light];
}

// ***************************************************************************
// Function:    DistanceToSpawn3D
// Description: Return the distance between two spawns, including Z
// ***************************************************************************
FLOAT DistanceToSpawn3D(PSPAWNINFO pChar, PSPAWNINFO pSpawn)
{
    FLOAT X = pChar->X - pSpawn->X;
    FLOAT Y = pChar->Y - pSpawn->Y;
    FLOAT Z = pChar->Z - pSpawn->Z;
    return sqrtf(X*X + Y*Y + Z*Z);
}

// ***************************************************************************
// Function:    DistanceToSpawn
// Description: Return the distance between two spawns
// ***************************************************************************
FLOAT EstimatedDistanceToSpawn(PSPAWNINFO pChar, PSPAWNINFO pSpawn)
{
    FLOAT RDistance = DistanceToSpawn(pChar,pSpawn);
    FLOAT X = pChar->X - (pSpawn->X + pSpawn->SpeedX * RDistance);
    FLOAT Y = pChar->Y - (pSpawn->Y + pSpawn->SpeedY * RDistance);
    return sqrtf(X*X + Y*Y);
}

// ***************************************************************************
// Function:    ConColor
// Description: Returns the con color for a spawn's level
// ***************************************************************************
DWORD ConColor(PSPAWNINFO pSpawn)
{
    PSPAWNINFO pChar=(PSPAWNINFO)pLocalPlayer;
    if (!pChar)
        return CONCOLOR_WHITE; // its you

    switch(pCharData->GetConLevel((EQPlayer*)pSpawn))
    {
    case 0:
    case 1:
        return CONCOLOR_GREY;
    case 2:
        return CONCOLOR_GREEN;
    case 3:
        return CONCOLOR_LIGHTBLUE;
    case 4:
        return CONCOLOR_BLUE;
    case 5:    
        return CONCOLOR_WHITE;
    case 6:
        return CONCOLOR_YELLOW;
    case 7:
        return CONCOLOR_RED;
    default:
        return COLOR_PURPLE;
    }
}

PCONTENTS GetEnviroContainer()
{
    if (!ppContainerMgr || !pContainerMgr) return NULL;
    PEQ_CONTAINERWND_MANAGER ContainerMgr=(PEQ_CONTAINERWND_MANAGER)pContainerMgr;
    if(!ContainerMgr->pWorldContents) return NULL;
    return ContainerMgr->pWorldContents;
}

PEQCONTAINERWINDOW FindContainerForContents(PCONTENTS pContents)
{
    if (!ppContainerMgr || !pContainerMgr) return NULL;
    PEQ_CONTAINERWND_MANAGER pMgr = (PEQ_CONTAINERWND_MANAGER)pContainerMgr;

    for (int j = 0; j < 35; j++)
    {
        if (pMgr->pPCContainers[j] && (pMgr->pPCContainers[j]->pContents == pContents))
            return (pMgr->pPCContainers[j]);
    }
    return NULL;
}

// ***************************************************************************
// FindSpeed(PSPAWNINFO) - Used to find the speed of a Spawn taking a mount into
//                               consideration.
// ***************************************************************************

FLOAT FindSpeed(PSPAWNINFO pSpawn)
{
    PSPAWNINFO pMount = NULL;
    FLOAT fRunSpeed = 0;
    pMount=FindMount(pSpawn);

    if (pMount)
        if(!fRunSpeed)
            fRunSpeed = pMount->SpeedRun * 10000 / 70;

    return fRunSpeed;
}

VOID GetItemLinkHash(PCONTENTS Item, PCHAR Buffer)
{
    ((EQ_Item*)Item)->CreateItemTagString(Buffer, 256);
}

VOID GetItemLink(PCONTENTS Item, PCHAR Buffer)
{
    char hash[256];
    ((EQ_Item*)Item)->CreateItemTagString(hash, 256);
    sprintf(Buffer,"%c0%s%s%c",0x12,hash,GetItemFromContents(Item)->Name,0x12);
    DebugSpew("GetItemLink() returns '%s'",&Buffer[0]);
}

PCHAR GetLoginName() 
{
	if(__LoginName) {
		return (PCHAR)__LoginName;
	}
    return NULL;
}

VOID STMLToPlainText(PCHAR in, PCHAR out)
{
    DWORD pchar_in_string_position = 0;
    DWORD pchar_out_string_position = 0;
    DWORD pchar_amper_string_position = 0;
    CHAR Amper[2048] = {0};
    while(in[pchar_in_string_position]!=0) {
        switch (in[pchar_in_string_position]) {
           case '<':
               while (in[pchar_in_string_position]!='>')
                   pchar_in_string_position++;
               pchar_in_string_position++;
               break;
           case '&':
               pchar_in_string_position++;
               pchar_amper_string_position = 0;
               ZeroMemory(Amper,2048);
               while (in[pchar_in_string_position]!=';') {
                   Amper[pchar_amper_string_position++] = in[pchar_in_string_position++];
               }
               pchar_in_string_position++;
               if (!stricmp(Amper,"nbsp")) {
                   out[pchar_out_string_position++] = ' ';
               } else if (!stricmp(Amper,"amp")) {
                   out[pchar_out_string_position++] = '&';
               } else if (!stricmp(Amper,"gt")) {
                   out[pchar_out_string_position++] = '>';
               } else if (!stricmp(Amper,"lt")) {
                   out[pchar_out_string_position++] = '<';
               } else if (!stricmp(Amper,"quot")) {
                   out[pchar_out_string_position++] = '\"';
               } else if (!stricmp(Amper,"pct")) {
                   out[pchar_out_string_position++] = '%';
               } else {
                   out[pchar_out_string_position++] = '?';
               }
               break;
           default:
               out[pchar_out_string_position++] = in[pchar_in_string_position++];
        }
    }
    out[pchar_out_string_position++] = 0;
}

VOID ClearSearchItem(SEARCHITEM &SearchItem)
{
    ZeroMemory(&SearchItem,sizeof(SEARCHITEM));
}
#define MaskSet(n) (SearchItem.FlagMask[(SearchItemFlag)n])
#define Flag(n) (SearchItem.Flag[(SearchItemFlag)n])
#define RequireFlag(flag,value) {if (MaskSet(flag) && Flag(flag)!=(CHAR)((value)!=0)) return false;}

BOOL ItemMatchesSearch(SEARCHITEM &SearchItem, PCONTENTS pContents)
{
    if (SearchItem.ID && GetItemFromContents(pContents)->ItemNumber!=SearchItem.ID)
        return false;
    RequireFlag(Lore,GetItemFromContents(pContents)->Lore);
    RequireFlag(NoRent,GetItemFromContents(pContents)->NoRent);
    RequireFlag(NoDrop,GetItemFromContents(pContents)->NoDrop);
    RequireFlag(Magic,GetItemFromContents(pContents)->Magic);
    RequireFlag(Pack,GetItemFromContents(pContents)->Type==ITEMTYPE_PACK);
    RequireFlag(Book,GetItemFromContents(pContents)->Type==ITEMTYPE_BOOK);
    RequireFlag(Combinable,GetItemFromContents(pContents)->ItemType==17);
    RequireFlag(Summoned,GetItemFromContents(pContents)->Summoned);
    RequireFlag(Instrument,GetItemFromContents(pContents)->InstrumentType);
    RequireFlag(Weapon,GetItemFromContents(pContents)->Damage && GetItemFromContents(pContents)->Delay);
    RequireFlag(Normal,GetItemFromContents(pContents)->Type==ITEMTYPE_NORMAL);

    CHAR szName[MAX_STRING] = {0};
    if (SearchItem.szName[0] && !strstr(_strlwr(strcpy(szName,GetItemFromContents(pContents)->Name)),SearchItem.szName))
        return FALSE;

    return true;
}

BOOL SearchThroughItems(SEARCHITEM &SearchItem, PCONTENTS* pResult, DWORD *nResult)
{
    // TODO
#define Result(pcontents,nresult)     {\
    if (pResult) \
    *pResult=pcontents;\
    if (nResult)\
    *nResult=nresult;\
    return TRUE;}

    if (MaskSet(Worn) && Flag(Worn))
    {
        // iterate through worn items
        for (unsigned long N = 0 ; N < 21 ; N++)
        {
            if (PCONTENTS pContents=GetCharInfo2()->pInventoryArray->InventoryArray[N])
                if (ItemMatchesSearch(SearchItem,pContents))
                    Result(pContents,N);
        }
    }

    if (MaskSet(Inventory) && Flag(Inventory))
    {
        unsigned long nPack;
        // iterate through inventory slots before in-pack slots
        for (nPack = 0 ; nPack<10 ; nPack++)
        {
            if (PCONTENTS pContents=GetCharInfo2()->pInventoryArray->Inventory.Pack[nPack])
            {
                if (ItemMatchesSearch(SearchItem,pContents))
                    Result(pContents,nPack+21);
            }
        }
        for (nPack = 0 ; nPack<10 ; nPack++)
        {
            if (PCONTENTS pContents=GetCharInfo2()->pInventoryArray->Inventory.Pack[nPack]) {
                if (GetItemFromContents(pContents)->Type==ITEMTYPE_PACK && pContents->pContentsArray)
                {
                    for (unsigned long nItem = 0 ; nItem<GetItemFromContents(pContents)->Slots ; nItem++)
                    {
                        if (PCONTENTS pItem=pContents->pContentsArray->Contents[nItem])
                            if (ItemMatchesSearch(SearchItem,pItem))
                                Result(pItem,nPack*100+nItem);
                    }
                }
			}
        }
    }
    // TODO
    return 0;
}
#undef TestFlag
#undef Flag
#undef MaskSet

VOID ClearSearchSpawn(PSEARCHSPAWN pSearchSpawn)
{
    if (!pSearchSpawn) return;
    ZeroMemory(pSearchSpawn,sizeof(SEARCHSPAWN));
    // 0? fine. set anything thats NOT zero.
    pSearchSpawn->MaxLevel = MAX_NPC_LEVEL;
    pSearchSpawn->SpawnType = NONE;
    pSearchSpawn->GuildID = 0xFFFF;
    pSearchSpawn->ZRadius = 10000.0f;
    pSearchSpawn->FRadius = 10000.0f;
}

// *************************************************************************** 
// Function:    DistanceToPoint 
// Description: Return the distance between a spawn and the specified point 
// *************************************************************************** 
FLOAT DistanceToPoint(PSPAWNINFO pSpawn, FLOAT xLoc, FLOAT yLoc) 
{ 
    FLOAT X = pSpawn->X - xLoc; 
    FLOAT Y = pSpawn->Y - yLoc; 
    return sqrtf(X*X + Y*Y); 
}
// *************************************************************************** 
// Function:    GetSpellRestrictions 
// Author: htw
// *************************************************************************** 
PCHAR GetSpellRestrictions(PSPELL pSpell, unsigned int nIndex, PCHAR szBuffer)
{
	CHAR szTemp[MAX_STRING] = { 0 };
	if (!szBuffer)
		return NULL;
	if (!pSpell)
	{
		szBuffer[0] = 0;
		return(szBuffer);
	}
	switch (pSpell->Base2[nIndex])
	{
	case 0:		  strcpy(szTemp, "None"); break;
	case 100:     strcpy(szTemp, "Only work on Animal or Humanoid"); break;
	case 101:     strcpy(szTemp, "Only work on Dragon"); break;
	case 102:     strcpy(szTemp, "Only work on Animal or Insect"); break;
	case 104:     strcpy(szTemp, "Only work on Animal"); break;
	case 105:     strcpy(szTemp, "Only work on Plant"); break;
	case 106:     strcpy(szTemp, "Only work on Giant"); break;
	case 108:     strcpy(szTemp, "Doesn't work on Animals or Humanoids"); break;
	case 109:     strcpy(szTemp, "Only work on Bixie"); break;
	case 110:     strcpy(szTemp, "Only work on Harpy"); break;
	case 111:     strcpy(szTemp, "Only work on Gnoll"); break;
	case 112:     strcpy(szTemp, "Only work on Sporali"); break;
	case 113:     strcpy(szTemp, "Only work on Kobold"); break;
	case 114:     strcpy(szTemp, "Only work on Shade"); break;
	case 115:     strcpy(szTemp, "Only work on Drakkin"); break;
	case 117:     strcpy(szTemp, "Only work on Animals or Plants"); break;
	case 118:     strcpy(szTemp, "Only work on Summoned"); break;
	case 119:     strcpy(szTemp, "Only work on Fire_Pet"); break;
	case 120:     strcpy(szTemp, "Only work on Undead"); break;
	case 121:     strcpy(szTemp, "Only work on Living"); break;
	case 122:     strcpy(szTemp, "Only work on Fairy"); break;
	case 123:     strcpy(szTemp, "Only work on Humanoid"); break;
	case 124:     strcpy(szTemp, "Undead HP Less Than 10%"); break;
	case 125:     strcpy(szTemp, "Clockwork HP Less Than 45%"); break;
	case 126:     strcpy(szTemp, "Wisp HP Less Than 10%"); break;
	case 190:     strcpy(szTemp, "Doesn't work on Raid Boss"); break;
	case 191:     strcpy(szTemp, "Only work on Raid Boss"); break;
	case 201:     strcpy(szTemp, "HP Above 75%"); break;
	case 203:     strcpy(szTemp, "HP Less Than 20%"); break;
	case 204:     strcpy(szTemp, "HP Less Than 50%"); break;
	case 216:     strcpy(szTemp, "Not In Combat"); break;
	case 221:     strcpy(szTemp, "At Least 1 Pet On Hatelist"); break;
	case 222:     strcpy(szTemp, "At Least 2 Pets On Hatelist"); break;
	case 223:     strcpy(szTemp, "At Least 3 Pets On Hatelist"); break;
	case 224:     strcpy(szTemp, "At Least 4 Pets On Hatelist"); break;
	case 225:     strcpy(szTemp, "At Least 5 Pets On Hatelist"); break;
	case 226:     strcpy(szTemp, "At Least 6 Pets On Hatelist"); break;
	case 227:     strcpy(szTemp, "At Least 7 Pets On Hatelist"); break;
	case 228:     strcpy(szTemp, "At Least 8 Pets On Hatelist"); break;
	case 229:     strcpy(szTemp, "At Least 9 Pets On Hatelist"); break;
	case 230:     strcpy(szTemp, "At Least 10 Pets On Hatelist"); break;
	case 231:     strcpy(szTemp, "At Least 11 Pets On Hatelist"); break;
	case 232:     strcpy(szTemp, "At Least 12 Pets On Hatelist"); break;
	case 233:     strcpy(szTemp, "At Least 13 Pets On Hatelist"); break;
	case 234:     strcpy(szTemp, "At Least 14 Pets On Hatelist"); break;
	case 235:     strcpy(szTemp, "At Least 15 Pets On Hatelist"); break;
	case 236:     strcpy(szTemp, "At Least 16 Pets On Hatelist"); break;
	case 237:     strcpy(szTemp, "At Least 17 Pets On Hatelist"); break;
	case 238:     strcpy(szTemp, "At Least 18 Pets On Hatelist"); break;
	case 239:     strcpy(szTemp, "At Least 19 Pets On Hatelist"); break;
	case 240:     strcpy(szTemp, "At Least 20 Pets On Hatelist"); break;
	case 250:     strcpy(szTemp, "HP Less Than 35%"); break;
	case 304:     strcpy(szTemp, "Chain Plate Classes"); break;
	case 399:     strcpy(szTemp, "HP Between 15 and 25%"); break;
	case 400:     strcpy(szTemp, "HP Between 1 and 25%"); break;
	case 401:     strcpy(szTemp, "HP Between 25 and 35%"); break;
	case 402:     strcpy(szTemp, "HP Between 35 and 45%"); break;
	case 403:     strcpy(szTemp, "HP Between 45 and 55%"); break;
	case 404:     strcpy(szTemp, "HP Between 55 and 65%"); break;
	case 412:     strcpy(szTemp, "HP Above 99%"); break;
	case 501:     strcpy(szTemp, "HP Below 5%"); break;
	case 502:     strcpy(szTemp, "HP Below 10%"); break;
	case 503:     strcpy(szTemp, "HP Below 15%"); break;
	case 504:     strcpy(szTemp, "HP Below 20%"); break;
	case 505:     strcpy(szTemp, "HP Below 25%"); break;
	case 506:     strcpy(szTemp, "HP Below 30%"); break;
	case 507:     strcpy(szTemp, "HP Below 35%"); break;
	case 508:     strcpy(szTemp, "HP Below 40%"); break;
	case 509:     strcpy(szTemp, "HP Below 45%"); break;
	case 510:     strcpy(szTemp, "HP Below 50%"); break;
	case 511:     strcpy(szTemp, "HP Below 55%"); break;
	case 512:     strcpy(szTemp, "HP Below 60%"); break;
	case 513:     strcpy(szTemp, "HP Below 65%"); break;
	case 514:     strcpy(szTemp, "HP Below 70%"); break;
	case 515:     strcpy(szTemp, "HP Below 75%"); break;
	case 516:     strcpy(szTemp, "HP Below 80%"); break;
	case 517:     strcpy(szTemp, "HP Below 85%"); break;
	case 518:     strcpy(szTemp, "HP Below 90%"); break;
	case 519:     strcpy(szTemp, "HP Below 95%"); break;
	case 521:     strcpy(szTemp, "Mana Below X%"); break;
	case 522:     strcpy(szTemp, "End Below 40%"); break;
	case 523:     strcpy(szTemp, "Mana Below 40%"); break;
	case 603:     strcpy(szTemp, "Only work on Undead2"); break;
	case 608:     strcpy(szTemp, "Only work on Undead3"); break;
	case 624:     strcpy(szTemp, "Only work on Summoned2"); break;
	case 701:     strcpy(szTemp, "Doesn't work on Pets"); break;
	case 818:     strcpy(szTemp, "Only work on Undead4"); break;
	case 819:     strcpy(szTemp, "Doesn't work on Undead4"); break;
	case 825:     strcpy(szTemp, "End Below 21%"); break;
	case 826:     strcpy(szTemp, "End Below 25%"); break;
	case 827:     strcpy(szTemp, "End Below 29%"); break;
	case 836:     strcpy(szTemp, "Only work on Regular Servers"); break;
	case 837:     strcpy(szTemp, "Doesn't work on Progression Servers"); break;
	case 842:     strcpy(szTemp, "Only work on Humanoid Level 84 Max"); break;
	case 843:     strcpy(szTemp, "Only work on Humanoid Level 86 Max"); break;
	case 844:     strcpy(szTemp, "Only work on Humanoid Level 88 Max"); break;
	case 1000:     strcpy(szTemp, "Between Level 1 and 75"); break;
	case 1001:     strcpy(szTemp, "Between Level 76 and 85"); break;
	case 1002:     strcpy(szTemp, "Between Level 86 and 95"); break;
	case 1003:     strcpy(szTemp, "Between Level 96 and 100"); break;
	case 1004:     strcpy(szTemp, "HP Less Than 80%"); break;
	case 38311:     strcpy(szTemp, "Mana Below 20%"); break;
	case 38312:     strcpy(szTemp, "Mana Below 10%"); break;
	default: strcpy(szTemp, "Unknown"); break;
	}
	strcpy(szBuffer, szTemp);
	return szBuffer;
}

// *************************************************************************** 
// Function:    GetSpellEffectName 
// Description: Return spell effect string 
// Usage:       Used by ShowSpellSlotInfo to list limited effect types 
// Author:      Koad [Taken from his MQ2SpellSearch Plugin
// *************************************************************************** 
PCHAR GetSpellEffectName(LONG EffectID, PCHAR szBuffer) 
{
	if(EffectID<=MAX_SPELLEFFECTS) {
		strcat(szBuffer,szSPATypes[EffectID]);
	} else {
		CHAR szTemp[MAX_STRING]={0};
		sprintf_s(szTemp, "UndefinedEffect%03d", EffectID);
		strcat(szBuffer,szTemp);
    }
	return szBuffer; 
} 

// *************************************************************************** 
// Function:    ShowSpellSlotInfo 
// Author:      Koad (used in his SpellSearch Plugin) 
// *************************************************************************** 
PCHAR ShowSpellSlotInfo(PSPELL pSpell, PCHAR szBuffer) 
{ 
    CHAR szTemp[MAX_STRING]={0}; 
    CHAR szBuff[MAX_STRING]={0}; 
    CHAR buf[MAX_STRING]={0}; 
    int szBase=0; 
    bool bSlotIsPH=false; 
    PITEMDB ItemDB=gItemDB; 

    for (int i=0; i<=11; i++) 
    { 
        sprintf(szTemp, "Slot %d: ", i+1); 
        strcpy(szBuff, szTemp); 
        bSlotIsPH=false; 
        szTemp[0]='\0';
		LONG SPAID = pSpell->Attrib[i];
		//we do this here we can dump a crapload of cases where
		//this is the only thing added to szBuff...
		GetSpellEffectName(SPAID,szBuff);
        switch(SPAID)
        { 
        case 0: //hp +/-: heals/regen/dd 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            if ( pSpell->DurationValue1 > 0 ) strcat(szBuff, " per tick"); 
            if (pSpell->Base2[i] > 99) {
				GetSpellRestrictions(pSpell,i,szTemp);
				strcat(szBuff, " -- Restrictions: ");
				strcat(szBuff, szTemp);
			}
			break; 
        case 1: //ac mod 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i, AC_EFFECTS); 
            break; 
        case 2: //attack mod 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            if ( pSpell->DurationValue1 > 0 ) strcat(szBuff, " per tick"); 
            break; 
        case 3: //movement speed mod 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            if ( pSpell->Max[i] == 0 ) { 
                _itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            } else { 
                SlotValueCalculate(szBuff, pSpell, i); 
            } 
            strcat(szBuff, "%"); 
            break; 
        case 4: //str mod 
        case 5: //dex mod 
        case 6: //agi mod 
        case 7: //sta mod 
        case 8: //int mod 
        case 9: //wis mod 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 10: //old lure slot: leaving this since even though patched, som 
            //spells still have a lure effect slot when base = (-6) 
            //this slot is also charisma 
            if ( (abs(pSpell->Base[i]) > 0) && (pSpell->Base[i] != -6) ) { 
                if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
                if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
                strcat(szBuff, " by "); 
                SlotValueCalculate(szBuff, pSpell, i); 
            } else { 
                if (pSpell->Base[i] == -6 ){ 
                    strcat(szBuff, "Lure(6)"); 
                } else { 
                    bSlotIsPH=true; 
                } 
            } 
            break; 
        case 11: //haste mod 
            //if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            //if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
			
            // Ziggy 31/12/04: Some haste buffs (Miraculous Visions) don't fill in the max slot, so use base instead. 
            if (pSpell->Max[i] == 0) { 
                szBase=pSpell->Base[i]-100; 
            } else { 
                szBase=pSpell->Max[i]-100; 
            } 
            if ( szBase < 0 ) strcat(szBuff, " Decrease"); 
            if ( szBase > 0 ) strcat(szBuff, " Increase"); 
			
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(szBase), szTemp, 10); strcat(szBuff, szTemp); 
            //strcat(szBuff, "%"); 
            break; 
        case 12: //Invisibility 
        case 13: //See Invisible(c) 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 14: //Water Breathing 
            break; 
        case 15: //mana +/- 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i);
			if ( pSpell->DurationValue1 > 0 ) strcat(szBuff, " per tick");
			if (pSpell->Base2[i] > 99) {
				GetSpellRestrictions(pSpell,i,szTemp);
				strcat(szBuff, " -- Restrictions: ");
				strcat(szBuff, szTemp);
			}
            break; 
        case 16: //NPC-Frenzy
        case 17: //NPC-Awareness
        case 18: //NPC Aggro
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i);
			if ( pSpell->DurationValue1 > 0 ) strcat(szBuff, " per tick");
            break; 
        case 19: //NPC Faction
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i);
			strcat(szBuff, "%");
            break; 
        case 20: //Blindness 
            break; 
        case 21: //stun  time = base in ms 
            sprintf(szTemp, "(%.1f sec/%d)", (float)(pSpell->Base[i] / 1000), pSpell->Max[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 22: //Charm(c/level) 
            sprintf(szTemp, "(%d/%d)", pSpell->Base[i], pSpell->Max[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 23: //Fear(c) 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 24: //Fatigue 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 25: //Bind Affinity 
            if (pSpell->Base[i]==2)
				strcat(szBuff, " (Secondary Bind Point)"); 
            break; 
        case 26: //Gate 
            if (pSpell->Base2[1]==1) { 
                break; 
            } else if (pSpell->Base2[1]==2) { 
                strcat(szBuff, " to Secondary Bind Point"); 
                break;        
            } 
        case 27: //Cancel Magic(c) 
        case 28: //Invisibility versus Undead 
        case 29: //Invisibility versus Animal 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 30: //NPC-ReactRange(c/level) 
        case 31: //Mesmerize(c/level) 
            sprintf(szTemp, "(%d/%d)", pSpell->Base[i], pSpell->Max[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 32: //Create Item
            while ((ItemDB) && ((DWORD)pSpell->Base[i] != ItemDB->ID)) { 
                ItemDB = ItemDB->pNext; 
            } 
            if (ItemDB) { 
                strcpy(szTemp,ItemDB->szName); 
            } else { 
                sprintf(szTemp,"Unknown%5d",(DWORD)pSpell->Base[i]); 
            } 
            strcat(szBuff, ": ");   strcat(szBuff, szTemp);
			sprintf(szTemp, "(Qty:%d)", pSpell->Calc[i]);
			strcat(szBuff, szTemp);
            break; 
        case 33: //Spawn NPC 
            sprintf(szTemp, ": %s", pSpell->Extra); 
            strcat(szBuff, szTemp); 
            break; 
        case 34: //Confuse
            break; 
        case 35: //disease counters 
        case 36: //poison counters 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " Counter by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 37: //DetectHostile 
        case 38: //DetectMagic 
        case 39: //DetectPoison 
        case 40: //Invulnerability 
        case 41: //Banish
            break; 
        case 42: //Shadow Step(c) 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 43: //Berserk
        case 44: //Lycanthropy 
        case 45: //Vampirism 
            break; 
        case 46: //fire resist 
        case 47: //cold resist 
        case 48: //poison resist 
        case 49: //disease resist 
        case 50: //magic resist 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 51: //Detect Traps
        case 52: //Sense Undead 
        case 53: //Sense Summoned 
        case 54: //Sense Animals 
            break; 
        case 55: //most runes 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 56: //True North 
            break; 
        case 57: //Levitate(c) 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 58: //Illusion: Base=Race 
            strcat(szBuff, " : "); strcat(szBuff, pEverQuest->GetRaceDesc(pSpell->Base[i])); 
            break; 
        case 59: //Damage Shield 
            //base value sign has opposite effect.. 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 60: //Transfer Item
        case 61: //Identify 
        case 62: //Item ID
            break; 
        case 63: //Memblur e% 
            sprintf(szTemp, "Memblur %d%%", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            //strcat(szBuff, "%"); 
            break; 
        case 64: //SpinStun 
        case 65: //Infravision 
        case 66: //ultravision 
        case 67: //Eye of Zomm 
        case 68: //Reclaim Energy
            break; 
        case 69: //max hp mod 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 70: //CorpseBomb
			break;
        case 71: //Create Undead Pet
            sprintf(szTemp, ": %s", pSpell->Extra); 
            strcat(szBuff, szTemp); 
            break; 
        case 72: //Preserve Corpse
        case 73: //Bind Sight 
        case 74: //Feign Death 
        case 75: //Voice Graft 
        case 76: //Sentinel 
        case 77: //Locate Corpse 
            break; 
        case 78: //Absorb Magic Damage 
        case 79: //+hp when cast (priest buffs that have heal component, DoTs with DDs) 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 80: //Enchant:Light
            break; 
        case 81: //Resurrect
            sprintf(szTemp, " and restore %d%% experience", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 82: //Summon Player 
            break; 
        case 83: //zone portal spells 
            if (pSpell->TargetType==6)
				strcat(szBuff, " Self to "); 
            else strcat(szBuff, " Group to "); 
            sprintf(szTemp, "%d, %d, %d in %s facing %s", pSpell->Base[1], pSpell->Base[2], pSpell->Base[3], GetFullZone(GetZoneID(pSpell->Extra)), szHeading[pSpell->Base[4]]); 
            strcat(szBuff, szTemp); 
            break; 
        case 84: //Toss on Z axis 
            //base value sign has opposite effect.. 
            if ( pSpell->Base[i] > 0 ) sprintf(szTemp, " Down(%d)", abs(pSpell->Base[i])); 
            if ( pSpell->Base[i] < 0 ) sprintf(szTemp, " Up(%d)", abs(pSpell->Base[i])); 
            strcat(szBuff, szTemp); 
            break; 
        case 85: //Add Proc 
            sprintf(szTemp,": %s", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp); 
            break; 
        case 86: //Reaction Radius(c/level) 
            sprintf(szTemp, "(%d/%d)", pSpell->Base[i], pSpell->Max[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 87: //perspective magnification 
            szBase=pSpell->Base[i]-100; 
            if ( szBase < 0 ) strcat(szBuff, " Decrease"); 
            if ( szBase > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(szBase), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 88: //evac portal spells 
            sprintf(szTemp, " to %d, %d, %d in %s facing %s", 
                pSpell->Base[1], pSpell->Base[2], pSpell->Base[3], pSpell->Extra, szHeading[pSpell->Base[4]]); 
            strcat(szBuff, szTemp); 
            break; 
        case 89: //player size 
            szBase=pSpell->Base[i]-100; 
            if ( szBase < 0 ) strcat(szBuff, " Decrease"); 
            if ( szBase > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(szBase), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 90: //Cloak 
        case 91: //Summon Corpse 
            break;
        case 92: //hate mod 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 93: //Stop Rain 
        case 94: //Make Fragile 
        case 95: //Sacrifice 
        case 96: //Silence 
            break; 
        case 97: //mana pool 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 98:  //Haste v2 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 99: //Root 
            break; 
        case 100: //hp mod: pet heals/regen 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            if ( pSpell->DurationValue1 > 0 ) strcat(szBuff, " per tick"); 
            break; 
        case 101: //Complete Heal (with duration) 
            strcat(szBuff, " (with duration)"); 
            break; 
        case 102: //Fearless 
        case 103: //Call Pet 
            break; 
        case 104: //zone translocate spells
            strcat(szBuff, " to ");
            if(pSpell->Extra[0])
            {
                if(pSpell->Extra[0]=='0') //wtf?
                {
                    // Spell: Teleport
                    strcat(szTemp, "Bind Point");
                }
                else
                {
                    sprintf(szTemp, "%d, %d, %d in %s facing %s", pSpell->Base[1], pSpell->Base[2], pSpell->Base[3], GetFullZone(GetZoneID(pSpell->Extra)), szHeading[pSpell->Base[4]]);
                }
            }
            else
            {
                // Spell: Translocate
                strcat(szTemp, "Bind Point");
            }
            strcat(szBuff, szTemp);
            break;
        case 105: //Anti-Gate 
            sprintf(szTemp, "(%d)",pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 106: //Summon Warder 
            sprintf(szTemp, ": %s", pSpell->Extra); 
            strcat(szBuff, szTemp); 
            break; 
        case 107: //Alter NPC Level
			break;
        case 108: //Summon Familiar 
            sprintf(szTemp, ": %s", pSpell->Extra); 
            strcat(szBuff, szTemp); 
            break; 
        case 109: //Summon Into Bag 
            while ((ItemDB) && ((DWORD)pSpell->Base[i] != ItemDB->ID)) { 
                ItemDB = ItemDB->pNext; 
            } 
            if (ItemDB) { 
                strcpy(szTemp,ItemDB->szName); 
            } else { 
                sprintf(szTemp,"Unknown%5d",(DWORD)pSpell->Base[i]); 
            } 
            strcat(szBuff, ": ");   strcat(szBuff, szTemp); 
            break; 
        case 110: //Increase Archery
			strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i);
			break;
        case 111: //Resistances 
        case 112: //Casting Level 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 113: //Summon Mount 
            sprintf(szTemp, ": %s", pSpell->Extra); 
            strcat(szBuff, szTemp); 
            break; 
        case 114: //aggro multiplier 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 115: //Food/Water 
            break; 
        case 116: //curse counters 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 117: //Make Weapons Magical 
            strcat(szBuff, " (Make Weapons Magical)"); 
            break; 
        case 118: //Singing Skill 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 119: //bard overhaste 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 120: //Reduce Healing Effectiveness (%) 
            if (pSpell->Base[i] > 0) strcat(szBuff, " Increase"); 
            if (pSpell->Base[i] < 0) strcat(szBuff, " Decrease"); 
            strcat(szBuff, " Healing Effectiveness by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 121: //Reverse Damage Shield 
            sprintf(szTemp, " (%d)", abs(pSpell->Base[i])); 
            strcat(szBuff, szTemp); 
            break; 
        case 122: //Reduce Skill
			break;
        case 123: //Immunity
            sprintf(szTemp,"(%d)",pSpell->Base[i]); 
            strcat(szBuff,szTemp); 
            break; 
        case 124: //spell damage 
        case 125: //healing 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 126: //spell resist rate 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%",abs(pSpell->Base[i])); 
            //strcat(szBuff,szTemp); 
            break; 
        case 127: //spell haste 
        case 128: //spell duration 
        case 129: //spell range 
        case 130: //spell/bash hate 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 131: //Decrease Chance of Using Reagent 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 132: //Spell Mana Cost - signs reversed 
        case 133: //Spell Mana Cost - signs reversed 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 134: //limit max level 
            if(pSpell->Base2[i]>0) 
            { 
                sprintf(szTemp, "(%d) (lose %d%% per level over cap)", pSpell->Base[i],pSpell->Base2[i]); 
            } 
            else 
            { 
                sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            } 
            strcat(szBuff, szTemp); 
            break; 
        case 135: //Limit: Resist 
            strcat(szBuff, " "); 
            switch(pSpell->Base[i])
            { 
            case 1: strcat(szBuff, "Magic"); break; 
            case 2: strcat(szBuff, "Fire"); break; 
            case 3: strcat(szBuff, "Cold/Ice"); break; 
            case 4: strcat(szBuff, "Poison"); break; 
            case 5: strcat(szBuff, "Disease"); break; 
            case 6: strcat(szBuff, "Chromatic"); break; 
            case 7: strcat(szBuff, "Prismatic"); break; 
            default: strcat(szBuff, "Unknown"); break; 
            } 
            if ( pSpell->Base[i] > 0 )  strcat(szBuff, " allowed"); 
            if ( pSpell->Base[i] < 0 )  strcat(szBuff, " excluded"); 
            break; 
        case 136: //limit target types this affects 
            strcat(szBuff, "("); 
            //sprintf(buf, "$spell(%d,targettype)", pSpell->TargetType); 
            //parmSpell(buf, szTemp, NULL );       // <---- wtf. 
            //strcat(szBuff,szTemp); 

            switch(abs(pSpell->Base[i])) 
            { 
			case 50:  strcat(szBuff,"Target AE No Players Pets"); break; // blanket of forgetfullness. beneficial, AE mem blur, with max targets
			case 47:  strcat(szBuff,"Pet Owner"); break;
			case 46:  strcat(szBuff,"Target of Target"); break;
			case 45:  strcat(szBuff,"Free Target"); break;
			case 44:  strcat(szBuff,"Beam"); break;
			case 43:  strcat(szBuff,"Single in Group"); break;
			case 42:  strcat(szBuff,"Directional AE"); break;
			case 39:  strcat(szBuff,"No Pets"); break;
			case 38:  strcat(szBuff,"Pet2"); break;
			case 37:  strcat(szBuff,"Caster PB NPC"); break;
			case 36:  strcat(szBuff,"Caster PB PC"); break;
			case 35:  strcat(szBuff,"Special Muramites"); break;
			case 34:  strcat(szBuff,"Chest"); break;
			case 33:  strcat(szBuff,"Hatelist2"); break;
			case 32:  strcat(szBuff,"Hatelist"); break;
            case 41:  strcat(szBuff,"Group v2"); break; 
            case 40:  strcat(szBuff,"AE PC v2"); break; 
            case 25:  strcat(szBuff,"AE Summoned"); break; 
            case 24:  strcat(szBuff,"AE Undead"); break; 
            case 20:  strcat(szBuff,"Targeted AE Tap"); break; 
            case 18:  strcat(szBuff,"Uber Dragons"); break; 
            case 17:  strcat(szBuff,"Uber Giants"); break; 
            case 16:  strcat(szBuff,"Plant"); break; 
            case 15:  strcat(szBuff,"Corpse"); break; 
            case 14:  strcat(szBuff,"Pet"); break; 
            case 13:  strcat(szBuff,"LifeTap"); break; 
            case 11:  strcat(szBuff,"Summoned"); break; 
            case 10:  strcat(szBuff,"Undead"); break; 
            case  9:  strcat(szBuff,"Animal"); break; 
            case  8:  strcat(szBuff,"Targeted AE"); break; 
            case  6:  strcat(szBuff,"Self"); break; 
            case  5:  strcat(szBuff,"Single"); break; 
            case  4:  strcat(szBuff,"PB AE"); break; 
            case  3:  strcat(szBuff,"Group v1"); break; 
            case  2:  strcat(szBuff,"AE PC v1"); break; 
            case  1:  strcat(szBuff,"Line of Sight"); break; 
            default:  strcat(szBuff,"Unknown"); break; 
            } 

            //GetTargetType(pSpell->Base[i],szTemp);       strcat(szBuff,szTemp); 
            if ( pSpell->Base[i] > 0 )  strcat(szBuff, " allowed"); 
            if ( pSpell->Base[i] < 0 )  strcat(szBuff, " excluded"); 
            strcat(szBuff, ")"); 
            break; 
        case 137: //limit effect types this affects 
            strcat(szBuff, "("); 
            GetSpellEffectName(pSpell->Base[i],szBuff);
            if ( pSpell->Base[i] > 0 )  strcat(szBuff, " allowed"); 
            if ( pSpell->Base[i] < 0 )  strcat(szBuff, " excluded"); 
            strcat(szBuff, ")"); 
            break; 
        case 138: //limit spelltype this affects 
            strcat(szBuff, "("); 
            switch(pSpell->Base[i]) 
            { 
            case 0: 
                strcat(szBuff,"Detrimental only"); 
                break; 
            case 1: 
                strcat(szBuff,"Beneficial only"); 
                break; 
            case 2: 
                strcat(szBuff,"Beneficial - Group Only"); 
                break; 
            default: 
                sprintf(szTemp, "Unknown%03d", pSpell->Base[i]); 
                strcat(szBuff,szTemp); 
                break; 
            } 
            strcat(szBuff, ")"); 
            break; 
        case 139: //limit spell this affects 
            strcat(szBuff, "("); 
            // changed this case to work for any future changes by referencing 
            // the spell array for the name 
            strcat(szBuff, GetSpellByID(abs(pSpell->Base[i]))->Name); 
            if ( pSpell->Base[i] > 0 )  strcat(szBuff, " allowed"); 
            if ( pSpell->Base[i] < 0 )  strcat(szBuff, " excluded"); 
            strcat(szBuff, ")"); 
            break; 
        case 140: //limit min duration of spells this affects (base= #ticks) 
            sprintf(szTemp, "(%d sec)", (pSpell->Base[i] *6)); 
            strcat(szBuff, szTemp); 
            break; 
        case 141: //limit to instant spells only 
            strcat(szBuff, " spells only"); 
            break; 
        case 142: //Limit: Min Level 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 143: //limit min casting time of spells this affects (base= seconds in ms) 
        case 144: //limit max casting time of spells this affects (base= seconds in ms) 
            sprintf(szTemp, "(%.1f sec)", (float)(pSpell->Base[i] / 1000)); 
            strcat(szBuff, szTemp); 
            break; 
        case 145: //Teleportv2 
            sprintf(szTemp, " to %d, %d, %d in %s facing %s", 
                pSpell->Base[1], pSpell->Base[2], pSpell->Base[3], pSpell->Extra, szHeading[pSpell->Base[4]]); 
            strcat(szBuff, szTemp); 
            break; 
        case 146: //Resist Electricity
			break;
        case 147: //Hit Points (75% Max) 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            sprintf(szTemp, " by %d (%d%% max)", pSpell->Max[i], pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 148: //Stacking: Block 
            sprintf(szTemp, " new spell if slot %d is effect '", pSpell->Calc[i]-200); 
            strcat(szBuff, szTemp); 
            GetSpellEffectName(pSpell->Base[i],szBuff);
            if ( pSpell->Max[i] > 0) { 
                strcat(szBuff, "' and < "); 
                SlotValueCalculate(szBuff, pSpell, i); 
                //_itoa(abs(pSpell->Max[i]), szTemp, 10); 
                //strcat(szBuff, szTemp); 
            } 
            break; 
        case 149: //Stacking: Overwrite 
            sprintf(szTemp, " existing spell if slot %d is effect '", pSpell->Calc[i]-200); 
            strcat(szBuff, szTemp); 
            GetSpellEffectName(pSpell->Base[i],szBuff);
            if ( pSpell->Max[i] > 0) { 
                strcat(szBuff, "' and < "); 
                SlotValueCalculate(szBuff, pSpell, i); 
                //_itoa(abs(pSpell->Max[i]), szTemp, 10); 
                //strcat(szBuff, szTemp); 
            } 
            break; 
        case 150: //Death Save - Restore Full Health 
            sprintf(szTemp, " - Restore Full Health"); 
            strcat(szBuff, szTemp); 
            break; 
        case 151: //Suspended Minion 
            strcat(szBuff, " ("); 
            switch(pSpell->Base[i]) 
            { 
            case 0: 
                strcat(szBuff,"Current HP Only"); 
                break; 
            case 1: 
                strcat(szBuff,"Current HP, Buffs, Weapons"); 
                break; 
            default: 
                sprintf(szTemp, "Unknown%03d", pSpell->Base[i]); 
                strcat(szBuff,szTemp); 
                break; 
            } 
            strcat(szBuff, ")"); 
            break; 
        case 152: //Summon Pets (swarm) 
            sprintf(szTemp, ": %s x%d for %dsec", pSpell->Extra, pSpell->Base[i], pSpell->Max[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 153: //Balance Party Health 
            sprintf(szTemp, "(%d%% penalty)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 154: //Remove Detrimental(c) 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 155: //PoP Resurrect
        case 156: //Illusion: Target 
            break; 
        case 157: //Spell-Damage Shield 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 158: //Chance to Reflect Spell 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " Spell by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //_itoa(abs(pSpell->Base[i]), szTemp, 10); strcat(szBuff, szTemp); 
            strcat(szBuff, "%"); 
            break; 
        case 159: //Stats 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", abs(pSpell->Base[i])); 
            //strcat(szBuff, szTemp); 
            break; 
        case 160: //Drunk effect 
            sprintf(szTemp," if Alcholol Tolerance is above %d",pSpell->Base[i]); 
            strcat(szBuff,szTemp); 
            break; 
        case 161: //Mitigate Spell Damage 
            if (pSpell->Max[i]>0) sprintf(szTemp, " by %d%% until %d absorbed", pSpell->Base[i], pSpell->Max[i]);
            else sprintf(szTemp, " by %d%%", pSpell->Base[i]);
            strcat(szBuff, szTemp); 
            break; 
        case 162: //Mitigate Melee Damage 
            if (pSpell->Max[i]>0) sprintf(szTemp, " by %d%% until %d absorbed", pSpell->Base[i], pSpell->Max[i]); 
            else sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 163: //Absorb damage 
            sprintf(szTemp, " (melee or spell, calc unknown)"); 
            strcat(szBuff, szTemp); 
            break; 
        case 164: //Attempt Sense (Cursed) Trap 
        case 165: //Attempt Disarm (Cursed) Trap 
        case 166: //Attempt Destroy (Cursed) Lock 
            sprintf(szTemp, "(%d,%d)", pSpell->Base[i], pSpell->Max[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 167: //Increase Pet Power 
            sprintf(szTemp, "(%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 168: //Mitigation 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 169: //Chance to Critical Hit 
            sprintf(szTemp, " for %s by %d%%", pSpell->Base2[i]>=0?szSkills[pSpell->Base2[i]]:"All Skills",pSpell->Base[i]);
            strcat(szBuff, szTemp); 
            break; 
        case 170: //Crit Cast, test spell only 
            sprintf(szTemp, " (unknown calc)"); 
            strcat(szBuff, szTemp); 
            break; 
        case 171: //Crippling Blow 
        case 172: //Melee Avoidance 
        case 173: //Riposte 
        case 174: //Dodge 
        case 175: //Parry 
        case 176: //Dual Wield 
        case 177: //Stat Cap Mod (how do they know which?) 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 178: //Lifetap Proc 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 179: //Puretone 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 180: //Spell Resist 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 181: //Fearless 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 182: //Hundred Hands 
            break; 
        case 183: //Melee Fury 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 184: //Chance to hit with Backstab (or throwing/archery [http://lucy.allakhazam.com/spellraw.html?id=9616&source=Live])
            sprintf(szTemp, " by %d%% with %s", pSpell->Base[i], pSpell->Base2[i]>=0?szSkills[pSpell->Base2[i]]:"All Skills");
            strcat(szBuff, szTemp); 
            break; 
        case 185: //Damage Mod (how to tell which, rogues get a backstab only, others get an all skills) 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 186: //Damage Mod (see above) 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 187: //Mana Balance
			sprintf(szTemp, "(%d%% penalty)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 188: //Block 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 189: //Endurance DoT/Regen 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, " per tick"); 
            //sprintf(szTemp, " by %d per tick", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 190: // Endurance pool 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp," by %d",abs(pSpell->Base[i])); 
            //strcat(szBuff,szTemp); 
            break; 
        case 191: //Amnesia
			break;
        case 192: //Discord Hate 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 193: //Skill Attack 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 194: //Fade 
            break; 
        case 195: //Stun Resist 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 196: //Strikethrough 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 197: //Skill Damage 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 198: //Endurance Heals 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 199: //Taunt
			break;
        case 200: //Proc Mod 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 201: //Ranged Proc 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 202: //Illusion 
        case 203: //Mass Group Buff
            break; 
        case 204: //War Cry 
            sprintf(szTemp, " (%d0.00 sec)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 205: //Rampage 
        case 206: //AE Taunt 
        case 207: //Flesh to Bone 
            break; 
        case 209: // Disspell beneficial buffs 
            sprintf(szTemp," (%d)",pSpell->Base[i]); 
            strcat(szBuff,szTemp); 
            break; 
        case 210: //Pet Shield 
            sprintf(szTemp, " (%d.00 sec)", (12*(pSpell->Base[i]))); 
            strcat(szBuff, szTemp); 
            break; 
        case 211: //AE Melee 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 212: //Frenzied Devastation 
            sprintf(szTemp, " (%d0.00 sec)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 213://Pet HP
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp);
            break; 
        case 214: //Change Max HP
            sprintf(szTemp, " by (%d)", ((pSpell->Base[i])/100)); 
            strcat(szBuff, szTemp); 
            break; 
        case 215: //Pet Avoidance
        case 216: //Accuracy 
        case 217: //Headshot 
        case 218: //Pet Crit Melee 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 219: //Slay undead (Holyforge) 
            sprintf(szTemp," (%d)",pSpell->Base[i]); 
            strcat(szBuff,szTemp); 
            break; 
        case 220: //Skill Damage Amt 
        case 221: //Reduce Weight
        case 222: //Block Behind 
        case 223: //Double Riposte 
        case 224: //AddRiposte
        case 225: //Give Dbl Atk 
        case 226: //2h bash 
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 227: //Reduce Skill Timer
        case 228: //Reduce Fall Dmg 
        case 229: //Cast Through Stun 
        case 230: //Increase Shield Dist
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 231: //Stun Bash Chance
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 232: //Divine Rez 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
		case 233: //Metabolism
        case 234: //Poison Mastery
        case 235: //Focus Channelling
        case 236: //Free Pet
        case 237: //Pet Affinity
        case 238: //Permanent Illusion
        case 239: //Stonewall
        case 240: //String Unbreakable
        case 241: //Improve Reclaim Energy
        case 242: //Increase Chance Memwipe
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 243: //NoBreak Charm Chance
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 244: //Root Break Chance
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 245: //Trap Circumvention
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 246: //Lung Capacity
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 247: //Increase SkillCap
        case 248: //Extra Specialization
        case 249: //Offhand Min
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 250: //Spell Proc Chance
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 251: //Endless Quiver
        case 252: //Backstab Front
        case 253: //Chaotic Stab
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 254: //placeholder of some kind 
            bSlotIsPH=true; 
            break;
		case 255: //Shielding Duration Mod
		case 256: //Shroud Of Stealth
		case 257: //Give Pet Hold
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 258: //Triple Backstab 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 259: //AC Limit Mod
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 260: //test puretone 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
		case 261: //Song Mod Cap
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 262: //Stats Cap
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 

            switch (pSpell->Base2[i]) { 
            case 0:   strcat (szBuff, "STR");   break; 
            case 1:   strcat (szBuff, "STA");   break; 
            case 2:   strcat (szBuff, "AGI");   break; 
            case 3:   strcat (szBuff, "DEX");   break; 
            case 4:   strcat (szBuff, "WIS");   break; 
            case 5:   strcat (szBuff, "INT");   break; 
            case 6:   strcat (szBuff, "CHA");   break; 
            default: 
                sprintf (szTemp, "UnknownStat(%d)", pSpell->Base2[i]); 
                strcat (szBuff, szTemp); 
            } 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", abs(pSpell->Base[i])); 
            //strcat(szBuff, szTemp); 
            break; 
        case 263: //Tradeskill Masteries
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 264: //Reduce AATimer
            sprintf(szTemp, " for %d ticks", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 265: //No Fizzle
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 266: //Attack Chance 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
		case 267: //Add Pet Commands
        case 268: //Alc Fail Rate
        case 269: //Bandage Perc Limit
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 270: //Bard Song Range
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " to "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " to %d", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 271: //BaseRunMod
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 272: //CastingLevel
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " to "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " to %d", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 273: //DoT Crit 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 274: //Heal Crit 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
		case 275: //Critical Mend
        case 276: //Dual Wield Amt
        case 277: //Extra DI Chance
        case 278: //Finishing Blow
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 279: //Flurry 
		case 280: //Pet Flurry Chance
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 281: //Give Pet Feign
        case 282: //Increase Bandage Amt
        case 283: //Special Attack Chain
        case 284: //LoH Set Heal
        case 285: //NoMove Check Sneak
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 286: //Additional Damage to Spell 
		case 287: //Fc_DurationMod (static)
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff,szTemp); 
            break;
        case 288: //Add Proc Hit
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 289: //Improved Spell Effect 
            sprintf(szTemp,": %s",GetSpellByID(pSpell->Base[i])->Name); 
            strcat(szBuff,szTemp); 
            break; 
        case 290://Increase Movement Cap
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
		case 291: //Purify
		case 292: //Strikethrough2
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 293: //StunResist2
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 294: //Spell Crit Chance
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 295: //Reduce Timer Special
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 296: //Blight 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 297: //Fc_Damage_Amt_Incoming 
            //sprintf(szTemp, " (%d) (Empathy? Please Check)", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 298: //Tiny Companion 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 299: //Wake the Dead 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 300: //Doppleganger 
			break;
        case 301: //Increase Range Damage
            sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 302: //Fc_Damage % Crit
            sprintf(szTemp, " (%d%%)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 303: //Fc_Damage Amt Crit
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 304: //Secondary Riposte Mod
            sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 305: //Mitigate Damage Shield
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 306: //Army of Dead 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 307: //Army of Dead (Identify?)
        case 308: //Suspend minion 
        case 309: //Teleport Bind 
            break; 
        case 310: //Reuse Timer 
            sprintf(szTemp, " by %d.00 sec", ((pSpell->Base[i])/1000)); 
            strcat(szBuff, szTemp); 
            break; 
        case 311: //No Combat Skills 
			break;
        case 312: //Sanc 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 313: //Forage Master
            sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 314: //Improved Invis
        case 315: //Improved Invis Undead
        case 316: //Improved Invis Animals
			break;
        case 317: //Worn Regen Cap
        case 318: //Worn Mana Cap
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 319: //Critical HP Regen
        case 320: //Shield Block Chance
        case 321: //Soothing 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
        case 322: //Origin 
            break; 
        case 323: //Add Defensive Proc 
            sprintf(szTemp,": %s", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp); 
            break; 
        case 324: //Spirit Channel
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
			break;
		case 325: //No Break AE Sneak
        case 326: //Spell Slots
        case 327: //Buff Slots
            sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 328: //Negative HP Limit
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 329: //Mind over Matter 
        case 330: //Crit Damage 
            sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 331: //Alchemy Item Recovery
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 332: //Call of the Wild 
            break; 
        case 333: //Trigger on fade 
            sprintf(szTemp,": %s on Fade",GetSpellByID(pSpell->Base[i])->Name); 
            strcat(szBuff,szTemp); 
            break; 
        case 334: //Song DoT 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            if ( pSpell->DurationValue1 > 0 ) strcat(szBuff, " per tick"); 
            //sprintf(szTemp, " Decrease Hitpoints by %d (L%d) to %d(L%d)", (pSpell->Max[i] + pSpell->Base[i]), pSpell->Level[Bard], pSpell->Max[i],MAX_PC_LEVEL); 
            //strcat(szBuff, szTemp); 
            break;
		case 335: //Fc_Immunity Focus
			break;
		case 336: //Illusionary Target
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
		case 337: //Experience buff 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%",pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 338: //Expedient Recovery
			break;
        case 339: //Cast DoT as well 
        case 340: //Cast DD as well 
            if (pSpell->Base[i]<100) sprintf(szTemp, "(%d%% Chance, Spell: %s)", pSpell->Base[i], GetSpellNameByID(pSpell->Base2[i])); 
            else sprintf(szTemp, "(Spell: %s)", GetSpellNameByID(pSpell->Base2[i])); 
            strcat(szBuff, szTemp); 
            break; 
        case 341: //Worn Attack Cap
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d",pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 342: //No Panic
			break;
        case 343: //Spell Interrupt
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%",pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 344: //Item Channeling
        case 345: //Assassinate Max
        case 346: //Headshot Max
        case 347: //Double Ranged Attack
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 348: //Ff_Mana Min
			sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 349: //Increase Damage With Shield
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 350: //Manaburn
            strcat(szBuff, " for "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " for %d",pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 351: //Spawn Interactive Object
			break;
        case 352: //Increase Trap Count
        case 353: //Increase SOI Count
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d",pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 354: //Deactivate All Traps
        case 355: //Learn Trap
			sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 356: //Change Trigger Type
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 357: //Fc_Mute
			sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 358: //Instant Mana
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 359: //Passive Sense Trap
			sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
		case 360: //Killshot Triggers 
            if (pSpell->Base2[i]) sprintf(szTemp,"Chance to Trigger on Non-Trivial Killshot: %s", GetSpellByID(pSpell->Base2[i])->Name); 
            strcat(szBuff,szTemp); 
            break; 
		case 361: //Proc On Death
			sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break; 
        case 362: //Potion Belt
        case 363: //Bandolier
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 364: //AddTripleAttackChance
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%",pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
		case 365: //Chance to Create EE on killshot 
            if (pSpell->Base2[i]) sprintf(szTemp,"Chance to Trigger on Non-Trivial Killshot: %s by %d%%", GetSpellByID(pSpell->Base2[i])->Name, pSpell->Base[i]); 
            strcat(szBuff,szTemp); 
            break; 
		case 366: //Group Shielding
			sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 367: //Change Body Type 
            sprintf(szTemp, " to "); 
            if (pSpell->Base[i]==25) strcat(szTemp, "Plant"); 
            else if (pSpell->Base[i]==21) strcat(szTemp, "Animal"); 
            else strcat(szTemp, "Unknown"); 
            strcat(szBuff,szTemp); 
            break;
		case 368: //Modify Faction
			switch(abs(pSpell->Base[i])) 
            {
			case 544:  sprintf(szTemp, "(Sebilisian Empire)"); break;
			case 566:  sprintf(szTemp, "(S.H.I.P. Workshop Base Population)"); break;
			case 567:  sprintf(szTemp, "(Jewel of Atiiki Efreetis)"); break;
			case 1229: sprintf(szTemp, "(Sebilisian Empire)"); break;
			default:  sprintf(szTemp, "(%d) (Please Check)", pSpell->Base[i]); break; 
            } 
            strcat(szBuff, szTemp); 
            break;
        case 369: //Corruption Counters 
            if (pSpell->Base[i] < 0) strcat(szBuff, " Decrease"); 
            if (pSpell->Base[i] > 0) strcat(szBuff, " Increase"); 
            strcat(szBuff," by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 370: //Corruption Resists 
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 371: //Slow
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%",pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
            break; 
        case 372: //Grant Foraging
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 373: //Doom Always
			sprintf(szTemp, ": %s on Fade", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
		case 374: //Trigger Spell
			if (pSpell->Base[i] < 100) sprintf(szTemp, ": %s Chance: %d%%", GetSpellNameByID(pSpell->Base2[i]), pSpell->Base[i]);
			else sprintf(szTemp, ": %s", GetSpellNameByID(pSpell->Base2[i])); 
            strcat(szBuff, szTemp);
			break;
		case 375: //Critical DoT Damage Mod
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 376: //Fling
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 377: //Doom Entity
			sprintf(szTemp, ": %s on Fade", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 378: //Resist Other SPA
			sprintf(szTemp, " (%d) (Please Check) by %d%%", pSpell->Base2[i], pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 379: //Directional Shadowstep
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
			break;
        case 380: //Knockback Explosive
			sprintf(szTemp, " (%d) and Toss Up (%d)", pSpell->Base[i], pSpell->Base2[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 381: //Fling to Self
			sprintf(szTemp, " (%d)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 382: //Suppression
            strcat(szBuff, ": Levitation Effect"); 
            break;
        case 383: //Fc_CastProcNormalized
			sprintf(szTemp, ": %s on Cast", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 384: //Fling to Target
        case 385: //Ff_WhichSpellGroup
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 386: //Doom Dispeller
			sprintf(szTemp, ": %s on Curer", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 387: //Doom Dispellee
			sprintf(szTemp, ": %s on Fade", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 388: //Summon All Corpses
			break;
        case 389: //Fc_Timer Refresh
        case 390: //Fc_Timer Lockout
        case 391: //Ff_Mana Max
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 392: //Fc_Heal Amt
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
			break;
        case 393: //Fc_Heal % Incoming
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 394: //Fc_Heal Amt Incoming
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            //sprintf(szTemp, " by %d", pSpell->Base[i]); 
			break;
        case 395: //Fc_Heal % Crit
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 396: //Fc_Heal Amt Crit
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 397: //Pet Amt Mitigation
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 398: //Fc_Swarm Pet Duration
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, " seconds"); 
            //sprintf(szTemp, " by %d seconds", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 399: //Fc_Twincast
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 400: //Healburn
            strcat(szBuff, " use up to "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, " mana to heal your group"); 
            //sprintf(szTemp, " use up to %d mana to heal your group", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 401: //Mana Ignite
        case 402: //Endurance Ignite
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by up to "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            if ( pSpell->DurationValue1 > 0 ) strcat(szBuff, " per tick"); 
            break; 
        case 403: //Ff_SpellClass
        case 404: //Ff_SpellSubclass
        case 405: //Staff Block Chance
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 406: //Doom Limit Use
			sprintf(szTemp, ": %s on Max Hits", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 407: //Doom Focus Used
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 408: //Limit HP
        case 409: //Limit Mana
        case 410: //Limit Endurance
            strcat(szBuff, " to "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 411: //Ff_ClassPlayer
        case 412: //Ff_Race
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 413: //Fc_BaseEffects
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 414: //Ff_CastingSkill
        case 415: //Ff_ItemClass
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 416: //AC2
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i, AC_EFFECTS); 
            break; 
        case 417: //Mana2
            if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            break; 
        case 418: //Increased Skill Damage2
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 419: //Contact Ability2
			sprintf(szTemp, ": %s", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 420: //Fc_Limit Use
        case 421: //Fc_Limit Use Amt
        case 422: //Ff_Limit Use Min
        case 423: //Ff_Limit Use Type
        case 424: //Gravitate
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 425: //Fly
			break;
        case 426: //AddExtTargetSlots
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 427: //Skill Proc
			sprintf(szTemp, ": %s on Fade", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 428: //Skill Proc Modifier
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 429: //Skill Proc Success
			sprintf(szTemp, ": %s", GetSpellNameByID(pSpell->Base[i])); 
            strcat(szBuff, szTemp);
			break;
        case 430: //PostEffect
        case 431: //PostEffectData
        case 432: //ExpandMaxActiveTrophyBenefits
        case 433: //CriticalDotDecay
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 434: //CriticalHealDecay
        case 435: //CriticalRegenDecay
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 436: //Beneficial Countdown Hold
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp); 
            break;
        case 437: //Teleport to Anchor
        case 438: //Translocate to Anchor
            switch (pSpell->Base[i]) { 
			case 50874:   strcat (szBuff, ": Guild Hall");   break; 
			case 52584:   strcat (szBuff, ": Primary");   break; 
			case 52585:   strcat (szBuff, ": Secondary");   break; 
            default: 
                sprintf (szTemp, "UnknownAnchor(%d)", pSpell->Base[i]); 
                strcat (szBuff, szTemp); 
            } 
        case 439: //Assassinate
        case 440: //FinishingBlowMax
        case 441: //Distance Removal
        case 442: //Doom Req Target
        case 443: //Doom Req Caster
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp);
            break;
        case 444: //Improved Taunt
			sprintf(szTemp, " to L%d and Reduce Ally Hate Generation by %d%%", pSpell->Base[i], pSpell->Base2[i]); 
            strcat(szBuff, szTemp);
            break;
        case 445: //Add Merc Slot
        case 446: //A_Stacker
        case 447: //B_Stacker
        case 448: //C_Stacker
        case 449: //D_Stacker
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp);
            break;
        case 450: //DoT Guard
        case 451: //Melee Threshold Guard
        case 452: //Spell Threshold Guard
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 453: //Doom Melee Threshold
        case 454: //Doom Spell Threshold
			sprintf(szTemp, ": %s on %d Damage Taken", GetSpellNameByID(pSpell->Base[i]), pSpell->Base2[i]); 
            strcat(szBuff, szTemp);
			break;
        case 455: //Add Hate %
        case 456: //Add Hate Over Time %
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        case 457: //Resource Tap
			sprintf(szTemp, " (%d) (Please Check)", pSpell->Base[i]); 
            strcat(szBuff, szTemp);
            break;
        case 458: //Faction Mod %
			if ( pSpell->Base[i] < 0 ) strcat(szBuff, " Decrease"); 
            if ( pSpell->Base[i] > 0 ) strcat(szBuff, " Increase"); 
            strcat(szBuff, " by "); 
            SlotValueCalculate(szBuff, pSpell, i); 
            strcat(szBuff, "%"); 
            //sprintf(szTemp, " by %d%%", pSpell->Base[i]); 
            //strcat(szBuff, szTemp); 
			break;
        default: //undefined effect 
            sprintf(szTemp, "UnknownEffect%03d", pSpell->Attrib[i]); 
            strcat(szBuff,szTemp); 
            break; 
        } 
        if ( !bSlotIsPH ) { 
            strcat(szBuffer, szBuff); 
            strcat(szBuffer, "<br>" ); 
        } 
    } 
    return szBuffer; 
} 

// *************************************************************************** 
// Function:    SlotValueCalculate
// Description: Interpret calc formula for spell slot effect i 
// Usage:       Used by ShowSpellSlotInfo to show scaling values 
//              mp = base multiplier (AC effects = 0.3000005, (for float rounding issues)
//                 default = 1.0) 
// *************************************************************************** 
VOID SlotValueCalculate(PCHAR szBuff, PSPELL pSpell, int i, double mp) 
{ 
    CHAR szTemp[MAX_STRING]={0};
	LONG SPAID = pSpell->Attrib[i];
    int level=0; 
	double incrementa=0.00;
	double incrementb=0.00;
    long szBase = abs(pSpell->Base[i]);
	long szMax = pSpell->Max[i];

    //find min level spell is usable 
	//int minlevel=pSpell->ClassLevel[1];
	int minlevel=((EQ_Spell*)pSpell)->GetSpellLevelNeeded(Warrior);
	int maxlevel=MAX_PC_LEVEL;
	int incrementblevel=0;
    for (int j=Warrior; j<=Berserker; j++) 
		if ( ((EQ_Spell*)pSpell)->GetSpellLevelNeeded(j)<minlevel )
			minlevel=((EQ_Spell*)pSpell)->GetSpellLevelNeeded(j);

    switch(pSpell->Calc[i]) 
    { 
    case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		incrementa=pSpell->Calc[i];
		break;
	case 14:
		incrementa=10.0;
		incrementb=20.0;
		incrementblevel=40;
	case 100: // Display Base[i]
		break;
	case 101: // Level/2
		incrementa=0.50;
		break;
	case 102:
	case 103:
	case 104:
	case 105:
		incrementa=pSpell->Calc[i]-101;
		break;
	case 109: // Level/4
		incrementa=0.25;
	case 110:
		incrementa=1.00;
		break;
	case 119: // Display Max[i]
	case 121: // Display Max[i]
		sprintf(szTemp, "%d", (long)(szMax*mp));
		strcat(szBuff, szTemp);
		return;
	case 123: // Display szBase[i] to Max[i] (random)
		sprintf(szTemp, "%d to %d (random)", abs(szBase), (long)(szMax*mp));
		strcat(szBuff, szTemp);
		return;
	case 201: // Display Max[i]
	case 202: // Display Max[i]
	case 203: // Display Max[i]
	case 204: // Display Max[i]
		sprintf(szTemp, "%d", (long)(szMax*mp));
		strcat(szBuff, szTemp);
		return;
    default: //undefined calc 
        sprintf(szTemp, "UnknownCalc: %d Range: %d->%d", pSpell->Calc[i], pSpell->Base[i], pSpell->Max[i]); 
        strcat(szBuff, szTemp); 
        return; 
    } 

	if (minlevel>MAX_PC_LEVEL) { minlevel=1; szBase=(long)(minlevel * (incrementa * mp) + szBase); }
	if (szMax==0) szMax=(long)((MAX_PC_LEVEL-minlevel) * (incrementa * mp) + (MAX_PC_LEVEL-incrementblevel) * (incrementb * mp) + szBase);
	// Modify Base/Max for Haste Mod
	if (SPAID==11) {
		szBase-=100;
		szMax-=100;
	}
	if (incrementa>0) 
		if (incrementb>0) maxlevel=(int)((szMax-szBase-(incrementa*incrementblevel)) / incrementb + incrementblevel + minlevel);
		else maxlevel=(int)((szMax-szBase) / incrementa + minlevel);
	if (maxlevel>MAX_PC_LEVEL) maxlevel=MAX_PC_LEVEL;
	if (SPAID==11) sprintf(szTemp, "%d%%", abs(szBase));
	else sprintf(szTemp, "%d", abs(szBase));
	strcat(szBuff, szTemp);
	if (szMax>szBase) {
		if (SPAID==11) sprintf(szTemp, " (L%d) to %d%% (L%d)", minlevel, (long)(szMax*mp), maxlevel);
		else sprintf(szTemp, " (L%d) to %d (L%d)", minlevel, (long)(szMax*mp), maxlevel);
		strcat(szBuff, szTemp);
	}
	//debugging info below
	//sprintf(szTemp, " (inc: %0.2f, mp: %0.1f, calc: %d)", increment, mp, pSpell->Calc[i]);
	//strcat(szBuff, szTemp);
}

VOID SlotValueCalculateOld(PCHAR szBuff, PSPELL pSpell, int i, double mp) 
{ 
    CHAR szTemp[MAX_STRING]={0};
	LONG SPAID = pSpell->Attrib[i];
    int level=0; 
	double incrementa=0.00;
	double incrementb=0.00;
    int szBase = abs(pSpell->Base[i]);
	int szMax = pSpell->Max[i];

    //find min level spell is usable 
	//int minlevel=pSpell->ClassLevel[1];
	int minlevel=((EQ_Spell*)pSpell)->GetSpellLevelNeeded(Warrior);
	int maxlevel=MAX_PC_LEVEL;
	int incrementblevel=0;
    for (int j=Warrior; j<=Berserker; j++) 
		if ( ((EQ_Spell*)pSpell)->GetSpellLevelNeeded(j)<minlevel )
			minlevel=((EQ_Spell*)pSpell)->GetSpellLevelNeeded(j);

    switch(pSpell->Calc[i]) 
    { 
    case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		incrementa=pSpell->Calc[i];
		break;
	case 14:
		incrementa=10.0;
		incrementb=20.0;
		incrementblevel=40;
	case 100: // Display Base[i]
		break;
	case 101: // Level/2
		incrementa=0.50;
		break;
	case 102:
	case 103:
	case 104:
	case 105:
		incrementa=pSpell->Calc[i]-101;
		break;
	case 109: // Level/4
		incrementa=0.25;
	case 110:
		incrementa=1.00;
		break;
	case 119: // Display Max[i]
	case 121: // Display Max[i]
		sprintf(szTemp, "%d", (long)(szMax*mp));
		strcat(szBuff, szTemp);
		return;
	case 123: // Display szBase[i] to Max[i] (random)
		sprintf(szTemp, "%d to %d (random)", abs(szBase), (long)(szMax*mp));
		strcat(szBuff, szTemp);
		return;
	case 201: // Display Max[i]
	case 202: // Display Max[i]
	case 203: // Display Max[i]
	case 204: // Display Max[i]
		sprintf(szTemp, "%d", (long)(szMax*mp));
		strcat(szBuff, szTemp);
		return;
    default: //undefined calc 
        sprintf(szTemp, "UnknownCalc: %d Range: %d->%d", pSpell->Calc[i], pSpell->Base[i], pSpell->Max[i]); 
        strcat(szBuff, szTemp); 
        return; 
    } 

	if (minlevel>MAX_PC_LEVEL) {
		minlevel=1;
		szBase=(long)(minlevel * (incrementa * mp) + szBase);
	}
	if (szMax==0) {
		szMax=(long)((MAX_PC_LEVEL-minlevel) * (incrementa * mp) + (MAX_PC_LEVEL-incrementblevel) * (incrementb * mp) + szBase);
	}
	// Modify Base/Max for Haste Mod
	if (SPAID==11) {
		if(pSpell->Base[i]>100)//its a hastespell
			szBase=pSpell->Base[i]-100;
		else //its a slowspell
			szBase=100-pSpell->Base[i];
		szMax=pSpell->Max[i];
	}
	if (incrementa>0) {
		if (incrementb>0) {
			maxlevel=(int)((szMax-szBase-(incrementa*incrementblevel)) / incrementb + incrementblevel + minlevel);
		} else {
			szBase = (int)(minlevel * incrementa) + szBase;
			maxlevel = ((szMax-szBase) * 2) + minlevel;
			maxlevel=(int)((szMax-szBase) / incrementa + minlevel);
		}
	}
	if (maxlevel>MAX_PC_LEVEL)
		maxlevel=MAX_PC_LEVEL;
	if (SPAID==11)
		sprintf(szTemp, "%d%%", abs(szBase));
	else
		sprintf(szTemp, "%d", abs(szBase));
	strcat(szBuff, szTemp);
	if (SPAID==11) {
		sprintf(szTemp, " (L%d) to %d%% (L%d)", minlevel, (long)(szMax*mp), maxlevel);
		strcat(szBuff, szTemp);
	} else if (szMax>szBase) {
		sprintf(szTemp, " (L%d) to %d (L%d)", minlevel, (long)(szMax*mp), maxlevel);
		strcat(szBuff, szTemp);
	}

	//debugging info below
	//sprintf(szTemp, " (inc: %0.2f, mp: %0.1f, calc: %d)", increment, mp, pSpell->Calc[i]);
	//strcat(szBuff, szTemp);
}

int FindMappableCommand(const char *name)
{
    for (unsigned long i = 0 ; i < nEQMappableCommands ; i++)
    {
        if((DWORD)szEQMappableCommands[i] == 0 || (DWORD)szEQMappableCommands[i] > (DWORD)__AC1_Data)
            continue;
        if (!stricmp(name,szEQMappableCommands[i]))
            return i;
    }
    return -1;
}    

void DisplayOverlayText(PCHAR szText, DWORD dwColor, DWORD dwTransparency, DWORD msFadeIn, DWORD msFadeOut, DWORD msHold)
{
    if (!pTextOverlay) {
        WriteChatColor(szText,dwColor);
        return;
    }
    DWORD dwAlpha = (DWORD)(dwTransparency*255/100);
    if (dwAlpha>255) dwAlpha=255;

    pTextOverlay->DisplayText(
        szText,
        dwColor,
        10, // Always 10 in eqgame.exe,
        // Doesn't seem to affect anything
        // (tried 0,1,10,20,100,500)
        dwAlpha,
        msFadeIn,
        msFadeOut,
        msHold);
}

BOOL ParseKeyCombo(PCHAR text, KeyCombo &Dest)
{
    KeyCombo Ret;
    if (!stricmp(text,"clear"))
    {
        Dest=Ret;
        return true;
    }
    CHAR Copy[MAX_STRING];
    strcpy(Copy,text);
    text=strtok(Copy,"+ ");
    while(text)
    {
        if (!stricmp(text,"alt"))
            Ret.Data[0]=1;
        else if (!stricmp(text,"ctrl"))
            Ret.Data[1]=1;
        else if (!stricmp(text,"shift"))
            Ret.Data[2]=1;
        else
        {
            for (unsigned long i=0 ; gDiKeyID[i].Id ; i++)
            {
                if (!stricmp(text,gDiKeyID[i].szName))
                {
                    Ret.Data[3]=(char)gDiKeyID[i].Id;
                    break;
                }
            }
        }
        text=strtok(NULL,"+ ");
    }
    if (Ret.Data[3])
    {
        Dest=Ret;
        return true;
    }
    return false;
}

PCHAR DescribeKeyCombo(KeyCombo &Combo, PCHAR szDest)
{
    unsigned long pos=0;
    szDest[0]=0;
    if (Combo.Data[2])
    {
        strcpy(&szDest[pos],"shift");
        pos+=5;
    }
    if (Combo.Data[1])
    {
        if (pos)
        {
            szDest[pos]='+';
            pos++;
        }
        strcpy(&szDest[pos],"ctrl");
        pos+=4;
    }
    if (Combo.Data[0])
    {
        if (pos)
        {
            szDest[pos]='+';
            pos++;
        }
        strcpy(&szDest[pos],"alt");
        pos+=3;
    }
    if (pos)
    {
        szDest[pos]='+';
        pos++;
    }
    if (Combo.Data[3])
    {
        strcpy(&szDest[pos],gDiKeyName[Combo.Data[3]]);
    }
    else
    {
        strcpy(&szDest[pos],"clear");
    }
    return &szDest[0];    
}

#ifndef ISXEQ
BOOL LoadCfgFile(PCHAR Filename, BOOL Delayed)
{
    FILE *file;
    CHAR szFilename[MAX_STRING]={0};
    strcpy(szFilename,Filename);
    if (!strchr(szFilename,'.'))
        strcat(szFilename,".cfg");
    CHAR szFull[MAX_STRING]={0};
#define TryFile(name)  \
    {\
    if (file=fopen(name,"rt"))\
    goto havecfgfile;\
    }
    sprintf(szFull,"%s\\Configs\\%s",gszINIPath,szFilename);
    TryFile(szFull);    
    sprintf(szFull,"%s\\%s",gszINIPath,szFilename);
    TryFile(szFull);
    TryFile(szFilename);
    TryFile(Filename);
#undef TryFile
    return false;
havecfgfile:
    CHAR szBuffer[MAX_STRING]={0};
    while(fgets(szBuffer,MAX_STRING,file))
    {
        PCHAR Cmd=strtok(szBuffer,"\r\n");
        if (Cmd && Cmd[0] && Cmd[0]!=';')
        {
            HideDoCommand(((PSPAWNINFO)pLocalPlayer),Cmd,Delayed);
        }
    }
    fclose(file);
    return true;
}
#endif

int FindInvSlotForContents(PCONTENTS pContents)
{
    int LastMatch = -1;

    // screw the old style InvSlot numbers
    // return the index into the INVSLOTMGR array
    DebugSpew("FindInvSlotForContents(0x%08X) (0x%08X)",pContents,GetItemFromContents(pContents));

#if 1
    PEQINVSLOTMGR pInvMgr=(PEQINVSLOTMGR)pInvSlotMgr;
    for (unsigned long N = 0 ; N < 0x800 ; N++)
    {
        class CInvSlot *pCIS = NULL;
        struct _CONTENTS *pC = NULL;

        if (pInvMgr->SlotArray[N]) {
            pCIS = (class CInvSlot *)pInvMgr->SlotArray[N];

            pCIS->GetItemBase(&pC);

            if (pC) {
                DebugSpew("pInvSlotMgr->SlotArray[%d] Contents==0x%08X",N,pC);
                if (pC==pContents) {

if (pInvMgr->SlotArray[N]->pInvSlotWnd) {
DebugSpew("%d slot %d wnd %d %d %d", N, pInvMgr->SlotArray[N]->InvSlot, 
    pInvMgr->SlotArray[N]->pInvSlotWnd->WindowType,
    pInvMgr->SlotArray[N]->pInvSlotWnd->InvSlot,
    pInvMgr->SlotArray[N]->pInvSlotWnd->BagSlot
    );
}
                    // if it is in the primary inventory,
                    // then pInvSlotWnd->WindowType is 0
                    if (pInvMgr->SlotArray[N]->pInvSlotWnd && pInvMgr->SlotArray[N]->pInvSlotWnd->WindowType == 0) {
                        return pInvMgr->SlotArray[N]->InvSlot; 
                    } else if (pInvMgr->SlotArray[N]->pInvSlotWnd && pInvMgr->SlotArray[N]->pInvSlotWnd->BagSlot!=65535) {
                        return pInvMgr->SlotArray[N]->InvSlot; 
                    } else if (pInvMgr->SlotArray[N]->pInvSlotWnd && pInvMgr->SlotArray[N]->pInvSlotWnd->WindowType==11) {
                        // loot window items should not be anywhere else
                        return pInvMgr->SlotArray[N]->InvSlot; 
                    } else {
                        LastMatch = N;
                    }
                }
            }
        }
    }
    // return specific window type if needed
    if (LastMatch != -1 && pInvMgr->SlotArray[LastMatch]->pInvSlotWnd->WindowType== 9999)
        return  pInvMgr->SlotArray[LastMatch]->InvSlot;
#endif

#if 0
    for(DWORD n = 0; n < NUM_INV_SLOTS; n++)
    {
        if(GetCharInfo2()->pInventoryArray->InventoryArray[n] && GetCharInfo2()->pInventoryArray->InventoryArray[n] == pContents)
        {
            //DebugSpew("Found '%s' at %d", GetCharInfo2()->pInventoryArray->InventoryArray[n]->Item->Name, n);
            return n;
        }
    }

    unsigned long nPack;
    for (nPack=0 ; nPack < 10 ; nPack++)
    {
        PCHARINFO pCharInfo=GetCharInfo();
        if (PCONTENTS pPack=GetCharInfo2()->pInventoryArray->Inventory.Pack[nPack])
        {
            if (pPack->Item->Type==ITEMTYPE_PACK && pContents->pContentsArray)
            {
                for (unsigned long nItem=0 ; nItem < pPack->Item->Slots ; nItem++)
                {
                    //DebugSpew("Pack[%d]->pContentsArray->Contents[%d]==0x%08X",nPack,nItem,pPack->pContentsArray->Contents[nItem]);
                    if (pPack->pContentsArray->Contents[nItem]==pContents)
                    {
                        return 262+(nPack*10)+nItem;
                    }
                }
            }
        }
    }

    for (nPack=0 ; nPack < NUM_BANK_SLOTS ; nPack++)
    {
        PCHARINFO pCharInfo=GetCharInfo();
        if (PCONTENTS pPack=pCharInfo->pBankArray->Bank[nPack])
        {
            if (pPack==pContents)
            {
                if (nPack<0x18)
                    return 2000+nPack;
                return 2500+nPack-0x10;
            }
            if (pPack->Item->Type==ITEMTYPE_PACK && pContents->pContentsArray)
            {
                for (unsigned long nItem=0 ; nItem < pPack->Item->Slots ; nItem++)
                {
                    if (pPack->pContentsArray->Contents[nItem]==pContents)
                    {
                        if (nPack<0x18)
                            return 2032+(nPack*10)+nItem;
                        return 2532+((nPack-0x18)*10)+nItem;
                    }
                }
            }
        }        
    }
#endif

    return -1;
}

DWORD LastFoundInvSlot=-1;

int FindInvSlot(PCHAR pName, BOOL Exact)
{
    CHAR Name[MAX_STRING]={0};
    strlwr(strcpy(Name,pName));
    CHAR szTemp[MAX_STRING]={0};
    PEQINVSLOTMGR pInvMgr=(PEQINVSLOTMGR)pInvSlotMgr;
    for (unsigned long N = 0 ; N < 0x800 ; N++)
    {
        if (pInvMgr->SlotArray[N])
        {
            class CInvSlot *x = (class CInvSlot *)pInvMgr->SlotArray[N];
            struct _CONTENTS *y = NULL;

            if (x) 
               x->GetItemBase(&y);

            if (y)
            {
                if (!Exact)
                {
                    _strlwr(strcpy(szTemp,GetItemFromContents(y)->Name));
                    if (strstr(szTemp,Name))
                    {
                        if (pInvMgr->SlotArray[N]->pInvSlotWnd)
                        {
                            LastFoundInvSlot=N;
                            return pInvMgr->SlotArray[N]->InvSlot;
                        }
                        // let it try to find it in an open slot if this fails
                    }
                }
                else if (!stricmp(Name,GetItemFromContents(y)->Name))
                {
                    if (pInvMgr->SlotArray[N]->pInvSlotWnd)
                    {
                        LastFoundInvSlot=N;
                        return pInvMgr->SlotArray[N]->InvSlot;
                    }
                    // let it try to find it in an open slot if this fails
                }

            }
        }
    }
    LastFoundInvSlot=-1;
    return -1;
}

int FindNextInvSlot(PCHAR pName, BOOL Exact)
{
    CHAR szTemp[MAX_STRING]={0};
    CHAR Name[MAX_STRING]={0};
    strlwr(strcpy(Name,pName));

#if 0
    PEQINVSLOTMGR pInvMgr=(PEQINVSLOTMGR)pInvSlotMgr;
    for (unsigned long N = LastFoundInvSlot+1 ; N < 0x800 ; N++)
    {
        if (pInvMgr->SlotArray[N])
        {
            if (pInvMgr->SlotArray[N]->ppContents && *pInvMgr->SlotArray[N]->ppContents)
            {
                if (!Exact)
                {
                    _strlwr(strcpy(szTemp,(*pInvMgr->SlotArray[N]->ppContents)->Item->Name));
                    if (strstr(szTemp,Name))
                    {
                        if (pInvMgr->SlotArray[N]->pInvSlotWnd)
                        {
                            LastFoundInvSlot=N;
                            return pInvMgr->SlotArray[N]->pInvSlotWnd->InvSlot;
                        }
                        // let it try to find it in an open slot if this fails
                    }
                }
                else if (!stricmp(Name,(*pInvMgr->SlotArray[N]->ppContents)->Item->Name))
                {
                    if (pInvMgr->SlotArray[N]->pInvSlotWnd)
                    {
                        LastFoundInvSlot=N;
                        return pInvMgr->SlotArray[N]->pInvSlotWnd->InvSlot;
                    }
                    // let it try to find it in an open slot if this fails
                }

            }
        }
    }
#endif
    LastFoundInvSlot=-1;
    return -1;
}
/*
BOOL ActualCalculate(PCHAR szFormula, DOUBLE &Result) {
    CHAR Buffer[MAX_STRING] = {0};
    DWORD i = 0, j=0, k=0;
    CHAR Arg[MAX_STRING][100] = {0};
    for (i=0;i<strlen(szFormula);i++) {
        if (szFormula[i] == '(')
        {
            BOOL Quote=false;
            PCHAR pStart=&szFormula[i+1];
            unsigned long nParens=1;
            while(nParens)
            {
                i++;
                if (szFormula[i]==0)
                {
                    FatalError("Calculate encountered an unmatched parenthesis");
                    return false;
                }
                if (szFormula[i]==')')
                {
                    if (!Quote)
                        nParens--;
                }
                else if (szFormula[i]=='(')
                {
                    if (!Quote)
                        nParens++;
                }
                else if (szFormula[i]=='\"')
                {
                    Quote=!Quote;
                }
            }
            szFormula[i]=0;
            DOUBLE CalcResult;
            if (ActualCalculate(pStart,CalcResult))
            {
                szFormula[i]=')';
                j+=sprintf(&Buffer[j],"%f",CalcResult);
            }
            else
            {
                // error condition would have been reported by the calculate already, dont give another
                szFormula[i]=')';
                return false;
            }
        }
        else
            if (szFormula[i] != ' ') 
                Buffer[j++]=szFormula[i];
    }
    Buffer[j]=0;
    j=0;
    k=0;
    for (i=0;Buffer[i];i++) {
        switch (Buffer[i]) {
case '-':
    if (k==0) {
        Arg[j][k] = Buffer[i];
        k++;
        break;
    }
case '+':
case '*':
case '\\':
case '/':
case '%':
case '^':
    Arg[j+1][0]=Buffer[i];
    j+=2;
    k=0;
    break;
case '&':
    if (Buffer[i+1]=='&')
    {
        Arg[j+1][0]='a';
        j+=2;
        k=0;
        i++;
    }
    else
    {
        Arg[j+1][0]='&';
        j+=2;
        k=0;
    }
    break;
case '|':
    if (Buffer[i+1]=='|')
    {
        Arg[j+1][0]='o';
        j+=2;
        k=0;
        i++;
    }
    else
    {
        Arg[j+1][0]='|';
        j+=2;
        k=0;
    }
    break;
case '!':
    if (Buffer[i+1]=='=')
    {
        Arg[j+1][0]='n';
        j+=2;
        k=0;
        i++;
    }
    else
    {
        Arg[j+1][0]='!';
        j+=2;
        k=0;
    }
    break;
case '=':
    if (Buffer[i+1]=='=')
    {
        Arg[j+1][0]='=';
        j+=2;
        k=0;
        i++;
    }
    else
    {
        FatalError("Calculate encountered a bad = formation");
        return false;
    }
    break;
case '>':
    if (Buffer[i+1]=='=')
    {
        Arg[j+1][0]=(char)0xf2;
        j+=2;
        k=0;
        i++;
    }
    else
    {
        Arg[j+1][0]='>';
        j+=2;
        k=0;
    }
    break;
case '<':
    if (Buffer[i+1]=='=')
    {
        Arg[j+1][0]=(char)0xf3;
        j+=2;
        k=0;
        i++;
    }
    else
    {
        Arg[j+1][0]='<';
        j+=2;
        k=0;
    }
    break;
case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '.':
    Arg[j][k] = Buffer[i];
    k++;
    break;
case '�':
case '�':
    //              GracefullyEndBadMacro(((PCHARINFO)pCharData)->pSpawn,gMacroBlock, "Calculate encountered a unparsed variable '%s'",&(Buffer[i]));
    return false;
default:
    FatalError("Calculate encountered unparsable text '%s'",&Buffer[i]);
    return false;
        }
    }
    j++;
    //for (i=0;i<j;i++) DebugSpewNoFile("%d. %s",i,Arg[i]);
    for (i=0;i<j;i++) {
        switch (Arg[i][0]) {
case '!':
    if (i+1==j) {
        FatalError("Calculate encountered a bad %c formation",Arg[i][0]);
        return false;
    }
    i--;
    if (atof(Arg[i+2])!=0.0f)
    {
        strcpy(Arg[i],"0");
    }
    else
        strcpy(Arg[i],"1");
    if (i==0)
    {
        for (k=i+1;k<j;k++) 
            strcpy(Arg[k],Arg[k+2]);
    }
    else
    {
        for (k=i+1;k<j;k++) 
            strcpy(Arg[k],Arg[k+2]);
    }
    j--;
        }
    }
    for (i=0;i<j;i++) {
        switch (Arg[i][0]) {
case '^':
    if ((i==0) || (i+1==j)) {
        FatalError("Calculate encountered a bad %c formation",Arg[i][0]);
        return false;
    }
    i--;
    sprintf(Buffer,"%f",pow(atof(Arg[i]),atof(Arg[i+2])));
    strcpy(Arg[i],Buffer);
    j-=2;
    for (k=i+1;k<j;k++) strcpy(Arg[k],Arg[k+2]);
        }
    }
    for (i=0;i<j;i++) {
        switch (Arg[i][0]) {
case '%':
case '/':
case '\\':
case '*':
    if ((i==0) || (i+1==j)) {
        FatalError("Calculate encountered a bad %c formation",Arg[i][0]);
        return false;
    }
    i--;
    switch (Arg[i+1][0]) {
case '%':
    if (atof(Arg[i+2])==0) 
    {
        return false;
    }
    ltoa(atoi(Arg[i])%atol(Arg[i+2]),Buffer,10);
    break;
case '\\':
    if (atof(Arg[i+2])==0) 
    {
        return false;
    }
    ltoa((LONG)(atol(Arg[i])/atol(Arg[i+2])),Buffer,10);
    break;
case '/':
    if (atof(Arg[i+2])==0) 
    {
        return false;
    }
    sprintf(Buffer,"%f",(atof(Arg[i])/atof(Arg[i+2])));
    break;
case '*':
    sprintf(Buffer,"%f",(atof(Arg[i])*atof(Arg[i+2])));
    break;
    }
    strcpy(Arg[i],Buffer);
    j-=2;
    for (k=i+1;k<j;k++) strcpy(Arg[k],Arg[k+2]);
        }
    }
    for (i=0;i<j;i++) {
        switch (Arg[i][0]) {
case '-':
    if (Arg[i][1] != 0) break;
case '+':
    if ((i==0) || (i+1==j)) {
        FatalError("Calculate encountered a bad %c formation",Arg[i][0]);
        return false;
    }
    i--;
    switch (Arg[i+1][0]) {
case '+':
    sprintf(Buffer,"%f",(atof(Arg[i])+atof(Arg[i+2])));
    break;
case '-':
    sprintf(Buffer,"%f",(atof(Arg[i])-atof(Arg[i+2])));
    break;
    }
    strcpy(Arg[i],Buffer);
    j-=2;
    for (k=i+1;k<j;k++) strcpy(Arg[k],Arg[k+2]);
        }
    }
    for (i=0;i<j;i++) {
        switch ((UCHAR)Arg[i][0]) {
case 'n':
case '<':
case '>':
case '=':
case 0xf2:
case 0xf3:
    if ((i==0) || (i+1==j)) {
        FatalError("Calculate encountered a bad %c formation",Arg[i][0]);
        return false;
    }
    i--;
    switch ((UCHAR)Arg[i+1][0]) {
case '<':
    sprintf(Buffer,"%d",atof(Arg[i])<atof(Arg[i+2]));
    break;
case '>':
    sprintf(Buffer,"%d",atof(Arg[i])>atof(Arg[i+2]));
    break;
case 'n':
    sprintf(Buffer,"%d",atof(Arg[i])!=atof(Arg[i+2]));
    break;
case '=':
    sprintf(Buffer,"%d",atof(Arg[i])==atof(Arg[i+2]));
    break;
case 0xf2:
    sprintf(Buffer,"%d",atof(Arg[i])>=atof(Arg[i+2]));
    break;
case 0xf3:
    sprintf(Buffer,"%d",atof(Arg[i])<=atof(Arg[i+2]));
    break;
    }
    strcpy(Arg[i],Buffer);
    j-=2;
    for (k=i+1;k<j;k++) strcpy(Arg[k],Arg[k+2]);
        }
    }

    for (i=0;i<j;i++) {
        switch (Arg[i][0]) {
case '&':
case '|':
    if ((i==0) || (i+1==j)) {
        FatalError("Calculate encountered a bad %c formation",Arg[i][0]);
        return false;
    }
    i--;
    switch (Arg[i+1][0]) {
case '&':
    sprintf(Buffer,"%d",atol(Arg[i])&atol(Arg[i+2]));
    break;
case '|':
    sprintf(Buffer,"%d",atol(Arg[i])|atol(Arg[i+2]));
    break;
    }
    strcpy(Arg[i],Buffer);
    j-=2;
    for (k=i+1;k<j;k++) strcpy(Arg[k],Arg[k+2]);
        }
    }

    for (i=0;i<j;i++) {
        switch (Arg[i][0]) {
case 'a':
case 'o':
    if ((i==0) || (i+1==j)) {
        FatalError("Calculate encountered a bad %c formation",Arg[i][0]);
        return false;
    }
    i--;
    switch (Arg[i+1][0]) {
case 'a':
    {
        FLOAT A=(FLOAT)atof(Arg[i]);
        FLOAT B=(FLOAT)atof(Arg[i+2]);
        if ((A&&B)==0)
        {
            Result=0;
            return true;
        }
        strcpy(Buffer,"1"); // if its not zero ,it must be 1 ;)
        //                        sprintf(Buffer,"%d",atof(Arg[i])&&atof(Arg[i+2]));
    }
    break;
case 'o':
    {
        FLOAT A=(FLOAT)atof(Arg[i]);
        FLOAT B=(FLOAT)atof(Arg[i+2]);
        if ((A||B)==1)
        {
            Result=1;
            return true;
        }
        strcpy(Buffer,"0"); // if its not 1,it must be 0!
        //                            sprintf(Buffer,"%d",atof(Arg[i])||atof(Arg[i+2]));
    }
    break;
    }
    strcpy(Arg[i],Buffer);
    j-=2;
    for (k=i+1;k<j;k++) strcpy(Arg[k],Arg[k+2]);
        }
    }
    Result=atof(Arg[0]);
    return true;
}
/**/
#ifndef ISXEQ
enum eCalcOp
{
    CO_NUMBER=0,
    CO_OPENPARENS=1,
    CO_CLOSEPARENS=2,
    CO_ADD=3,
    CO_SUBTRACT=4,
    CO_MULTIPLY=5,
    CO_DIVIDE=6,
    CO_IDIVIDE=7,
    CO_LAND=8,
    CO_AND=9,
    CO_LOR=10,
    CO_OR=11,
    CO_XOR=12,
    CO_EQUAL=13,
    CO_NOTEQUAL=14,
    CO_GREATER=15,
    CO_NOTGREATER=16,
    CO_LESS=17,
    CO_NOTLESS=18,
    CO_MODULUS=19,
    CO_POWER=20,
    CO_LNOT=21,
    CO_NOT=22,
    CO_SHL=23,
    CO_SHR=24,
    CO_NEGATE=25,
    CO_TOTAL=26,
};

int CalcOpPrecedence[CO_TOTAL]=
{
    0,
    0,
    0,
    9,//add
    9,//subtract
    10,//multiply
    10,//divide
    10,//integer divide
    2,//logical and
    5,//bitwise and
    1,//logical or
    3,//bitwise or
    4,//bitwise xor
    6,//equal
    6,//not equal
    7,//greater
    7,//not greater
    7,//less
    7,//not less
    10,//modulus
    11,//power
    12,//logical not
    12,//bitwise not
    8,//shl
    8,//shr
    12,//negate
};

struct _CalcOp
{
    eCalcOp Op;
    DOUBLE Value;
};

BOOL EvaluateRPN(_CalcOp *pList, int Size, DOUBLE &Result)
{
    if (!Size)
        return 0;
    int StackSize=(sizeof(DOUBLE)*(Size/2+2));
    DOUBLE *pStack = (DOUBLE*) malloc(StackSize);
    int nStack=0;
#define StackEmpty() (nStack==0)
#define StackTop() (pStack[nStack])
#define StackSetTop(do_assign) {pStack[nStack]##do_assign;}
#define StackPush(val) {nStack++;pStack[nStack]=val;}
#define StackPop() {if (!nStack) {FatalError("Illegal arithmetic in calculation");free(pStack);return 0;};nStack--;}

#define BinaryIntOp(op) {int RightSide=(int)StackTop();StackPop();StackSetTop(=(DOUBLE)(((int)StackTop())##op##RightSide));}
#define BinaryOp(op) {DOUBLE RightSide=StackTop();StackPop();StackSetTop(=StackTop()##op##RightSide);}
#define BinaryAssign(op) {DOUBLE RightSide=StackTop();StackPop();StackSetTop(##op##=RightSide);}

#define UnaryIntOp(op) {StackSetTop(=op##((int)StackTop()));}
#define UnaryOp(op)    {StackSetTop(=op##(StackTop()));}
    for (int i = 0 ; i < Size ; i++)
    {
        switch(pList[i].Op)
        {
        case CO_NUMBER:
            StackPush(pList[i].Value);
            break;
        case CO_ADD:
            BinaryAssign(+);
            break;
        case CO_MULTIPLY:
            BinaryAssign(*);
            break;
        case CO_SUBTRACT:
            BinaryAssign(-);
            break;
        case CO_NEGATE:
            UnaryOp(-);
            break;
        case CO_DIVIDE:
            if (StackTop())
            {
                BinaryAssign(/);
            }
            else
            {
                //printf("Divide by zero error\n");
                FatalError("Divide by zero in calculation");
                free(pStack);
                return false;
            }
            break;
        case CO_IDIVIDE://TODO: SPECIAL HANDLING
            {
                int Right=(int)StackTop();
                if (Right)
                {
                    StackPop();
                    int Left=(int)StackTop();
                    Left/=Right;
                    StackSetTop(=Left);
                }
                else
                {
                    //printf("Integer divide by zero error\n");
                    FatalError("Divide by zero in calculation");
                    free(pStack);
                    return false;
                }
            }
            break;
        case CO_MODULUS://TODO: SPECIAL HANDLING
            {
                int Right=(int)StackTop();
                if (Right)
                {
                    StackPop();
                    int Left=(int)StackTop();
                    Left%=Right;
                    StackSetTop(=Left);
                }
                else
                {
                    //printf("Modulus by zero error\n");
                    FatalError("Modulus by zero in calculation");
                    free(pStack);
                    return false;
                }
            }
            break;
        case CO_LAND:
            BinaryOp(&&);
            break;
        case CO_LOR:
            BinaryOp(||);
            break;
        case CO_EQUAL:
            BinaryOp(==);
            break;
        case CO_NOTEQUAL:
            BinaryOp(!=);
            break;
        case CO_GREATER:
            BinaryOp(>);
            break;
        case CO_NOTGREATER:
            BinaryOp(<=);
            break;
        case CO_LESS:
            BinaryOp(<);
            break;
        case CO_NOTLESS:
            BinaryOp(>=);
            break;
        case CO_SHL:
            BinaryIntOp(<<);
            break;
        case CO_SHR:
            BinaryIntOp(>>);
            break;
        case CO_AND:
            BinaryIntOp(&);
            break;
        case CO_OR:
            BinaryIntOp(|);
            break;
        case CO_XOR:
            BinaryIntOp(^);
            break;
        case CO_LNOT:
            UnaryIntOp(!);
            break;
        case CO_NOT:
            UnaryIntOp(~);
            break;
        case CO_POWER:
            {
                DOUBLE RightSide=StackTop();
                StackPop();
                StackSetTop(=pow(StackTop(),RightSide));
            }
            break;
        }
    }
    Result=StackTop();

#undef StackEmpty
#undef StackTop
#undef StackPush
#undef StackPop
    free(pStack);
    return true;
}

BOOL FastCalculate(PCHAR szFormula, DOUBLE &Result)
{
    //DebugSpew("FastCalculate(%s)",szFormula);
    if (!szFormula || !szFormula[0])
        return false;
    int Length=(int)strlen(szFormula);
    int MaxOps=(Length+1);
    int ListSize=sizeof(_CalcOp)*MaxOps;
    int StackSize=sizeof(eCalcOp)*MaxOps;
    _CalcOp *pOpList=(_CalcOp *)malloc(ListSize);
    eCalcOp *pStack=(eCalcOp *)malloc(StackSize);
    memset(pOpList,0,ListSize);
    memset(pStack,0,StackSize);
    int nOps=0;
    int nStack=0;
    char *pEnd=szFormula+Length;
    char CurrentToken[MAX_STRING]={0};
    char *pToken=&CurrentToken[0];

#define OpToList(op) {pOpList[nOps].Op=op;nOps++;}
#define ValueToList(val) {pOpList[nOps].Value=val;nOps++;}
#define StackEmpty() (nStack==0)
#define StackTop() (pStack[nStack])
#define StackPush(op) {nStack++;pStack[nStack]=op;}
#define StackPop() {if (!nStack) {FatalError("Illegal arithmetic in calculation");free(pOpList);free(pStack);return 0;} nStack--;}
#define HasPrecedence(a,b) (CalcOpPrecedence[a]>=CalcOpPrecedence[b])
#define MoveStack(op)  \
    { \
    while(!StackEmpty() && StackTop()!=CO_OPENPARENS && HasPrecedence(StackTop(),op)) \
    { \
    OpToList(StackTop()); \
    StackPop(); \
    } \
    }

#define FinishString() {if (pToken!=&CurrentToken[0]) {*pToken=0;ValueToList(atof(CurrentToken));pToken=&CurrentToken[0];*pToken=0;}}
#define NewOp(op) {FinishString();MoveStack(op);StackPush(op);}
#define NextChar(ch) {*pToken=ch;pToken++;}

    bool WasParen=false;
    for (char *pCur=szFormula ; pCur<pEnd ; pCur++)
    {
        switch(*pCur)
        {
        case ' ':
            continue;
        case '(':
            FinishString();
            StackPush(CO_OPENPARENS);
            break;
        case ')':
            FinishString();
            while(StackTop()!=CO_OPENPARENS)
            {
                OpToList(StackTop());
                StackPop();
            }
            StackPop();
            WasParen=true;
            continue;
        case '+':
            if (pCur[1]!='+')
                NewOp(CO_ADD);
            break;
        case '-':
            if (pCur[1]=='-')
            {
                pCur++;
                NewOp(CO_ADD);
            }
            else
            {
                if (CurrentToken[0] || WasParen) 
                { 
                    NewOp(CO_SUBTRACT); 
                } 
                else 
                    NewOp(CO_NEGATE);
            }
            break;
        case '*':
            NewOp(CO_MULTIPLY);
            break;
        case '\\':
            NewOp(CO_IDIVIDE);
            break;
        case '/':
            NewOp(CO_DIVIDE);
            break;
        case '|':
            if (pCur[1]=='|')
            {
                // Logical OR
                ++pCur;
                NewOp(CO_LOR);
            }
            else
            {
                // Bitwise OR
                NewOp(CO_OR);
            }
            break;
        case '%':
            NewOp(CO_MODULUS);
            break;
        case '~':
            NewOp(CO_NOT);
            break;
        case '&':
            if (pCur[1]=='&')
            {
                // Logical AND
                ++pCur;
                NewOp(CO_LAND);
            }
            else
            {
                // Bitwise AND
                NewOp(CO_AND);
            }
            break;
        case '^':
            if (pCur[1]=='^')
            {
                // XOR
                ++pCur;
                NewOp(CO_XOR);
            }
            else
            {
                // POWER
                NewOp(CO_POWER);
            }
            break;
        case '!':
            if (pCur[1]=='=')
            {
                ++pCur;
                NewOp(CO_NOTEQUAL);
            }
            else
            {
                NewOp(CO_LNOT);
            }
            break;
        case '=':
            if (pCur[1]=='=')
            {
                ++pCur;
                NewOp(CO_EQUAL);
            }
            else
            {
                //printf("Unparsable: '%c'\n",*pCur);
                // error
                free(pOpList);
                free(pStack);
                return false;
            }
            break;
        case '<':
            if (pCur[1]=='=')
            {
                ++pCur;
                NewOp(CO_NOTGREATER);
            }
            else if (pCur[1]=='<')
            {
                ++pCur;
                NewOp(CO_SHL);
            }
            else
            {
                NewOp(CO_LESS);
            }
            break;
        case '>':
            if (pCur[1]=='=')
            {
                ++pCur;
                NewOp(CO_NOTLESS);
            }
            else if (pCur[1]=='>')
            {
                ++pCur;
                NewOp(CO_SHR);
            }
            else
            {
                NewOp(CO_GREATER);
            }
            break;
        case '.':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            NextChar(*pCur);
            break;
        default:
            {
                //printf("Unparsable: '%c'\n",*pCur);
                FatalError("Unparsable in Calculation: '%c'",*pCur);
                // unparsable
                free(pOpList);
                free(pStack);
                return false;
            }
            break;
        }
        WasParen=false;
    }
    FinishString();

    while(!StackEmpty())
    {
        OpToList(StackTop());
        StackPop();
    }
    free(pStack);
    /*
    for (int i = 0 ; i < nOps ; i++)
    {
    if (pOpList[i].Op)
    printf("Op: %d\n",pOpList[i].Op);
    else
    printf("Value: %f\n",pOpList[i].Value);
    }
    /**/
    BOOL Ret=EvaluateRPN(pOpList,nOps,Result);
    free(pOpList);
    return Ret;
}

BOOL Calculate(PCHAR szFormula, DOUBLE &Result) 
{
    CHAR Buffer[MAX_STRING]={0};
    strcpy(Buffer,szFormula);
    while(PCHAR pNull=strstr(Buffer,"NULL"))
    {
        pNull[0]='0';
        pNull[1]='.';
        pNull[2]='0';
        pNull[3]='0';
    }
    while(PCHAR pTrue=strstr(Buffer,"TRUE"))
    {
        pTrue[0]='1';
        pTrue[1]='.';
        pTrue[2]='0';
        pTrue[3]='0';
    }
    while(PCHAR pFalse=strstr(Buffer,"FALSE"))
    {
        pFalse[0]='0';
        pFalse[1]='.';
        pFalse[2]='0';
        pFalse[3]='0';
        pFalse[4]='0';
    }
    BOOL Ret;
    //Benchmark(bmCalculate,Ret=ActualCalculate(Buffer,Result));
    Benchmark(bmCalculate,Ret=FastCalculate(Buffer,Result));
    return Ret;
}
#endif

bool PlayerHasAAAbility(DWORD AAIndex)
{
    for (int i = 0; i < AA_CHAR_MAX_REAL; i++)
    {
        if ( pPCData->GetAltAbilityIndex(i) == AAIndex )
            return true;
    }
    return false;
}

#if 0
PCHAR GetAANameByIndex(DWORD AAIndex)
{
    for (unsigned long nAbility=0 ; nAbility<NUM_ALT_ABILITIES_ARRAY ; nAbility++)
    {
        if ( ((PALTADVMGR)pAltAdvManager)->AltAbilities->AltAbilityList->Abilities[nAbility])
        {
            if ( PALTABILITY pAbility=((PALTADVMGR)pAltAdvManager)->AltAbilities->AltAbilityList->Abilities[nAbility]->Ability) 
            {
                if (pAbility->Index == AAIndex)
                {
                    return pStringTable->getString(pAbility->nName,0);
                }
            }
        }
    }
    return "AA Not Found";
}
#endif

DWORD GetAAIndexByName(PCHAR AAName)
{
    unsigned long nAbility;
    for (nAbility=0 ; nAbility<AA_CHAR_MAX_REAL ; nAbility++) {
        if ( PALTABILITY pAbility=pAltAdvManager->GetAltAbility(pPCData->GetAltAbilityIndex(nAbility))) {
            if (PCHAR pName=pCDBStr->GetString(pAbility->nName, 1, NULL)) {
                if (!stricmp(AAName,pName)) {
                    return pAbility->Index;
                }
            }
        }
    }
#if  0
    for (unsigned long nAbility=0 ; nAbility<NUM_ALT_ABILITIES_ARRAY ; nAbility++)
    {
        if ( ((PALTADVMGR)pAltAdvManager)->AltAbilities->AltAbilityList->Abilities[nAbility])
        {
            if ( PALTABILITY pAbility=((PALTADVMGR)pAltAdvManager)->AltAbilities->AltAbilityList->Abilities[nAbility]->Ability) 
            {
                if (PCHAR pName=pCDBStr->GetString(pAbility->nName, 1, NULL))
                {
                    if (!stricmp(AAName,pName))
                    {
                        return pAbility->Index;
                    }
                }
            }
        }
    }
#endif
    return 0;
}

DWORD GetAAIndexByID(DWORD ID)
{
    unsigned long nAbility;
    for (nAbility=0 ; nAbility<AA_CHAR_MAX_REAL ; nAbility++) {
        if ( PALTABILITY pAbility=pAltAdvManager->GetAltAbility(pPCData->GetAltAbilityIndex(nAbility))) {
            if (pAbility->ID == ID ) {
                return pAbility->Index;
            }
        }
    }
#if 0
    for (unsigned long nAbility=0 ; nAbility<NUM_ALT_ABILITIES_ARRAY ; nAbility++)
    {
        if ( ((PALTADVMGR)pAltAdvManager)->AltAbilities->AltAbilityList->Abilities[nAbility])
        {
            if ( PALTABILITY pAbility=((PALTADVMGR)pAltAdvManager)->AltAbilities->AltAbilityList->Abilities[nAbility]->Ability) 
            {
                if (pAbility->ID == ID )
                {
                    return pAbility->Index;
                }
            }
        }
    }
#endif
    return 0;
}

BOOL IsPCNear(PSPAWNINFO pSpawn, FLOAT Radius)
{
    PSPAWNINFO pClose = NULL;
    if (ppSpawnManager && pSpawnList) 
    {
        pClose = (PSPAWNINFO)pSpawnList;
    }
    while (pClose) 
    {
        if (!IsInGroup(pClose) && (pClose->Type == SPAWN_PLAYER))
        {
            if ((pClose != pSpawn) && (DistanceToSpawn(pClose, pSpawn)<Radius)) 
                return TRUE;
        }
        pClose = pClose->pNext;
    }
    return false;
}

BOOL IsInGroup(PSPAWNINFO pSpawn)
{
    DWORD i;
    PCHARINFO pChar=GetCharInfo();
    if (!pChar->pGroupInfo) return FALSE;
    if (pSpawn==pChar->pSpawn)
        return TRUE;
    for (i=1;i<6;i++) 
        if (pChar->pGroupInfo->pMember[i])
        {
            CHAR Name[MAX_STRING]={0};
            GetCXStr(pChar->pGroupInfo->pMember[i]->pName,Name,MAX_STRING);
            if (!stricmp(Name,pSpawn->Name))
                return TRUE;
        }
        return FALSE;
}

EQLIB_API BOOL IsInRaid(PSPAWNINFO pSpawn)
{
    DWORD i;
    if (pSpawn==GetCharInfo()->pSpawn)
        return TRUE;
    DWORD l = strlen(pSpawn->Name);
    for (i=0;i<72;i++)
    {
        if (!strnicmp(pRaid->RaidMember[i].Name,pSpawn->Name,l+1) && pRaid->RaidMember[i].nClass == pSpawn->Class)
            return TRUE;
    }
    return FALSE;
} 

BOOL IsNamed(PSPAWNINFO pSpawn)
{
    CHAR szTemp[MAX_STRING]={0};

    if (GetSpawnType(pSpawn) != NPC)
        return false;
    if (!IsTargetable(pSpawn))
        return false;
    if (pSpawn->Class >= 20 && pSpawn->Class <= 35)  // NPC GMs
        return false;
    if (pSpawn->Class == 40)  // NPC bankers
        return false;
    if (pSpawn->Class == 41 || pSpawn->Class == 70)  // NPC/Quest/TBS merchants
        return false;
    if (pSpawn->Class == 60 || pSpawn->Class == 61)  //Ldon Merchants/Recruiters
        return false;
    if (pSpawn->Class == 62)  // Destructible Objects
        return false;
    if (pSpawn->Class == 63 || pSpawn->Class == 64)  // Tribute Master/Guild Tribute Master
        return false;
    if (pSpawn->Class == 66)  // Guild Banker
        return false;
    if (pSpawn->Class == 67 || pSpawn->Class == 68)  //Don Merchants (Norrath's Keepers/Dark Reign)
        return false;
    if (pSpawn->Class == 69)  // Fellowship Registrar
        return false;
    if (pSpawn->Class == 71)  // Mercenary Liason
        return false;

    strcpy(szTemp,pSpawn->Name);
    strtok(szTemp," ");

    // Checking for mobs that have 'A' or 'An' as their first name
    if (szTemp[0] == 'A')
    {
        if (szTemp[1] == '_')
            return false;
        else if (szTemp[1] == 'n')
            if (szTemp[2] == '_')
                return false;
    }
    if ((!strnicmp(szTemp,"Guard",5))          ||
        (!strnicmp(szTemp,"Defender",8))       ||
        (!strnicmp(szTemp,"Soulbinder",10))    ||
        (!strnicmp(szTemp,"Sage",4))           ||
        //(!strnicmp(szTemp,"High_Priest",11))   ||
        (!strnicmp(szTemp,"Ward",4))           ||
        //(!strnicmp(szTemp,"Shroudkeeper",12))  ||
        (!strnicmp(szTemp,"Eye of",6))         ||
        (!strnicmp(szTemp,"Diaku",5)))
        return false;
    if (isupper(szTemp[0]) || szTemp[0] == '#')
        return true;

    return false;
}

PCHAR FormatSearchSpawn(PCHAR Buffer, PSEARCHSPAWN pSearchSpawn)
{
    CHAR szTemp[MAX_STRING] = {0};
    if (!Buffer) return NULL;
    if (!pSearchSpawn) return strcpy(Buffer,"None");
    PCHAR pszSpawnType;
    switch(pSearchSpawn->SpawnType)
    {
    case NONE:
    default:
        pszSpawnType="any";
        break;
    case PC:
        pszSpawnType="pc";
        break;
    case MOUNT:
        pszSpawnType="mount";
        break;
    case PET:
        pszSpawnType="pet";
        break;
    case NPC:
        pszSpawnType="npc";
        break;
    case CORPSE:
        pszSpawnType="corpse";
        break;
    case TRIGGER:
        pszSpawnType="trigger";
        break;
    case TRAP:
        pszSpawnType="trap";
        break;
    case CHEST:
        pszSpawnType="chest";
        break;
    case TIMER:
        pszSpawnType="timer";
        break;
    case UNTARGETABLE:
        pszSpawnType="untargetable";
        break;
    case MERCENARY:
        pszSpawnType="mercenary";
        break;
    case FLYER:
        pszSpawnType="flyer";
        break;
    }

    sprintf(Buffer,"(%d-%d) %s",pSearchSpawn->MinLevel,pSearchSpawn->MaxLevel,pszSpawnType);

    if (pSearchSpawn->szName[0]!=0) {
        sprintf(szTemp," %s",pSearchSpawn->szName);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->szRace[0]!=0) {
        sprintf(szTemp," Race:%s",pSearchSpawn->szRace);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->szClass[0]!=0) {
        sprintf(szTemp," Class:%s",pSearchSpawn->szClass);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->szBodyType[0]!=0) {
        sprintf(szTemp," Body:%s",pSearchSpawn->szBodyType);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->GuildID!=0xFFFF) {
        char *szGuild = GetGuildByID(pSearchSpawn->GuildID);
        sprintf(szTemp," Guild:%s", szGuild ? szGuild : "Unknown");
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->bKnownLocation) {
        sprintf(szTemp," at %1.2f,%1.2f", pSearchSpawn->yLoc, pSearchSpawn->xLoc);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->FRadius<10000.0f) {
        sprintf(szTemp," Radius:%1.2f",pSearchSpawn->FRadius);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->ZRadius<10000.0f) {
        sprintf(szTemp," Z:�%1.2f",pSearchSpawn->ZRadius);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->Radius>0.0f) {
        sprintf(szTemp," NoPC:%1.2f",pSearchSpawn->Radius);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->SpawnID) {
        sprintf(szTemp," ID:%d",pSearchSpawn->SpawnID);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->NotID) {
        sprintf(szTemp," NotID:%d",pSearchSpawn->NotID);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->bAlert) {
        sprintf(szTemp," Alert:%d",pSearchSpawn->AlertList);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->bNoAlert) {
        sprintf(szTemp," NoAlert:%d",pSearchSpawn->NoAlertList);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->bNearAlert) {
        sprintf(szTemp," NearAlert:%d",pSearchSpawn->NearAlertList);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->bNotNearAlert) {
        sprintf(szTemp," NotNearAlert:%d",pSearchSpawn->NotNearAlertList);
        strcat(Buffer,szTemp);
    }
    if (pSearchSpawn->bGM && pSearchSpawn->SpawnType != NPC ) strcat(Buffer," GM");
    if (pSearchSpawn->bTrader) strcat(Buffer," Trader");
    if (pSearchSpawn->bLFG) strcat(Buffer," LFG");
    if (pSearchSpawn->bLight) {
        strcat(Buffer," Light");
        if (pSearchSpawn->szLight[0]) {
            strcat(Buffer,":");
            strcat(Buffer,pSearchSpawn->szLight);
        }
    }
    if (pSearchSpawn->bLoS) strcat(Buffer," LoS");

    return Buffer;
}

PSPAWNINFO NthNearestSpawn(PSEARCHSPAWN pSearchSpawn, DWORD Nth, PSPAWNINFO pOrigin, BOOL IncludeOrigin)
{
    if (!pSearchSpawn || !Nth || !pOrigin)
        return 0;
    CIndex<PMQRANK> SpawnSet;
    PSPAWNINFO pSpawn=(PSPAWNINFO)pSpawnList;
    // create our set    
    DWORD TotalMatching=0;
    if (IncludeOrigin)
    {
        while (pSpawn)
        {
            if (SpawnMatchesSearch(pSearchSpawn,pOrigin,pSpawn))
            {
                // matches search, add to our set
                TotalMatching++;
                PMQRANK pNewRank=new MQRANK;
                pNewRank->VarPtr.Ptr=pSpawn;
                pNewRank->Value.Float=GetDistance(pOrigin->X,pOrigin->Y,pSpawn->X,pSpawn->Y);
                SpawnSet+=pNewRank;
            }
            pSpawn=pSpawn->pNext;
        }
    }
    else
    {
        while (pSpawn)
        {
            if (pSpawn!=pOrigin && SpawnMatchesSearch(pSearchSpawn,pOrigin,pSpawn))
            {
                // matches search, add to our set
                TotalMatching++;
                PMQRANK pNewRank=new MQRANK;
                pNewRank->VarPtr.Ptr=pSpawn;
                pNewRank->Value.Float=GetDistance(pOrigin->X,pOrigin->Y,pSpawn->X,pSpawn->Y);
                SpawnSet+=pNewRank;
            }
            pSpawn=pSpawn->pNext;
        }
    }
    if (TotalMatching<Nth)
    {// CIndex.Cleanup will call "delete" on every valid pointer in our list.
        SpawnSet.Cleanup();
        return 0;
    }
    // sort our list
    qsort(&SpawnSet.List[0],TotalMatching,sizeof(PMQRANK),pMQRankFloatCompare);
    // get our Nth nearest
    pSpawn=(PSPAWNINFO)SpawnSet[Nth-1]->VarPtr.Ptr;
    SpawnSet.Cleanup();
    return pSpawn;
}

DWORD CountMatchingSpawns(PSEARCHSPAWN pSearchSpawn, PSPAWNINFO pOrigin, BOOL IncludeOrigin)
{
    if (!pSearchSpawn || !pOrigin)
        return 0;
    DWORD TotalMatching=0;
    PSPAWNINFO pSpawn=(PSPAWNINFO)pSpawnList;
    if (IncludeOrigin)
    {
        while (pSpawn)
        {
            if (SpawnMatchesSearch(pSearchSpawn,pOrigin,pSpawn))
            {
                TotalMatching++;
            }
            pSpawn=pSpawn->pNext;
        }
    }
    else
    {
        while (pSpawn)
        {
            if (pSpawn!=pOrigin && SpawnMatchesSearch(pSearchSpawn,pOrigin,pSpawn))
            {
                // matches search, add to our set
                TotalMatching++;
            }
            pSpawn=pSpawn->pNext;
        }
    }
    return TotalMatching;
}


PSPAWNINFO SearchThroughSpawns(PSEARCHSPAWN pSearchSpawn, PSPAWNINFO pChar)
{
    PSPAWNINFO pFromSpawn = NULL;

    if (pSearchSpawn->FromSpawnID>0 && (pSearchSpawn->bTargNext ||pSearchSpawn->bTargPrev)) 
    {
        pFromSpawn=(PSPAWNINFO)GetSpawnByID(pSearchSpawn->FromSpawnID);
        if (!pFromSpawn) return NULL;
        for ( int N=0 ; N < 3000 ; N++)
        {
            if (EQP_DistArray[N].VarPtr.Ptr==pFromSpawn)
            {
                if (pSearchSpawn->bTargPrev)
                {
                    N--;
                    for (N ; N >= 0 ; N--)
                    {
                        if (EQP_DistArray[N].VarPtr.Ptr && 
                            SpawnMatchesSearch(pSearchSpawn,pFromSpawn,(PSPAWNINFO)EQP_DistArray[N].VarPtr.Ptr))
                            return (PSPAWNINFO)EQP_DistArray[N].VarPtr.Ptr;
                    }
                }
                else
                {
                    N++;
                    for (N ; N < 3000 ; N++)
                    {
                        if (EQP_DistArray[N].VarPtr.Ptr && 
                            SpawnMatchesSearch(pSearchSpawn,pFromSpawn,(PSPAWNINFO)EQP_DistArray[N].VarPtr.Ptr))
                            return (PSPAWNINFO)EQP_DistArray[N].VarPtr.Ptr;
                    }
                }
                return NULL;
            }
        }
    }
    return NthNearestSpawn(pSearchSpawn,1,pChar,TRUE);
}

BOOL SpawnMatchesSearch(PSEARCHSPAWN pSearchSpawn, PSPAWNINFO pChar, PSPAWNINFO pSpawn)
{
    CHAR szName[MAX_STRING] = {0};
    CHAR szSearchName[MAX_STRING] = {0};
    eSpawnType SpawnType = GetSpawnType(pSpawn);
    if (pSearchSpawn->SpawnType != SpawnType && pSearchSpawn->SpawnType!=NONE)
    {
        if (pSearchSpawn->SpawnType==NPCCORPSE) {
            if (SpawnType != CORPSE || pSpawn->Deity)
                return FALSE;
        } else if (pSearchSpawn->SpawnType==PCCORPSE) {
            if (SpawnType != CORPSE || !pSpawn->Deity)
                return FALSE;
        } else {

            // if the search type is not npc or the mob type is UNT, continue?
            // stupid /who
            if (pSearchSpawn->SpawnType!=NPC || SpawnType!=UNTARGETABLE)
                return FALSE;
        }
    }
	if(gbExactSearchCleanNames)
		pSearchSpawn->bExactName = 1;
    _strlwr(strcpy(szName,pSpawn->Name));
    _strlwr(strcpy(szSearchName,pSearchSpawn->szName));
	if (!strstr(szName,szSearchName) && !strstr(CleanupName(szName,FALSE),szSearchName))
		return FALSE;
	if(pSearchSpawn->bExactName) {
		if (stricmp(CleanupName(szName,FALSE,FALSE),pSearchSpawn->szName))
			return FALSE;
	}
    if (pSearchSpawn->MinLevel && pSpawn->Level < pSearchSpawn->MinLevel)
        return FALSE;
    if (pSearchSpawn->MaxLevel && pSpawn->Level > pSearchSpawn->MaxLevel)
        return FALSE;
    if (pSearchSpawn->NotID == pSpawn->SpawnID)
        return FALSE;
    if (pSearchSpawn->bSpawnID && pSearchSpawn->SpawnID != pSpawn->SpawnID)
        return FALSE;
    if (pSearchSpawn->GuildID!=0xFFFF && pSearchSpawn->GuildID!=pSpawn->GuildID)
        return FALSE;
    if (pSearchSpawn->bGM && pSearchSpawn->SpawnType != NPC )
        if (!pSpawn->GM)
            return FALSE;
    if (pSearchSpawn->bGM && pSearchSpawn->SpawnType == NPC )
        if (pSpawn->Class < 20 || pSpawn->Class > 35 )
            return FALSE;
    if (pSearchSpawn->bNamed && !IsNamed(pSpawn))
        return FALSE;
    if (pSearchSpawn->bMerchant && pSpawn->Class != 41 )
        return FALSE;
    if (pSearchSpawn->bTributeMaster && pSpawn->Class != 63 )
        return FALSE;
    if (pSearchSpawn->bNoGuild && (pSpawn->GuildID != 0xFFFFFFFF))
        return FALSE;
    if (pSearchSpawn->bKnight && pSearchSpawn->SpawnType != NPC) 
        if (pSpawn->Class != 3 && pSpawn->Class != 5 )
            return FALSE;
    if (pSearchSpawn->bTank && pSearchSpawn->SpawnType != NPC) 
        if (pSpawn->Class != 3 && pSpawn->Class != 5 && pSpawn->Class != 1 )
            return FALSE;
    if (pSearchSpawn->bHealer && pSearchSpawn->SpawnType != NPC)
        if (pSpawn->Class != 2 && pSpawn->Class != 6)
            return FALSE;
    if (pSearchSpawn->bDps && pSearchSpawn->SpawnType != NPC)
        if (pSpawn->Class != 4 && pSpawn->Class != 9 && pSpawn->Class != 12 )
            return FALSE;
    if (pSearchSpawn->bSlower && pSearchSpawn->SpawnType != NPC)
        if (pSpawn->Class != 10 && pSpawn->Class != 14 && pSpawn->Class != 15)
            return FALSE;
    if (pSearchSpawn->bLFG && !pSpawn->LFG)
        return FALSE;
    if (pSearchSpawn->bTrader && !pSpawn->Trader)
        return FALSE;
    if (pSearchSpawn->bGroup && !IsInGroup(pSpawn))
        return FALSE;
    if (pSearchSpawn->bRaid && !IsInRaid(pSpawn))
        return FALSE;
    if (pSearchSpawn->bKnownLocation) 
    {
        if ((pSearchSpawn->xLoc!=pSpawn->X || pSearchSpawn->yLoc!=pSpawn->Y))
            if (pSearchSpawn->FRadius<10000.0f && DistanceToPoint(pSpawn,pSearchSpawn->xLoc,pSearchSpawn->yLoc)>pSearchSpawn->FRadius)
                return FALSE;
    }
    else if (pSearchSpawn->FRadius<10000.0f && DistanceToSpawn(pChar, pSpawn)>pSearchSpawn->FRadius)
        return FALSE;

    if (pSearchSpawn->Radius>0.0f && IsPCNear(pSpawn,pSearchSpawn->Radius))
        return FALSE;
    if (gZFilter<10000.0f && ( (pSpawn->Z > pChar->Z + gZFilter) || (pSpawn->Z < pChar->Z - gZFilter)))
        return FALSE;
    if (pSearchSpawn->ZRadius<10000.0f &&  (pSpawn->Z > pChar->Z + pSearchSpawn->ZRadius ||pSpawn->Z < pChar->Z - pSearchSpawn->ZRadius))
        return FALSE;
    if (pSearchSpawn->bLight) 
    {
        PCHAR pLight=GetLightForSpawn(pSpawn);
        if (!stricmp(pLight,"NONE")) 
            return FALSE;
        if (pSearchSpawn->szLight[0] && stricmp(pLight,pSearchSpawn->szLight)) 
            return FALSE;
    }
    if ((pSearchSpawn->bAlert) && (GetAlert(pSearchSpawn->AlertList))) 
    {
        if (!IsAlert(pChar,pSpawn,pSearchSpawn->AlertList)) 
            return FALSE;
    }
    if ((pSearchSpawn->bNoAlert) && (GetAlert(pSearchSpawn->NoAlertList))) 
    {
        if (IsAlert(pChar,pSpawn,pSearchSpawn->NoAlertList)) 
            return FALSE;
    }
    if ((pSearchSpawn->bNotNearAlert) && (GetClosestAlert(pSpawn, pSearchSpawn->NotNearAlertList))) 
        return FALSE;
    if ((pSearchSpawn->bNearAlert) && (!GetClosestAlert(pSpawn,pSearchSpawn->NearAlertList))) 
        return FALSE;

    if (pSearchSpawn->szClass[0] && stricmp(pSearchSpawn->szClass,GetClassDesc(pSpawn->Class)))
        return FALSE;
    if (pSearchSpawn->szBodyType[0] && stricmp(pSearchSpawn->szBodyType,GetBodyTypeDesc(GetBodyType(pSpawn))))
        return FALSE;
    if (pSearchSpawn->szRace[0] && stricmp(pSearchSpawn->szRace,pEverQuest->GetRaceDesc(pSpawn->Race)))
        return FALSE;
    if (pSearchSpawn->bLoS && (!LineOfSight(pChar,pSpawn)))
        return FALSE;
    if (pSearchSpawn->bTargetable && (!IsTargetable(pSpawn)))
        return FALSE;
    return TRUE;
}

#ifndef ISXEQ
PCHAR ParseSearchSpawnArgs(PCHAR szArg, PCHAR szRest, PSEARCHSPAWN pSearchSpawn)
{
    if (szArg && pSearchSpawn) {
        if (!stricmp(szArg,"pc")) {
            pSearchSpawn->SpawnType = PC;
        } else if (!stricmp(szArg,"npc")) {
            pSearchSpawn->SpawnType = NPC;
        } else if (!stricmp(szArg,"mount")) {
            pSearchSpawn->SpawnType = MOUNT;
        } else if (!stricmp(szArg,"pet")) {
            pSearchSpawn->SpawnType = PET;
        } else if (!stricmp(szArg,"nopet")) { 
            pSearchSpawn->bNoPet = TRUE; 
        } else if (!stricmp(szArg,"corpse")) {
            pSearchSpawn->SpawnType = CORPSE;
        } else if (!stricmp(szArg,"npccorpse")) {
            pSearchSpawn->SpawnType = NPCCORPSE;
        } else if (!stricmp(szArg,"pccorpse")) {
            pSearchSpawn->SpawnType = PCCORPSE;
        } else if (!stricmp(szArg,"trigger")) {
            pSearchSpawn->SpawnType = TRIGGER;
        } else if (!stricmp(szArg,"untargetable")) {
            pSearchSpawn->SpawnType = UNTARGETABLE;
        } else if (!stricmp(szArg,"trap")) {
            pSearchSpawn->SpawnType = TRAP;
        } else if (!stricmp(szArg,"chest")) {
            pSearchSpawn->SpawnType = CHEST;
        } else if (!stricmp(szArg,"timer")) {
            pSearchSpawn->SpawnType = TIMER;
        } else if (!stricmp(szArg,"aura")) {
            pSearchSpawn->SpawnType = AURA;
        } else if (!stricmp(szArg,"object")) {
            pSearchSpawn->SpawnType = OBJECT;
        } else if (!stricmp(szArg,"banner")) {
            pSearchSpawn->SpawnType = BANNER;
        } else if (!stricmp(szArg,"campfire")) {
            pSearchSpawn->SpawnType = CAMPFIRE;
        } else if (!stricmp(szArg,"mercenary")) {
            pSearchSpawn->SpawnType = MERCENARY;
        } else if (!stricmp(szArg,"flyer")) {
            pSearchSpawn->SpawnType = FLYER;
        } else if (!stricmp(szArg,"any")) {
            pSearchSpawn->SpawnType = NONE;
        } else if (!stricmp(szArg,"next")) {
            pSearchSpawn->bTargNext = TRUE;
        } else if (!stricmp(szArg,"prev")) {
            pSearchSpawn->bTargPrev = TRUE;
        } else if (!stricmp(szArg,"lfg")) {
            pSearchSpawn->bLFG = TRUE;
        } else if (!stricmp(szArg,"gm")) {
            pSearchSpawn->bGM = TRUE;
        } else if (!stricmp(szArg,"group")) {
            pSearchSpawn->bGroup = TRUE;
        } else if (!stricmp(szArg,"raid")) {
            pSearchSpawn->bRaid = TRUE; 
        } else if (!stricmp(szArg,"noguild")) {
            pSearchSpawn->bNoGuild = TRUE;
        } else if (!stricmp(szArg,"trader")) {
            pSearchSpawn->bTrader = TRUE;
        } else if (!stricmp(szArg,"named")) {
            pSearchSpawn->bNamed = TRUE;
        } else if (!stricmp(szArg,"merchant")) {
            pSearchSpawn->bMerchant = TRUE;
        } else if (!stricmp(szArg,"tribute")) {
            pSearchSpawn->bTributeMaster = TRUE;
        } else if (!stricmp(szArg,"knight")) {
            pSearchSpawn->bKnight = TRUE;
        } else if (!stricmp(szArg,"tank")) {
            pSearchSpawn->bTank = TRUE;
        } else if (!stricmp(szArg,"healer")) {
            pSearchSpawn->bHealer = TRUE;
        } else if (!stricmp(szArg,"dps")) {
            pSearchSpawn->bDps = TRUE;
        } else if (!stricmp(szArg,"slower")) {
            pSearchSpawn->bSlower = TRUE;
        } else if (!stricmp(szArg,"los")) {
            pSearchSpawn->bLoS = TRUE;
        } else if (!stricmp(szArg,"targetable")) {
            pSearchSpawn->bTargetable = TRUE;
        } else if (!stricmp(szArg,"range")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->MinLevel = atoi(szArg);
            GetArg(szArg,szRest,2);
            pSearchSpawn->MaxLevel = atoi(szArg);
            szRest = GetNextArg(szRest,2);
        } else if (!stricmp(szArg,"loc")) {
            pSearchSpawn->bKnownLocation = TRUE;
            GetArg(szArg,szRest,1);
            pSearchSpawn->xLoc = (FLOAT)atof(szArg);
            GetArg(szArg,szRest,2);
            pSearchSpawn->yLoc = (FLOAT)atof(szArg);
            szRest = GetNextArg(szRest,2);
        } else if (!stricmp(szArg,"id")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->bSpawnID = TRUE;
            pSearchSpawn->SpawnID = atoi(szArg);
            szRest = GetNextArg(szRest,1);
        } else if (!stricmp(szArg,"radius")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->FRadius = atof(szArg);
            szRest = GetNextArg(szRest,1);
        } else if (!stricmp(szArg,"body")) {
            GetArg(szArg,szRest,1);
            strcpy(pSearchSpawn->szBodyType,szArg);
            szRest = GetNextArg(szRest,1);
        } else if (!stricmp(szArg,"class")) {
            GetArg(szArg,szRest,1);
            strcpy(pSearchSpawn->szClass,szArg);
            szRest = GetNextArg(szRest,1);
        } else if (!stricmp(szArg,"race")) {
            GetArg(szArg,szRest,1);
            strcpy(pSearchSpawn->szRace,szArg);
            szRest = GetNextArg(szRest,1);
        } else if (!stricmp(szArg,"light")) {
            DWORD Light = -1;
            DWORD i=0;
            GetArg(szArg,szRest,1);
            if (szArg[0]!=0) for (i=0;i<LIGHT_COUNT;i++) if (!stricmp(szLights[i],szArg)) Light=i;
            if (Light != -1) {
                strcpy(pSearchSpawn->szLight,szLights[Light]);
                szRest = GetNextArg(szRest,1);
            } else {
                pSearchSpawn->szLight[0]=0;
            }
            pSearchSpawn->bLight=TRUE;
        } else if (!strcmp(szArg,"GUILD")) {
            pSearchSpawn->GuildID=GetCharInfo()->pSpawn->GuildID;
        } else if (!stricmp(szArg,"guild")) {
            DWORD GuildID = 0xFFFF;
            GetArg(szArg,szRest,1);
            if (szArg[0]!=0) GuildID = GetGuildIDByName(szArg);
            if (GuildID!=0xFFFF) {
                pSearchSpawn->GuildID = GuildID;
                szRest = GetNextArg(szRest,1);
            } else {
                pSearchSpawn->GuildID = GetCharInfo()->pSpawn->GuildID;
            }
        } else if (!stricmp(szArg,"alert")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->AlertList = atoi(szArg);
            szRest = GetNextArg(szRest,1);
            pSearchSpawn->bAlert = TRUE;
        } else if (!stricmp(szArg,"noalert")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->NoAlertList = atoi(szArg);
            szRest = GetNextArg(szRest,1);
            pSearchSpawn->bNoAlert = TRUE;
        } else if (!stricmp(szArg,"notnearalert")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->NotNearAlertList = atoi(szArg);
            szRest = GetNextArg(szRest,1);
            pSearchSpawn->bNotNearAlert = TRUE;
        } else if (!stricmp(szArg,"nearalert")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->NearAlertList = atoi(szArg);
            szRest = GetNextArg(szRest,1);
            pSearchSpawn->bNearAlert = TRUE;
        } else if (!stricmp(szArg,"zradius")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->ZRadius = atof(szArg);
            szRest = GetNextArg(szRest,1);
        } else if (!stricmp(szArg,"notid")) {
            GetArg(szArg,szRest,1);
            pSearchSpawn->NotID = atoi(szArg);
            szRest = GetNextArg(szRest,1);
        } else if (!stricmp(szArg,"nopcnear")) {
            GetArg(szArg,szRest,1);
            if ((szArg[0]==0) || (0.0f == (pSearchSpawn->Radius = (FLOAT)atof(szArg)))) {
                pSearchSpawn->Radius = 200.0f;
            } else {
                szRest = GetNextArg(szRest,1);
            }
        } else if (IsNumber(szArg)) {
            pSearchSpawn->MinLevel=atoi(szArg);
            pSearchSpawn->MaxLevel=pSearchSpawn->MinLevel;
        } else {
            for (DWORD N=1;N<17;N++)
            {
                if (!stricmp(szArg,ClassInfo[N].Name) || !stricmp(szArg,ClassInfo[N].ShortName))
                {
                    strcpy(pSearchSpawn->szClass,pEverQuest->GetClassDesc(N));
                    return szRest;
                }
            }
            if (pSearchSpawn->szName[0])
            {// multiple word name
                strcat(pSearchSpawn->szName," ");
                strcat(pSearchSpawn->szName,szArg);
            }
            else {
                if (szArg[0]=='=')
                {
                    pSearchSpawn->bExactName=TRUE;
                    szArg++;
                }
                strcpy(pSearchSpawn->szName,szArg);
            }
        }
    }
    return szRest;
}

VOID ParseSearchSpawn(PCHAR Buffer, PSEARCHSPAWN pSearchSpawn)
{
    CHAR szArg[MAX_STRING] = {0};
    CHAR szMsg[MAX_STRING] = {0};
    CHAR szLLine[MAX_STRING] = {0};
    PCHAR szFilter = szLLine;
    BOOL DidTarget = FALSE;
    BOOL bArg = TRUE;

    bRunNextCommand = TRUE;
    _strlwr(strcpy(szLLine,Buffer));
    while (bArg) {
        GetArg(szArg,szFilter,1);
        szFilter = GetNextArg(szFilter,1);
        if (szArg[0]==0) {
            bArg = FALSE;
        } else {
            szFilter = ParseSearchSpawnArgs(szArg,szFilter,pSearchSpawn);
        }
    }
}
#else
VOID ParseSearchSpawn(int BeginInclusive, int EndExclusive,char *argv[], SEARCHSPAWN &SearchSpawn)
{
    for (int arg = BeginInclusive ; arg < EndExclusive ; arg++)
        arg+=ParseSearchSpawnArg(arg,EndExclusive,argv,SearchSpawn);
}
#endif

PALERT GetAlert(DWORD Id) {
    PALERTLIST pAlertList = gpAlertList;
    for (;pAlertList;pAlertList=pAlertList->pNext) {
        if (pAlertList->Id==Id) return (pAlertList->pAlert);
    }
    return NULL;
}

VOID AddNewAlertList(DWORD Id, PALERT pAlert)
{
    PALERTLIST pAlertList = (PALERTLIST)malloc(sizeof(ALERTLIST));
    if (!pAlertList) {
        free(pAlert);
        return;
    }
    pAlertList->Id = Id;
    pAlertList->pAlert = pAlert;
    pAlertList->pNext = gpAlertList;
    gpAlertList = pAlertList;
}

VOID FreeAlertList(PALERTLIST pAlertList)
{
    PALERT pTemp = NULL;
    while (pAlertList->pAlert) {
        pTemp = pAlertList->pAlert->pNext;
        free(pAlertList->pAlert);
        pAlertList->pAlert = pTemp;
    }
    free(pAlertList);
}

VOID FreeAlerts(DWORD List)
{
    PALERTLIST pAlertList = gpAlertList;
    PALERTLIST pTempList = NULL;
    CHAR szBuffer[MAX_STRING] = {0};
    if (gpAlertList) {
        if (gpAlertList->Id == List) {
            pTempList = gpAlertList->pNext;
            FreeAlertList(gpAlertList);
            gpAlertList = pTempList;
        } else {
            while (pAlertList) {
                pTempList = pAlertList->pNext;
                if (pTempList && (pTempList->Id == List)) {
                    pTempList = pTempList->pNext;
                    FreeAlertList(pAlertList->pNext);
                    pAlertList->pNext = pTempList;
                }
                pAlertList = pTempList;
            }
        }
    }
    sprintf(szBuffer,"Alert list %d cleared.",List);
    WriteChatColor(szBuffer,USERCOLOR_DEFAULT);
}

BOOL GetClosestAlert(PSPAWNINFO pChar, DWORD List)
{
    CHAR szName[MAX_STRING] = {0};
    if (!ppSpawnManager) return FALSE;
    if (!pSpawnList) return FALSE;
    PSPAWNINFO pSpawn, pClosest = FALSE;
    FLOAT ClosestDistance = 50000.0f;
    PALERT pCurrent = GetAlert(List);
    if (!pCurrent) return FALSE;
    for (; pCurrent; pCurrent = pCurrent->pNext) {
        if (pSpawn = SearchThroughSpawns(&(pCurrent->SearchSpawn),pChar)) {
            if (DistanceToSpawn(pChar,pSpawn)<ClosestDistance) {
                pClosest = pSpawn;
            }
        }
    }
    return (pClosest!=NULL);
}

BOOL IsAlert(PSPAWNINFO pChar, PSPAWNINFO pSpawn, DWORD List)
{
    CHAR szName[MAX_STRING] = {0};
    SEARCHSPAWN SearchSpawn;
    PALERT pCurrent = GetAlert(List);
    if (!pCurrent) return NULL;
    for (; pCurrent; pCurrent = pCurrent->pNext) {
        CopyMemory(&SearchSpawn,&(pCurrent->SearchSpawn),sizeof(SEARCHSPAWN));
        if ((SearchSpawn.SpawnID>0) && (SearchSpawn.SpawnID!=pSpawn->SpawnID)) continue;
        SearchSpawn.SpawnID = pSpawn->SpawnID;
        // if this spawn matches, it's true 
        // this is an implied logical or
        if (SpawnMatchesSearch(&SearchSpawn, pChar, pSpawn))
            return TRUE;
        //if (SearchThroughSpawns(&SearchSpawn,pChar)) {
        //      return TRUE;
        //{
    }
    return FALSE;
}

BOOL CheckAlertForRecursion(PALERT pAlert,DWORD AlertNumber)
{
    PALERT pNextAlert = NULL;
    if (!pAlert) return FALSE;
    for (;pAlert;pAlert=pAlert->pNext) {
        if (pAlert->SearchSpawn.bAlert) {
            if (pAlert->SearchSpawn.AlertList == AlertNumber) {
                return TRUE;
            }
            if (pNextAlert = GetAlert(pAlert->SearchSpawn.AlertList)) {
                if (CheckAlertForRecursion(pNextAlert,AlertNumber)) return TRUE;
                if (CheckAlertForRecursion(pNextAlert,pAlert->SearchSpawn.AlertList)) return TRUE;
            }
        }
        if (pAlert->SearchSpawn.bNoAlert) {
            if (pAlert->SearchSpawn.NoAlertList == AlertNumber) {
                return TRUE;
            }
            if (pNextAlert = GetAlert(pAlert->SearchSpawn.NoAlertList)) {
                if (CheckAlertForRecursion(pNextAlert,AlertNumber)) return TRUE;
                if (CheckAlertForRecursion(pNextAlert,pAlert->SearchSpawn.NoAlertList)) return TRUE;
            }
        }
        if (pAlert->SearchSpawn.bNearAlert) {
            if (pAlert->SearchSpawn.NearAlertList == AlertNumber) {
                return TRUE;
            }
            if (pNextAlert = GetAlert(pAlert->SearchSpawn.NearAlertList)) {
                if (CheckAlertForRecursion(pNextAlert,AlertNumber)) return TRUE;
                if (CheckAlertForRecursion(pNextAlert,pAlert->SearchSpawn.NearAlertList)) return TRUE;
            }
        }
        if (pAlert->SearchSpawn.bNotNearAlert) {
            if (pAlert->SearchSpawn.NotNearAlertList == AlertNumber) {
                return TRUE;
            }
            if (pNextAlert = GetAlert(pAlert->SearchSpawn.NotNearAlertList)) {
                if (CheckAlertForRecursion(pNextAlert,AlertNumber)) return TRUE;
                if (CheckAlertForRecursion(pNextAlert,pAlert->SearchSpawn.NotNearAlertList)) return TRUE;
            }
        }
    }
    return FALSE;
}

// ***************************************************************************
// Function:    CleanupName
// Description: Cleans up NPC names
//              an_iksar_marauder23 = iksar marauder, an
// ***************************************************************************
PCHAR CleanupName(PCHAR szName, BOOL Article, BOOL ForWhoList)
{
    DWORD i,j=0;
    CHAR szTemp[MAX_STRING] = {0};
    for (i=0;i<strlen(szName);i++) {
        switch (szName[i]) 
        {
        case '_':
            szTemp[j++]=' ';
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            break;
        case '#':
            if (!ForWhoList)
                break;
        default:
            szTemp[j++]=szName[i];
        }
    }
    strcpy(szName,szTemp);
    if (!Article) return szName;
    if (!strnicmp(szName,"a ",2)) {
        sprintf(szTemp, "%s, a",szName+2);
        strcpy(szName,szTemp);
    } else if (!strnicmp(szName,"an ",3)) {
        sprintf(szTemp, "%s, an",szName+3);
        strcpy(szName,szTemp);
    } else if (!strnicmp(szName,"the ",4)) {
        sprintf(szTemp, "%s, the",szName+4);
        strcpy(szName,szTemp);
    }
    return szName;
}

// ***************************************************************************
// Function:    SuperWhoDisplay
// Description: Displays our SuperWho / SuperWhoTarget
// ***************************************************************************
VOID SuperWhoDisplay(PSPAWNINFO pSpawn, DWORD Color)
{
    CHAR szName[MAX_STRING]={0};
    CHAR szMsg[MAX_STRING]={0};
    CHAR szMsgL[MAX_STRING]={0};
    CHAR szTemp[MAX_STRING]={0};
    strcpy(szName,pSpawn->Name);
    if (pSpawn->Type == SPAWN_PLAYER) {
        if (gFilterSWho.Lastname && strlen(pSpawn->Lastname)>0) {
            strcat(szName," ");
            strcat(szName,pSpawn->Lastname);
        }
        if (gFilterSWho.Guild && pSpawn->GuildID != 0xFFFFFFFF && pGuildList) {
            strcat(szName," <");
            char *szGuild = GetGuildByID(pSpawn->GuildID);
            strcat(szName, szGuild ? szGuild : "Unknown Guild");
            strcat(szName,">");
        }
    } else {
        CleanupName(szName);
        if (gFilterSWho.Lastname && strlen(pSpawn->Lastname)>0) {
            strcat(szName," (");
            strcat(szName,pSpawn->Lastname);
            strcat(szName,")");
        }
    }
    CHAR GM[MAX_STRING] = {0};
    if (gFilterSWho.GM && pSpawn->GM) {
        if (pSpawn->Level >= 50) {
            strcpy(GM,"\ay*GM*\ax");
        } else if (pSpawn->Level == 20) {
            strcpy(GM,"\a-y*Guide Applicant*\ax");
        } else {
            strcpy(GM,"\a-y*Guide*\ax");
        }
    }
    szMsg[0]='\a';
    szMsg[2]=0;
    if (Color || gFilterSWho.ConColor)
    {
        switch(ConColor(pSpawn))
        {
        case CONCOLOR_WHITE:
            szMsg[1]='w';
            break;
        case CONCOLOR_YELLOW:
            szMsg[1]='y';
            break;
        case CONCOLOR_RED:
            szMsg[1]='r';
            break;
        case CONCOLOR_BLUE:
            szMsg[1]='u';
            break;
        case CONCOLOR_LIGHTBLUE:
            szMsg[1]='t';
            break;
        case CONCOLOR_GREEN:
            szMsg[1]='g';
            break;
        case CONCOLOR_GREY:
            szMsg[1]='-';
            szMsg[2]='w';
            break;
        default:
            szMsg[1]='m';
            break;
        }
    }
    else
    {
        szMsg[1]='w';
    }
    if (gFilterSWho.GM) strcat(szMsg,GM);
    if (gFilterSWho.Level || gFilterSWho.Race || gFilterSWho.Body || gFilterSWho.Class) {
        strcat(szMsg,"\a-u[\ax");
        if (gFilterSWho.Level) {
            itoa(pSpawn->Level,szTemp,10);
            strcat(szMsg,szTemp);
            strcat(szMsg," ");
        }
        if (gFilterSWho.Race) {
            strcat(szMsg,pEverQuest->GetRaceDesc(pSpawn->Race));
            strcat(szMsg," ");
        }
        if (gFilterSWho.Body) {
            strcat(szMsg,GetBodyTypeDesc(GetBodyType(pSpawn)));
            strcat(szMsg," ");
        }
        if (gFilterSWho.Class) {
            strcat(szMsg,GetClassDesc(pSpawn->Class));
            strcat(szMsg," ");
        }
        szMsg[strlen(szMsg)-1]=0;
        strcat(szMsg,"\a-u]\ax");
    }
    strcat(szMsg," ");
    strcat(szMsg,szName);
    //strcat(szMsg,"\ax");

    if (pSpawn->Type == SPAWN_PLAYER) {
        if (gFilterSWho.Anon && pSpawn->Anon>0) {
            if (pSpawn->Anon==2) {
                strcat(szMsg," \ag*RP*\ax");
            } else {
                strcat(szMsg," \ag*Anon*\ax");
            }
        }
        if (gFilterSWho.LD && pSpawn->Linkdead) strcat(szMsg," \ag<LD>\ax");
        if (gFilterSWho.Sneak && pSpawn->Sneak) strcat(szMsg," \ag<Sneak>\ax"); 
        if (gFilterSWho.AFK && pSpawn->AFK) strcat(szMsg," \ag<AFK>\ax");
        if (gFilterSWho.LFG && pSpawn->LFG) strcat(szMsg," \ag<LFG>\ax");
        if (gFilterSWho.Trader && pSpawn->Trader) strcat(szMsg," \ag<Trader>\ax");
    } else if (gFilterSWho.NPCTag && pSpawn->Type == SPAWN_NPC) {
        if (pSpawn->MasterID != 0) {
            strcat(szMsg," <PET>");
        } else {
            strcat(szMsg," <NPC>");
        }
    }
    if (gFilterSWho.Light) {
        PCHAR szLight = GetLightForSpawn(pSpawn);
        if (stricmp(szLight,"NONE")) {
            strcat(szMsg," (");
            strcat(szMsg,szLight);
            strcat(szMsg,")");
        }
    }
    strcpy(szMsgL,szMsg);
    if (gFilterSWho.Distance) 
    {
        INT Angle = (INT)((atan2f(GetCharInfo()->pSpawn->X - pSpawn->X, GetCharInfo()->pSpawn->Y - pSpawn->Y) * 180.0f / PI + 360.0f) / 22.5f + 0.5f) % 16;
        sprintf(szTemp," \a-u(\ax%1.2f %s\a-u,\ax %1.2fZ\a-u)\ax", GetDistance(GetCharInfo()->pSpawn,pSpawn), szHeadingShort[Angle], pSpawn->Z-GetCharInfo()->pSpawn->Z);
        strcat(szMsg,szTemp);
    }
    if (gFilterSWho.SpawnID) 
    {
        strcat(szMsg," \a-u(\axID:");
        itoa(pSpawn->SpawnID,szTemp,10);
        strcat(szMsg,szTemp);
        strcat(szMsg,"\a-u)\ax");
    }
    if (gFilterSWho.Holding && (pSpawn->Equipment.Primary.ID || pSpawn->Equipment.Offhand.ID )) 
    {
        strcat(szMsg," \a-u(\ax");
        if (pSpawn->Equipment.Primary.ID)
        {
            itoa(pSpawn->Equipment.Primary.ID,szTemp,10);
            strcat(szMsg,"Pri: ");
            strcat(szMsg,szTemp);
            if (pSpawn->Equipment.Offhand.ID)
                strcat(szMsg," ");
        }
        if (pSpawn->Equipment.Offhand.ID)
        {
            itoa(pSpawn->Equipment.Offhand.ID,szTemp,10);
            strcat(szMsg,"Off: ");
            strcat(szMsg,szTemp);
        }
        strcat(szMsg,"\a-u)\ax");
    }

    switch(GetSpawnType(pSpawn))
    {
    case CHEST:
        strcat(szMsg," \ar*CHEST*\ax");
        break;
    case TRAP:
        strcat(szMsg," \ar*TRAP*\ax");
        break;
    case TRIGGER:
        strcat(szMsg," \ar*TRIGGER*\ax");
        break;
    case TIMER:
        strcat(szMsg," \ar*TIMER*\ax");
        break;
    case UNTARGETABLE:
        strcat(szMsg," \ar*UNTARGETABLE*\ax");
        break;
    }

    WriteChatColor(szMsg,USERCOLOR_WHO);
}

DWORD SWhoSortValue=0;
PSPAWNINFO SWhoSortOrigin=0;

static int pWHOSORTCompare(const void *A, const void *B)
{
    PSPAWNINFO SpawnA=*(PSPAWNINFO*)A;
    PSPAWNINFO SpawnB=*(PSPAWNINFO*)B;
    switch(SWhoSortValue)
    {
        /*
        PCHAR szSortBy[] = { 
        "level",   // Default sort by 
        "name", 
        "race", 
        "class", 
        "distance", 
        "guild", 
        "id", 
        NULL }; 
        /**/
    case 0://level
        if (SpawnA->Level>SpawnB->Level)
            return 1;
        if (SpawnA->Level<SpawnB->Level)
            return -1;
        break;
    //case 1://name   done at the bottom ;)
    //    break;
    case 2://race
        {
            int RaceCompare=stricmp(pEverQuest->GetRaceDesc(SpawnA->Race),pEverQuest->GetRaceDesc(SpawnB->Race));
            if (RaceCompare)
                return RaceCompare;
        }
        break;
    case 3://class
        {
            int ClassCompare=stricmp(GetClassDesc(SpawnA->Class),GetClassDesc(SpawnB->Class));
            if (ClassCompare)
                return ClassCompare;
        }
        break;
    case 4://distance
        {
            FLOAT DistA=GetDistance(SWhoSortOrigin,SpawnA);
            FLOAT DistB=GetDistance(SWhoSortOrigin,SpawnB);
            if (DistA>DistB)
                return 1;
            if (DistA<DistB)
                return -1;
        }
        break;
    case 5://guild
        {
            char *szGuild1 = GetGuildByID(SpawnA->GuildID);
            char *szGuild2 = GetGuildByID(SpawnB->GuildID);
            if(szGuild1 && szGuild2)
            {
                int GuildCompare=stricmp(szGuild1, szGuild2);
                if (GuildCompare)
                    return GuildCompare;
            }
            else
                return -1;
        }
        break;
    case 6://id
        if (SpawnA->SpawnID>SpawnB->SpawnID)
            return 1;
        if (SpawnA->SpawnID<SpawnB->SpawnID)
            return -1;
        break;
    }
    CHAR szNameA[MAX_STRING]={0};
    CHAR szNameB[MAX_STRING]={0};
    CleanupName(strcpy(szNameA,SpawnA->Name));
    CleanupName(strcpy(szNameB,SpawnB->Name));
    return stricmp(szNameA,szNameB);
}

VOID SuperWhoDisplay(PSPAWNINFO pChar, PSEARCHSPAWN pSearchSpawn, DWORD Color)
{
    if (!pSearchSpawn)
        return;
    CIndex<PSPAWNINFO> SpawnSet;
    PSPAWNINFO pSpawn=(PSPAWNINFO)pSpawnList;
    PSPAWNINFO pOrigin=0;
    // create our set
    DWORD TotalMatching=0;
    if (pSearchSpawn->FromSpawnID)
        pOrigin=(PSPAWNINFO)GetSpawnByID(pSearchSpawn->FromSpawnID);
    if (!pOrigin)
        pOrigin=pChar;
    while (pSpawn)
    {
        if (SpawnMatchesSearch(pSearchSpawn,pOrigin,pSpawn))
        {
            // matches search, add to our set
            TotalMatching++;
            SpawnSet+=pSpawn;
        }
        pSpawn=pSpawn->pNext;
    }
    if (TotalMatching)
    {
        if (TotalMatching>1)
        {
            // sort our list
            SWhoSortValue=pSearchSpawn->SortBy;
            SWhoSortOrigin=pOrigin;
            qsort(&SpawnSet.List[0],TotalMatching,sizeof(PSPAWNINFO),pWHOSORTCompare);
        }
        WriteChatColor("List of matching spawns",USERCOLOR_WHO);
        WriteChatColor("--------------------------------",USERCOLOR_WHO);
        for (DWORD N=0 ; N < TotalMatching ; N++)
        {
            SuperWhoDisplay(SpawnSet[N],Color);
        }
        PCHAR pszSpawnType;
        switch(pSearchSpawn->SpawnType)
        {
        case NONE:
        default:
            pszSpawnType="any";
            break;
        case PC:
            pszSpawnType="pc";
            break;
        case MOUNT:
            pszSpawnType="mount";
            break;
        case PET:
            pszSpawnType="pet";
            break;
        case NPC:
            pszSpawnType="npc";
            break;
        case CORPSE:
            pszSpawnType="corpse";
            break;
        case TRIGGER:
            pszSpawnType="trigger";
            break;
        case TRAP:
            pszSpawnType="trap";
            break;
        case CHEST:
            pszSpawnType="chest";
            break;
        case TIMER:
            pszSpawnType="timer";
            break;
        case UNTARGETABLE:
            pszSpawnType="untargetable";
            break;
        case MERCENARY:
            pszSpawnType="mercenary";
            break;
        case FLYER:
            pszSpawnType="flyer";
            break;
        }
        WriteChatf("There %s \ag%d\ax %s%s in %s.",(TotalMatching == 1)?"is":"are",TotalMatching, pszSpawnType, (TotalMatching==1)?"":"s", GetFullZone(GetCharInfo()->zoneId));
    }
    else
    {
        WriteChatColor("List of matching spawns",USERCOLOR_WHO);
        WriteChatColor("--------------------------------",USERCOLOR_WHO);
        CHAR szMsg[MAX_STRING]={0};
        FormatSearchSpawn(szMsg,pSearchSpawn);
        strcat(szMsg," was not found.");
        WriteChatColor(szMsg,USERCOLOR_WHO);
    }
}

DWORD WINAPI thrMsgBox(LPVOID lpParameter)
{
    MessageBox(NULL,(PCHAR)lpParameter,"MacroQuest",MB_OK);
    free(lpParameter);
    return 0;
}

FLOAT StateHeightMultiplier(DWORD StandState) 
{ 
    switch (StandState) { 
case STANDSTATE_BIND: 
case STANDSTATE_DUCK: 
    return 0.5f; 
case STANDSTATE_SIT: 
    return 0.3f; 
case STANDSTATE_FEIGN: 
case STANDSTATE_DEAD: 
    return 0.1f; 
case STANDSTATE_STAND: 
default: 
    return 0.9f; 
    } 
} 
DWORD FindSpellListByName(PCHAR szName)
{
    DWORD Index;
    for (Index=0;Index<10;Index++) {
        if (!stricmp(pSpellSets[Index].Name,szName)) return Index;
    }
    return -1;
}

VOID RewriteAliases(VOID)
{
    PALIAS pLoop = pAliases;
    WritePrivateProfileSection("Aliases","",gszINIFilename);
    while (pLoop) {
        WritePrivateProfileString("Aliases",pLoop->szName,pLoop->szCommand,gszINIFilename);
        pLoop = pLoop->pNext;
    }
}

VOID RewriteSubstitutions(VOID)
{
    PSUB pSubLoop = pSubs;
    WritePrivateProfileSection("Substitutions","",gszINIFilename);
    while (pSubLoop) {
        WritePrivateProfileString("Substitutions",pSubLoop->szOrig,pSubLoop->szSub,gszINIFilename);
        pSubLoop = pSubLoop->pNext;
    }
}

PCHAR GetFriendlyNameForGroundItem(PGROUNDITEM pItem, PCHAR szName)
{
    szName[0]=0;
    DWORD Item = atoi(pItem->Name + 2);
    struct _actordefentry *ptr = MQ2Globals::ActorDefList;

    while (ptr->Def) {
        if (ptr->Def == Item) {
            sprintf(szName,"%s",ptr->Name);
            return &szName[0];
        }
        ptr++;
    }
    sprintf(szName,"Drop%05d/%d",Item,pItem->DropID);
	return &szName[0];
}

// deprecated
#if 0
PCHAR GetModel(PSPAWNINFO pSpawn, DWORD Slot)
{
    if (!pSpawn) return NULL;
    if (Slot>20) return NULL;
    PMODELINFO pMod = pSpawn->Model[Slot];
    if (!pMod) return NULL;
    if (!pMod->pModelInfo) return NULL;
    if (pMod->pModelInfo->Type != 0x48) return NULL;
    PMODELINFO_48 pModInfo = (PMODELINFO_48)pMod->pModelInfo;
    if (!pModInfo->pModelName) return NULL;
    PCHAR szModel = pModInfo->pModelName->Name;
    if (!szModel) return szItemName[0];
    return szItemName[atoi(szModel+2)];
}
#endif

VOID SetDisplaySWhoFilter(PBOOL bToggle, PCHAR szFilter, PCHAR szToggle)
{
    CHAR szTemp[MAX_STRING] = {0};
    if (!stricmp(szToggle,"on")) *bToggle = TRUE;
    else if (!stricmp(szToggle,"off")) *bToggle = FALSE;
    sprintf(szTemp,"%s is: %s",szFilter,(*bToggle)?"on":"off");
    WriteChatColor(szTemp,USERCOLOR_DEFAULT);
    itoa(*bToggle,szTemp,10);
    WritePrivateProfileString("SWho Filter",szFilter,szTemp,gszINIFilename);
}

VOID WriteFilterNames(VOID)
{
    CHAR szBuffer[MAX_STRING] = {0};
    int filternumber=1;
    PFILTER pFilter = gpFilters;
    WritePrivateProfileSection("Filter Names",szBuffer,gszINIFilename);
    while (pFilter) {
        if (pFilter->pEnabled == &gFilterCustom) {
            sprintf(szBuffer,"Filter%d",filternumber++);
            WritePrivateProfileString("Filter Names",szBuffer,pFilter->FilterText, gszINIFilename);
        }
        pFilter = pFilter->pNext;
    }

}
bool GetShortBuffID(PSPELLBUFF pBuff, DWORD &nID)
{
    PCHARINFO2 pChar=GetCharInfo2();
    unsigned long N=(pBuff-&pChar->ShortBuff[0]);
    if (N<NUM_SHORT_BUFFS)
    {
        nID=N+1;
        return true;
    }
    return false;
}
bool GetBuffID(PSPELLBUFF pBuff, DWORD &nID)
{
    PCHARINFO2 pChar=GetCharInfo2();
    unsigned long N=(pBuff-&pChar->Buff[0]);
    if (N<NUM_LONG_BUFFS)
    {
        nID=N+1;
        return true;
    }
    return false;
}

#define IS_SET(flag, bit)   ((flag) & (bit))
#define LDON_Non    0
#define LDON_DG     1
#define LDON_MIR    2
#define LDON_MIS    4
#define LDON_RUJ    8
#define LDON_TAK    16

PCHAR GetLDoNTheme(DWORD LDTheme)
{
    if (LDTheme == 31) return "All";
    if (IS_SET(LDTheme, LDON_DG )) return "Deepest Guk";
    if (IS_SET(LDTheme, LDON_MIR)) return "Miragul's";
    if (IS_SET(LDTheme, LDON_MIS)) return "Mistmoore";
    if (IS_SET(LDTheme, LDON_RUJ)) return "Rujarkian";
    if (IS_SET(LDTheme, LDON_TAK)) return "Takish";
    return "Unknown";
} 

DWORD GetItemTimer(PCONTENTS pItem)
{
    DWORD Timer=pPCData->GetItemTimerValue((EQ_Item*)&pItem);
    if(Timer<GetFastTime()) return 0;
    return Timer-GetFastTime();
}

PCONTENTS GetItemContentsBySlotID(DWORD dwSlotID)
{
    int InvSlot=-1; 
    int SubSlot=-1; 
    if(dwSlotID>=0 && dwSlotID<NUM_INV_SLOTS) InvSlot=dwSlotID; 
    else if(dwSlotID>=262 && dwSlotID<342) { 
        InvSlot=BAG_SLOT_START+(dwSlotID-262)/10; 
        SubSlot=(dwSlotID-262)%10; 
    } 
    if(InvSlot>=0 && InvSlot<NUM_INV_SLOTS) { 
        if(PCONTENTS iSlot=GetCharInfo2()->pInventoryArray->InventoryArray[InvSlot]) {
            if(SubSlot<0) return iSlot; 
            if(GetCharInfo2()->pInventoryArray->InventoryArray[InvSlot]->pContentsArray)
            if(PCONTENTS sSlot=GetCharInfo2()->pInventoryArray->InventoryArray[InvSlot]->pContentsArray->Contents[SubSlot]) return sSlot; 
        } 
    } 
    return NULL; 
}

PCONTENTS GetItemContentsByName(CHAR *ItemName)
{
    for(unsigned long nSlot=0; nSlot<NUM_INV_SLOTS; nSlot++) {
		PCHARINFO2 pChar2 = GetCharInfo2();
		if(pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) {
			if(PCONTENTS pItem=pChar2->pInventoryArray->InventoryArray[nSlot]) {
				if(!strcmp(ItemName,GetItemFromContents(pItem)->Name)) {
					return pItem;
				}
			}
		}
	}


    for (unsigned long nPack=0 ; nPack < 10 ; nPack++)
        if (PCONTENTS pPack=GetCharInfo2()->pInventoryArray->Inventory.Pack[nPack])
            if (GetItemFromContents(pPack)->Type==ITEMTYPE_PACK && pPack->pContentsArray)
                for (unsigned long nItem=0 ; nItem < GetItemFromContents(pPack)->Slots ; nItem++)
                    if (PCONTENTS pItem=pPack->pContentsArray->Contents[nItem])
                        if (!stricmp(ItemName,GetItemFromContents(pItem)->Name)) return pItem;

    return NULL; 
}

CXWnd * GetParentWnd(class CXWnd const * pWnd)
{
    CXWnd * tWnd=(CXWnd *)pWnd;
    while (tWnd)
    {
        if (!tWnd->pParentWindow) return tWnd;
        tWnd=(CXWnd *)tWnd->pParentWindow;
    }
    return NULL;
}

bool LoH_HT_Ready() 
{
    DWORD i=((PSPAWNINFO)pLocalPlayer)->InnateETA;
    DWORD j=i-((PCDISPLAY)pDisplay)->TimeStamp;
    if(i<j) return true;
    return false;
}

DWORD GetSkillIDFromName(PCHAR name)
{
    for(DWORD i=0; i<NUM_SKILLS; i++)
        if (PSKILL pSkill=pSkillMgr->pSkill[i])
            if(!stricmp(name,pStringTable->getString(pSkill->nName,0)))
                return i;
    return 0;
}

bool InHoverState()
{
    if(GetCharInfo() && GetCharInfo()->Stunned==3)
        return true;
    return false;
}

DWORD GetGameState(VOID)
{
    if (!ppEverQuest || !pEverQuest) 
    {
        //DebugSpew("Could not retrieve gamestate in GetGameState()");
        return -1;
    }
    return ((PEVERQUEST)pEverQuest)->GameState;
}

// ***************************************************************************
// Function:    SpellEffectTest
// Description: Return boolean true if the spell effect is to be ignored
//              for stacking purposes
// ***************************************************************************
bool SpellEffectTest (PSPELL aSpell, PSPELL bSpell, int i){
	return ((aSpell->Attrib[i]==57 || bSpell->Attrib[i]==57)		// Levitate
		 || (aSpell->Attrib[i]==134 || bSpell->Attrib[i]==134)		// Limit: Max Level
		 || (aSpell->Attrib[i]==135 || bSpell->Attrib[i]==135)		// Limit: Resist
		 || (aSpell->Attrib[i]==136 || bSpell->Attrib[i]==136)		// Limit: Target
		 || (aSpell->Attrib[i]==137 || bSpell->Attrib[i]==137)		// Limit: Effect
		 || (aSpell->Attrib[i]==138 || bSpell->Attrib[i]==138)		// Limit: SpellType
		 || (aSpell->Attrib[i]==139 || bSpell->Attrib[i]==139)		// Limit: Spell
		 || (aSpell->Attrib[i]==140 || bSpell->Attrib[i]==140)		// Limit: Min Duraction
		 || (aSpell->Attrib[i]==141 || bSpell->Attrib[i]==141)		// Limit: Instant
		 || (aSpell->Attrib[i]==142 || bSpell->Attrib[i]==142)		// Limit: Min Level
		 || (aSpell->Attrib[i]==143 || bSpell->Attrib[i]==143)		// Limit: Min Cast Time
		 || (aSpell->Attrib[i]==144 || bSpell->Attrib[i]==144)		// Limit: Max Cast Time
		 || (aSpell->Attrib[i]==254 || bSpell->Attrib[i]==254)		// Placeholder
		 || (aSpell->Attrib[i]==311 || bSpell->Attrib[i]==311)		// Limit: Combat Skills not Allowed
		 || (aSpell->Attrib[i]==385 || bSpell->Attrib[i]==385)		// Limit: SpellGroup
		 || (aSpell->Attrib[i]==391 || bSpell->Attrib[i]==391)		// Limit: Max Mana
		 || (aSpell->Attrib[i]==403 || bSpell->Attrib[i]==403)		// Limit: SpellClass
		 || (aSpell->Attrib[i]==404 || bSpell->Attrib[i]==404)		// Limit: SpellSubclass
		 || (aSpell->Attrib[i]==411 || bSpell->Attrib[i]==411)		// Limit: PlayerClass
		 || (aSpell->Attrib[i]==412 || bSpell->Attrib[i]==412)		// Limit: Race
		 || (aSpell->Attrib[i]==422 || bSpell->Attrib[i]==422)		// Limit: Use Min
		 || (aSpell->Attrib[i]==423 || bSpell->Attrib[i]==423)		// Limit: Use Type
	     || (aSpell->Attrib[i]==428 || bSpell->Attrib[i]==428)		// Skill_Proc_Modifier
		 || ((aSpell->Attrib[i]==2 && bSpell->Attrib[i]==2) &&      // ATK
				((aSpell->SpellType==1 || aSpell->SpellType==2) && (bSpell->SpellType==1 || bSpell->SpellType==2)) &&
				!(aSpell->DurationWindow==bSpell->DurationWindow))
		 || ((aSpell->Attrib[i]==4 && bSpell->Attrib[i]==4) &&      // STR
				((aSpell->SpellType==1 || aSpell->SpellType==2) && (bSpell->SpellType==1 || bSpell->SpellType==2)) &&
				!(aSpell->DurationWindow==bSpell->DurationWindow))
		 || ((aSpell->Attrib[i]==5 && bSpell->Attrib[i]==5) &&      // DEX
				((aSpell->SpellType==1 || aSpell->SpellType==2) && (bSpell->SpellType==1 || bSpell->SpellType==2)) &&
				!(aSpell->DurationWindow==bSpell->DurationWindow))
		 || ((aSpell->Attrib[i]==6 && bSpell->Attrib[i]==6) &&      // AGI
				((aSpell->SpellType==1 || aSpell->SpellType==2) && (bSpell->SpellType==1 || bSpell->SpellType==2)) &&
				!(aSpell->DurationWindow==bSpell->DurationWindow))
		 || ((aSpell->Attrib[i]==7 && bSpell->Attrib[i]==7) &&      // STA
				((aSpell->SpellType==1 || aSpell->SpellType==2) && (bSpell->SpellType==1 || bSpell->SpellType==2)) &&
				!(aSpell->DurationWindow==bSpell->DurationWindow))
		 || ((aSpell->Attrib[i]==8 && bSpell->Attrib[i]==8) &&      // INT
				((aSpell->SpellType==1 || aSpell->SpellType==2) && (bSpell->SpellType==1 || bSpell->SpellType==2)) &&
				!(aSpell->DurationWindow==bSpell->DurationWindow))
		 || ((aSpell->Attrib[i]==9 && bSpell->Attrib[i]==9) &&      // WIS
				((aSpell->SpellType==1 || aSpell->SpellType==2) && (bSpell->SpellType==1 || bSpell->SpellType==2)) &&
				!(aSpell->DurationWindow==bSpell->DurationWindow)));
}

// ***************************************************************************
// Function:    BuffStackTest
// Description: Return boolean true if the two spells will stack
// Usage:       Used by ${Spell[xxx].Stacks}, ${Spell[xxx].StacksPet},
//                ${Spell[xxx].WillStack[yyy]}, ${Spell[xxx].StacksWith[yyy]}
// Author:      Pinkfloydx33
// ***************************************************************************
bool BuffStackTest(PSPELL aSpell, PSPELL bSpell){
    if (aSpell->ID==bSpell->ID) return true;

    int i;
    for (i=0; i<=11; i++) {
        //Compare 1st Buff to 2nd. If Attrib[i]==254 its a place holder. If it is 10 it
        //can be 1 of 3 things: PH(Base=0), CHA(Base>0), Lure(Base=-6). If it is Lure or
        //Placeholder, exclude it so slots don't match up. Now Check to see if the slots
        //have equal attribute values. If the do, they don't stack.
        //WriteChatf("\nSlot %d: bSpell->Attrib=%d, bSpell->Base=%d, bSpell->TargetType=%d, aSpell->Attrib=%d, aSpell->Base=%d, aSpell->TargetType=%d", i, bSpell->Attrib[i], bSpell->Base[i], bSpell->TargetType, aSpell->Attrib[i], aSpell->Base[i], aSpell->TargetType);
		if (bSpell->Attrib[i]==aSpell->Attrib[i] && !(SpellEffectTest(aSpell, bSpell, i)))
            if (!((bSpell->Attrib[i]==10 && (bSpell->Base[i]==-6 || bSpell->Base[i]==0)) ||
                (aSpell->Attrib[i]==10 && (aSpell->Base[i]==-6 || aSpell->Base[i]==0)) ||
                (bSpell->Attrib[i]==79 && bSpell->Base[i]>0 && bSpell->TargetType==6) ||
                (aSpell->Attrib[i]==79 && aSpell->Base[i]>0 && aSpell->TargetType==6) ||
                (bSpell->Attrib[i]==0  && bSpell->Base[i]<0) ||
                (aSpell->Attrib[i]==0  && aSpell->Base[i]<0) ||
                (bSpell->Attrib[i]==148 || bSpell->Attrib[i]==149) ||
                (aSpell->Attrib[i]==148 || aSpell->Attrib[i]==149)))
                return false;
        //Check to see if second buffs blocks first buff:
        //148: Stacking: Block new spell if slot %d is effect
        //149: Stacking: Overwrite existing spell if slot %d is effect
        if ((bSpell->Attrib[i]==148) || (bSpell->Attrib[i]==149)) {
            int tmpSlot = bSpell->Calc[i]-200;                           
            int tmpAttrib = bSpell->Base[i];
            //WriteChatf("aSpell->Attrib[%d]=%d, aSpell->Base[%d]=%d, tmpAttrib=%d, tmpVal=%d", tmpSlot-1, aSpell->Attrib[tmpSlot-1], tmpSlot-1, aSpell->Base[tmpSlot-1], tmpAttrib, abs(bSpell->Max[i]));
            if (bSpell->Max[i] > 0) {
                int tmpVal = abs(bSpell->Max[i]);
                if ((aSpell->Attrib[tmpSlot-1] == tmpAttrib) && (aSpell->Base[tmpSlot-1] < tmpVal)) return false;
            } else if (aSpell->Attrib[tmpSlot-1] == tmpAttrib) return false;
        }
        //Now Check to see if the first buff blocks second buff. This is necessary 
        //because only some spells carry the Block Slot. Ex. Brells and Spiritual 
        //Vigor don't stack Brells has 1 slot total, for HP. Vigor has 4 slots, 2 
        //of which block Brells.
        if ((aSpell->Attrib[i]==148) || (aSpell->Attrib[i]==149)) {
            int tmpSlot = aSpell->Calc[i]-200;
            int tmpAttrib = aSpell->Base[i];
            //WriteChatf("bSpell->Attrib[%d]=%d, bSpell->Base[%d]=%d, tmpAttrib=%d, tmpVal=%d", tmpSlot-1, bSpell->Attrib[tmpSlot-1], tmpSlot-1, bSpell->Base[tmpSlot-1], tmpAttrib, abs(aSpell->Max[i]));
            if (aSpell->Max[i] > 0) {
                int tmpVal = abs(aSpell->Max[i]);
                if ((bSpell->Attrib[tmpSlot-1] == tmpAttrib) && (bSpell->Base[tmpSlot-1] < tmpVal)) return false;
            } else if (bSpell->Attrib[tmpSlot-1] == tmpAttrib) return false;   
        }
    }      
    return true;
} 

float GetMeleeRange(class EQPlayer *pSpawn1,class EQPlayer *pSpawn2)
{
    float f, g, h;
    if(pSpawn1 && pSpawn2)
    {
        f=((PSPAWNINFO)pSpawn1)->GetMeleeRangeVar1 * ((PSPAWNINFO)pSpawn1)->GetMeleeRangeVar2;
        g=((PSPAWNINFO)pSpawn2)->GetMeleeRangeVar1 * ((PSPAWNINFO)pSpawn2)->GetMeleeRangeVar2;

        h=abs(((PSPAWNINFO)pSpawn1)->AvatarHeight - ((PSPAWNINFO)pSpawn2)->AvatarHeight);

        f=(f+g)*0.75f;

        if(f<14.0f)
            f=14.0f;

        g=f+2+h;

        if(g>75.0f)
            return 75.0f;
        else
            return g;
    }
    return 14.0f;
}

DWORD GetSpellGemTimer(DWORD nGem)
{
    _EQCASTSPELLGEM *g = ((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[nGem];

    if(g->TimeStamp)
        return g->TimeStamp + g->RecastTime - EQGetTime();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions that were built into commands and people used DoCommand to execute                  //
void AttackRanged(EQPlayer *pRangedTarget)
{
    if (pRangedTarget && gbRangedAttackReady)
    {
        pLocalPlayer->DoAttack(0x0B,0,pRangedTarget);
        gbRangedAttackReady=0;
    }
}

void UseAbility(char *sAbility) {

    CHAR szBuffer[MAX_STRING] = {0};
    sprintf(szBuffer, "%s",sAbility);

    if (!cmdDoAbility)
    {
        PCMDLIST pCmdListOrig = (PCMDLIST)EQADDR_CMDLIST;
        for (int i=0;pCmdListOrig[i].fAddress != 0;i++) {
            if (!strcmp(pCmdListOrig[i].szName,"/doability")) {
                cmdDoAbility = (fEQCommand)pCmdListOrig[i].fAddress;
            }
        }
    }
    if (!cmdDoAbility) return;

    //if (argc<2 || atoi(argv[1]) || !EQADDR_DOABILITYLIST) {
    if (atoi(szBuffer) || !EQADDR_DOABILITYLIST) {
        cmdDoAbility((PSPAWNINFO)pLocalPlayer,szBuffer);
        return;
    }

    DWORD Index, DoIndex = 0xFFFFFFFF;
    PSPAWNINFO pChar =(PSPAWNINFO)pCharSpawn;

    for (Index=0;Index<10;Index++) {
        if (EQADDR_DOABILITYLIST[Index]!= 0xFFFFFFFF) {
            if (!strnicmp(szBuffer,szSkills[EQADDR_DOABILITYLIST[Index]],strlen(szSkills[EQADDR_DOABILITYLIST[Index]]))) {
                if (Index<4) {
                    DoIndex = Index+7; // 0-3 = Combat abilities (7-10)
                } else {
                    DoIndex = Index-3; // 4-9 = Abilities (1-6)
                }
            }
        }
    }

    if (DoIndex!=0xFFFFFFFF) {
        cmdDoAbility(pChar,itoa(DoIndex,szBuffer,10));
    } else {
        PSPELL pCA = NULL;
        for (Index=0;Index<NUM_COMBAT_ABILITIES;Index++) {

            if (GetCharInfo2()->CombatAbilities[Index]) {

                pCA = GetSpellByID(GetCharInfo2()->CombatAbilities[Index]);
                if (!stricmp(pCA->Name, szBuffer)) {
                    //We got the cookie, let's try and do it 
                    pCharData->DoCombatAbility(pCA->ID);
                    break;
                }
            }
        }
        if (Index >= NUM_COMBAT_ABILITIES)
            WriteChatColor("You do not seem to have that ability available",USERCOLOR_DEFAULT);
    }
    return;
}

// Function to check if the account has a given expansion enabled.
// Pass exansion macros from EQData.h to it -- e.g. HasExpansion(EXPANSION_RoF)
bool HasExpansion(DWORD nExpansion)
{
    return (bool)((GetCharInfo()->ExpansionFlags & nExpansion) != 0);
}
//Just a Function that needs more work
//I use this to test merc aa struct -eqmule
VOID ListMercAltAbilities()
{
	if(pMercAltAbilities) {
		int mercaapoints = ((PCHARINFO)pCharData)->MercAAPoints;
		for(int i=0;i<12;i++) {
			PEQMERCALTABILITIES pinfo = pMercAltAbilities;
			if(pinfo->MercAAInfo[i]) {
				if(pinfo->MercAAInfo[i]->Ptr) {
					int nName = pinfo->MercAAInfo[i]->Ptr->nName;
					int maxpoints = pinfo->MercAAInfo[i]->Max;
					if(nName) {
						CHAR szBuffer[256] = {0};
						sprintf(szBuffer,"%s",pCDBStr->GetString(nName, 37, NULL));
						WriteChatf("You have %d mercaapoints to spend on %s (max is %d)",mercaapoints,szBuffer,maxpoints);
					}
				}
			}
		}
	}
}
PCONTENTS FindItemByName(PCHAR pName, BOOL bExact)
{
	CHAR Name[MAX_STRING]={0};
	CHAR Temp[MAX_STRING]={0};
	strlwr(strcpy(Name, pName));
	PCHARINFO2 pChar2 = GetCharInfo2();
	if(pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) {
		for (unsigned long nSlot=0 ; nSlot < NUM_INV_SLOTS ; nSlot++)
		{
			if (PCONTENTS pItem=pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (bExact)
				{
					if (!stricmp(Name,GetItemFromContents(pItem)->Name))
					{
						return pItem;
					}
				} else {
					if(strstr(strlwr(strcpy(Temp,GetItemFromContents(pItem)->Name)),Name))
					{
						return pItem;
					}
				}
			}
		}
	}
	if(pChar2 && pChar2->pInventoryArray) {
		for (unsigned long nPack=0 ; nPack < 10 ; nPack++)
		{
			if (PCONTENTS pPack=pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (GetItemFromContents(pPack)->Type==ITEMTYPE_PACK && pPack->pContentsArray)
				{
					for (unsigned long nItem=0 ; nItem < GetItemFromContents(pPack)->Slots ; nItem++)
					{
						if (PCONTENTS pItem=pPack->pContentsArray->Contents[nItem])
						{
							if (bExact)
							{
								if (!stricmp(Name,GetItemFromContents(pItem)->Name))
								{
									return pItem;
								}
							} else {
								if(strstr(strlwr(strcpy(Temp,GetItemFromContents(pItem)->Name)),Name))
								{
									return pItem;
								}
							}
						}
					}
				}
			}
		}
	}
    return 0;
}
PCONTENTS FindItemByID(DWORD ItemID)
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	if(pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) {
		for (unsigned long nSlot=0 ; nSlot < NUM_INV_SLOTS ; nSlot++)
		{
			if (PCONTENTS pItem=pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (ItemID==GetItemFromContents(pItem)->ItemNumber)
				{
						return pItem;
				}
			}
		}
	}
	if(pChar2 && pChar2->pInventoryArray) {
		for (unsigned long nPack=0 ; nPack < 10 ; nPack++)
		{
			if (PCONTENTS pPack=pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (GetItemFromContents(pPack)->Type==ITEMTYPE_PACK && pPack->pContentsArray)
				{
					for (unsigned long nItem=0 ; nItem < GetItemFromContents(pPack)->Slots ; nItem++)
					{
						if (PCONTENTS pItem=pPack->pContentsArray->Contents[nItem])
						{
							if (ItemID==GetItemFromContents(pItem)->ItemNumber) {
									return pItem;
							}
						}
					}
				}
			}
		}
	}
    return 0;
}
PCONTENTS FindItemBySlot(WORD InvSlot, WORD BagSlot)
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	//check regular inventory
	if(pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) {
		for (unsigned long nSlot=0 ; nSlot < NUM_INV_SLOTS ; nSlot++)
		{
			if (PCONTENTS pItem=pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (pItem->ItemSlot==InvSlot && pItem->ItemSlot2==BagSlot)
				{
					return pItem;
				}
			}
		}
	}
	//not found? ok check inside bags
	if(pChar2 && pChar2->pInventoryArray) {
		for (unsigned long nPack=0 ; nPack < 10 ; nPack++)
		{
			if (PCONTENTS pPack=pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (GetItemFromContents(pPack)->Type==ITEMTYPE_PACK && pPack->pContentsArray)
				{
					for (unsigned long nItem=0 ; nItem < GetItemFromContents(pPack)->Slots ; nItem++)
					{
						if (PCONTENTS pItem=pPack->pContentsArray->Contents[nItem])
						{
							if (pItem->ItemSlot==InvSlot && pItem->ItemSlot2==BagSlot)	{
								return pItem;
							}
						}
					}
				}
			}
		}
	}
	//still not found? check bank
	PCHARINFO pChar = GetCharInfo();
	if(pChar && pChar->pBankArray && pChar->pBankArray->Bank) {
		for (unsigned long nSlot=0 ; nSlot < NUM_BANK_SLOTS ; nSlot++)
		{
			if (PCONTENTS pItem=pChar->pBankArray->Bank[nSlot])
			{
				if (pItem->ItemSlot==InvSlot && pItem->ItemSlot2==BagSlot)
				{
					return pItem;
				}
			}
		}
	}
	//not found? ok check inside bank bags
	if(pChar && pChar->pBankArray && pChar->pBankArray->Bank) {
		for (unsigned long nPack=0 ; nPack < NUM_BANK_SLOTS ; nPack++)
		{
			if (PCONTENTS pPack=pChar->pBankArray->Bank[nPack])
			{
				if (GetItemFromContents(pPack)->Type==ITEMTYPE_PACK && pPack->pContentsArray)
				{
					for (unsigned long nItem=0 ; nItem < GetItemFromContents(pPack)->Slots ; nItem++)
					{
						if (PCONTENTS pItem=pPack->pContentsArray->Contents[nItem])
						{
							if (pItem->ItemSlot==InvSlot && pItem->ItemSlot2==BagSlot)	{
								return pItem;
							}
						}
					}
				}
			}
		}
	}
	//what? still not found? ok fine, check shared bank
	if(pChar && pChar->pSharedBankArray && pChar->pSharedBankArray->SharedBank) {
		for (unsigned long nSlot=0 ; nSlot < NUM_SHAREDBANK_SLOTS ; nSlot++)
		{
			if (PCONTENTS pItem=pChar->pSharedBankArray->SharedBank[nSlot])
			{
				if (pItem->ItemSlot==InvSlot && pItem->ItemSlot2==BagSlot)
				{
					return pItem;
				}
			}
		}
	}
    //not found? ok check inside sharedbank bags
	if(pChar && pChar->pSharedBankArray && pChar->pSharedBankArray->SharedBank) {
		for (unsigned long nPack=0 ; nPack < NUM_SHAREDBANK_SLOTS ; nPack++)
		{
			if (PCONTENTS pPack=pChar->pSharedBankArray->SharedBank[nPack])
			{
				if (GetItemFromContents(pPack)->Type==ITEMTYPE_PACK && pPack->pContentsArray)
				{
					for (unsigned long nItem=0 ; nItem < GetItemFromContents(pPack)->Slots ; nItem++)
					{
						if (PCONTENTS pItem=pPack->pContentsArray->Contents[nItem])
						{
							if (pItem->ItemSlot==InvSlot && pItem->ItemSlot2==BagSlot)	{
								return pItem;
							}
						}
					}
				}
			}
		}
	}
    return 0;
}
PEQINVSLOT GetInvSlot(DWORD type,WORD invslot,WORD bagslot)
{
	PEQINVSLOTMGR pInvMgr=(PEQINVSLOTMGR)pInvSlotMgr;
	if(pInvMgr) {
		PEQINVSLOT pSlot = 0;
		for (DWORD i=0;i<pInvMgr->TotalSlots;i++) {
			pSlot = pInvMgr->SlotArray[i];	
			if (pSlot && pSlot->Valid && pSlot->pInvSlotWnd && pSlot->pInvSlotWnd->WindowType == type && pSlot->pInvSlotWnd->InvSlot == invslot && pSlot->pInvSlotWnd->BagSlot == bagslot) {
				CXMLData *pXMLData=((CXWnd*)pSlot->pInvSlotWnd)->GetXMLData();
				if(pXMLData) {
					CHAR szType[256] = {0};
					GetCXStr(pXMLData->ScreenID.Ptr,szType,255);
					if(!_stricmp(szType,"HB_InvSlot")) {//we dont want this, the user specified a container , not a hotbutton...
						continue;
					}
				}
				return pSlot;
			}
		}
	}
	return NULL;
}
//work in progress -eqmule
BOOL IsItemInsideContainer(PCONTENTS pItem)
{
	if(pItem && pItem->ItemSlot>=0 && pItem->ItemSlot<=NUM_INV_SLOTS) {
		PCHARINFO2 pChar2 = GetCharInfo2();
		if(pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray[pItem->ItemSlot]) {
			if(PCONTENTS pItemFound = pChar2->pInventoryArray->InventoryArray[pItem->ItemSlot]) {
				if(pItemFound!=pItem) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
BOOL OpenContainer(PCONTENTS pItem,bool hidden,bool flag)
{
	if(!pItem)
		return FALSE;
	if(PCONTENTS pcont = FindItemBySlot(pItem->ItemSlot)) {
		if(pcont->Open)
			return FALSE;
		if(GetItemFromContents(pcont)->Type==ITEMTYPE_PACK) {
			if(PEQINVSLOT pSlot = GetInvSlot(0,pcont->ItemSlot)) {
				if(hidden) {
					//put code to hide bag here
					//until i can figure out how to call moveitemqty
				}
				CMoveItemData To = {0};
				To.InventoryType = 0;To.Unknown0x02 = 0;
				To.InvSlot = pSlot->pInvSlotWnd->InvSlot;
				To.BagSlot = pSlot->pInvSlotWnd->BagSlot;
				To.GlobalSlot = pSlot->pInvSlotWnd->GlobalSlot;
				To.RandomNum = pSlot->pInvSlotWnd->RandomNum;
				To.Selection = (long)pcont;
				pContainerMgr->OpenContainer((EQ_Container*)&pcont,(int)&To,flag);
				//pPCData->AlertInventoryChanged();
				if (pcont->Open) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
BOOL CloseContainer(PCONTENTS pItem)
{
	if(!pItem)
		return FALSE;
	if(PCONTENTS pcont = FindItemBySlot(pItem->ItemSlot)) {
		if (!pcont->Open)
			return FALSE;
		if(GetItemFromContents(pcont)->Type==ITEMTYPE_PACK) {
			pContainerMgr->CloseContainer((EQ_Container*)&pcont,1);
			if (!pcont->Open) {
				return TRUE;
			}
		}
	}
	return FALSE;
}
//WaitForBagToOpen code by eqmule 2014
DWORD __stdcall WaitForBagToOpen(PVOID pData)
{
	PLARGE_INTEGER i64tmp = (PLARGE_INTEGER)pData;
	DWORD type = i64tmp->LowPart;
	PCONTENTS pItem = (PCONTENTS)i64tmp->HighPart;
	int timeout = 0;
	if(PCONTENTS pcont = FindItemBySlot(pItem->ItemSlot)) {
		//((EQ_Item*)pcont)-
		if(CInvSlot * theslot = pInvSlotMgr->FindInvSlot(pItem->ItemSlot)) {
			if(((PEQINVSLOT)theslot)->pInvSlotWnd) {
				while(!((PEQINVSLOT)theslot)->pInvSlotWnd->Wnd.dShow) {
					Sleep(10);
					timeout+=100;
					if(timeout>=1000) {
						break;
					}
				}
			}
		}
		//this is most likely completely useless
		//since the bag will actually always be open at this point
		//how can i check if the item is in the slot?
		//need to look into this further
		//get the texture maybe? -eqmule
		/*while(!pcont->Open) {
			Sleep(10);
			timeout+=100;
			if(timeout>=1000) {
				break;
			}
		}*/
	}
	Sleep(100);
	bool Old=((PCXWNDMGR)pWndMgr)->KeyboardFlags[1];
    ((PCXWNDMGR)pWndMgr)->KeyboardFlags[1]=1;
	PickupOrDropItem(type,pItem);
    ((PCXWNDMGR)pWndMgr)->KeyboardFlags[1]=Old;
	LocalFree(pData);
	//CloseContainer(pItem);
	return 1;
}
BOOL PickupOrDropItem(DWORD type, PCONTENTS pItem)
{
	//check if merchantwindow is open
	//if it is do some magic and open the bag so we can get the pslot
	if(!pItem)
		return FALSE;
	PEQINVSLOTMGR pInvMgr  = (PEQINVSLOTMGR)pInvSlotMgr;
	WORD InvSlot = pItem->ItemSlot,BagSlot = 0xFFFF;
	BOOL itsinsideapack = 0;
	if(IsItemInsideContainer(pItem)) {
		itsinsideapack = 1;
		if (pInvSlotMgr && pMerchantWnd && pMerchantWnd->dShow) {
			OpenContainer(pItem,true);
			if(CInvSlot * theslot = pInvSlotMgr->FindInvSlot(pItem->ItemSlot,pItem->ItemSlot2)) {
				PEQINVSLOT cSlot = (PEQINVSLOT)theslot;
				//ok so here is how this works:
				//we select the slot, and thet will set pSelectedItem correctly
				//we do this cause later on we need that address for the .Selection member
				//
				pInvSlotMgr->SelectSlot(theslot);
				//int imagenum = ((EQ_Item*)pItem)->GetImageNum();
				//CTextureAnimation *TextureAnim = pIconCache->GetIcon(imagenum);
				CMoveItemData To = {0};
				To.InventoryType = 0;To.Unknown0x02 = 0;
				To.InvSlot = cSlot->pInvSlotWnd->InvSlot;
				To.BagSlot = cSlot->pInvSlotWnd->BagSlot;
				To.GlobalSlot = cSlot->pInvSlotWnd->GlobalSlot;
				To.RandomNum = cSlot->pInvSlotWnd->RandomNum;
				To.Selection = (long)((PEQINVSLOTMGR)pInvSlotMgr)->pSelectedItem;
				pMerchantWnd->ActualSelect(&To);
				return TRUE;
			}
			WriteChatf("[PickupOrDropItem]no invslot found");
			return FALSE;
		}
		BagSlot = pItem->ItemSlot2;
	} else {
		if (pInvSlotMgr && pMerchantWnd && pMerchantWnd->dShow) {
			if(CInvSlot * theslot = pInvSlotMgr->FindInvSlot(pItem->ItemSlot)) {
				PEQINVSLOT cSlot = (PEQINVSLOT)theslot;
				pInvSlotMgr->SelectSlot(theslot);
				CMoveItemData To = {0};
				To.InventoryType = 0;To.Unknown0x02 = 0;
				To.InvSlot = cSlot->pInvSlotWnd->InvSlot;
				To.BagSlot = cSlot->pInvSlotWnd->BagSlot;
				To.GlobalSlot = cSlot->pInvSlotWnd->GlobalSlot;
				To.RandomNum = cSlot->pInvSlotWnd->RandomNum;
				To.Selection = (long)((PEQINVSLOTMGR)pInvSlotMgr)->pSelectedItem;
				pMerchantWnd->ActualSelect(&To);
				return TRUE;
			}
			WriteChatf("Invslot %d not found",InvSlot);
			return FALSE;
		}
	}
	BOOL bMoveFromCursor = 0;
	PCHARINFO2 pChar2 = GetCharInfo2();
	if(pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor) {
		bMoveFromCursor=1;
	}
	PEQINVSLOT pSlot = GetInvSlot(type,InvSlot);
	if(!pSlot || !pSlot->pInvSlotWnd) {
		//if we got all the way here this really shouldnt happen... but why assume...
		WriteChatf("Could not find an item in slot %d",InvSlot);
		return FALSE;
	}
	if(!bMoveFromCursor) {//user is picking up something
		CMoveItemData From = {0};
		From.InventoryType = (WORD)type;
		From.Unknown0x02 = 0;
		From.InvSlot = InvSlot;
		From.BagSlot = BagSlot;
		From.GlobalSlot = pSlot->pInvSlotWnd->GlobalSlot;
		From.RandomNum = pSlot->pInvSlotWnd->RandomNum;
	
		CMoveItemData To = {0};
		To.InventoryType = 0;
		To.Unknown0x02 = 0;
		To.InvSlot = 33;//cursor
		To.BagSlot = 0xFFFF;
		To.GlobalSlot = 0xFFFF;
		if(((EQ_Item *)pItem)->IsStackable()) {
			To.RandomNum = From.RandomNum-0xc;//I *THINK* this is correct, want to get dkaa to look at assembly and confirm... -eqmule
		} else {
			To.RandomNum = 0;
		}
		//OpenContainer(pItem,0);
		//if(CInvSlot *newslot = pInvSlotMgr->FindInvSlot(InvSlot,BagSlot)) {
		//	pQuantityWnd->Open((CXWnd *)((PEQINVSLOT)newslot)->pInvSlotWnd,3,pItem->StackCount,
		//		0x34F,0x14A,1,0,0);
		//}
		DWORD keybflag = pWndMgr->GetKeyboardFlags();
/*							   shr     eax, 1
.text:0069E9D1                 and     al, 1
.text:0069E9D3                 mov     [esp+1Ch], al
.text:0069E9D7                 mov     ecx, [esp+1Ch]
.text:0069E9DB                 push    ecx
*/
		if(keybflag==2 && To.RandomNum && pItem->StackCount>1) {//ctrl was pressed and it is a stackable item
			//until i figure out how to call moveitemqty
			//I need to open the bag and notify it cause moveitem only picks up full stacks
			BOOL wechangedpackopenstatus = 0;
			if(itsinsideapack) {
				wechangedpackopenstatus = OpenContainer(pItem,true);
				if(wechangedpackopenstatus) {
					PLARGE_INTEGER i64tmp = (PLARGE_INTEGER)LocalAlloc(LPTR,sizeof(LARGE_INTEGER));
					i64tmp->LowPart = type;
					i64tmp->HighPart = (LONG)pItem;
					DWORD nThreadId = 0;
					CreateThread(NULL,0,WaitForBagToOpen,i64tmp,0,&nThreadId);
					return FALSE;
				}
				pSlot = (PEQINVSLOT)pInvSlotMgr->FindInvSlot(pItem->ItemSlot,pItem->ItemSlot2);
			}
			if (!pSlot || !pSlot->pInvSlotWnd || !SendWndClick2((CXWnd*)pSlot->pInvSlotWnd,"leftmouseup"))
			{
				WriteChatf("[PickupOrDropItem] falied");
				return FALSE;
			}
			//thread this? hmm if i close it before item ends up on cursor, it wont...
			//if(wechangedpackopenstatus)
			//	CloseContainer(pItem);
		} else {
			pInvSlotMgr->MoveItem(&From,&To,1,1,0,0);
			//pPCData->AlertInventoryChanged();
		}
		return TRUE;
	} else {
		//user has something on the cursor, lets drop it
		CMoveItemData From = {0};
		From.InventoryType = 0;
		From.Unknown0x02 = 0;
		From.InvSlot = 33;//cursor
		From.BagSlot = 0xFFFF;
		From.GlobalSlot = 0xFFFF;
		From.RandomNum = 0;

		CMoveItemData To = {0};
		To.InventoryType = (WORD)type;
		To.Unknown0x02 = 0;
		To.InvSlot = InvSlot;
		To.BagSlot = BagSlot;
		To.GlobalSlot = pSlot->pInvSlotWnd->GlobalSlot;
		To.RandomNum = pSlot->pInvSlotWnd->RandomNum;
		pInvSlotMgr->MoveItem(&From,&To,1,1,0,0);
		return TRUE;
		//need to update cursor here
		//pPCData->AlertInventoryChanged();
		/*if(pCharSpawn) {
			DoCommand((PSPAWNINFO)pCharSpawn,"/autoinventory");
			return TRUE;
		}*/
	}
	return FALSE;
}
int GetTargetBuffBySubCat(PCHAR subcat)
{
	if(!(((PCTARGETWND)pTargetWnd)->Type > 0))
		return false;
	int buffID = 0;
	for(int i = 0; i < NUM_BUFF_SLOTS; i++)
	{
		buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
		if(buffID > 0) {
			PSPELL pSpell = GetSpellByID(buffID);
			if(DWORD cat = pSpell->Subcategory) {
				if (char *ptr = pCDBStr->GetString(cat, 5, NULL)) {
					if(!_stricmp(ptr,subcat))  
					{
						return i;//Dest.DWord = ((((PCTARGETWND)pTargetWnd)->BuffTimer[i] / 1000) + 6)/6;
					}
				}
			}
		}
	}
	return -1;
}
//Usage: The spa is the spellaffect id, for example 11 for Melee Speed
//       the bIncrease tells the function if we want spells that increase or decrease the SPA
int GetTargetBuffBySPA(int spa,bool bIncrease)
{
	if(!(((PCTARGETWND)pTargetWnd)->Type > 0))
		return false;
	int buffID = 0;
	for(int i = 0; i < NUM_BUFF_SLOTS; i++)
	{
		buffID = ((PCTARGETWND)pTargetWnd)->BuffSpellID[i];
		if(buffID > 0) {
			PSPELL pSpell = GetSpellByID(buffID);
			if (LONG base = ((EQ_Spell *)pSpell)->GetSpellBaseByAttrib(spa)) {
				//int test = ((CharacterZoneClient*)pCharData1)->CalcAffectChange((EQ_Spell*)pSpell,0,0,NULL,1,1);
				if(spa==11)//Melee Speed
				{
					if(!bIncrease && base<100) {//below 100 means its a slow above its haste...
						return i;
					} else if(bIncrease && base>100) {
						return i;
					}
					return -1;
				}
				if(spa==3)//Movement Rate
				{
					if(!bIncrease && base<0) {//below 0 means its a snare above its runspeed increase...
						return i;
					} else if(bIncrease && base>0) {
						return i;
					}
					return -1;
				}
				return i;
			}
		}
	}
	return -1;
}
//                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace EQData 
{

EQLIB_API struct _ITEMINFO *GetItemFromContents(struct _CONTENTS *c)
{
    return c->Item1 ? c->Item1 : c->Item2;
}
};

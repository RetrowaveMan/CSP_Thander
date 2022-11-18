// For BaseInterface.c
//Interfaces[INTERFACE_LEAVE_BATTLE].SectionName = "interface\NewGameType.c";
//Interfaces[INTERFACE_LEAVE_BATTLE].IniFile = "INTERFACES\NewGameType.ini";

string totalInfo = "";

void InitInterface(string iniName)
{
	StartAboveForm(true);

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

    SetFormatedText("MAP_CAPTION", XI_ConvertString("titleNewGame"));

	SetFormatedText("INFO_TEXT_QUESTION", XI_ConvertString("NewGameWhatType"));

	//CalculateInfoData();

	//SetFormatedText("INFO_TEXT",totalInfo);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);

	EI_CreateFrame("INFO_BORDERS", 250,152,550,342);
}

void ProcessBreakExit()
{
    if( CheckAttribute(&InterfaceStates,"showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true)
    {
        //isOkExit = true;
        IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
        return;
    }
	IDoExit( RC_INTERFACE_ANY_EXIT );
	ReturnToMainMenu();
}

void ProcessCancelExit()
{
    if( CheckAttribute(&InterfaceStates,"showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true)
    {
        //isOkExit = true;
        IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
        return;
    }
	IDoExit( RC_INTERFACE_ANY_EXIT );
	ReturnToMainMenu();
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

    EndAboveForm(true);

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
	case "B_OK":
		if(comName=="activate" || comName=="click")
		{
			//KillCompanions();
   			IDoExit(RC_INTERFACE_ANY_EXIT);
   			LaunchSelectCharacter();
		}
		if(comName=="downstep")
		{
			if(GetSelectable("B_CANCEL"))	{SetCurrentNode("B_CANCEL");}
		}
	break;

	case "B_CANCEL":
		if(comName=="activate" || comName=="click")
		{
			IDoExit(RC_INTERFACE_ANY_EXIT);
			LaunchHistoricBattle();
		}
		if(comName=="upstep")
		{
			if(GetSelectable("B_OK"))	{SetCurrentNode("B_OK");}
		}
	break;
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData()
{
	aref    rootItems;
	string  sEnd;
	int     cn, i;
	ref     chr;

	makearef(rootItems, pchar.CheckEnemyCompanionDistance);

	if (GetAttributesNum(rootItems) == 0)
	{
		trace("ERROR: NewGameType cannot find ships in contact")
		return
	}
	if (GetAttributesNum(rootItems) == 1)
	{
		totalInfo = xiStr("MSG_LeaveBattle_3");
		sEnd = xiStr("MSG_LeaveBattle_4");
	}
	if (GetAttributesNum(rootItems) > 1)
	{
		totalInfo = xiStr("MSG_LeaveBattle_1");
		sEnd = xiStr("MSG_LeaveBattle_2");
	}
	cn = sti(GetAttributeValue(GetAttributeN(rootItems, 0)));
	if (cn != -1)
	{
		chr = GetCharacter(cn);
		totalInfo += XI_ConvertString(RealShips[sti(chr.Ship.Type)].BaseName) + " '" + chr.Ship.Name + "'";
	}
	for (i = 1; i < GetAttributesNum(rootItems); i++)
	{
		cn = sti(GetAttributeValue(GetAttributeN(rootItems, i)));
		if (cn != -1)
		{
			chr = GetCharacter(cn);
			totalInfo += ", " + XI_ConvertString(RealShips[sti(chr.Ship.Type)].BaseName) + " '" + chr.Ship.Name + "'";
		}
	}
	totalInfo += sEnd;
}

void KillCompanions()
{
	aref    rootItems;
	int     cn, i;
	ref     chr;

	makearef(rootItems, pchar.CheckEnemyCompanionDistance);
	for (i = 0; i < GetAttributesNum(rootItems); i++)
	{
		cn = sti(GetAttributeValue(GetAttributeN(rootItems, i)));
		if (cn != -1)
		{
			chr = GetCharacter(cn);
			RemoveCharacterCompanion(PChar, chr);
			if (!CheckAttribute(chr, "PGGAi"))
			{
		        chr.LifeDay = 0;
			}
			else
			{
				chr.PGGAi.IsPGG = true;
				chr.RebirthPhantom = true;
				chr.PGGAi.location.town = PGG_FindRandomTownByNation(sti(chr.nation));
				PGG_ChangeRelation2MainCharacter(chr, -40);
			}
	        chr.location = "";
	        chr.location.group = "";
	        chr.location.locator = "";
		}
	}
	PChar.GenQuest.CallFunctionParam = pchar.CheckEnemyCompanionType;
	DoQuestCheckDelay("CallFunctionParam", 1.6);
}

// Ugeen && Warship --> интерфейс картографического атласа

ref xi_refCharacter;
string CurRow;
int iSelected = 1; // курсор в таблице
int iStartX = 20;
int iStartY = 62;
int iLengthX = 580;//606;
int iLengthY = 520;//542
object oMapTeleport;

void InitInterface(string iniName)
{
	GameInterface.title = "titleMapView";

	xi_refCharacter = pchar;

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("SelectRColony","SelectRColony",0);
	SetEventHandler("OpenMapBest", "OpenMapBest",0);

	FillMapsTable();
	InitMapTeleport();
}

void ProcessBreakExit()
{
	IDoExit(-1);
}

void ProcessCancelExit()
{
	IDoExit(-1);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("SelectRColony","SelectRColony");
	DelEventHandler("OpenMapBest", "OpenMapBest");

	// По всему файлу мне лень править, а здесь оно тоже будет работать прекрасно
	if(CheckAttribute(PChar, "ShowMapsAtlas")) // Смотрим из меню
	{
		DeleteAttribute(PChar, "ShowMapsAtlas");
		interfaceResultCommand = RC_INTERFACE_TO_ITEMS;
	}
	else
	{
		interfaceResultCommand = exitCode;
	}

	EndCancelInterface(true);
}

void OpenMapBest()
{
	IDoExit(RC_INTERFACE_BEST_MAP);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
		case "TABLE_MAPS":
			if(comName == "click")
			{
				nodName = "TABLE_MAPS";
			}
		break;
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void FillMapsTable()
{
	int n, i;
	string row;
	string sGood, selectedId = "";
	int  idLngFile;
	bool ok = true;
	aref rootItems, arItem;
	aref  curItem;

	n = 1;
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	if(CheckAttribute(pchar, "showlastmap")) { selectedId = pchar.showlastmap; }

	GameInterface.TABLE_MAPS.hr.td1.str = "";
	SetFormatedText("STR_1", XI_ConvertString("Select map"));
	Table_UpdateWindow("TABLE_MAPS");

	// Заполним картами
	makearef(rootItems, xi_refCharacter.Items);
    for (i=0; i<GetAttributesNum(rootItems); i++)
    {
		curItem = GetAttributeN(rootItems, i);
		if (Items_FindItem(GetAttributeName(curItem), &arItem)>=0 )
		{
			row = "tr" + n;
			sGood = arItem.id;
			if(CheckAttribute(arItem,"mapSpecial"))
			{
				ok = false;
			}
			else
			{
				ok = true;
			}
			if (arItem.ItemType != "MAP" || !ok) continue;

			if (selectedId == sGood) { iSelected = n; }
			if (GetCharacterItem(xi_refCharacter, sGood) > 0)
			{
				GameInterface.TABLE_MAPS.(row).index = GetItemIndex(arItem.id);
				GameInterface.TABLE_MAPS.(row).td1.icon.group = arItem.picTexture;
				GameInterface.TABLE_MAPS.(row).td1.icon.image = "itm" + arItem.picIndex;
				GameInterface.TABLE_MAPS.(row).td1.icon.offset = "-2, -1";
				GameInterface.TABLE_MAPS.(row).td1.icon.width = 20;
				GameInterface.TABLE_MAPS.(row).td1.icon.height = 20;
				GameInterface.TABLE_MAPS.(row).td1.textoffset = "20,0";
				GameInterface.TABLE_MAPS.(row).td1.str = LanguageConvertString(idLngFile, arItem.name);
				GameInterface.TABLE_MAPS.(row).td1.scale = 0.9;
				n++;
			}
		}
    }
	GameInterface.TABLE_MAPS.select = iSelected;
	CurRow   =  "tr" + (iSelected);
	SetNewMapPicture();

	Table_UpdateWindow("TABLE_MAPS");
	LanguageCloseFile(idLngFile);
}

void TableSelectChange()
{
	string sControl = GetEventData();
	iSelected = GetEventData();
    CurRow   =  "tr" + (iSelected);
    SetNewMapPicture();
}

void SetNewMapPicture()
{
	int  iGoodIndex = sti(GameInterface.TABLE_MAPS.(CurRow).index);
	ref itmRef = &Items[iGoodIndex];

	if (CheckAttribute(itmRef, "groupID"))
	{
		string itmGroup = itmRef.groupID;
		if (itmGroup == MAPS_ITEM_TYPE)
		{
			if (itmRef.id != "Map_Best")
			{
				SetNewPicture("MAP", "interfaces\Maps\" + itmRef.imageTga + ".tga");
				SetNodeUsing("OPEN_MAP_BEST", false);
			}
			else
			{
				SetNewPicture("MAP", "");
				SetNodeUsing("OPEN_MAP_BEST", true);
			}
			pchar.showlastmap = itmRef.id;
		}
	}
}

void ShowInfoWindow()
{
	int  iGoodIndex = sti(GameInterface.TABLE_MAPS.(CurRow).index);
	ref  itmRef = &Items[iGoodIndex];
	int  idLngFile;

	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup = itmRef.picTexture;
	string sGroupPicture = "itm" + itmRef.picIndex

	sPicture = "-1";

	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	sHeader = LanguageConvertString(idLngFile, itmRef.name);
	sText1  = GetItemDescribe(itmRef.id);
	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
	LanguageCloseFile(idLngFile);
}

void HideInfoWindow()
{
	CloseTooltip();
}

void SelectRColony()
{
	float X, Y;

	float fMouseX = stf(GameInterface.mousepos.x);
	float fMouseY = stf(GameInterface.mousepos.y);

	int  iGoodIndex = sti(GameInterface.TABLE_MAPS.(CurRow).index);
	ref  itmRef = &Items[iGoodIndex];
	
	if (!chrDisableReloadToLocation) DoTeleport(fMouseX,fMouseY,itmRef.id);
}

void DoTeleport(float x, float y, string mapid)
{
	log_info(FloatToString(x,1)+" "+FloatToString(y,1)+" "+mapid+" "+loadedLocation.islandId);
	
	aref mapname,shorename,shorenum;
	string shore_name,shore_numname;
	int i,j;

	makearef(mapname,oMapTeleport.(mapid));
	if (CheckAttribute(loadedLocation,"islandId") && mapname.island == loadedLocation.islandId)
	{
		for(i=0; i<GetAttributesNum(mapname); i++)
		{
			shorename = GetAttributeN(mapname,i);
			shore_name = GetAttributeName(shorename);
			for (j=0; j< GetAttributesNum(shorename); j++)
			{
				shorenum = GetAttributeN(shorename,j);
				shore_numname = GetAttributeName(shorenum);
				if (CC(x,y,sti(shorename.(shore_numname).X),sti(shorename.(shore_numname).Y)))
				{
					if (!HasSubStr(shore_name,"Entr"))
					{
						setCharacterShipLocation(pchar, shore_name);
						setWDMPointXZ(shore_name);
					}
					DoQuestReloadToLocation(shore_name, "reload", "reload1", "");
					return;
				}
			}
		}
	}
}

bool CC(float x, float y, int dx, int dy)
{
	if (x >= dx && x <= dx+30 && y >= dy && y <= dy+30) return true;
	return false;
}

void InitMapTeleport()
{
	oMapTeleport.map_bermudas.island = "Bermudes";
	oMapTeleport.map_bermudas.Shore3.Pos0.X = 380;
	oMapTeleport.map_bermudas.Shore3.Pos0.Y = 185;
	oMapTeleport.map_bermudas.Shore_Ship1.Pos0.X = 485;
	oMapTeleport.map_bermudas.Shore_Ship1.Pos0.Y = 305;
	oMapTeleport.map_bermudas.Bermudes_CaveEntrance.Pos0.X = 415;
	oMapTeleport.map_bermudas.Bermudes_CaveEntrance.Pos0.Y = 225;
	oMapTeleport.map_bermudas.Bermudes_CaveEntrance.Pos1.X = 465;
	oMapTeleport.map_bermudas.Bermudes_CaveEntrance.Pos1.Y = 275;
	oMapTeleport.map_bermudas.Pirates_town.Pos0.X = 380;
	oMapTeleport.map_bermudas.Pirates_town.Pos0.Y = 285;
	
	oMapTeleport.map_tortuga.island = "Tortuga";
	oMapTeleport.map_tortuga.Mayak6.Pos0.X = 520;
	oMapTeleport.map_tortuga.Mayak6.Pos0.Y = 390;
	oMapTeleport.map_tortuga.Shore58.Pos0.X = 530;
	oMapTeleport.map_tortuga.Shore58.Pos0.Y = 290;
	oMapTeleport.map_tortuga.Tortuga_CaveEntrance.Pos0.X = 425;
	oMapTeleport.map_tortuga.Tortuga_CaveEntrance.Pos0.Y = 230;
	oMapTeleport.map_tortuga.Tortuga_ExitTown.Pos0.X = 480;
	oMapTeleport.map_tortuga.Tortuga_ExitTown.Pos0.Y = 405;
	
	oMapTeleport.map_terks.island = "Terks";
	oMapTeleport.map_terks.Shore56.Pos0.X = 360;
	oMapTeleport.map_terks.Shore56.Pos0.Y = 300;
	oMapTeleport.map_terks.Shore57.Pos0.X = 315;
	oMapTeleport.map_terks.Shore57.Pos0.Y = 415;
	oMapTeleport.map_terks.Terks_CaveEntrance.Pos0.X = 305;
	oMapTeleport.map_terks.Terks_CaveEntrance.Pos0.Y = 328;
	
	oMapTeleport.map_sm.island = "SentMartin";
	oMapTeleport.map_sm.Marigo_ExitTown.Pos0.X = 300;
	oMapTeleport.map_sm.Marigo_ExitTown.Pos0.Y = 365;
	oMapTeleport.map_sm.Shore40.Pos0.X = 395;
	oMapTeleport.map_sm.Shore40.Pos0.Y = 135;
	oMapTeleport.map_sm.Shore41.Pos0.X = 285;
	oMapTeleport.map_sm.Shore41.Pos0.Y = 270;
	oMapTeleport.map_sm.Marigo_CaveEntrance.Pos0.X = 420;
	oMapTeleport.map_sm.Marigo_CaveEntrance.Pos0.Y = 375;
	
	oMapTeleport.map_puerto.island = "PuertoRico";
	oMapTeleport.map_puerto.SanJuan_ExitTown.Pos0.X = 475;
	oMapTeleport.map_puerto.SanJuan_ExitTown.Pos0.Y = 275;
	oMapTeleport.map_puerto.Shore45.Pos0.X = 595;
	oMapTeleport.map_puerto.Shore45.Pos0.Y = 375;
	oMapTeleport.map_puerto.Shore44.Pos0.X = 215;
	oMapTeleport.map_puerto.Shore44.Pos0.Y = 220;
	oMapTeleport.map_puerto.SanJuan_CaveEntrance.Pos0.X = 370;
	oMapTeleport.map_puerto.SanJuan_CaveEntrance.Pos0.Y = 350;
}
/// BOAL 01.08.06 форма обмена-грабежа корабля
#define FOOD_BY_ENEMY_CREW 			10
#define RUM_BY_ENEMY_CREW 			20
#define MEDICAMENT_BY_ENEMY_CREW 	30
#define WEAPON_BY_ENEMY_CREW		10
#define HP_BY_ENEMY_CREW			50
#define HP_PER_BOAT					25

int nCurScrollOfficerNum;
int nCurScrollNum = -1;
ref xi_refCharacter, refEnemyCharacter;
ref refCharacter;

string slastvideo1, slastvideo2;
int iTest = 0;
string CurTable, CurRow;
int iSelected; // курсор в таблице

// для выкидывания
int iShipQty, iUnits, iCurGoodsIdx;

int	iCrewQty = 0;
int iBackCrew = 0;
int iGetHired = 0;
string sMessageMode;
bool bTransferMode;
bool bSwap;
bool bShipScrollEnabled = false;

void InitInterface_RRS(string iniName, ref rLeftChar, ref rRightChar, string _type)
{
    bQuestCheckProcessFreeze = true;

	slastvideo1 = "";
	slastvideo2 = "";

	xi_refCharacter   = &Characters[sti(rRightChar.index)];
	refEnemyCharacter = xi_refCharacter; // изначальный кэп
	bool bIsDriftCap = HasSubStr(xi_refCharacter.id, "_DriftCap_");
	if (bIsDriftCap)
	{
		LAi_SetCurHP(xi_refCharacter, 0.0); // умер, чтоб на форме обмена не жил
	}

	refCharacter = &Characters[sti(rLeftChar.index)];

	if(refEnemyCharacter.id == "ShipWreck_BadPirate")
	{
		xi_refCharacter = CharacterFromID("ShipWreck_0");
		refEnemyCharacter = xi_refCharacter;
	}
	if (CheckAttribute(xi_refCharacter, "Ship.Cargo.RecalculateCargoLoad") && sti(xi_refCharacter.Ship.Cargo.RecalculateCargoLoad))
	{
		RecalculateCargoLoad(xi_refCharacter);
		xi_refCharacter.Ship.Cargo.RecalculateCargoLoad = 0;
	}
	if (CheckAttribute(refCharacter, "Ship.Cargo.RecalculateCargoLoad") && sti(refCharacter.Ship.Cargo.RecalculateCargoLoad))
	{
		RecalculateCargoLoad(refCharacter);
		refCharacter.Ship.Cargo.RecalculateCargoLoad = 0;
	}
	if (_type == "Transfer")
    {
		GameInterface.title = "titleTransfer";
		bTransferMode = true;
	}
	else
	{
		DeleteAttribute(refCharacter, "abordage_active"); // от к3, хотя трется до этой формы
		GameInterface.title = "titleRansack";
		bTransferMode = false;
		if (_type == "") // дошли до логического конца абордажа, кэп помер
		{
			if(xi_refCharacter.Id != "PiratesOnUninhabited_BadPirate" && !CheckAttribute(refCharacter,"GenQuest.ShipSituation.Explosion"))
			{
				LAi_SetCurHP(xi_refCharacter, 0.0); // умер, чтоб на форме обмена не жил
			}
		}

		SetShipRemovable(xi_refCharacter, true);
		SetCharacterRemovable(xi_refCharacter, true); // разрешить обмен после абордажа

		DeleteParticles();
		CreateParticleEntity();
	}

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("frame","ProcessFrame",1);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ShowItemInfo", "ShowItemInfo", 0);
	SetEventHandler("GoodsExitCancel", "GoodsExitCancel", 0);
	SetEventHandler("TransactionOK", "TransactionOK", 0);
	SetEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT", 0);
	SetEventHandler("ExitShipChangeMenu", "ExitShipChangeMenu", 0);
	SetEventHandler("ShowOtherClick", "ShowOtherClick", 0);
	SetEventHandler("GoToShipChange", "GoToShipChange", 0);
	SetEventHandler("ExitOfficerMenu","ExitOfficerMenu",0);
	SetEventHandler("acceptaddofficer","AcceptAddOfficer",0);
	SetEventHandler("ExitCrewWindow","ExitCrewWindow",0);
	SetEventHandler("ExitCaptureWindow","ExitCaptureWindow",0);
	SetEventHandler("ExitCaptureCrewWindow","ExitCaptureCrewWindow",0);
	SetEventHandler("ExitDecisionWindow","ExitDecisionWindow",0);
	SetEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON", 0);
	SetEventHandler("ADD_BUTTON","ADD_BUTTON",0);
	SetEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON", 0);
	SetEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON", 0);

	SetEventHandler("HireTransactionOK", "HireTransactionOK", 0);
	SetEventHandler("HireTransactionCancel", "HireTransactionCancel", 0);
	SetEventHandler("HIRE_ADD_ALL_BUTTON", "HIRE_ADD_ALL_BUTTON", 0);
	SetEventHandler("HIRE_ADD_BUTTON","HIRE_ADD_BUTTON",0);
	SetEventHandler("HIRE_REMOVE_BUTTON", "HIRE_REMOVE_BUTTON", 0);
	SetEventHandler("HIRE_REMOVE_ALL_BUTTON", "HIRE_REMOVE_ALL_BUTTON", 0);
    //////////////////
    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",9,53,219,313); // tak from SHIP_BIG_PICTURE
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 14,284,214,1, 4);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 14,78,214,1, 4);

    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER2",582,53,792,313); // tak from SHIP_BIG_PICTURE2
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER2", 586,284,788,1, 4);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER2", 586,78,788,1, 4);

    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 431, 214, 573, 1, 4);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 431, 85, 573, 1, 4);
    EI_CreateVLine("SHIP_BIG_PICTURE_BORDER", 431, 57, 249, 1, 4);

    EI_CreateHLine("CREW_FRAME_BORDER", 228, 435, 573, 1, 4);

    SetNewGroupPicture("CREW_PICTURE", "SHIP_STATE_ICONS", "Crew");
	SetNewGroupPicture("CREW_PICTURE2", "SHIP_STATE_ICONS", "Crew");

    SetNewGroupPicture("EXTRAHULLON1", "SHIP_UPGRADES", "EXTRAHULLON");
    SetNewGroupPicture("EXTRASAILON1", "SHIP_UPGRADES", "EXTRASAILON");
    SetNewGroupPicture("EXTRAGUNSON1", "SHIP_UPGRADES", "EXTRAGUNSON");
    SetNewGroupPicture("EXTRAHULLKRENGON1", "SHIP_UPGRADES", "EXTRAHULLKRENGON");
    SetNewGroupPicture("EXTRACAPACITYON1", "SHIP_UPGRADES", "EXTRACAPACITYON");
    SetNewGroupPicture("EXTRABIGSIDESON1", "SHIP_UPGRADES", "EXTRABIGSIDESON");

    SetNewGroupPicture("EXTRAHULLON2", "SHIP_UPGRADES", "EXTRAHULLON");
    SetNewGroupPicture("EXTRASAILON2", "SHIP_UPGRADES", "EXTRASAILON");
    SetNewGroupPicture("EXTRAGUNSON2", "SHIP_UPGRADES", "EXTRAGUNSON");
    SetNewGroupPicture("EXTRAHULLKRENGON2", "SHIP_UPGRADES", "EXTRAHULLKRENGON");
    SetNewGroupPicture("EXTRACAPACITYON2", "SHIP_UPGRADES", "EXTRACAPACITYON");
    SetNewGroupPicture("EXTRABIGSIDESON2", "SHIP_UPGRADES", "EXTRABIGSIDESON");

    SetNewGroupPicture("Money_PIC", "ICONS_CHAR", "Money");

    SetFormatedText("Money_TEXT", MakeMoneyShow(sti(refCharacter.Money), MONEY_SIGN,MONEY_DELIVER));

    GameInterface.TABLE_LIST.hr.td1.str = "Кол-во";
	GameInterface.TABLE_LIST.hr.td1.scale = 0.8;
	GameInterface.TABLE_LIST.hr.td2.str = "Вес";
	GameInterface.TABLE_LIST.hr.td2.scale = 0.8;
	GameInterface.TABLE_LIST.hr.td3.str = "Товары";
	GameInterface.TABLE_LIST.hr.td3.scale = 0.8;
	GameInterface.TABLE_LIST.hr.td4.str = "Кол-во";
	GameInterface.TABLE_LIST.hr.td4.scale = 0.8;
	GameInterface.TABLE_LIST.hr.td5.str = "Вес";
	GameInterface.TABLE_LIST.hr.td5.scale = 0.8;
	GameInterface.TABLE_LIST.hr.td6.str = "Пачка/Вес";
	GameInterface.TABLE_LIST.hr.td6.scale = 0.8;
	GameInterface.TABLE_LIST.select = 0;
	OnShipScrollChange();
	sMessageMode = "";
	SetGoodsArrows();
	trace("xi_refCharacter.id : " + xi_refCharacter.id);

	if (_type == "companion_captured")
	{
		SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Auto_Captured_Management_Caption"));
		string sMessage =
			XI_ConvertString("Auto_Captured_Management_Question_0") + refCharacter.Ship.Name +
			XI_ConvertString("Auto_Captured_Management_Question_1") + refEnemyCharacter.Ship.Name +
			XI_ConvertString("Auto_Captured_Management_Question_2");
		SetFormatedText("REMOVE_WINDOW_TEXT", sMessage);
		SetSelectable("REMOVE_ACCEPT_OFFICER", true);
		sMessageMode = "AskEnterCompanionCapturedTransfer";
		ShowShipChangeMenu();
	}

	// сообщение о захвате
	if(!bTransferMode && !LAi_IsDead(xi_refCharacter) && _type != "MaryCelesteTransfer" && xi_refCharacter.Id != "ShipWreck_0" &&
		xi_refCharacter.Id != "PiratesOnUninhabited_BadPirate" && !CheckAttribute(refCharacter,"GenQuest.ShipSituation")) //пленный
	{
		SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Surrendered_caption_1"));
		iGetHired = GetEnemyHiredCrew();
		if(iGetHired > 0)
		{
			xi_refCharacter.Ship.Crew.Hire = iGetHired;
			SetBackupQty();
			SetBackupExp();
			SetFormatedText("REMOVE_WINDOW_TEXT", XI_ConvertString("Surrendered_" + _type)+"\n"+XI_ConvertString("Surrendered_wishes"));
		}
		else
		{
			SetFormatedText("REMOVE_WINDOW_TEXT", XI_ConvertString("Surrendered_" + _type));
		}
		sMessageMode = "Surrendered_Ok";
		ShowOkMessage();
		if (CheckAttribute(refCharacter,"ShipsSurr"))
		{
			if (RealShips[sti(xi_refCharacter.Ship.Type)].Type.Merchant == true)
			{
				refCharacter.ShipsSurr = sti(refCharacter.ShipsSurr) + 1;
				if (sti(refCharacter.ShipsSurr) <= 5)
				{
					PlaySound("interface\AchievementComplite.wav");
					string sTemp1 = 5 - sti(refCharacter.ShipsSurr);
					if (sti(refCharacter.ShipsSurr) == 5)
					{
						sTemp1 = " Пора сообщить ему о пяти сдавшихся посудинах.";
					}
					else
					{
						if (sti(sTemp1) == 1)
							sTemp1 = " Осталась одна посудина.";
						else
							sTemp1 = " Осталось " + sti(sTemp1) + " посудины.";
					}
					Log_info("Победа! Корабль '" + xi_refCharacter.Ship.name + "' сдался без боя. Чёрная Борода будет доволен." + sTemp1);
				}
			}
		}
	}
	bSwap = false;
	if(!bTransferMode)
	{
		int stolenShip = sti(xi_refCharacter.Ship.Type);
		RealShips[stolenShip].Stolen = true;
	}

	bool bOk = !bSeaActive && LAi_grp_alarmactive;
	if (bTransferMode && !bDisableMapEnter && !bOk && !chrDisableReloadToLocation && !bIsDriftCap)
	{
		FillShipsScroll();
		bShipScrollEnabled = true;
		SetCurrentNode("SHIPS_SCROLL");
	}
	else
	{
		SetNodeUsing("SHIPS_SCROLL",false);
		SetNodeUsing("SHIPS_LEFTSCROLLBUTTON",false);
		SetNodeUsing("SHIPS_RIGHTSCROLLBUTTON",false);
		SetNodeUsing("SHIPS_SCROLL_FRAME",false);
	}
}

void FillShipsScroll()
{
	nCurScrollNum = -1;
	FillScrollImageWithCompanions("SHIPS_SCROLL", COMPANION_MAX);

	if(!CheckAttribute(&GameInterface,"SHIPS_SCROLL.current"))
	{
		GameInterface.SHIPS_SCROLL.current = 0;
	}
}

void SetDescription()
{
 	string sChrId;

	if(GetCurrentNode() == "SHIPS_SCROLL")
	{
		string attributeName = attributeName = "pic" + (nCurScrollNum+1);
		if(CheckAttribute(&GameInterface, "SHIPS_SCROLL." + attributeName))
		{
			int iCharacter = GameInterface.SHIPS_SCROLL.(attributeName).companionIndex;
			sChrId = characters[iCharacter].id;
			xi_refCharacter = characterFromID(sChrId);
			int QtyMax = GetCargoFreeSpace(xi_refCharacter);
		}
	}
	OnShipScrollChange();
	SetVariable();
}

void ProcessExitCancel()
{
	ref realShip;

	// Warship 09.07.09 Мэри Селест
	if(xi_refCharacter.id == "MaryCelesteCapitan")
	{
		realShip = GetRealShip(GetCharacterShipType(xi_refCharacter));
		realShip.TurnRate = 1; // Понижаем маневренность, чтобы не крутилась сильно
		IDoExit(-1);
		return;
	}

	// Warship 20.08.09 Генер "Пираты на необитайке"
	if(xi_refCharacter.Id == "PiratesOnUninhabited_BadPirate")
	{
		sMessageMode = "ShipGoFreeAsk";
		GoToShipChange(); // Выход там
		return;
	}

	if(xi_refCharacter.Id == "ShipWreck_0")
	{
		sMessageMode = "ShipGoFreeAsk";
		GoToShipChange(); // Выход там
		return;
	}

	// Ugeen - ситуация "Взрыв на вражеском корабле"
	if(CheckAttribute(refCharacter,"GenQuest.ShipSituation.Explosion") || CheckAttribute(refCharacter,"GenQuest.ShipSituation.Epidemy"))
	{
		sMessageMode = "ShipGoFreeAsk";
		GoToShipChange(); // Выход там
		return;
	}
	if (sMessageMode == "CREW_WINDOW")
	{
		ExitCrewWindow();
		return;
	}
	if (sMessageMode == "HIRE_CREW_WINDOW")
	{
		ExitHireCrewWindow();
		return;
	}
	if (!isCompanion(xi_refCharacter))
	{
		// не наш корабль, спрашиваем топить или оставить в дрейфе (дрейф доступен только на локалке)
		if (LAi_IsDead(xi_refCharacter))
		{
			if (bSeaActive)
			{
				realShip = GetRealShip(GetCharacterShipType(xi_refCharacter));
				SetNewPicture("DECISION_PICTURE", "interfaces\ships\" + realShip.BaseName + ".tga.tx");
				ShowDecisionWindow();
			}
			else
			{
				SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Capture Ship"));
				SetFormatedText("REMOVE_WINDOW_TEXT", XI_ConvertString("Surrendered_exit_1")); // Корабль остался без капитана. Потопить его?
				SetSelectable("REMOVE_ACCEPT_OFFICER", true);
				sMessageMode = "ShipDeadAsk";
				ShowShipChangeMenu();
			}
		}
		else
		{//пленный кэп, живой ещё
			SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Surrendered_caption_2"));
			SetFormatedText("REMOVE_WINDOW_TEXT", XI_ConvertString("Surrendered_exit_2")); // Закончить грабеж и отпустить сдавшегося капитана и его экипаж?
			SetSelectable("REMOVE_ACCEPT_OFFICER", true);
			sMessageMode = "ShipGoFreeAsk";
			ShowShipChangeMenu();
		}
	}
	else
	{  // наш компаньон, корабль наш, все ок
		if (bShipScrollEnabled)
		{
			IDoExit(RC_INTERFACE_ANY_EXIT);
			return;
		}
	    if (bSwap) SeaAI_SwapShipAfterAbordage(refCharacter, refEnemyCharacter);
	    if (xi_refCharacter.id != refEnemyCharacter.id) // новый назначенец
	    {
			SeaAI_SetOfficer2ShipAfterAbordage(xi_refCharacter, refEnemyCharacter); // to_do делать один раз на закрытии могут быть баги со множественой сменой в море
			//DeleteAttribute(xi_refCharacter, "ship"); // трем корабль
			//refEnemyCharacter.ship.type = SHIP_NOTUSED;  // если это был фантом, то он сам подохнет позже

			// сам стотрется, тк НР == 0 будет, а это нельзя, иначе наш офф потрется refEnemyCharacter.LifeDay = 0; // стереть при выходе, если был кэп по захвату корабля
		    refEnemyCharacter.location = "none";

			if (bSeaActive)
			{
				if (!bTransferMode)
				{
					ShipTaken(sti(refEnemyCharacter.index), KILL_BY_ABORDAGE, sti(refCharacter.index));
				}

				PostEvent("evntQuestsCheck", 400);
				UpdateRelations();
				RefreshBattleInterface();
			}
			//DeleteAttribute(refEnemyCharacter,"ship");
			//refEnemyCharacter.ship.type = SHIP_NOTUSED;
			ClearShipTypeForPassenger();
		}
		if (bTransferMode)
		{
			IDoExit(RC_INTERFACE_ANY_EXIT);
		}
		else
		{
			IDoExit(RC_INTERFACE_RANSACK_MAIN_EXIT);
		}
	}
}

void IDoExit(int exitCode)
{

	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("frame","ProcessFrame");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ShowItemInfo", "ShowItemInfo");
	DelEventHandler("GoodsExitCancel", "GoodsExitCancel");
	DelEventHandler("TransactionOK", "TransactionOK");
	DelEventHandler("confirmChangeQTY_EDIT", "confirmChangeQTY_EDIT");
	DelEventHandler("ExitShipChangeMenu", "ExitShipChangeMenu");
	DelEventHandler("ShowOtherClick", "ShowOtherClick");
	DelEventHandler("GoToShipChange", "GoToShipChange");
	DelEventHandler("ExitOfficerMenu","ExitOfficerMenu");
	DelEventHandler("acceptaddofficer","AcceptAddOfficer");
	DelEventHandler("ExitCrewWindow","ExitCrewWindow");
	DelEventHandler("ExitCaptureWindow","ExitCaptureWindow");
	DelEventHandler("ExitCaptureCrewWindow","ExitCaptureCrewWindow");
	DelEventHandler("ExitDecisionWindow","ExitDecisionWindow");

	DelEventHandler("ADD_ALL_BUTTON", "ADD_ALL_BUTTON");
	DelEventHandler("ADD_BUTTON","ADD_BUTTON");
	DelEventHandler("REMOVE_BUTTON", "REMOVE_BUTTON");
	DelEventHandler("REMOVE_ALL_BUTTON", "REMOVE_ALL_BUTTON");

	DelEventHandler("HireTransactionOK", "HireTransactionOK");
	DelEventHandler("HireTransactionCancel", "HireTransactionCancel");
	DelEventHandler("HIRE_ADD_ALL_BUTTON", "HIRE_ADD_ALL_BUTTON");
	DelEventHandler("HIRE_ADD_BUTTON","HIRE_ADD_BUTTON");
	DelEventHandler("HIRE_REMOVE_BUTTON", "HIRE_REMOVE_BUTTON");
	DelEventHandler("HIRE_REMOVE_ALL_BUTTON", "HIRE_REMOVE_ALL_BUTTON");
	UpdateRelations();
    if(bSeaActive)
    {
        RefreshBattleInterface();
    }
    DoQuestCheckDelay("NationUpdate", 1.5);

	PostEvent("StopQuestCheckProcessFreeze", 100);

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
        DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		EndCancelInterface(true);
	}
}
void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
    switch(nodName)
	{
		case "REMOVE_OK":
			if(comName=="click" || comName=="activate")
			{
				if(sMessageMode == "Surrendered_Ok") { ShowCrewCaptureHire(); }
			}
		break;
		case "CAPTURE_CAPTAIN_PRISON":
			if(comName=="click" || comName=="activate")
			{
			    SetEnemyToPrisoner();
			}
		break;

		case "CAPTURE_CAPTAIN_KILL":
			if(comName=="click" || comName=="activate")
			{
			    SetEnemyToKilled();
			}
		break;

		case "CAPTURE_CREW_PRISON":
			if(comName=="click" || comName=="activate")
			{
			    SetEnemyCrewToPrisoner();
			}
		break;

		case "CAPTURE_CREW_KILL":
			if(comName=="click" || comName=="activate")
			{
			    SetEnemyCrewToKilled();
			}
		break;

		case "CAPTURE_CREW_FREE":
			if(comName=="click" || comName=="activate")
			{
			    SetEnemyCrewToFree();
			}
		break;
		case "EXIT_BTN":
			if (comName=="click")
			{
				ExitCrewWindow();
			}
		break;

		case "DECISION_DROWN_SHIP":
			if(comName=="click" || comName=="activate")
			{
			    SetShipToDrown();
			}
		break;

		case "DECISION_DRIFT_SHIP":
			if(comName=="click" || comName=="activate")
			{
			    SetShipToDrift();
			}
		break;

		case "CREW_RIGHT":
			if (comName=="click")
			{
				GiveCrew(1);
			}
			if (comName=="rclick")
			{
				if (GetCrewQuantity(xi_refCharacter) < GetMinCrewQuantity(xi_refCharacter))	GiveCrew(GetMinCrewQuantity(xi_refCharacter)-GetCrewQuantity(xi_refCharacter));
			}
		break;

		case "CREW_LEFT":
	        if (comName=="click")
			{
				TakeCrew(1);
			}
			if (comName=="rclick")
			{
				if (GetCrewQuantity(refCharacter) < GetMinCrewQuantity(refCharacter))
					TakeCrew(GetMinCrewQuantity(refCharacter)-GetCrewQuantity(refCharacter));
			}
		break;

		case "CREW_Win_fix":
			if(comName=="leftstep")
			{
				TakeCrew(1);
			}
			if(comName=="rightstep")
			{
				GiveCrew(1);
			}
			if(comName=="speedleft")
			{
				TakeCrew(9000); // очень много
			}
			if(comName=="speedright")
			{
				GiveCrew(9000); // очень много
			}
		break;

		case "B_RIGHT":
			if (comName=="click")
			{
				GiveGoods(1);
			}
			if (comName=="rclick")
			{
				GiveGoods(10);
			}
		break;

		case "B_LEFT":
	        if (comName=="click")
			{
				TakeGoods(1);
			}
			if (comName=="rclick")
			{
				TakeGoods(10);
			}
		break;

		case "TABLE_LIST":
			if(comName=="leftstep")
			{
				TakeGoods(1);
			}
			if(comName=="rightstep")
			{
				GiveGoods(1);
			}
			if(comName=="speedleft")
			{
				TakeGoods(9000000); // очень много
			}
			if(comName=="speedright")
			{
				GiveGoods(9000000); // очень много
			}
		break;
		////////////////
		case "QTY_OK_BUTTON":
			if(comName=="leftstep")
			{
	            ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
	            REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
	      		ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
	            REMOVE_ALL_BUTTON();
			}
		break;

		case "QTY_CANCEL_BUTTON":
			if(comName=="leftstep")
			{
	            ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
	            REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
	      		ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
	            REMOVE_ALL_BUTTON();
			}
		break;

		case "TABLE_OTHER":
			if(comName=="click")
			{
			    ShowShipFoodInfo(refCharacter);
			}
		break;

		case "TABLE_OTHER2":
			if(comName=="click")
			{
			    ShowShipFoodInfo(xi_refCharacter);
			}
		break;

		case "SWAP_BUTTON":
			if(comName=="click" || comName=="activate")
			{
			    SwapProcess();
			}
		break;

		case "CAPTAN_BUTTON":
			if(comName=="click" || comName=="activate")
			{
				ExitCrewWindow(); // для профигактики
				ShipChangeCaptan();
			}
		break;

		case "TAKE_GOODS":
			if(comName=="click" || comName=="activate")
			{
				TakeAllGoods();
			}
		break;
		case "CANNONS_REMOVE_ALL":
			//if(comName=="click")//не знаю в чём разница, но лучше так пока что. Может, это как раз доппроверка, что обмен недоступен с торговцами или квестовыми ситациями
			if(comName=="click" || comName=="activate")
			{
			    CanonsRemoveAll();
			}
		break;
		case "CREW_BUTTON":
			if(comName=="click" || comName=="activate")
			{
				ShowCrewWindow();
			}
		break;

		case "HIRE_QTY_OK_BUTTON":
			if(comName=="leftstep")
			{
	            HIRE_ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
	            HIRE_REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
	      		HIRE_ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
	            HIRE_REMOVE_ALL_BUTTON();
			}
		break;

		case "HIRE_QTY_CANCEL_BUTTON":
			if(comName=="leftstep")
			{
	            HIRE_ADD_BUTTON();
			}
			if(comName=="rightstep")
			{
	            HIRE_REMOVE_BUTTON();
			}
			if(comName=="speedleft")
			{
	      		HIRE_ADD_ALL_BUTTON();
			}
			if(comName=="speedright")
			{
	            HIRE_REMOVE_ALL_BUTTON();
			}
		break;
	}
}

void ProcessFrame()
{
	string attributeName;
	int iCharacter;

	string sNode = GetCurrentNode();
	if (sNode == "PASSENGERSLIST" && sti(GameInterface.PASSENGERSLIST.current)!= nCurScrollOfficerNum)
	{
		nCurScrollOfficerNum = sti(GameInterface.PASSENGERSLIST.current);
		SetOfficersSkills();
	}
	if (sNode != "TABLE_LIST")
	{
		SetNodeUsing("B_RIGHT",  false);
		SetNodeUsing("B_LEFT", false);
	}
	if(GetCurrentNode() == "SHIPS_SCROLL")
	{
		if(sti(GameInterface.SHIPS_SCROLL.current)!=nCurScrollNum)
		{
			nCurScrollNum = sti(GameInterface.SHIPS_SCROLL.current);
			SetDescription();
		}
	}
}
void OnShipScrollChange()
{
	ShowShipInfo(refCharacter, "");
	SetShipQualityTable(refCharacter, "BAR1_HP", "BAR1_Mast", "BAR1_Speed", "BAR1_TurnRate", "BAR1_WAS", "BAR1_Capacity", "BAR1_Crew");
	ShowShipInfo(xi_refCharacter, "2");
	SetShipQualityTable(xi_refCharacter, "BAR2_HP", "BAR2_Mast", "BAR2_Speed", "BAR2_TurnRate", "BAR2_WAS", "BAR2_Capacity", "BAR2_Crew");
	ShowShipFoodInfo(refCharacter);
	FillGoodsTable();
	//Ship info window
	SetFormatedText("Money_TEXT", MakeMoneyShow(sti(refCharacter.Money), MONEY_SIGN,MONEY_DELIVER));
	if (isCompanion(xi_refCharacter))
	{ // наш товарищ

		SetNodeUsing("MAIN_CHARACTER_PICTURE2", true);
		SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"CAPTAN_BUTTON",0, "#"+XI_ConvertString("Remove Captain"));
		if (GetRemovable(xi_refCharacter))
		{ // обмен с квестовыми
		    if (GetShipRemovable(xi_refCharacter))
		    {
				SetSelectable("CAPTAN_BUTTON", true);
				SetSelectable("SWAP_BUTTON", true);
				SetSelectable("CANNONS_REMOVE_ALL", true);
			}
			else
			{
			    SetSelectable("CAPTAN_BUTTON", false);
				SetSelectable("SWAP_BUTTON", false);
				SetSelectable("CANNONS_REMOVE_ALL", false);
			}
			SetSelectable("CREW_BUTTON", true);
			SetSelectable("TAKE_GOODS", true);
		}
		else
		{
			SetSelectable("CAPTAN_BUTTON", false);
			SetSelectable("SWAP_BUTTON", false);
			SetSelectable("CREW_BUTTON", false);
			SetSelectable("TAKE_GOODS", false);
		}
	}
	else
	{  // не наш, значит убит или сдался
		// Warship 09.07.09 Мэри Селест и (20.08.09) генер "Пираты на необитайке"
		if(xi_refCharacter.id == "BS_Vein" || xi_refCharacter.id == "MaryCelesteCapitan" || xi_refCharacter.Id == "PiratesOnUninhabited_BadPirate" || (refCharacter != PChar) || CheckAttribute(refCharacter,"GenQuest.ShipSituation.Explosion") || xi_refCharacter.Id == "ShipWreck_0")
		{
			SetSelectable("CAPTAN_BUTTON", false);
			SetSelectable("SWAP_BUTTON", false);
			SetSelectable("CREW_BUTTON", false);
		}
		else
		{
		if (LAi_IsDead(xi_refCharacter))
		{
			SetNodeUsing("MAIN_CHARACTER_PICTURE2", false); // убитый кэп
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"CAPTAN_BUTTON",0, "#"+XI_ConvertString("Set Captain"));
			SetSelectable("SWAP_BUTTON", true);
		}
		else
		{
			SetNodeUsing("MAIN_CHARACTER_PICTURE2", true); // живой враг
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"CAPTAN_BUTTON",0, "#"+XI_ConvertString("Remove Captain"));
			SetSelectable("SWAP_BUTTON", false);
		}
		}
	}
	if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.HullSpecial")) SetNewGroupPicture("EXTRAHULLON1", "SHIP_UPGRADES", "EXTRAHULLON");
	else SetNewGroupPicture("EXTRAHULLON1", "SHIP_UPGRADES", "EXTRAHULLOFF");
	if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.SailsSpecial")) SetNewGroupPicture("EXTRASAILON1", "SHIP_UPGRADES", "EXTRASAILON");
	else SetNewGroupPicture("EXTRASAILON1", "SHIP_UPGRADES", "EXTRASAILOFF");
	if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.CannonsSpecial")) SetNewGroupPicture("EXTRAGUNSON1", "SHIP_UPGRADES", "EXTRAGUNSON");
	else SetNewGroupPicture("EXTRAGUNSON1", "SHIP_UPGRADES", "EXTRAGUNSOFF");
	if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.CuBot")) SetNewGroupPicture("EXTRAHULLKRENGON1", "SHIP_UPGRADES", "EXTRAHULLKRENGON");
	else SetNewGroupPicture("EXTRAHULLKRENGON1", "SHIP_UPGRADES", "EXTRAHULLKRENGOFF");
	if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.BotPack")) SetNewGroupPicture("ExtraCapacityOn1", "SHIP_UPGRADES", "ExtraCapacityOn");
	else SetNewGroupPicture("ExtraCapacityOn1", "SHIP_UPGRADES", "ExtraCapacityOff");
	if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.HighBort")) SetNewGroupPicture("ExtraBigSidesOn1", "SHIP_UPGRADES", "ExtraBigSidesOn");
	else SetNewGroupPicture("ExtraBigSidesOn1", "SHIP_UPGRADES", "ExtraBigSidesOff");
	if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.HullSpecial")) SetNewGroupPicture("EXTRAHULLON2", "SHIP_UPGRADES", "EXTRAHULLON");
	else SetNewGroupPicture("EXTRAHULLON2", "SHIP_UPGRADES", "EXTRAHULLOFF");
	if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.SailsSpecial")) SetNewGroupPicture("EXTRASAILON2", "SHIP_UPGRADES", "EXTRASAILON");
	else SetNewGroupPicture("EXTRASAILON2", "SHIP_UPGRADES", "EXTRASAILOFF");
	if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.CannonsSpecial")) SetNewGroupPicture("EXTRAGUNSON2", "SHIP_UPGRADES", "EXTRAGUNSON");
	else SetNewGroupPicture("EXTRAGUNSON2", "SHIP_UPGRADES", "EXTRAGUNSOFF");
	if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.CuBot")) SetNewGroupPicture("EXTRAHULLKRENGON2", "SHIP_UPGRADES", "EXTRAHULLKRENGON");
	else SetNewGroupPicture("EXTRAHULLKRENGON2", "SHIP_UPGRADES", "EXTRAHULLKRENGOFF");
	if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.BotPack")) SetNewGroupPicture("ExtraCapacityOn2", "SHIP_UPGRADES", "ExtraCapacityOn");
	else SetNewGroupPicture("ExtraCapacityOn2", "SHIP_UPGRADES", "ExtraCapacityOff");
	if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.HighBort")) SetNewGroupPicture("ExtraBigSidesOn2", "SHIP_UPGRADES", "ExtraBigSidesOn");
	else SetNewGroupPicture("ExtraBigSidesOn2", "SHIP_UPGRADES", "ExtraBigSidesOff");
}

void ShowShipInfo(ref chr, string sAdd)
{
	SetNewPicture("MAIN_CHARACTER_PICTURE" + sAdd, "interfaces\portraits\128\face_" + chr.FaceId + ".tga");

	int iShip = sti(chr.ship.type);
	if (iShip != SHIP_NOTUSED)
	{
		ref refBaseShip = GetRealShip(iShip);
		string sShip = refBaseShip.BaseName;
		SetNewPicture("SHIP_BIG_PICTURE" + sAdd, "interfaces\ships\" + sShip + ".tga.tx");
		if (!CheckAttribute(refBaseShip,"QuestShip")) SetNodeUsing("SHIP_BIG_PICTURE_VIDEO"+sAdd,false);
		else
		{
			SetNewPicture("SHIP_BIG_PICTURE" + sAdd, "interfaces\ships\" + sShip + ".tga.tx");

			string slastvideo;
			if (sAdd == "") slastvideo = slastvideo1; else slastvideo = slastvideo2;
			if ( ("SHIP_"+sShip) != slastvideo)
			{
				SetNodeUsing("SHIP_BIG_PICTURE_VIDEO"+sAdd,true);
				SetNewVideoPicture("SHIP_BIG_PICTURE_VIDEO"+sAdd,"SHIP_"+sShip);
				if (sAdd == "") slastvideo1 = "SHIP_"+sShip; else slastvideo2 = "SHIP_"+sShip;
			}
		}

		SetFormatedText("SHIP_NAME" + sAdd, chr.ship.name);
		SetFormatedText("SHIP_TYPE" + sAdd, XI_ConvertString(refBaseShip.BaseName));
		SetFormatedText("SHIP_RANK" + sAdd, refBaseShip.Class);
		SetFormatedText("CLASS_ARMOR" + sAdd, refBaseShip.HullArmor);
		SetNewGroupPicture("CLASS_ARMOR_ICON" + sAdd, "ICONS_CHAR", "Reputation");
		SetShipOTHERTable("TABLE_OTHER" + sAdd, chr);
	}
}

void ShowShipFoodInfo(ref chr)
{
	SetFormatedText("FOOD", chr.ship.name);
	// еда -->
	// на одном корабле
	SetFoodShipInfo(chr, "FOOD_SHIP");
	SetRumShipInfo(chr, "RUM_SHIP");
	// еда <--
	SetFormatedText("MONEY_SHIP", "");

	if (GetRemovable(chr) && isCompanion(chr)) // считаем только своих, а то вских сопровождаемых кормить!!!
	{
	    // для каждого корабля учитываем класс и считаем отдельно
	    SetFormatedText("MONEY_SHIP", "Содержание корабля: " + NewStr() + FindRussianMoneyString(GetSalaryForShip(chr)));
	}
}
void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;
	int iItem, iCharacter;

	//Boyer fix #20170401-01 Can't load texture -1.tx log errors
	//sPicture = "-1";
	sPicture = "";
	string sAttributeName, sCharacter;
	int nChooseNum = -1;
	int iShip;
	int xx = 256;
	int yy = 256;
	ref refBaseShip;
	ref Cannon;
	ref chr;
	bool  bShowHint = true;
	switch (sCurrentNode)
	{
		case "SHIP_BIG_PICTURE":
		    iShip = sti(refCharacter.ship.type);
		    refBaseShip = GetRealShip(iShip);
			sHeader = XI_ConvertString(refBaseShip.BaseName);
			sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");
		break;

		case "SHIP_BIG_PICTURE2":
		    iShip = sti(xi_refCharacter.ship.type);
		    refBaseShip = GetRealShip(iShip);
			sHeader = XI_ConvertString(refBaseShip.BaseName);
			sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");
		break;

		case "MAIN_CHARACTER_PICTURE":
			// отдельная форма
			bShowHint = false;
			ShowRPGHint();
		break;

		case "MAIN_CHARACTER_PICTURE2":
			// отдельная форма
			bShowHint = false;
			ShowRPGHint2();
		break;

		case "TABLE_LIST":
			sHeader = XI_Convertstring("Goods");
			sText1  = GetRPGText("GoodsCargo_hint");
			sText2  = GetConvertStr("TakeAllGoods_hint", "ShipsDescribe.txt");
			sText3  = GetConvertStr("GoodsKeys_hint", "ShipsDescribe.txt");
		break;

		case "TABLE_CREW":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
		break;

		case "TABLE_OTHER":
			sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
		    sText1  = GetConvertStr(GameInterface.(CurTable).(CurRow).UserData.ID, "ShipsDescribe.txt");
		    if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType" && sti(refCharacter.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
		    {
		    	Cannon = GetCannonByType(sti(refCharacter.Ship.Cannons.Type));
		    	sText2 = "Тип: " + XI_ConvertString(GetCannonType(sti(refCharacter.Ship.Cannons.Type)));
		    	sText2 = sText2 + NewStr() + "Калибр: " + XI_ConvertString("caliber" + GetCannonCaliber(sti(refCharacter.Ship.Cannons.Type)));
		    	sText2 = sText2 + NewStr() + "Дальность: "  + sti(Cannon.FireRange);
		    	sText2 = sText2 + NewStr() + "Урон: x"  + FloatToString(stf(Cannon.DamageMultiply), 1);
		    	sText2 = sText2 + NewStr() + "Перезарядка: "  + sti(GetCannonReloadTime(Cannon)) + " сек.";
		    	sText2 = sText2 + NewStr() + "Вес: "  + sti(Cannon.Weight) + " ц.";

		    	sGroup = "GOODS";
				sGroupPicture = GetCannonType(sti(refCharacter.Ship.Cannons.Type)) + "_" + GetCannonCaliber(sti(refCharacter.Ship.Cannons.Type));
		    }
		    if (GameInterface.(CurTable).(CurRow).UserData.ID == "Crew" && sti(refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText2 = "Команды может быть больше максимальной, но это вызывает перегруз и условия жизни на корабле становятся ужасными, что влечет ежедневное падение морали. В среднем можно взять на борт до 25% больше матросов.";
				sText2 = sText2 + NewStr() + "Максимальное количество экипажа с учетом перегруза: " + GetMaxCrewQuantity(refCharacter);
			}
			// процент ремонта
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "Hull" && sti(refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText3 = xiStr("Hull") + ": " + FloatToString(GetHullPercent(refCharacter), 1)  + " %";
			}
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "Sails" && sti(refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText3 = xiStr("Sails") + ": " + FloatToString(GetSailPercent(refCharacter), 1) + " %";
			}
			// трюм
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "Capacity" && sti(refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText3 = "Занято: " + FloatToString((stf(GetCargoLoad(refCharacter))  /  stf(GetCargoMaxSpace(refCharacter))) * 100.0, 1)+ " %";
			}
		break;

		case "TABLE_OTHER2":
			sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
		    sText1  = GetConvertStr(GameInterface.(CurTable).(CurRow).UserData.ID, "ShipsDescribe.txt");
		    if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType" && sti(xi_refCharacter.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
		    {
		    	Cannon = GetCannonByType(sti(xi_refCharacter.Ship.Cannons.Type));
		    	sText2 = "Тип: " + XI_ConvertString(GetCannonType(sti(xi_refCharacter.Ship.Cannons.Type)));
		    	sText2 = sText2 + NewStr() + "Калибр: " + XI_ConvertString("caliber" + GetCannonCaliber(sti(xi_refCharacter.Ship.Cannons.Type)));
		    	sText2 = sText2 + NewStr() + "Дальность: "  + sti(Cannon.FireRange);
		    	sText2 = sText2 + NewStr() + "Урон: x"  + FloatToString(stf(Cannon.DamageMultiply), 1);
		    	sText2 = sText2 + NewStr() + "Перезарядка: "  + sti(GetCannonReloadTime(Cannon)) + " сек.";
		    	sText2 = sText2 + NewStr() + "Вес: "  + sti(Cannon.Weight) + " ц.";

		    	sGroup = "GOODS";
				sGroupPicture = GetCannonType(sti(xi_refCharacter.Ship.Cannons.Type)) + "_" + GetCannonCaliber(sti(xi_refCharacter.Ship.Cannons.Type));
		    }
		    if (GameInterface.(CurTable).(CurRow).UserData.ID == "Crew" && sti(xi_refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText2 = "Команды может быть больше максимальной, но это вызывает перегруз и условия жизни на корабле становятся ужасными, что влечет ежедневное падение морали. В среднем можно взять на борт до 25% больше матросов.";
				sText2 = sText2 + NewStr() + "Максимальное количество экипажа с учетом перегруза: " + GetMaxCrewQuantity(xi_refCharacter);
			}
			// процент ремонта
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "Hull" && sti(xi_refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText3 = xiStr("Hull") + ": " + FloatToString(GetHullPercent(xi_refCharacter), 1)  + " %";
			}
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "Sails" && sti(xi_refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText3 = xiStr("Sails") + ": " + FloatToString(GetSailPercent(xi_refCharacter), 1) + " %";
			}
			// трюм
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "Capacity" && sti(xi_refCharacter.ship.type) != SHIP_NOTUSED)
			{
				sText3 = "Занято: " + FloatToString((stf(GetCargoLoad(xi_refCharacter))  /  stf(GetCargoMaxSpace(xi_refCharacter))) * 100.0, 1)+ " %";
			}
		break;
		case "CREW_Win_fix":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
		break;
		case "HIRE_TABLE_CREW":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
		break;
		case "HIRE_TABLE_CREW2":
			sHeader = GetConvertStr("Crew_Exp", "ShipsDescribe.txt");
			sText1  = GetConvertStr("Crew_Exp_hint", "ShipsDescribe.txt");
		break;

		case "PASSENGERSLIST":
			nChooseNum = SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"PASSENGERSLIST", 2 );
			sAttributeName = "pic" + (nChooseNum+1);
			if(CheckAttribute(GameInterface, "PASSENGERSLIST." + sAttributeName))
			{
				if(CheckAttribute(GameInterface, "PASSENGERSLIST." + sAttributeName + ".character"))
				{
					sCharacter = GameInterface.PASSENGERSLIST.(sAttributeName).character;
					sHeader = XI_ConvertString("OfficerJob");
					sText1 = GetOfficerPosition(sCharacter);
				}
			}
		break;

		// LEO - чудим с доп инфой (надо доработать немного)
		case "EXTRAHULLON1":
			sHeader = XI_ConvertString("EXTRAHULLON");
			sText1 = GetConvertStr("SUP_HULL_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.HullSpecial")) sGroupPicture = "EXTRAHULLON";
			else sGroupPicture = "EXTRAHULLOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRASAILON1":
			sHeader = XI_ConvertString("EXTRASAILON");
			sText1 = GetConvertStr("SUP_SAIL_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.SailsSpecial")) sGroupPicture = "EXTRASAILON";
			else sGroupPicture = "EXTRASAILOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRAGUNSON1":
			sHeader = XI_ConvertString("EXTRAGUNSON");
			sText1 = GetConvertStr("SUP_CANNONS_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.CannonsSpecial")) sGroupPicture = "EXTRAGUNSON";
			else sGroupPicture = "EXTRAGUNSOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRAHULLKRENGON1":
			sHeader = XI_ConvertString("EXTRAHULLKRENGON");
			sText1 = GetConvertStr("SUP_HULLCOPPER_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.CuBot")) sGroupPicture = "EXTRAHULLKRENGON";
			else sGroupPicture = "EXTRAHULLKRENGOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRACAPACITYON1":
			sHeader = XI_ConvertString("EXTRACAPACITYON");
			sText1 = GetConvertStr("SUP_CAPACITY_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.BotPack")) sGroupPicture = "EXTRACAPACITYON";
			else sGroupPicture = "EXTRACAPACITYOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRABIGSIDESON1":
			sHeader = XI_ConvertString("EXTRABIGSIDESON");
			sText1 = GetConvertStr("SUP_BIGSIDES_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(refCharacter.Ship.Type)],"Tuning.HighBort")) sGroupPicture = "EXTRABIGSIDESON";
			else sGroupPicture = "EXTRABIGSIDESOFF";
			xx = 256;
			yy = 256;
		break;

		//не гг
		case "EXTRAHULLON2":
			sHeader = XI_ConvertString("EXTRAHULLON");
			sText1 = GetConvertStr("SUP_HULL_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.HullSpecial")) sGroupPicture = "EXTRAHULLON";
			else sGroupPicture = "EXTRAHULLOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRASAILON2":
			sHeader = XI_ConvertString("EXTRASAILON");
			sText1 = GetConvertStr("SUP_SAIL_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.SailsSpecial")) sGroupPicture = "EXTRASAILON";
			else sGroupPicture = "EXTRASAILOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRAGUNSON2":
			sHeader = XI_ConvertString("EXTRAGUNSON");
			sText1 = GetConvertStr("SUP_CANNONS_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.CannonsSpecial")) sGroupPicture = "EXTRAGUNSON";
			else sGroupPicture = "EXTRAGUNSOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRAHULLKRENGON2":
			sHeader = XI_ConvertString("EXTRAHULLKRENGON");
			sText1 = GetConvertStr("SUP_HULLCOPPER_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.CuBot")) sGroupPicture = "EXTRAHULLKRENGON";
			else sGroupPicture = "EXTRAHULLKRENGOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRACAPACITYON2":
			sHeader = XI_ConvertString("EXTRACAPACITYON");
			sText1 = GetConvertStr("SUP_CAPACITY_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.BotPack")) sGroupPicture = "EXTRACAPACITYON";
			else sGroupPicture = "EXTRACAPACITYOFF";
			xx = 256;
			yy = 256;
		break;
		case "EXTRABIGSIDESON2":
			sHeader = XI_ConvertString("EXTRABIGSIDESON");
			sText1 = GetConvertStr("SUP_BIGSIDES_descr", "ShipsDescribe.txt");

			sGroup = "SHIP_UPGRADES";
			if (CheckAttribute(RealShips[sti(xi_refCharacter.Ship.Type)],"Tuning.HighBort")) sGroupPicture = "EXTRABIGSIDESON";
			else sGroupPicture = "EXTRABIGSIDESOFF";
			xx = 256;
			yy = 256;
		break;
	}
	if (bShowHint)
	{
		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, xx, yy);
	}
}

void HideInfoWindow()
{
	CloseTooltip();
	ExitRPGHint();
}

void TableSelectChange()
{
	string sControl = GetEventData();
	iSelected = GetEventData();
    CurTable = sControl;
    CurRow   =  "tr" + (iSelected);
    NullSelectTable("TABLE_LIST");
    NullSelectTable("TABLE_OTHER");
    NullSelectTable("TABLE_OTHER2");
    NullSelectTable("TABLE_CREW");
    SetGoodsArrows();
}

void ExitRPGHint()
{
	if (sMessageMode == "RPG_Hint")
	{
		XI_WindowShow("RPG_WINDOW", false);
		XI_WindowDisable("RPG_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", false);
		sMessageMode = "";
	}
}

void ShowRPGHint()
{
	SetSPECIALMiniTable("TABLE_SMALLSKILL2", refCharacter);
    SetOTHERMiniTable("TABLE_SMALLOTHER2", refCharacter);
    SetFormatedText("RPG_NAME", GetFullName(refCharacter));

	XI_WindowShow("RPG_WINDOW", true);
	XI_WindowDisable("RPG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "RPG_Hint";
}

void ShowRPGHint2()
{
	SetSPECIALMiniTable("TABLE_SMALLSKILL2", xi_refCharacter);
    SetOTHERMiniTable("TABLE_SMALLOTHER2", xi_refCharacter);
    SetFormatedText("RPG_NAME", GetFullName(xi_refCharacter));

	XI_WindowShow("RPG_WINDOW", true);
	XI_WindowDisable("RPG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "RPG_Hint";
}

void FillGoodsTable()
{
	int n, i, qty2, qty1;
	string row;
	ref rShip;
	string sGood;
	aref refGoods;

	Table_Clear("TABLE_LIST", false, true, false);
	n = 1;
	if (!CheckAttribute(&GameInterface, "TABLE_LIST.BackUp"))
	{
		GameInterface.TABLE_LIST.select = 0;
		GameInterface.TABLE_LIST.top    = 0;
		GameInterface.TABLE_LIST.BackUp = true;
	}
    for (i = 0; i< GOODS_QUANTITY; i++)
	{
        row = "tr" + n;
		sGood = Goods[i].name;
		qty1 = GetCargoGoods(refCharacter, i);
		qty2 = GetCargoGoods(xi_refCharacter, i);
		if (qty2 <= 0 && qty1 <= 0) continue; // только не нули

		GameInterface.TABLE_LIST.(row).index = i;

		GameInterface.TABLE_LIST.(row).td1.str = qty1;
		GameInterface.TABLE_LIST.(row).td1.scale = 0.81;
		GameInterface.TABLE_LIST.(row).td2.str = GetGoodWeightByType(i, qty1);
		GameInterface.TABLE_LIST.(row).td2.scale = 0.81;
		GameInterface.TABLE_LIST.(row).td4.str = qty2;
		GameInterface.TABLE_LIST.(row).td4.scale = 0.81;
		GameInterface.TABLE_LIST.(row).td5.str = GetGoodWeightByType(i, qty2);
		GameInterface.TABLE_LIST.(row).td5.scale = 0.81;
		GameInterface.TABLE_LIST.(row).td6.str = Goods[i].Units + " / " + Goods[i].Weight;
		GameInterface.TABLE_LIST.(row).td6.scale = 0.81;

        GameInterface.TABLE_LIST.(row).td3.icon.group = "GOODS";
		GameInterface.TABLE_LIST.(row).td3.icon.image = sGood;
		GameInterface.TABLE_LIST.(row).td3.icon.offset = "-2, -1";
		GameInterface.TABLE_LIST.(row).td3.icon.width = 20;
		GameInterface.TABLE_LIST.(row).td3.icon.height = 20;
		GameInterface.TABLE_LIST.(row).td3.textoffset = "27,0";
		GameInterface.TABLE_LIST.(row).td3.str = XI_ConvertString(sGood);
		GameInterface.TABLE_LIST.(row).td3.scale = 0.85;
		n++;
	}
	Table_UpdateWindow("TABLE_LIST");
}

void NullSelectTable(string sControl)
{
	if (sControl != CurTable)
	{
	    GameInterface.(sControl).select = 0;
	    Table_UpdateWindow(sControl);
	}
}

void ShowItemInfo()
{
	if (CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".index")) // нет товара вообще
	{
	    ShowGoodsInfo(sti(GameInterface.(CurTable).(CurRow).index));
		XI_WindowDisable("QTY_WINDOW", false);
		XI_WindowShow("QTY_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", true);
		SetCurrentNode("QTY_CANCEL_BUTTON");
	}
}

void GoodsExitCancel()
{

	XI_WindowDisable("QTY_WINDOW", true);
	XI_WindowShow("QTY_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", false);
	SetCurrentNode("TABLE_LIST");
	// пересчитаем новые пареметры от веса
	if (iShipQty != GetCargoGoods(refCharacter, iCurGoodsIdx))
	{
		SetCharacterGoods(refCharacter, iCurGoodsIdx, iShipQty); // вернем обратно
		OnShipScrollChange();
	}
}

void ShowGoodsInfo(int iGoodIndex)
{
	string GoodName = goods[iGoodIndex].name;

	int lngFileID = LanguageOpenFile("GoodsDescribe.txt");
	string sHeader = XI_ConvertString(GoodName);

    iCurGoodsIdx = iGoodIndex;
	string goodsDescr = GetAssembledString( LanguageConvertString(lngFileID,goodName+"_descr"), &Goods[iGoodIndex]);
    goodsDescr += newStr() + XI_ConvertString("weight") + " " + Goods[iGoodIndex].weight + " " + XI_ConvertString("cwt") +
	              ", пачка " + Goods[iGoodIndex].Units + " " + XI_ConvertString("units");

	iUnits  = sti(Goods[iGoodIndex].Units);
	//fWeight = stf(Goods[iGoodIndex].weight);
	if(checkAttribute(refCharacter, "ship.cargo.goods." + GoodName + ".isquest"))
	{
		string sTradeQ = refCharacter.ship.cargo.goods.(GoodName).isquest.quantity;
		string sColony = refCharacter.ship.cargo.goods.(GoodName).isquest.colony;
		goodsDescr += NewStr() + XI_ConvertString("YouNeedToDelivery") + sTradeQ + XI_ConvertString("QuantityOfGoodsToColony") + XI_ConvertString("Colony"+sColony) + ".";
	}

    SetFormatedText("QTY_TypeOperation", "Выкинуть?");
    SetFormatedText("QTY_Result", "");
    GameInterface.qty_edit.str = "0";

	SetNewGroupPicture("QTY_GOODS_PICTURE", "GOODS", GoodName);
    SetFormatedText("QTY_CAPTION", sHeader);
    SetFormatedText("QTY_GOODS_INFO", goodsDescr);
	LanguageCloseFile(lngFileID);

	iShipQty = GetCargoGoods(refCharacter, iGoodIndex);
    SetFormatedText("QTY_INFO_SHIP_QTY", its(iShipQty))
    SetFormatedText("QTY_INFO_STORE_QTY", its(GetGoodWeightByType(iGoodIndex, iShipQty)));

    SetFormatedText("QTY_INFO_STORE_PRICE", "Вес в трюме");
    SetFormatedText("QTY_INFO_SHIP_PRICE",  "Количество в трюме");
}

void TransactionOK()
{
	int nTradeQuantity;
	confirmChangeQTY_EDIT();
	nTradeQuantity = sti(GameInterface.qty_edit.str);

    SetCharacterGoods(refCharacter, iCurGoodsIdx, iShipQty);
	if (nTradeQuantity > 0)
	{
		DropGoodsToSeaFromInterface(iCurGoodsIdx, nTradeQuantity); // остается плавать в море
		iShipQty = GetCargoGoods(refCharacter, iCurGoodsIdx);
	}
	ShipSituation_SetQuestSituation(ShipSituation_1);
	GoodsExitCancel();

	//SetShipOTHERTable("TABLE_OTHER", xi_refCharacter);
}

void confirmChangeQTY_EDIT()
{
	ChangeQTY_EDIT();
    SetCurrentNode("QTY_OK_BUTTON");
}

void ChangeQTY_EDIT()
{
	int  iWeight;
	GameInterface.qty_edit.str = abs(sti(GameInterface.qty_edit.str));  // приведение к числу

	string GoodName = goods[iCurGoodsIdx].name;

    // проверка на колво доступное -->
    if (sti(GameInterface.qty_edit.str) > iShipQty)
    {
        GameInterface.qty_edit.str = iShipQty;
    }
    SetFormatedText("QTY_INFO_SHIP_QTY", its(iShipQty - sti(GameInterface.qty_edit.str)))
	SetFormatedText("QTY_INFO_STORE_QTY", its(GetGoodWeightByType(iCurGoodsIdx, (iShipQty - sti(GameInterface.qty_edit.str)))));
	// прикидываем место в трюме
	iWeight = sti(GameInterface.qty_edit.str);
	SetCharacterGoods(refCharacter, iCurGoodsIdx, iShipQty);
	if (iWeight > 0)
	{
		RemoveCharacterGoods(refCharacter, iCurGoodsIdx, iWeight);
	}
	OnShipScrollChange();
	///iWeight = GetGoodWeightByType(iCurGoodsIdx, sti(GameInterface.qty_edit.str));
	//GameInterface.TABLE_OTHER.tr6.td3.str = (GetCargoLoad(xi_refCharacter) - iWeight) + " / " + GetCargoMaxSpace(xi_refCharacter);
	//Table_UpdateWindow("TABLE_OTHER");
}

void REMOVE_ALL_BUTTON()  // продать все
{
	GameInterface.qty_edit.str = iShipQty;
	ChangeQTY_EDIT();
}

void ADD_ALL_BUTTON()  // купить все
{
	GameInterface.qty_edit.str = 0;
	ChangeQTY_EDIT();
}

void REMOVE_BUTTON()  // продать
{
	GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) + iUnits);
	ChangeQTY_EDIT();
}

void ADD_BUTTON()  // купить
{
	GameInterface.qty_edit.str = (sti(GameInterface.qty_edit.str) - iUnits);
	if (sti(GameInterface.qty_edit.str) < 0) GameInterface.qty_edit.str = 0;
	ChangeQTY_EDIT();
}

void DropGoodsToSeaFromInterface(int iGoodIndex, int iQuantity)
{
	ref rGood;
	string sGood;

	sGood = Goods[iGoodIndex].name;

	if (CheckAttribute(refCharacter, "Ship.Cargo.Goods."+sGood))
	{
		RemoveCharacterGoods(refCharacter, iGoodIndex, iQuantity);
		if (bSeaActive && !bAbordageStarted)  // море, но не каюта
		{
			iQuantity = iQuantity / sti(Goods[iGoodIndex].Units);

			int iSwimQuantity = rand(iQuantity/4) + rand(iQuantity/4) + rand(iQuantity/4) + rand(iQuantity/4);

			if(iSwimQuantity < 1)
			{
				iSwimQuantity = 1;
			}

			RecalculateCargoLoad(refCharacter);

			if (CheckAttribute(&Goods[iGoodIndex], "Swim"))
			{
				float fTime = stf(Goods[iGoodIndex].Swim.Time);
				string sModel = Goods[iGoodIndex].Swim.Model;

				fTime = fTime + (frnd() - 0.5) * fTime / 2.0;

				AISeaGoods_AddGood(refCharacter, sGood, sModel, fTime * 60.0, iSwimQuantity);
			}
		}
	}
}

void ShipChangeCaptan()
{
	if (isCompanion(xi_refCharacter))
	{ // наш товарищ
		if (!CheckAttribute(xi_refCharacter, "Tasks.Clone")) //zagolski. баг с двойниками в каюте
		{
			SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Captain"));
			SetFormatedText("REMOVE_WINDOW_TEXT", "Сместить капитана?");
			SetSelectable("REMOVE_ACCEPT_OFFICER", true);
			sMessageMode = "ShipChangeCaptanRemove";
			ShowShipChangeMenu();
		}
		else
		{
			SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Captain"));
			SetFormatedText("REMOVE_WINDOW_TEXT", "В данный момент этот капитан не может быть смещен.");
			sMessageMode = "ShipChangeCaptanMessage";
			ShowOkMessage();
		}
	}
	else
	{
		// если пленный
		if (!LAi_IsDead(xi_refCharacter))
		{
			SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Surrendered_caption_2"));
			if (!CheckAttribute(xi_refCharacter, "Tasks.Clone")) SetFormatedText("REMOVE_WINDOW_TEXT", XI_ConvertString("Surrendered_text_2"));
			else SetFormatedText("REMOVE_WINDOW_TEXT", "Этот капитан в данный момент не может быть снят.");
			SetSelectable("REMOVE_ACCEPT_OFFICER", true);
			sMessageMode = "SurrenderedCaptanRemove";
			ShowShipChangeMenu();
		}
		else
		{
			//Boyer add
			FlagPerkForCapturedShip(xi_refCharacter);
			/// проверка мин команд
			if ((GetCrewQuantity(xi_refCharacter) + GetCrewQuantity(refCharacter)) < (GetMinCrewQuantity(xi_refCharacter) + GetMinCrewQuantity(refCharacter)))
			{
				SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Capture Ship"));
				SetFormatedText("REMOVE_WINDOW_TEXT", "Необходимо наличие матросов на минимальные команды для обоих кораблей.\n Назначение капитана невозможно.");
				sMessageMode = "ShipChangeCaptanMessage";
				ShowOkMessage();
			}
			else
			{
				// проверка на 5 кораблей
				if (GetCompanionQuantity(refCharacter) < COMPANION_MAX)
				{
					FillPassengerScroll();
				    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"PASSENGERSLIST",-1);
				    SetCurrentNode("PASSENGERSLIST");
					ProcessFrame();
					SetOfficersSkills();

					XI_WindowShow("OFFICERS_WINDOW", true);
					XI_WindowDisable("OFFICERS_WINDOW", false);
					XI_WindowDisable("MAIN_WINDOW", true);
				}
				else
				{
				    SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Capture Ship"));
					SetFormatedText("REMOVE_WINDOW_TEXT", "В эскадре героя может быть только восемь кораблей.");
					sMessageMode = "ShipChangeCaptanMessage";
					ShowOkMessage();
				}
			}
		}
	}
}

void ExitShipChangeMenu()
{
	XI_WindowShow("REMOVE_OFFICER_WINDOW", false);
	XI_WindowDisable("REMOVE_OFFICER_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

    if (1)//sMessageMode == "ShipChangeCaptanMessage")
    {
		SetCurrentNode("CAPTAN_BUTTON");
	}
	if (sMessageMode != "HIRE_CREW_WINDOW")
		sMessageMode = "";
}

void KillShipAndExit()
{
	if (bSwap) {
		ShipSituation_SetQuestSituation(ShipSituation_3);
		SeaAI_SwapShipAfterAbordage(refCharacter, refEnemyCharacter);
	}
	if (xi_refCharacter.id != refEnemyCharacter.id) // новый назначенец
	{
		ShipSituation_SetQuestSituation(ShipSituation_2);
		SeaAI_SetOfficer2ShipAfterAbordage(xi_refCharacter, refEnemyCharacter); // to_do делать один раз на закрытии могут быть баги со множественой сменой в море
	}
	// убить на выходе
	if (bSeaActive)
	{
		if (bTransferMode)
		{
			ShipDead(sti(xi_refCharacter.index), KILL_BY_SELF, sti(refCharacter.index));  // сами же и топим
		}
		else
		{
			ShipDead(sti(xi_refCharacter.index), KILL_BY_ABORDAGE, sti(refCharacter.index));
		}
	}
	CheckQuestAboardCabinSituation(xi_refCharacter);
	//DeleteAttribute(refEnemyCharacter,"ship");
	//refEnemyCharacter.ship.type = SHIP_NOTUSED;
	ClearShipTypeForPassenger();
	if (bTransferMode)
	{
		IDoExit(RC_INTERFACE_ANY_EXIT);
	}
	else
	{
		IDoExit(RC_INTERFACE_RANSACK_MAIN_EXIT);
	}
}

void GoToShipChange() // нажатие ОК на табличке ок-отмена
{
	ref     sld;
	aref    arTo, arFrom;

	switch (sMessageMode)
	{
		case "SurrenderedCaptanRemove":  // убрать сдавшегося капитана
			ExitShipChangeMenu();
			// первый проход - кэп ещё жив
			SetFormatedText("CAPTURE_TEXT", XI_ConvertString("Surrendered_captain_capture"));
			SetNewPicture("CAPTURE_PICTURE", "interfaces\portraits\128\face_" + xi_refCharacter.FaceId + ".tga");
			ShowCaptureWindow();
			SetNodeUsing("CAPTURE_CREW_KILL",  false);
			SetNodeUsing("CAPTURE_CREW_PRISON",  false);
			SetCurrentNode("CAPTURE_CANCEL");
		break;

		case "ShipChangeCaptanRemove":  // снять кэпа
			sld = GetCharacter(NPC_GenerateCharacter("DeadShipCap"+xi_refCharacter.id, "off_hol_2", "man", "man", 60, sti(refCharacter.nation), 0, true)); // фантом, на время
			sld.faceId = 666;
			sld.name = "";
			sld.lastname = "";
			DeleteAttribute(sld,"ship");
			sld.ship = "";
			LAi_SetCurHP(sld, 0.0); // умер

			makearef(arTo,   sld.ship);
			makearef(arFrom, xi_refCharacter.Ship);
			CopyAttributes(arTo, arFrom);

			//SeaAI_SetOfficer2ShipAfterAbordage(sld, xi_refCharacter); // to_do делать один раз на закрытии могут быть баги со множественой сменой в море
			// убить на выходе
			//DeleteAttribute(xi_refCharacter,"ship"); // трем корабль
			//xi_refCharacter.ship.type = SHIP_NOTUSED;
			// снимем компаньона в офы -->
			RemoveCharacterCompanion(refCharacter, xi_refCharacter);
			AddPassenger(refCharacter, xi_refCharacter, false);
			DelBakSkill();
			// снимем компаньона в офы <--
			xi_refCharacter = sld;
			ExitShipChangeMenu();
			CheckQuestAboardCabinSituation(xi_refCharacter);
			OnShipScrollChange();

			DeleteAttribute(&GameInterface,"SHIPS_SCROLL");
			SetNodeUsing("SHIPS_SCROLL",false);
			SetNodeUsing("SHIPS_LEFTSCROLLBUTTON",false);
			SetNodeUsing("SHIPS_RIGHTSCROLLBUTTON",false);
			SetNodeUsing("SHIPS_SCROLL_FRAME",false);
		break;

		case "ShipDeadAsk": // выход с убиением корабля
            KillShipAndExit();
		break;

		// отпустить кэпа
		case "ShipGoFreeAsk":
			sld = GetCharacter(NPC_GenerateCharacter(refEnemyCharacter.id + "_free", "off_hol_2", "man", "man", 60, sti(refEnemyCharacter.nation), 0, true)); // фантом, на время
			ChangeAttributesFromCharacter(sld, refEnemyCharacter, false);
			DeleteAttribute(sld, "ship");
			sld.ship = "";

			makearef(arTo,   sld.ship);
			makearef(arFrom, refEnemyCharacter.Ship);
			CopyAttributes(arTo, arFrom);

			if(CheckAttribute(refCharacter,"GenQuest.ShipSituation.Explosion")){ LAi_SetCurHP(refEnemyCharacter, 0.0); }
			sld.AlwaysFriend = true;
			sld.Abordage.Enable    = false; // запрет абордажа
			// на случай диалдога на палубе потом
			if(xi_refCharacter.Id == "PiratesOnUninhabited_BadPirate")
			{
				sld.Dialog.Filename = "GenQuests_Dialog.c";
				sld.greeting        = "Gr_MiddPirate";
				sld.DeckDialogNode  = "PiratesOnUninhabited_50_Deck1";
			}
			else
			{
				sld.Dialog.Filename = "Capitans_dialog.c";
				sld.greeting        = "Gr_Commander";
				sld.DeckDialogNode  = "Go_away";
			}
			xi_refCharacter = sld;
			// обмена кораблями тут нет
			//SeaAI_SetOfficer2ShipAfterAbordage(xi_refCharacter, refEnemyCharacter);
			SeaAI_SetCaptainFree(xi_refCharacter, refEnemyCharacter);
		    refEnemyCharacter.location = "none";

			if (bSeaActive)
			{
				if (!bTransferMode)
				{
					ShipTakenFree(sti(refEnemyCharacter.index), KILL_BY_ABORDAGE, sti(refCharacter.index)); // тут умер реальный кэп, апдайтим ещё
				}

				PostEvent("evntQuestsCheck", 400);
				SetCharacterRelationBoth(sti(xi_refCharacter.index), GetMainCharacterIndex(), RELATION_FRIEND);
				UpdateRelations();
				RefreshBattleInterface();
				CheckQuestAboardCabinSituation(xi_refCharacter);
				DoQuestCheckDelay("NationUpdate", 1.5);
			}
			ClearShipTypeForPassenger();
			if (bTransferMode)
			{
				IDoExit(RC_INTERFACE_ANY_EXIT);
			}
			else
			{
				IDoExit(RC_INTERFACE_RANSACK_MAIN_EXIT);
			}
		break;

		case "AskEnterCompanionCapturedTransfer": // возможность автоматически распорядиться добычей компаньона
			CompaniontDefaultCapturedDecision(refCharacter, xi_refCharacter);
			IDoExit(RC_INTERFACE_RANSACK_MAIN_EXIT);
		break;
	}
}

void ShowShipChangeMenu()
{
	XI_WindowShow("REMOVE_OFFICER_WINDOW", true);
	XI_WindowDisable("REMOVE_OFFICER_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

    SetNodeUsing("REMOVE_OK", false);
	SetNodeUsing("REMOVE_ACCEPT_OFFICER", true);
	SetNodeUsing("REMOVE_CANCEL_OFFICER", true);

	SetCurrentNode("REMOVE_CANCEL_OFFICER");
}

void ShowOkMessage()
{
	XI_WindowShow("REMOVE_OFFICER_WINDOW", true);
	XI_WindowDisable("REMOVE_OFFICER_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

    SetNodeUsing("REMOVE_OK", true);
	SetNodeUsing("REMOVE_ACCEPT_OFFICER", false);
	SetNodeUsing("REMOVE_CANCEL_OFFICER", false);

	SetCurrentNode("REMOVE_OK");
}

void ShowOtherClick()
{
	// test не работает  :( CreateMessageBox("sCaptionID", "sMessageID", "", "", "", "");
}

void SwapProcess()
{
	SeaAI_SwapShipsAttributes(refCharacter, xi_refCharacter, true);
	//if (xi_refCharacter.id == refEnemyCharacter.id)
	//{
	//	SeaAI_SwapShipAfterAbordage(refCharacter, xi_refCharacter);
	//}
	//#20200329-02
	if(!IsCompanion(xi_refCharacter)) {
        int nSwapCrew = GetCrewQuantity(xi_refCharacter);
        TakeCrew(nSwapCrew);
	}
	bSwap = !bSwap;
    // оптимизация скилов -->
    DelBakSkill();
    // оптимизация скилов <--
	OnShipScrollChange();
	ExitCrewWindow(); // для профигактики
	bool bOk = !bSeaActive && LAi_grp_alarmactive;
	if (bTransferMode && !bDisableMapEnter && !bOk && !chrDisableReloadToLocation && !HasSubStr(refEnemyCharacter.id, "_DriftCap_"))
	{
		DeleteAttribute(&GameInterface,"SHIPS_SCROLL");
		FillScrollImageWithCompanions("SHIPS_SCROLL", COMPANION_MAX);
		SeaAI_SwapShipAfterAbordage(refCharacter, xi_refCharacter);
	}
}

//////////////
void ExitOfficerMenu()
{
	XI_WindowShow("OFFICERS_WINDOW", false);
	XI_WindowDisable("OFFICERS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CAPTAN_BUTTON");
}

void AcceptAddOfficer()
{
	string  attributeName2 = "pic"+(nCurScrollOfficerNum+1);
    ref     sld;
	aref    arTo, arFrom;
    if (checkAttribute(GameInterface, "PASSENGERSLIST."+attributeName2 + ".character"))
    {
		int iChar = sti(GameInterface.PASSENGERSLIST.(attributeName2).character);
        // назначение нового кэпа, возможно, если там уже не наш, те или враг или снят
        sld = GetCharacter(iChar);

		if(sld.id != "pet_crab" && sld.id != "Albreht_Zalpfer" && !CheckAttribute(sld,"HPminusDaysNeedtoRestore"))
		{
			DeleteAttribute(sld,"ship");
			sld.ship = "";

			makearef(arTo,   sld.ship);
			makearef(arFrom, xi_refCharacter.ship);
			CopyAttributes(arTo, arFrom);
			// снимем пассажира -->
			CheckForReleaseOfficer(iChar);
			RemovePassenger(refCharacter, sld);
			// снимем пассажира <--
			SetCompanionIndex(refCharacter, -1, iChar);

			xi_refCharacter = sld;
			DelBakSkill();
		}
	}
	ExitOfficerMenu();
	// оптимизация скилов -->
    DelBakSkill();
    // оптимизация скилов <--
	OnShipScrollChange();

	bool bOk = !bSeaActive && LAi_grp_alarmactive;
	if (bTransferMode && !bDisableMapEnter && !bOk && !chrDisableReloadToLocation && !HasSubStr(refEnemyCharacter.id, "_DriftCap_"))
	{
		FillScrollImageWithCompanions("SHIPS_SCROLL",COMPANION_MAX);
		SetNodeUsing("SHIPS_SCROLL",true);
		SetNodeUsing("SHIPS_LEFTSCROLLBUTTON",true);
		SetNodeUsing("SHIPS_RIGHTSCROLLBUTTON",true);
		SetNodeUsing("SHIPS_SCROLL_FRAME",true);
	}
}

void SetOfficersSkills()
{
	string sCharacter = "pic"+(sti(GameInterface.PASSENGERSLIST.current)+1);
	if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter))
	{
		if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter + ".character"))
		{
			sCharacter = GameInterface.PASSENGERSLIST.(sCharacter).character;
			ref otherchr = &characters[sti(sCharacter)];
	        SetSPECIALMiniTable("TABLE_SMALLSKILL", otherchr);
	        SetOTHERMiniTable("TABLE_SMALLOTHER", otherchr);
	        SetFormatedText("OFFICER_NAME", GetFullName(otherchr));
	        SetSelectable("ACCEPT_ADD_OFFICER", true);
	        return;
        }
	}
    Table_Clear("TABLE_SMALLSKILL", false, true, true);
    Table_Clear("TABLE_SMALLOTHER", false, true, true);
    SetFormatedText("OFFICER_NAME", "");
    SetSelectable("ACCEPT_ADD_OFFICER", false);
}

void FillPassengerScroll()
{
	int i, howWork;
	string faceName;
	string attributeName;
	int _curCharIdx;
	ref _refCurChar;
	bool  ok;

	DeleteAttribute(&GameInterface, "PASSENGERSLIST");

	nCurScrollOfficerNum = -1;
	GameInterface.PASSENGERSLIST.current = 0;

	int nListSize = GetPassengersQuantity(refCharacter);
	int nListSizeFree = nListSize;

	GameInterface.PASSENGERSLIST.NotUsed = 6;
	GameInterface.PASSENGERSLIST.ListSize = nListSizeFree + 2;

	GameInterface.PASSENGERSLIST.ImagesGroup.t0 = "EMPTYFACE";

	FillFaceList("PASSENGERSLIST.ImagesGroup", refCharacter, 2); // passengers

	GameInterface.PASSENGERSLIST.BadTex1 = 0;
	GameInterface.PASSENGERSLIST.BadPic1 = "emptyface";
	int m = 0;
	for(i=0; i<nListSize; i++)
	{
		attributeName = "pic" + (m+1);
		_curCharIdx = GetPassenger(refCharacter,i);

		if (_curCharIdx!=-1)
		{
			ok = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
			if (!ok && GetRemovable(&characters[_curCharIdx]))
			{
				GameInterface.PASSENGERSLIST.(attributeName).character = _curCharIdx;
				GameInterface.PASSENGERSLIST.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.PASSENGERSLIST.(attributeName).tex1 = FindFaceGroupNum("PASSENGERSLIST.ImagesGroup", "FACE128_"+Characters[_curCharIdx].FaceID);
				m++;
			}
		}
	}
	GameInterface.PASSENGERSLIST.ListSize = m + 2; // не знаю зачем, но для совместимости с 'было'
}

void DelBakSkill()
{
	DelBakSkillAttr(xi_refCharacter);
    ClearCharacterExpRate(xi_refCharacter);
    RefreshCharacterSkillExpRate(xi_refCharacter);
    SetEnergyToCharacter(xi_refCharacter);
    DeleteAttribute(xi_refCharacter, "TmpSkillRecall");

    DelBakSkillAttr(refCharacter);
    ClearCharacterExpRate(refCharacter);
    RefreshCharacterSkillExpRate(refCharacter);
    SetEnergyToCharacter(refCharacter);
    DeleteAttribute(refCharacter, "TmpSkillRecall");
}

void ClearShipTypeForPassenger()
{
	int iPassenger;

	for (int io = 0; io<GetPassengersQuantity(refCharacter); io++)
	{   // любой пассажир у кого есть пристрастие может свалить если наши дела ему не по душе
		iPassenger = GetPassenger(refCharacter, io);
		if (iPassenger != -1)
		{
			characters[iPassenger].ship.type = SHIP_NOTUSED; //пассажиры по определению не могут иметь корабли
		}
	}
}

void TakeAllGoods()
{
	int i, j, idx, qty;
	float fMaxCost;

	for (j = 0; j< GOODS_QUANTITY; j++)
	{
		fMaxCost = 0;
		idx = -1;
		for (i = 0; i< GOODS_QUANTITY; i++)
		{
			if (GetCargoGoods(xi_refCharacter, i) > 0)
			{
				if (fMaxCost < stf(Goods[i].Cost)/stf(Goods[i].Weight)) // поиск ликвидного товара
				{
					fMaxCost = stf(Goods[i].Cost)/stf(Goods[i].Weight);
					idx = i;
				}
			}
		}
		if (fMaxCost > 0)
		{
			qty = AddCharacterGoodsSimple(refCharacter, idx, GetCargoGoods(xi_refCharacter, idx));
			if (qty > 0)
			{
				RemoveCharacterGoodsSelf(xi_refCharacter, idx, qty);
			}
			else
			{   // нет места
				OnShipScrollChange();
				ShipSituation_SetQuestSituation(ShipSituation_1);
				return;
			}
		}
	}
	OnShipScrollChange();
	ShipSituation_SetQuestSituation(ShipSituation_1);
	ExitCrewWindow(); // для профилактики
}

/*ниже добавляем функции снятия пушек*/
//неплохо было бы блокировать кнопку после использования. Но можно же обменять потом корабль и снять пушки уже со второго. Хотя такое и не нужно. Но пусть не блокируется.
void CanonsRemoveAll()
{
	SetCannonsToBort(xi_refCharacter, "fcannon", 0);
	SetCannonsToBort(xi_refCharacter, "bcannon", 0);
	SetCannonsToBort(xi_refCharacter, "rcannon", 0);
	SetCannonsToBort(xi_refCharacter, "lcannon", 0);

	OnShipScrollChange();
}


void SetCannonsToBort(ref chr, string sBort, int iQty)//тут нужен только iQty=0, но оставлю так для совместимости
{
	int     curQty = GetBortCannonsQty(chr, sBort);
	int     maxQty = GetBortCannonsMaxQty(chr, sBort);
	int     i;
	string  attr;
	string  sBort_real;

	if(sBort == "rcannon") sBort_real = "cannonr";
	if(sBort == "lcannon") sBort_real = "cannonl";
	if(sBort == "fcannon") sBort_real = "cannonf";
	if(sBort == "bcannon") sBort_real = "cannonb";

	int idx = GetCannonGoodsIdxByType(sti(chr.Ship.Cannons.Type));

	if (iQty < curQty)
	{// лишние на склад
		SetCharacterGoods(chr, idx, GetCargoGoods(chr, idx) + (curQty - iQty)); // этот метод, тк перегруз может быть, а  AddCharacterGoodsSimple режет перегруз
	}
	// нулим колво пушек на борту и распределяем заново от центра (как они на модели по номерам не знаю, допуск, что подряд)
	for (i = 0; i < maxQty; i++)
	{
		attr = "c" + i;
		chr.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0; // поломана на 100 процентов, не палит, те нет её
		chr.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0; // поломана на 100 процентов, не палит, те нет её
	}

	RecalculateCargoLoad(chr);  // пересчет, тк пушки снялись
}
/*выше добавляем функции снятия пушек*/

void SetGoodsArrows()
{
    int  idx;
    int  iLine;
	if (CurTable == "TABLE_LIST")
	{
	    if (GetRemovable(xi_refCharacter) && CheckAttribute(&GameInterface, CurTable + "." + CurRow + ".index")) // с купцами нельзя
	    {
			idx = sti(GameInterface.(CurTable).(CurRow).index);
		    iLine = iSelected - 1 - sti(GameInterface.TABLE_LIST.top);
			if  (GetCargoGoods(refCharacter, idx) > 0 && GetGoodQuantityByWeight(idx, GetCargoFreeSpace(xi_refCharacter)) > 0)
	        {
				SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_RIGHT",0, 438 ,286 + 20*iLine, 458, 286 + 20 + 20*iLine, 0);
				SetNodeUsing("B_RIGHT", true);
	        }
	        else
	        {
	        	SetNodeUsing("B_RIGHT",  false);
	        }
	        if (GetCargoGoods(xi_refCharacter, idx) > 0 && GetGoodQuantityByWeight(idx, GetCargoFreeSpace(refCharacter)) > 0)
	        {
				SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_LEFT",0, 280,286 + 20*iLine,300,286 + 20 + 20*iLine, 0);
	            SetNodeUsing("B_LEFT",  true);
	        }
	        else
	        {
	        	SetNodeUsing("B_LEFT", false);
	        }
	        return;
        }
	}
	SetNodeUsing("B_RIGHT",  false);
	SetNodeUsing("B_LEFT", false);
}

void TakeGoods(int inc)
{  // лево
	int  idx;
	int  qty;

	if (!GetRemovable(xi_refCharacter)) return; // с купцами нельзя

	if (CurTable == "TABLE_LIST")
	{
		idx = sti(GameInterface.(CurTable).(CurRow).index);
		inc = sti(Goods[idx].Units) * inc;
		if (inc > GetCargoGoods(xi_refCharacter, idx)) inc = GetCargoGoods(xi_refCharacter, idx);
		if (inc > 0)
		{
			qty = AddCharacterGoodsSimple(refCharacter, idx, inc);
			if (qty > 0)
			{
				RemoveCharacterGoodsSelf(xi_refCharacter, idx, qty);
			}
			OnShipScrollChange();
			SetGoodsArrows();
			ShipSituation_SetQuestSituation(ShipSituation_1);
		}
	}
}

void GiveGoods(int inc)
{  // право
	int  idx;
	int  qty;

	if (!GetRemovable(xi_refCharacter)) return; // с купцами нельзя

	if (CurTable == "TABLE_LIST")
	{
		idx = sti(GameInterface.(CurTable).(CurRow).index);
		inc = sti(Goods[idx].Units) * inc;
		if (inc > GetCargoGoods(refCharacter, idx)) inc = GetCargoGoods(refCharacter, idx);
		if (inc > 0)
		{
			qty = AddCharacterGoodsSimple(xi_refCharacter, idx, inc);
			if (qty > 0)
			{
				RemoveCharacterGoodsSelf(refCharacter, idx, qty);
			}
			OnShipScrollChange();
			SetGoodsArrows();
			ShipSituation_SetQuestSituation(ShipSituation_1);
		}
	}
}
////////////////////////////// crew ///////////////
void ShowCrewWindow()
{
	if (sMessageMode == "CREW_WINDOW")
	{
		ExitCrewWindow();
	}
	else
	{
		if (!isCompanion(xi_refCharacter) && !LAi_IsDead(xi_refCharacter))// пленный кэп
		{
			SetFormatedText("REMOVE_WINDOW_CAPTION", XI_ConvertString("Surrendered_caption_1"));
			SetFormatedText("REMOVE_WINDOW_TEXT", XI_ConvertString("Surrendered_text_1")); // Невозможно перемещать экипаж между не своими кораблями. ...
			sMessageMode = "SurrenderedCrewOkMessage";
			ShowOkMessage();
		}
		else
		{
			SetCrewVariable();

			XI_WindowShow("CREW_WINDOW", true);
			XI_WindowDisable("CREW_WINDOW", false);

			SetCurrentNode("CREW_Win_fix");
			SetNodeUsing("CREW_BUTTON", false);
			SetNodeUsing("CAPTAN_BUTTON", false);
			SetNodeUsing("SWAP_BUTTON", false);
			SetNodeUsing("TAKE_GOODS", false);
			SetNodeUsing("CANNONS_REMOVE_ALL", false);
			SetSelectable("SHIPS_SCROLL", false);
			SetSelectable("SHIPS_LEFTSCROLLBUTTON", false);
			SetSelectable("SHIPS_RIGHTSCROLLBUTTON", false);
			sMessageMode = "CREW_WINDOW";
		}
	}
}
void SetCrewVariable()
{
	string sText, sTextSecond;
	int iColor;

	SetCrewExpTable(refCharacter, "TABLE_CREW", "BAR_Sailors", "BAR_Cannoners", "BAR_Soldiers");

	SetFormatedText("CREW_QTY", ""+GetCrewQuantity(refCharacter));
	if (GetCrewQuantity(refCharacter) > GetOptCrewQuantity(refCharacter) || GetCrewQuantity(refCharacter) < GetMinCrewQuantity(refCharacter))
	{
		iColor = argb(255,255,64,64);
	}
	else
	{
		iColor = argb(255,255,255,255);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CREW_QTY", 8,-1,iColor);
	SetNewGroupPicture("CREW_MORALE_PIC", "MORALE_SMALL", GetMoraleGroupPicture(stf(refCharacter.ship.crew.morale)));
	SetFormatedText("CREW_MORALE_TEXT", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(refCharacter.Ship.crew.morale))));

	////  заполнялка 2
	SetCrewExpTable(xi_refCharacter, "TABLE_CREW2", "BAR_Sailors2", "BAR_Cannoners2", "BAR_Soldiers2");

	SetFormatedText("CREW_QTY2", ""+GetCrewQuantity(xi_refCharacter));
	if (GetCrewQuantity(xi_refCharacter) > GetOptCrewQuantity(xi_refCharacter) || GetCrewQuantity(xi_refCharacter) < GetMinCrewQuantity(xi_refCharacter))
	{
		iColor = argb(255,255,64,64);
	}
	else
	{
		iColor = argb(255,255,255,255);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CREW_QTY2", 8,-1,iColor);
	SetNewGroupPicture("CREW_MORALE_PIC2", "MORALE_SMALL", GetMoraleGroupPicture(stf(xi_refCharacter.ship.crew.morale)));
	SetFormatedText("CREW_MORALE_TEXT2", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(xi_refCharacter.Ship.crew.morale))));
}

void ExitCrewWindow()
{
	XI_WindowShow("CREW_WINDOW", false);
	XI_WindowDisable("CREW_WINDOW", true);

	SetNodeUsing("CREW_BUTTON", true);
	SetNodeUsing("CAPTAN_BUTTON", true);
	SetNodeUsing("SWAP_BUTTON", true);
	SetNodeUsing("TAKE_GOODS", true);
	SetNodeUsing("CANNONS_REMOVE_ALL", true);
	SetSelectable("SHIPS_SCROLL", true);
	SetSelectable("SHIPS_LEFTSCROLLBUTTON", true);
	SetSelectable("SHIPS_RIGHTSCROLLBUTTON", true);
	SetCurrentNode("CREW_BUTTON");
	sMessageMode = "";
}

void TakeCrew(int inc)
{  // лево
    float fTemp;
	if (inc > GetCrewQuantity(xi_refCharacter)) inc = GetCrewQuantity(xi_refCharacter);
	if ( (GetCrewQuantity(refCharacter) + inc) > GetMaxCrewQuantity(refCharacter)) inc = GetMaxCrewQuantity(refCharacter) - GetCrewQuantity(refCharacter);
	if (bSailorsWeight)
	{
		if ( inc > GetCargoFreeSpace(refCharacter)) inc = GetCargoFreeSpace(refCharacter);
	}
	if (inc > 0)
	{
		fTemp =  stf(GetCrewQuantity(refCharacter) + inc);
        refCharacter.Ship.Crew.Exp.Sailors      = (stf(refCharacter.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refCharacter) +
		                                            stf(xi_refCharacter.Ship.Crew.Exp.Sailors)*inc) / fTemp;
		refCharacter.Ship.Crew.Exp.Cannoners    = (stf(refCharacter.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refCharacter) +
		                                            stf(xi_refCharacter.Ship.Crew.Exp.Cannoners)*inc) / fTemp;
		refCharacter.Ship.Crew.Exp.Soldiers     = (stf(refCharacter.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refCharacter) +
			                                        stf(xi_refCharacter.Ship.Crew.Exp.Soldiers)*inc) / fTemp;

		refCharacter.Ship.Crew.morale           = (stf(refCharacter.Ship.Crew.morale)*GetCrewQuantity(refCharacter) +
			                                        stf(xi_refCharacter.Ship.Crew.morale)*inc) / fTemp;

		refCharacter.Ship.Crew.Quantity = sti(refCharacter.Ship.Crew.Quantity) + inc;
		xi_refCharacter.Ship.Crew.Quantity = sti(xi_refCharacter.Ship.Crew.Quantity) - inc;
		OnShipScrollChange();
		SetCrewVariable();
	}
}

void GiveCrew(int inc)
{  // право
	float fTemp;
	if (inc > GetCrewQuantity(refCharacter)) inc = GetCrewQuantity(refCharacter);
	if ( (GetCrewQuantity(xi_refCharacter) + inc) > GetMaxCrewQuantity(xi_refCharacter)) inc = GetMaxCrewQuantity(xi_refCharacter) - GetCrewQuantity(xi_refCharacter);
	if (bSailorsWeight)
	{
		if ( inc > GetCargoFreeSpace(xi_refCharacter)) inc = GetCargoFreeSpace(xi_refCharacter);
	}
	if (inc > 0)
	{
		fTemp =  stf(GetCrewQuantity(xi_refCharacter) + inc);
        xi_refCharacter.Ship.Crew.Exp.Sailors   = (stf(xi_refCharacter.Ship.Crew.Exp.Sailors)*GetCrewQuantity(xi_refCharacter) +
		                                            stf(refCharacter.Ship.Crew.Exp.Sailors)*inc) / fTemp;
		xi_refCharacter.Ship.Crew.Exp.Cannoners = (stf(xi_refCharacter.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(xi_refCharacter) +
		                                            stf(refCharacter.Ship.Crew.Exp.Cannoners)*inc) / fTemp;
		xi_refCharacter.Ship.Crew.Exp.Soldiers  = (stf(xi_refCharacter.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(xi_refCharacter) +
			                                        stf(refCharacter.Ship.Crew.Exp.Soldiers)*inc) / fTemp;

		xi_refCharacter.Ship.Crew.morale        = (stf(xi_refCharacter.Ship.Crew.morale)*GetCrewQuantity(xi_refCharacter) +
			                                        stf(refCharacter.Ship.Crew.morale)*inc) / fTemp;

		xi_refCharacter.Ship.Crew.Quantity = sti(xi_refCharacter.Ship.Crew.Quantity) + inc;
		refCharacter.Ship.Crew.Quantity = sti(refCharacter.Ship.Crew.Quantity) - inc;
		OnShipScrollChange();
		SetCrewVariable();
	}
}

/////////////////////////////// capture_window
void ExitCaptureWindow()
{
	XI_WindowShow("CAPTURE_WINDOW", false);
	XI_WindowDisable("CAPTURE_WINDOW", true);
    XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CAPTAN_BUTTON");
	sMessageMode = "";
}

void ShowCaptureWindow()
{
	XI_WindowShow("CAPTURE_WINDOW", true);
	XI_WindowDisable("CAPTURE_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "CAPTURE_WINDOW";
}

void ExitCaptureCrewWindow()
{
	if (sMessageMode == "CAPTURE_CREW_WINDOW") { return; }

	XI_WindowShow("CAPTURE_CREW_WINDOW", false);
	XI_WindowDisable("CAPTURE_CREW_WINDOW", true);
    XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CAPTAN_BUTTON");
	sMessageMode = "";
}

void ShowCaptureCrewWindow()
{
	XI_WindowShow("CAPTURE_CREW_WINDOW", true);
	XI_WindowDisable("CAPTURE_CREW_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "CAPTURE_CREW_WINDOW";
}

void ExitDecisionWindow()
{
	XI_WindowShow("DECISION_WINDOW", false);
	XI_WindowDisable("DECISION_WINDOW", true);
    XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CAPTAN_BUTTON");
	sMessageMode = "";
}

void ShowDecisionWindow()
{
	XI_WindowShow("DECISION_WINDOW", true);
	XI_WindowDisable("DECISION_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "DECISION_WINDOW";
}

void ShowHireCrewWindow()
{
	XI_WindowShow("HIRE_CREW_WINDOW", true);
	XI_WindowDisable("HIRE_CREW_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "HIRE_CREW_WINDOW";
}

void ExitHireCrewWindow()
{
	XI_WindowShow("HIRE_CREW_WINDOW", false);
	XI_WindowDisable("HIRE_CREW_WINDOW", true);
    XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CAPTAN_BUTTON");
	sMessageMode = "";

	OnShipScrollChange();
}

void SetEnemyToPrisoner()
{
	ExitCaptureWindow();
	SetCharToPrisoner(xi_refCharacter);
	LAi_SetCurHP(xi_refCharacter, 0.0); // умер
	//Boyer add
	FlagPerkForCapturedShip(xi_refCharacter);
	// нет изменений репы - будет при судьбе пленного
	OnShipScrollChange();
	// второй проход - судьба команды
	ShowCrewCaptureAsk();
}

void SetEnemyToKilled()
{
	ExitCaptureWindow();
	LAi_SetCurHP(xi_refCharacter, 0.0); // умер
	//Boyer add
	FlagPerkForCapturedShip(xi_refCharacter);
	ChangeCharacterReputation(refCharacter, -2); // плохое дело
	OfficersReaction("bad");
	OnShipScrollChange();
	// второй проход - судьба команды
	ShowCrewCaptureAsk();
}

void ShowCrewCaptureHire()
{
	if (GetCrewQuantity(xi_refCharacter) > 0 && iGetHired > 0)
	{
		InitVariable();
		SetVariable();
		ShowHireCrewWindow();
	}
}

void ShowCrewCaptureAsk()
{
	if (GetCrewQuantity(xi_refCharacter) > 0)
	{
		// второй проход - Команда
		SetFormatedText("CAPTURE_TEXT_CREW", XI_ConvertString("Surrendered_crew_capture"));
		SetNewPicture("CAPTURE_CREW_PICTURE", "interfaces\portraits\128\face_" + xi_refCharacter.FaceId + ".tga");
		ShowCaptureCrewWindow();

		int iMode = CheckEnemyShipHPFree();
		switch (iMode)
		{
			case 0:
				SetSelectable("CAPTURE_CREW_FREE", false);
				SetCurrentNode("CAPTURE_CREW_PRISON");
			break;
			case 1:
				SetCurrentNode("CAPTURE_CREW_FREE");
			break;
			case 2:
				SetCurrentNode("CAPTURE_CREW_FREE");
			break;
		}
	}
}

void SetEnemyCrewToPrisoner()
{
	sMessageMode = "";
	ExitCaptureCrewWindow();
	ChangeCharacterReputation(refCharacter, -(sti(xi_refCharacter.Ship.Crew.Quantity) + 9) / 10); // плохое дело
	OfficersReaction("bad");
	SetCharacterGoods(refCharacter, GOOD_SLAVES, (GetCargoGoods(refCharacter, GOOD_SLAVES) + sti(xi_refCharacter.Ship.Crew.Quantity))); // в перегруз, потом сам выкинет
	xi_refCharacter.Ship.Crew.Quantity = 0;
	OnShipScrollChange();
}

void SetEnemyCrewToKilled()
{
	sMessageMode = "";
	ExitCaptureCrewWindow();
	ChangeCharacterReputation(refCharacter, -(sti(xi_refCharacter.Ship.Crew.Quantity) + 4) / 5); // плохое дело
	OfficersReaction("bad");
	xi_refCharacter.Ship.Crew.Quantity = 0;
	OnShipScrollChange();
}

void SetEnemyCrewToFree()
{
	sMessageMode = "";
	ExitCaptureCrewWindow();
	if(SetEnemyCrewGoods() == true)
	{
		ChangeCharacterReputation(refCharacter, -(sti(xi_refCharacter.Ship.Crew.Quantity) + 49) / 50); // хорошее дело
		OfficersReaction("good");
	}
	RemoveEnemyShipHPFree();
	xi_refCharacter.Ship.Crew.Quantity = 0;
	OnShipScrollChange();
}

// ugeen --> снабжение сдавшихся врагов, отпущенных  на шлюпках
bool SetEnemyCrewGoodOne(int iGood, int iGoodRateEat)
{
	int iQty, iGoodQty;
	int iCrewQty = GetCrewQuantity(xi_refCharacter);

	iGoodQty = makeint(iCrewQty/iGoodRateEat + 1);
	iQty = GetCargoGoods(xi_refCharacter, iGood);
	if(iQty >= iGoodQty)
	{
		iQty = RemoveCharacterGoodsSelf(xi_refCharacter,iGood,iGoodQty); // сначала на корабле противника
		return iQty;
	}
	else
	{
		iQty = GetCargoGoods(refCharacter, iGood);
		if(iQty >= iGoodQty)
		{
			iQty = RemoveCharacterGoodsSelf(refCharacter,iGood,iGoodQty); // если нет у врага - спишем  с флагмана ГГ
			return iQty;
		}
	}
	return false;
}

int CheckEnemyShipHPFree()
{
	int iHP = makeint(GetCrewQuantity(xi_refCharacter)/HP_BY_ENEMY_CREW + 1) * HP_PER_BOAT;
	if(stf(xi_refCharacter.ship.hp) > iHP + 100)
	{   // списываем HP у абордируемого
		return 1;
	}
	else
	{
		// cписываем HP у абордирующего
		if(stf(refCharacter.ship.hp) > iHP + 200)
		{
			return 2;
		}
	}
	return 0; // блокируем кнопку
}

void RemoveEnemyShipHPFree()
{
	int iCrewQty = GetCrewQuantity(xi_refCharacter)/HP_BY_ENEMY_CREW;
	int iHP = makeint(iCrewQty + 1) * HP_PER_BOAT;
	if(stf(xi_refCharacter.ship.hp) > iHP + 100)
	{
		xi_refCharacter.ship.hp = stf(xi_refCharacter.ship.hp) - iHP;
	}
	else
	{
		refCharacter.ship.hp = stf(refCharacter.ship.hp) - iHP;
	}
}

bool SetEnemyCrewGoods() // снабдим отпущенных всем необходимым
{
	bool bOk;

	bOk = SetEnemyCrewGoodOne(GOOD_FOOD, FOOD_BY_ENEMY_CREW);
	bOk = bOk && SetEnemyCrewGoodOne(GOOD_RUM, RUM_BY_ENEMY_CREW);
	bOk = bOk && SetEnemyCrewGoodOne(GOOD_MEDICAMENT, MEDICAMENT_BY_ENEMY_CREW);
	SetEnemyCrewGoodOne(GOOD_WEAPON, WEAPON_BY_ENEMY_CREW);

	return bOk;
}
// ugeen <--

void SetShipToDrown()
{
	ExitDecisionWindow();
	KillShipAndExit();
}

void SetShipToDrift()
{
	ExitDecisionWindow();

	if (bSwap) SeaAI_SwapShipAfterAbordage(refCharacter, refEnemyCharacter);

	if (HasSubStr(xi_refCharacter.id, "_DriftCap_"))
	{
		LAi_SetCurHP(xi_refCharacter, 1.0);
	}
	else
	{
		ref sld = GetCharacter(NPC_GenerateCharacter("_DriftCap_" + refEnemyCharacter.id, "off_hol_2", "man", "man", 1, sti(refEnemyCharacter.nation), 0, true));
		DeleteAttribute(sld, "ship");
		sld.ship = "";

		aref arTo, arFrom;
		makearef(arTo, sld.ship);
		makearef(arFrom, refEnemyCharacter.Ship);
		CopyAttributes(arTo, arFrom);

		sld.AlwaysFriend = true;
		sld.Abordage.Enable = false;
		sld.Dialog.Filename = "Capitans_dialog.c";
		sld.greeting = "Gr_Commander";
		sld.DeckDialogNode = "Go_away";

		xi_refCharacter = sld;
		SeaAI_SetCaptainFree(xi_refCharacter, refEnemyCharacter);
		refEnemyCharacter.location = "none";
	}

	if (bSeaActive)
	{
		if (!bTransferMode)
		{
			ShipTakenFree(sti(refEnemyCharacter.index), KILL_BY_ABORDAGE, sti(refCharacter.index));
		}

		PostEvent("evntQuestsCheck", 400);
		UpdateRelations();
		RefreshBattleInterface();
		CheckQuestAboardCabinSituation(xi_refCharacter);
		DoQuestCheckDelay("NationUpdate", 1.5);
	}
	ClearShipTypeForPassenger();

	if (bTransferMode)
	{
		IDoExit(RC_INTERFACE_ANY_EXIT);
	}
	else
	{
		IDoExit(RC_INTERFACE_RANSACK_MAIN_EXIT);
	}
}

int GetEnemyHiredCrew()
{
	int iEnemyHiredCrew = 0;

	if(CheckAttribute(refCharacter,"GenQuest.ShipExplode")) return iEnemyHiredCrew;

	if (CheckAttribute(xi_refCharacter, "Ship.Mode") && xi_refCharacter.Ship.Mode == "War")
	{
		iEnemyHiredCrew = makeint(GetCrewQuantity(xi_refCharacter) * rand( makeint(GetSummonSkillFromName(refCharacter, SKILL_LEADERSHIP)*0.2))/100.0 + GetCrewQuantity(xi_refCharacter) * rand( makeint(GetSummonSkillFromName(refCharacter, SKILL_FORTUNE)*0.2))/100.0 );
	}
	else // все остальные
	{
		iEnemyHiredCrew = makeint(GetCrewQuantity(xi_refCharacter) * rand(makeint(GetSummonSkillFromName(refCharacter, SKILL_LEADERSHIP)*0.7))/100.0);
	}
	int iDiffCrew = GetMaxCrewQuantity(refCharacter) - GetCrewQuantity(refCharacter);
	// учет веса экипажа -- если нет места на корабле, то в команду не пойдут :(
	if(iDiffCrew > GetCargoFreeSpace(refCharacter))
	{
		iDiffCrew = GetCargoFreeSpace(refCharacter);
	}

	if(iEnemyHiredCrew > iDiffCrew)
	{
		iEnemyHiredCrew = iDiffCrew;
	}
	return iEnemyHiredCrew;
}

void SetShipInfoPic(ref chr, string sAdd)
{
	int iShip = sti(chr.ship.type);
	if (iShip != SHIP_NOTUSED)
	{
		ref refBaseShip = GetRealShip(iShip);
		string sShip = refBaseShip.BaseName;
		SetNewPicture("MAIN_SHIP_PICTURE" + sAdd, "interfaces\ships\" + sShip + ".tga");
	}
}

void InitVariable()
{
	int nShipType = sti(refCharacter.ship.type);

	ref refBaseShip = GetRealShip(nShipType);

	SetShipInfoPic(refCharacter, "1");
	SetShipInfoPic(xi_refCharacter, "2");

	if (CheckAttribute(refCharacter, "ship.name"))
	{
		SetFormatedText("CAPACITY", refCharacter.ship.name);
	}
	else
	{
		SetFormatedText("CAPACITY", "");
	}
	if (CheckAttribute(xi_refCharacter, "ship.name"))
	{
		SetFormatedText("HIRE_CAPACITY", xi_refCharacter.ship.name);
	}
	else
	{
		SetFormatedText("HIRE_CAPACITY", "");
	}

	SetNewPicture("MAIN_HERO_PICTURE", "interfaces\portraits\256\face_" + its(refCharacter.FaceId) + ".tga");
	SetFormatedText("MAIN_HERO_NAME", GetFullName(refCharacter));

	SetNewGroupPicture("HIRE_CREW_PICTURE", "SHIP_STATE_ICONS", "Crew");
	SetNewGroupPicture("HIRE_CREW_PICTURE2", "SHIP_STATE_ICONS", "Crew");

	SetFormatedText("HIRE_CREW_CAPACITY", "Команда:\nмин. "+GetMinCrewQuantity(refCharacter) + ", опт. " + GetOptCrewQuantity(refCharacter) + ", макс. " + GetMaxCrewQuantity(refCharacter));
	SetFormatedText("HIRE_QTY_TypeOperation", "Желающих перейти к нам в команду");
}

void SetVariable()
{
	int iColor;
	string sText;

	SetCrewExpTable(refCharacter, "HIRE_TABLE_CREW", "HIRE_BAR_Sailors", "HIRE_BAR_Cannoners", "HIRE_BAR_Soldiers");

	SetFormatedText("HIRE_CREW_QTY", ""+GetCrewQuantity(refCharacter));
	if (GetCrewQuantity(refCharacter) > GetOptCrewQuantity(refCharacter) || GetCrewQuantity(refCharacter) < GetMinCrewQuantity(refCharacter))
	{
		iColor = argb(255,255,64,64);
	}
	else
	{
		iColor = argb(255,255,255,255);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"HIRE_CREW_QTY", 8,-1,iColor);
	SetNewGroupPicture("HIRE_CREW_MORALE_PIC", "MORALE_SMALL", GetMoraleGroupPicture(stf(refCharacter.ship.crew.morale)));
	SetFormatedText("HIRE_CREW_MORALE_TEXT", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(refCharacter.Ship.crew.morale))));

	// провиант на корабле ГГ
	sText = "Провианта на корабле:\nна ";
	int iFood = CalculateShipFood(refCharacter);
	sText = sText + FindRussianDaysString(iFood);
	SetFormatedText("HIRE_FOOD_SHIP", sText);

	// ром на корабле ГГ
	sText = "Рому на корабле:\nна ";
	int iRum = CalculateShipRum(refCharacter);
	sText = sText + FindRussianDaysString(iRum);
	SetFormatedText("HIRE_RUM_SHIP", sText);

	//  заполнялка противника
	SetCrewExpTable(xi_refCharacter, "HIRE_TABLE_CREW2", "HIRE_BAR_Sailors2", "HIRE_BAR_Cannoners2", "HIRE_BAR_Soldiers2");

	SetFormatedText("HIRE_CREW_QTY2", ""+GetCrewQuantity(xi_refCharacter));
	SetNewGroupPicture("HIRE_CREW_MORALE_PIC2", "MORALE_SMALL", GetMoraleGroupPicture(stf(xi_refCharacter.ship.crew.morale)));
	SetFormatedText("HIRE_CREW_MORALE_TEXT2", XI_ConvertString("CrewMorale") + ": " + XI_ConvertString(GetMoraleName(sti(xi_refCharacter.Ship.crew.morale))));

	if (CheckAttribute(xi_refCharacter,"Ship.Crew.Hire")) SetFormatedText("HIRED_CREW", ""+sti(xi_refCharacter.Ship.Crew.Hire));
	else SetFormatedText("HIRED_CREW", "0");
}

// бакап значений, до применения
void SetBackupQty()
{
	aref    arTo, arFrom;
	NullCharacter.CrewBak.Hero = "";
	NullCharacter.CrewBak.ECrew = "";

	makearef(arTo, NullCharacter.CrewBak.Hero);
	makearef(arFrom, refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);

	makearef(arTo, NullCharacter.CrewBak.ECrew);
	makearef(arFrom, xi_refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void GetBackupQty()
{
	aref    arTo, arFrom;

	makearef(arFrom, NullCharacter.CrewBak.Hero);
	makearef(arTo, refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);

	makearef(arFrom, NullCharacter.CrewBak.ECrew);
	makearef(arTo, xi_refCharacter.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void SetBackupExp()
{
	aref    arTo, arFrom;
	NullCharacter.CrewBak.Exp = "";
	NullCharacter.CrewBak.Morale = "";

	makearef(arTo, NullCharacter.CrewBak.Exp);
	makearef(arFrom, refCharacter.Ship.Crew.Exp);
	CopyAttributes(arTo, arFrom);

	makearef(arTo, NullCharacter.CrewBak.Morale);
	makearef(arFrom, refCharacter.Ship.Crew.Morale);
	CopyAttributes(arTo, arFrom);
}

void GetBackupExp()
{
	aref    arTo, arFrom;

	makearef(arFrom, NullCharacter.CrewBak.Exp);
	makearef(arTo, refCharacter.Ship.Crew.Exp);
	CopyAttributes(arTo, arFrom);

	makearef(arFrom, NullCharacter.CrewBak.Morale);
	makearef(arTo, xi_refCharacter.Ship.Crew.Morale);
	CopyAttributes(arTo, arFrom);
}

void HireTransactionCancel()
{
	GetBackupQty();
	DeleteAttribute(xi_refCharacter, "Ship.Crew.Hire");
	ExitHireCrewWindow();
}

void HireTransactionOK()
{
	DeleteAttribute(xi_refCharacter, "Ship.Crew.Hire");
	ExitHireCrewWindow();
}

void HIRE_ChangeQTY_EDIT()
{
	float fQty;
	string sText, sTextSecond;
	int iColor;

	GetBackupExp();

	fQty = stf(GetCrewQuantity(refCharacter) + iCrewQty);

	refCharacter.Ship.Crew.Exp.Sailors   = (stf(refCharacter.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refCharacter) +
			                                        stf(xi_refCharacter.Ship.Crew.Exp.Sailors)*iCrewQty) / fQty;
	refCharacter.Ship.Crew.Exp.Cannoners = (stf(refCharacter.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refCharacter) +
			                                        stf(xi_refCharacter.Ship.Crew.Exp.Cannoners)*iCrewQty) / fQty;
	refCharacter.Ship.Crew.Exp.Soldiers  = (stf(refCharacter.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refCharacter) +
			                                        stf(xi_refCharacter.Ship.Crew.Exp.Soldiers)*iCrewQty) / fQty;
	refCharacter.Ship.Crew.Morale        = (stf(refCharacter.Ship.Crew.Morale)*GetCrewQuantity(refCharacter) +
			                                        stf(xi_refCharacter.Ship.Crew.Morale)*iCrewQty) / fQty;

	SetCrewQuantity(refCharacter, GetCrewQuantity(refCharacter) + iCrewQty - iBackCrew);
	SetCrewQuantity(xi_refCharacter, GetCrewQuantity(xi_refCharacter) - iCrewQty + iBackCrew);

	iBackCrew = iCrewQty;

	SetVariable();
}

void HIRE_REMOVE_ALL_BUTTON()
{
	if(iCrewQty > 0)
	{
		xi_refCharacter.Ship.Crew.Hire = sti(xi_refCharacter.Ship.Crew.Hire) + iCrewQty;
		iCrewQty = 0;
		HIRE_ChangeQTY_EDIT();
	}
}

void HIRE_ADD_ALL_BUTTON()
{
	if(sti(xi_refCharacter.Ship.Crew.Hire) > 0)
	{
		iCrewQty = iCrewQty + sti(xi_refCharacter.Ship.Crew.Hire);
		xi_refCharacter.Ship.Crew.Hire = 0;
		HIRE_ChangeQTY_EDIT();
	}
}

void HIRE_REMOVE_BUTTON()
{
	if(iCrewQty > 0)
	{
		iCrewQty = iCrewQty -1;
		xi_refCharacter.Ship.Crew.Hire = sti(xi_refCharacter.Ship.Crew.Hire) + 1;
		HIRE_ChangeQTY_EDIT();
	}
}

void HIRE_ADD_BUTTON()
{
	if(sti(xi_refCharacter.Ship.Crew.Hire) > 0)
	{
		iCrewQty = iCrewQty +1;
		xi_refCharacter.Ship.Crew.Hire = sti(xi_refCharacter.Ship.Crew.Hire) - 1;
		HIRE_ChangeQTY_EDIT();
	}
}

/* Vorius 09/06/2009 -- взято с PiratesAhoy
   returns a string representing the given officer's position */
string GetOfficerPosition(string sCharacter)
{
	if (CheckAttribute(refCharacter,"Fellows.Passengers.navigator") && sCharacter == sti(refCharacter.Fellows.Passengers.navigator))
		return XI_ConvertString("navigator");
	if (CheckAttribute(refCharacter,"Fellows.Passengers.boatswain") && sCharacter == sti(refCharacter.Fellows.Passengers.boatswain))
		return XI_ConvertString("boatswain");
	if (CheckAttribute(refCharacter,"Fellows.Passengers.cannoner") && sCharacter == sti(refCharacter.Fellows.Passengers.cannoner))
		return XI_ConvertString("cannoner");
	if (CheckAttribute(refCharacter,"Fellows.Passengers.doctor") && sCharacter == sti(refCharacter.Fellows.Passengers.doctor))
		return XI_ConvertString("doctor");
	if (CheckAttribute(refCharacter,"Fellows.Passengers.treasurer") && sCharacter == sti(refCharacter.Fellows.Passengers.treasurer))
		return XI_ConvertString("treasurer");
	if (CheckAttribute(refCharacter,"Fellows.Passengers.carpenter") && sCharacter == sti(refCharacter.Fellows.Passengers.carpenter))
		return XI_ConvertString("carpenter");

	for(int i=1; i<MAX_NUM_FIGHTERS; i++)
	{
		if (sti(GetOfficersIndex(refCharacter, i)) == sti(sCharacter))
			return XI_ConvertString("fighter");
	}

	return XI_ConvertString("passengership");
}

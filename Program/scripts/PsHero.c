//Обработка ПГГ.
int PsHeroQty = 0;

#define PGG_TASK_NOTASK			"NoTask"
#define PGG_TASK_WAITINTAVERN	"WaitInTavern"
#define PGG_TASK_WORKONMAYOR	"WorkOnMayor"
#define PGG_TASK_SAILTOISLAND	"SailToIsland"
#define PGG_TASK_GOTOWAR		"GoToWar"
#define PGG_TASK_WORKCONVOY		"WorkConvoy"
#define PGG_TASK_WORKONSTORE	"WorkOnStore"
#define PGG_TASK_WORKWITHCONTRA	"WorkWithContra"
#define PGG_TASK_LOCKSERVICE	"LockService"

//уменьшаю кач до 30 процентов от СВМЛ... т.к. пока не ясно чего оно так быстро качается.
#define EXP_MODIFIER  0.25

// Инициализация ПГГ
void InitPsHeros()
{
    int  i, n, k, iStoreQ;
    ref  ch, ch1;
    bool ok;
    int  heroQty   = sti(GetNewMainCharacterParam("ps_hero_qty"));
    i = 1;iStoreQ = 0;
	string sname;
    PsHeroQty = 0; // глобальная переменная
	for (n=1; n<=heroQty; n++)
	{
		if(n != startHeroType)
		{
			PsHeroQty++;
			ch = GetCharacter(NPC_GenerateCharacter("PsHero_" + PsHeroQty, "off_hol_2", "man", "man", 5, PIRATE, -1, true));
			ch.PGGAi.HeroNum = n; // номер в файле
			ch.reputation = 2 + rand(86);
			ch.RebirthPhantom = true;  // не тереть фантома-многодневку (с -1), если умер
			setNewMainCharacter(ch, n);
			ch.Dialog.Filename = GetPGGDialog(ch);
		    ch.greeting = GetPggTavernGreeting(ch);
			//navy -->
			ch.SaveItemsForDead = true; //сохраняем все вещи
			ch.DontClearDead = true;  // не убирать труп через 200с
			ch.DontRansackCaptain = true; //квестовые не сдаются
			ch.AlwaysSandbankManeuver = true;  // тупым запрет тонуть об берег
			ch.SuperShooter  = true;
			ch.perks.list.ByWorker = "1";
			ch.perks.list.ShipEscape = "1";
			ch.loyality = 10 + rand(10); //лояльность
			if (sti(ch.reputation) > 41)
			{
				ch.alignment = "good";
			}
			else
			{
				ch.alignment = "bad";
			}
			ch.HoldEquip = true; //не отдавать оружие
			ch.PGGAi = true;    // признак, что ПГГ
			ch.PGGAi.IsPGG = true;  // в данный момент не офицер ГГ
			ch.PGGAi.location = "land";   // где суша-море
			ch.PGGAi.location.town = PGG_FindRandomTown(ch);
			//navy <--
			SetFantomParamFromRank_PPG(ch, 1 + rand(5), true); //WW  генерим статы TO_DO на отдельно специал и эмулятор скилов
			trace("PGG " + GetFullName(ch) + " starting rank " + sti(ch.rank));
			SetCharacterPerk(ch, "Energaiser"); // скрытый перк дает 1.5 к приросту энергии
			ch.Money = 8000 + rand(1000)*10; // чтоб быстрее корабли купили
			k = rand(13)+1;
			sname = "Totem_"+k;

			if (sname != "") GiveItem2Character(ch, sname);

			if (rand(4) < 3)
			{
				ch.Ship.Type = GenerateShipExt((SHIP_BERMSLOOP + rand(11)), 0, ch);
				SetBaseShipData(ch);
				SetRandomNameToShip(ch);

				SetCrewQuantity(ch, GetMinCrewQuantity(ch));
				ChangeCrewExp(ch, "Sailors", 50);
				ChangeCrewExp(ch, "Cannoners", 50);
				ChangeCrewExp(ch, "Soldiers", 50);
				SetCharacterGoods(ch, GOOD_SAILCLOTH, 20 + rand(50));
				SetCharacterGoods(ch, GOOD_PLANKS, 20 + rand(50));
				SetCharacterGoods(ch, GOOD_RUM, 50 + rand(100));
				ch.ship.HP = sti(ch.ship.HP) - makeint(sti(ch.ship.HP)*0.05) * makeint(MOD_SKILL_ENEMY_RATE/2);

			}
			else
			{
				ch.Money += ch.Money;
			}
			DeleteCloneHeros(ch);

			string sBlockPGG = "PGG" + ch.PGGAi.HeroNum;
			if (CheckAttribute(pchar,"RemovePGG." + sBlockPGG) && sti(pchar.RemovePGG.(sBlockPGG)) == 1)
			{//Убираем неугодных ПГГ с помощью галочек на старте
				ch.willDie = true;
				ch.DontCountDeath = true;
				LAi_KillCharacter(ch);
			}
	    }
	}
	//нормальное отношение всех ко всем.
	for (n = 1; n <= PsHeroQty; n++)
	{
		ch = CharacterFromID("PsHero_" + n);
		PGG_SetRelation2OtherAsNormal(ch);
	}
	trace("ПГГ загружено " + PsHeroQty);
	trace("Параметры апдейта ПГГ установлены.");
	SaveCurrentQuestDateParam("PGG_DailyUpdateReload");
	SaveCurrentQuestDateParam("PGG_DailyUpdateSeaReload");
	SaveCurrentQuestDateParam("PGG_DailyUpdateWorldMapReload");
	SaveCurrentQuestDateParam("QuestTalk");
	if(iStoreQ < 31)
	{
		SaveCurrentQuestDateParam("ChangeImport"+iStoreQ);
		iStoreQ++;
	}
	for(i=0; i<STORE_QUANTITY; i++)
    {
		SaveCurrentQuestDateParam("ChangeImport"+i);
	}
	SaveCurrentQuestDateParam("QuestOffer");//кд на приставания в тавернах
}
// boal 091004 много героев  <--

void DeleteCloneHeros(ref sld)
{
	if (startHeroType > 1 && startHeroType < 10)
	{
		if (startHeroType == 2)
		{
			if(sld.FaceId == 487 || sld.FaceId == 535 || sld.FaceId == 211 || sld.FaceId == 495)
			{//Его мы позже наймем оффом, так что убираем из ПГГ
				sld.willDie = true;
				sld.DontCountDeath = true;
				LAi_KillCharacter(sld);
			}
		}
		if (startHeroType == 4 || startHeroType == 5)
		{
			if(sld.FaceId == 1 || sld.FaceId == 522)
			{//Его мы позже наймем оффом, так что убираем из ПГГ
				sld.willDie = true;
				sld.DontCountDeath = true;
				LAi_KillCharacter(sld);
			}
		}
		if (startHeroType == 6 || startHeroType == 7)
		{
			if(sld.FaceId == 508 || sld.FaceId == 517)
			{//Его мы позже наймем оффом, так что убираем из ПГГ
				sld.willDie = true;
				sld.DontCountDeath = true;
				LAi_KillCharacter(sld);
			}
		}
	}
}

void PGG_DailyUpdateSeaReload()
{
	int i, j, iDays;

    if(!actLoadFlag)
    {
		iDays = GetQuestPastDayParam("PGG_DailyUpdateSeaReload");
		if (iDays < 1) return;

		for (i = 0; i < iDays; i++)
		{
			for (j = 1; j <= MakeInt(PsHeroQty/3 - 1); j++)
			{
				PGG_DailyUpdateEx(j);
			}
		}
		SaveCurrentQuestDateParam("PGG_DailyUpdateSeaReload");
	}
}
void PGG_DailyUpdateWorldMapReload()
{
	int i, j, iDays;

    if(!actLoadFlag)
    {
		iDays = GetQuestPastDayParam("PGG_DailyUpdateWorldMapReload");
		if (iDays < 1) return;

		for (i = 0; i < iDays; i++)
		{
			for (j = MakeInt(PsHeroQty/3) ; j <= MakeInt(PsHeroQty * 2 / 3 - 1); j++)
			{
				PGG_DailyUpdateEx(j);
			}
		}
		SaveCurrentQuestDateParam("PGG_DailyUpdateWorldMapReload");
	}
}
void PGG_DailyUpdateReload()
{
	int i, j, iDays;

    if(!actLoadFlag)
    {
		iDays = GetQuestPastDayParam("PGG_DailyUpdateReload");
		if (iDays < 1) return;

		for (i = 0; i < iDays; i++)
		{
			for (j = MakeInt(PsHeroQty * 2 / 3) ; j <= PsHeroQty; j++)
			{
				PGG_DailyUpdateEx(j);
			}
		}
		SaveCurrentQuestDateParam("PGG_DailyUpdateReload");

	}
}


void PGG_DailyUpdateEx(int i)
{
	string sTask;
	ref chr;
	int iDays;

//	i = GetEventData();

	chr = CharacterFromID("PsHero_" + i);

	if (CheckAttribute(chr,"PGGAi.DontUpdate"))
	{
		Log_TestInfo(GetFullName(chr)+ " пропускает апдейт.");
		chr.PGGAi.Task.SetSail = true;
		if(!CheckAttribute(chr,"LambiniAsoleda") && GetQuestPastDayParam("PGGAlmostDied" + i) >= 20)
		{
			DeleteAttribute(chr,"PGGAi.DontUpdate");
			DeleteAttribute(chr,"PGGAi.Task.SetSail");
		}
		return;
	}

	if (CheckAttribute(chr, "PGGAi.SeenToday"))
	{
		DeleteAttribute(chr, "PGGAi.SeenToday")
	}

	//лояльность компаньона
	if (IsCompanion(chr))
	{
		//раз в месяц проверка.
		if (GetNpcQuestPastDayParam(chr, "Companion.CheckRelation") > 30)
		{
			//тут вожможно стоит дописать ещё и общую лояльность и мораль матросов.
			if (PGG_ChangeRelation2MainCharacter(chr, 0) < 5 && sti(chr.Ship.Type) != SHIP_NOTUSED)
			{
				pchar.Quest.PGG_Companion_Leave.win_condition.l1 = "Location_Type";
				pchar.Quest.PGG_Companion_Leave.win_condition.l1.location_type = "tavern";
				pchar.Quest.PGG_Companion_Leave.function = "PGG_CompanionLeave";
				pchar.questTemp.PGG_Companion_Leave.index = chr.index;
			}
			SaveCurrentNpcQuestDateParam(chr, "Companion.CheckRelation");
		}
	}

	//помер, нефиг мертвых качать.
	//временно либо офицер, либо компаньон... не работаем с ним.
	if (LAi_IsDead(chr) || !sti(chr.PGGAi.IsPGG))
	{
		if (LAi_IsDead(chr))
		{
			if (CheckAttribute(chr, "PGGAi.Rebirth") && sti(chr.PGGAi.Rebirth))
			{
				Train_PPG(chr, true, true);
				LAi_SetCurHPMax(chr);
				DeleteAttribute(chr, "PGGAi.Rebirth");
				DeleteAttribute(chr, "Ship");
				chr.Ship.Type = SHIP_NOTUSED;
				if (chr.sex != "woman")	chr.Dialog.FileName = "PGG_dialog.c";
				else	chr.Dialog.FileName = "pgg_dialog_town.c";
				chr.Dialog.CurrentNode = "Second Time";

				chr.PGGAi.DontUpdate = true;
				chr.PGGAi.Task.SetSail = true;
				SaveCurrentQuestDateParam("PGGAlmostDied"+i);
			}
			else
			{
				PGG_CheckDead(chr);
			}
		}
/*
		i++;
		if (i <= PsHeroQty)
		{
			PostEvent("PGG_Update", 1000, "l", i);
		}
*/
		return;
	}

	//качаем ПГГ
	//оптимизация...
	if (rand(100) > 60) PGG_UpdateStats(chr, "Daily");
	//у нас есть задание
	if (CheckAttribute (chr, "PGGAi.Task") && chr.PGGAi.Task != PGG_TASK_NOTASK)
	{
		sTask = chr.PGGAi.Task;

		//лок, работу у мэра снимаем через сутки.
		if (sTask == PGG_TASK_LOCKSERVICE || sTask == PGG_TASK_WORKONMAYOR)
		{
			DeleteAttribute(chr, "PGGAi.LockService");
			PGG_SelectTask(chr);
		}
		//контру снимаем через 3 суток.
		if (sTask == PGG_TASK_WORKWITHCONTRA)
		{
			chr.PGGAi.Task.DaysCounter = sti(chr.PGGAi.Task.DaysCounter) + 1;
			if (sti(chr.PGGAi.Task.DaysCounter) > 3)
			{
				if (CheckAttribute(pchar, "questTemp.PGGContra.Know"))
				{
					RemoveSmugglersFromShore();
					pchar.Quest.Rand_Smuggling.Over = "yes";
				}
				DeleteAttribute(chr, "PGGAi.LockService");
				DeleteAttribute(chr, "PGGAi.Task.DaysCounter");
				PGG_SelectTask(chr);
			}
			else PGG_DebugLog(chr.id + " " + chr.PGGAi.Task + " " + chr.PGGAi.Task.DaysCounter);
		}
		//без денег и корабля...
		if (sTask == PGG_TASK_WAITINTAVERN)
		{
			//денег на кораблик нет.
			if (sti(chr.Money) < 16000)
			{
				//можем взять истребление банд, чтобы просто не бездельничать
				if (rand(100) < 60)
				{
					chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
					PGG_UpdateStats(chr, "Task");
				}
				else
				{
					PGG_DebugLog(chr.id + " stayed in tavern.");
				}
			}
			//есть деньги на корбаль... валим, кто не успел, тот опоздал
			else
			{
				PGG_UpdateStats(chr, "");
				PGG_SelectTask(chr);
			}
		}
		//морские задания...
		if (sTask == PGG_TASK_SAILTOISLAND || sTask == PGG_TASK_WORKCONVOY || sTask == PGG_TASK_WORKONSTORE)
		{
			if (CheckAttribute(chr, "PGGAi.Task.days") && sti(chr.PGGAi.Task.Target.days) <= GetNpcQuestPastDayParam(chr, "PGGAi.Task.days"))
			{
				PGG_Disband_Fleet(chr);
				//Log_TestInfo(GetHeroName(sti(chr.PGGAi.HeroNum))+" прибывает в" +
				//chr.PGGAi.Task.Target);
				Map_ReleaseQuestEncounter(chr.id);
				group_DeleteGroup(chr.id+"_Group");
				DeleteAttribute(chr, "PGGAi.ActiveQuest");
				chr.PGGAi.location = "land";
				chr.PGGAi.location.town = chr.PGGAi.Task.Target;
				//а это заполнение магазина товаром, привезенным ПГГ.
				AddRemoveGoodsInStore(chr.PGGAi.location.town, sti(chr.PGGAi.Task.Target.Goods), sti(chr.PGGAi.Task.Target.Goods.Qty), true);
				DeleteAttribute(chr, "PGGAi.Task");
				PGG_SelectTask(chr);
			}
			else
			{
				iDays = sti(chr.PGGAi.Task.Target.days);
				PGG_DebugLog(chr.id + " " + chr.PGGAi.Task + " Target: " +
				chr.PGGAi.Task.Target + " days: " + iDays);
			}
		}
	}
	//задания нет
	else
	{
		PGG_SelectTask(chr);
	}
	if (chr.PGGAi.location == "land" && sti(chr.Ship.Type) != SHIP_NOTUSED)
	{
		chr.location.from_sea = chr.PGGAi.location.town + "_town";
	}
	else
	{
		chr.location.from_sea = "";
	}

	if (CheckAttribute(chr, "nation_backup") && !CheckAttribute(chr, "PGG_warrior"))
	{
		chr.nation = chr.nation_backup;
		DeleteAttribute(chr, "nation_backup")
	}
/*
	i++
	if (i <= PsHeroQty)
	{
		PostEvent("PGG_Update", 500, "l", i);
	}
*/
}

//проверка на смерть ПГГ
void PGG_CheckDead(ref chr)
{
    //#20181013-02
    if(!CheckAttribute(chr, "PGGAi.location")) {
        if(bSeaActive)
            chr.PGGAi.location = "sea";
        else
            chr.PGGAi.location = "land";
    }
	if (chr.PGGAi.location == "Dead") return;
	if (IsCompanion(chr) && GetRemovable(chr) && CheckOfficersPerk(chr, "ShipEscape")) return;

	//убираем из локаций на выходе.
	PChar.questTemp.Chr2Remove = chr.id;
	if (!bSeaActive)
	{
		PChar.Quest.RemovePGG.win_condition.l1 = "ExitFromLocation";
		PChar.Quest.RemovePGG.win_condition.l1.Location = PChar.location;
	}
	else
	{
		PChar.Quest.RemovePGG.win_condition.l1 = "ExitFromSea";
	}
	PChar.Quest.RemovePGG.win_condition = "RemoveCharacterFromLocation";

	int pggStage = 0;

	if(!CheckAttribute(Pchar,"GenQuest.PGG_Quest.Stage"))
	{
		pggStage = -10;
	}
	else
	{
		pggStage = sti(Pchar.GenQuest.PGG_Quest.Stage);
	}

	if(pggStage > 3 || CheckAttribute(PChar,"PGG_Enemy") || CheckAttribute(PChar,"PGG_EnemyPP"))
	{
		if (!CheckAttribute(chr, "PGG_Hunter") && !CheckAttribute(chr, "AlwaysEnemy"))
		{
			if(MakeInt(chr.reputation) >= 70)
			{
				ChangeCharacterNationReputation(PChar, sti(chr.Nation), -15);
			}
			if(MakeInt(chr.reputation) <= 20)
			{
				ChangeCharacterNationReputation(PChar, sti(chr.Nation), -5);
			}
			if (MakeInt(chr.reputation) > 20 && MakeInt(chr.reputation) < 70)
			{
				ChangeCharacterNationReputation(PChar, sti(chr.Nation), -10);
			}
		}
		DeleteAttribute(PChar,"PGG_Enemy");
	}

	//ухудшение отношений с остальными ПГГ из этой нации
/*
	for (i = 1; i <= PsHeroQty; i++)
	{
		sld = CharacterFromID("PsHero_" + i);
		if(sld.nation == chr.nation && sld.id != chr.id)
		{
			PGG_ChangeRelation2MainCharacter(sld, -5);
		}
	}
*/
	PGG_Disband_Fleet(chr);

	if (!CheckAttribute(chr, "KilledTimes"))	chr.KilledTimes = 1;
	else	chr.KilledTimes = sti(chr.KilledTimes)+1;
	int respawnChance = 25 + sti(chr.KilledTimes) * MOD_SKILL_ENEMY_RATE;

	//выжил или нет
			if (sti(chr.PGGAi.IsPGG) && rand(100) > respawnChance && chr.id != "PsHero_2")
			{
				Log_TestInfo(chr.id + " уходит из игры :(");
			}
			else
			{
				if (IsOfficer(chr) || IsCompanion(chr))
				{
					Log_TestInfo(chr.id + " уходит из игры :(((");
				}
				else
				{
					if (CheckAttribute(chr, "willDie"))
					{
						Log_TestInfo(chr.id + " уходит из игры :(((");
					}
					else
					{
						chr.PGGAi.Rebirth = true;
						Log_TestInfo(chr.id + " выжил :)))");
						if (CheckAttribute(chr, "PGGAi.location.town.back"))
						{
							chr.PGGAi.location.town = chr.PGGAi.location.town.back;
						}
						if (CheckAttribute(chr, "nation_backup"))
						{
							chr.PGGAi.location.town = PGG_FindRandomTownByNation(sti(chr.nation_backup));
						}
						else
						{
							chr.PGGAi.location.town = PGG_FindRandomTown(chr);
						}
						PGG_ChangeRelation2MainCharacter(chr, -30);
						DeleteAttribute(chr, "PGGAi.Task");
						DeleteAttribute(chr, "PGGAi.LockService");
						DeleteAttribute(chr, "AlwaysFriend");
						DeleteAttribute(chr, "PGG_Hunter");
						DeleteAttribute(chr, "PGG_Convoy");
						chr.PGGAi.RemoveEncounter = true;
						wdmEmptyAllDeadQuestEncounter();
						DeleteAttribute(chr, "PGGAi.RemoveEncounter");
						DeleteAttribute(PChar, "GenQuest.PGG_Quest.PGG_Enemy");
						return;
					}
				}
			}

	if (!CheckAttribute(chr, "DontCountDeath"))
	{
		if (!CheckAttribute(pchar, "PGG_killed"))
		{
			pchar.PGG_killed = 1;
		}
		else
		{
			pchar.PGG_killed = sti(sti(pchar.PGG_killed)+1);
		}
		Log_TestInfo("Смертей ПГГ : "+ pchar.PGG_killed);
	}
	else
	{
		if (!CheckAttribute(pchar, "PGG_NotKilled"))
		{
			pchar.PGG_NotKilled = 1;
		}
		else
		{
			pchar.PGG_NotKilled = sti(sti(pchar.PGG_NotKilled)+1);
		}
	}
	chr.chr_ai.hp = 0.0;

	DeleteAttribute(chr, "PGGAi.Task");
	DeleteAttribute(chr, "PGGAi.LockService");

	chr.PGGAi.location = "Dead";
	chr.PGGAi.location.town = "None";
}
//==================================================
// Tasks
//==================================================
//выбрать задание для ПГГ
void PGG_SelectTask(ref chr)
{
	ref rShip;
	int iRnd = -1;
	int iShipType = sti(chr.Ship.Type);
	//если нет корабля, то сидим в таверне, бухаем...
	if (iShipType == SHIP_NOTUSED)
	{
		if (rand(100) < 30)
		{
			chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
		}
		else
		{
			chr.PGGAi.Task = PGG_TASK_WAITINTAVERN;
		}
	}
	else iRnd = rand(6);

	switch(iRnd)
	{
	case 0:
		chr.PGGAi.Task = PGG_TASK_SAILTOISLAND;
		break;
	case 1:
		if(rand(1) == 0)
		{
			chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
			PGG_LockService(chr, "Store");
		}
		else chr.PGGAi.Task = PGG_TASK_SAILTOISLAND;
		break;
	//залочить сервис
	case 2:
		chr.PGGAi.Task = PGG_TASK_LOCKSERVICE;
		PGG_LockService(chr, "");
		break;
	//конвой
	case 3:
		chr.PGGAi.Task = PGG_TASK_WORKCONVOY;
		break;
	//на магазин
	case 4:
		chr.PGGAi.Task = PGG_TASK_WORKONSTORE;
		break;
	//контра
	case 5:
		chr.PGGAi.Task = PGG_TASK_WORKWITHCONTRA;
		chr.PGGAi.Task.DaysCounter = 1;
		PGG_LockService(chr, "Smugglers");
		break;
	//напасть на колонию
	case 6:
		chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
		PGG_LockService(chr, "Store");
		break;
	}

	PGG_SetUpForTask(chr);
	PGG_UpdateStats(chr, "Task");

	//15 процентов иначе офигеем от обилия слухов :)
	if (rand(100) < 15)
	{
		PGG_AddRumour(chr, chr.PGGAi.Task);
	}
}

//вынес отдельным методом, чтобы можно было из разных мест вызывать, чаще будет.
//если передавать пустую строку, то будет рандомно выбираться...
void PGG_LockService(ref chr, string _service)
{
	if (_service != "")
	{
		chr.PGGAi.LockService = _service;
		return;
	}

	if (rand(1) == 1)
	{
		chr.PGGAi.LockService = "Shipyard";
	}
	else
	{
		chr.PGGAi.LockService = "Store";
	}
}

//Настроить параметры ПГГ под задание (корабль, команду, трюм и тд)
void PGG_SetUpForTask(ref chr)
{
	int iTradeGoods, iQuantityGoods, iMoney;
	string sTask = chr.PGGAi.Task;

	if (sTask == PGG_TASK_WORKONSTORE || sTask == PGG_TASK_SAILTOISLAND || sTask == PGG_TASK_WORKCONVOY)
	{
		if (!CheckAttribute(pchar, "PGGsAtSea"))
		{
			pchar.PGGsAtSea = 0;
		}
		if (CheckAttribute(chr, "PGGAi.Task.SetSail"))
		{
			DeleteAttribute(chr, "PGGAi.Task.SetSail");
			DeleteAttribute(chr, "PGG_Hunter");
			DeleteAttribute(chr, "PGG_Convoy");
		}
		if (!CheckAttribute(pchar, "LastPgg"))
		{
			pchar.LastPgg = "";
		}
		if (sti(pchar.PGGsAtSea) > 15 || pchar.LastPgg == chr.id || CheckAttribute(chr, "PGGAi.AfterSail.TaskCounter"))
		{//Не пускаем в море слишком много ПГГ
			DeleteAttribute(pchar, "LastPgg");

			int iRnd = rand(3);
			switch(iRnd)
			{//Выбираем сухопутный таск
			case 0:
				chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "Store");
				break;
			//залочить сервис
			case 1:
				chr.PGGAi.Task = PGG_TASK_LOCKSERVICE;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "");
				break;
			//контра
			case 2:
				chr.PGGAi.Task = PGG_TASK_WORKWITHCONTRA;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "Smugglers");
				break;
			//напасть на колонию
			case 3:
				chr.PGGAi.Task = PGG_TASK_WORKONMAYOR;
				chr.PGGAi.Task.DaysCounter = 2;
				PGG_LockService(chr, "Store");
				break;
			}

			if (CheckAttribute(chr, "PGGAi.AfterSail.TaskCounter"))
			{
				int aftersailTasks = sti(chr.PGGAi.AfterSail.TaskCounter) + 1
				chr.PGGAi.AfterSail.TaskCounter = aftersailTasks;

				if (sti(chr.PGGAi.AfterSail.TaskCounter) >= 5)
				{
					DeleteAttribute(chr, "PGGAi.AfterSail.TaskCounter");
				}
			}
			//PGG_SelectTask(chr);
		}
		else
		{
			PGG_UpdateShipEquip(chr);
			//убираем из таверны, уплыл нафиг :)
			PGG_PlaceCharacter2Tavern(chr, false);

			iTradeGoods = rand(GOOD_SILVER);
			RecalculateSquadronCargoLoad(chr);
			iQuantityGoods = GetSquadronFreeSpace(chr, iTradeGoods);
			iQuantityGoods = iQuantityGoods - rand(makeint(iQuantityGoods/3)) - 10;
			// 1.2.3 Это лишние миллионы в ПГГ
			//iMoney = makeint((iQuantityGoods * sti(Goods[iTradeGoods].Weight) / sti(Goods[iTradeGoods].Units)) * (4+rand(3) + GetSummonSkillFromNameToOld(chr, SKILL_COMMERCE)) + 0.5);
			iMoney = makeint((iQuantityGoods / sti(Goods[iTradeGoods].Units)) * sti(Goods[iTradeGoods].Cost) * GetSummonSkillFromNameToOld(chr, SKILL_COMMERCE)/8);

			if (iQuantityGoods < 0) iQuantityGoods = 0;
			AddCharacterGoods(chr, iTradeGoods, iQuantityGoods);
			PGG_AddMoneyToCharacter(chr, iMoney);

			//а это такая засада, ;) убираем из магазина товары. -->
			AddRemoveGoodsInStore(chr.PGGAi.location.town, iTradeGoods, iQuantityGoods, false);
			//<--
			PGG_FindTargetTown(chr);
			chr.PGGAi.Task.Target.Goods = iTradeGoods;
			chr.PGGAi.Task.Target.Goods.Qty = iQuantityGoods;

			chr.PGGAi.Encountername = chr.id;
			Map_ReleaseQuestEncounter(chr.id);
			string  sGroup = "Sea_" + chr.id;
			group_DeleteGroup(sGroup);
			Group_FindOrCreateGroup(sGroup);
			Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
			Group_LockTask(sGroup);
			Group_AddCharacter(sGroup, chr.id);
			Group_SetGroupCommander(sGroup, chr.id);

			// chr.mapEnc.worldMapShip = "PGGShipMan";
			if (chr.sex != "woman")
			{
				chr.mapEnc.worldMapShip = "PGGShipMan";
			}
			else
			{
				chr.mapEnc.worldMapShip = "PGGShipWoman";
			}
			chr.mapEnc.Name = GetHeroName(sti(chr.PGGAi.HeroNum));
			string sColony = chr.PGGAi.Task.Target;
			int daysQty = ((GetMaxDaysFromIsland2Island(GetArealByCityName(sColony), GetArealByCityName(chr.PGGAi.location.town)))/3)+1;
			int iGoods;
			int iSpace;

			if (PGG_ChangeRelation2MainCharacter(chr, 0) < 51 && sti(chr.reputation) < 15 && rand(100) == 5 && sti(pchar.money) >= sti(chr.rank)*10000+100000 && GetCharacterShipClass(chr) <= GetCharacterShipClass(PChar))
			{//Любой пгг с определенным шансом будет охотиться за нами
				chr.AlwaysEnemy = true;
				chr.PGG_Hunter = true;
				chr.DontRansackCaptain = true;
				chr.mapEnc.type = "war";
				chr.hunter = "pirate";
				daysQty = 15;
				chr.PGG_warrior = true;
				chr.nation_backup = chr.nation;
				chr.nation = PIRATE;
				DeleteAttribute(chr, "Abordage.Enable");
				DeleteAttribute(chr, "AlwaysFriend");
			}
			else
			{
				if (PGG_ChangeRelation2MainCharacter(chr, 0) < 41 && GetCharacterShipClass(chr) <= GetCharacterShipClass(PChar))
				{//если у нас плохие отношения с ПГГ, он будет нас преследовать
					chr.AlwaysEnemy = true;
					chr.PGG_Hunter = true;
					chr.DontRansackCaptain = true;
					chr.mapEnc.type = "war";
					chr.hunter = "pirate";
					daysQty = 15;
					chr.PGG_warrior = true;
					chr.nation_backup = chr.nation;
					chr.nation = PIRATE;
					DeleteAttribute(chr, "Abordage.Enable");
					DeleteAttribute(chr, "AlwaysFriend");
				}
				else
				{
					chr.mapEnc.type = "trade";
					//chr.AlwaysFriend = true;
					SetCharacterRelationBoth(sti(PChar.index), sti(chr.index), RELATION_FRIEND);
					Map_CreateTrader(chr.PGGAi.location.town, sColony, chr.id, daysQty);
					DeleteAttribute(chr, "Abordage.Enable");
					if (chr.PGGAi.Task == PGG_TASK_WORKCONVOY && sti(chr.nation) != PIRATE && !CheckAttribute(chr, "PGG_warrior"))
					{
						sld = GetCharacter(NPC_GenerateCharacter(chr.id+"_Trader", "", "man", "man", sti(chr.rank), sti(chr.nation), daysQty, true));
						CreatePGG_Trade(sld, sti(chr.nation), chr);
						SetRandomNameToShip(sld);
						SetBaseShipData(sld);
						SetCrewQuantityFull(sld);
						Fantom_SetCannons(sld, "trade");
						Fantom_SetBalls(sld, "trade");
						SetSeaFantomParam(sld, "trade");
						SetCaptanModelByEncType(sld, "trade");
						sld.PGG_companion = true;//Чтобы не было диалога
						ResetShipCannonsDamages(sld);
						SetBaseShipData(sld);
						SetCrewQuantityFull(sld);
						Fantom_SetBalls(sld, "pirate");
						SetBaseShipData(sld);
						//SetCompanionIndex(chr, -1, sti(sld.index));
						DeleteAttribute(chr, "AlwaysFriend");
						SetCharacterRelationBoth(sti(PChar.index), sti(sld.index), RELATION_FRIEND);
						//Положим товар
						iGoods = GOOD_SLAVES + rand(2);
						iSpace = GetCharacterFreeSpace(sld, iGoods);
						iSpace = MakeInt(iSpace/2 + rand(iSpace/2));
						Fantom_SetCharacterGoods(sld, iGoods, iSpace, 1);
						Group_AddCharacter(sGroup, sld.id);
						Log_TestInfo(GetHeroName(sti(chr.PGGAi.HeroNum))+" сопровождает торговца.");
						chr.mapEnc.Name = "Корабль торговцев, который сопровождает " + GetHeroName(sti(chr.PGGAi.HeroNum));
						chr.PGG_Convoy = true;
						chr.PGG_trader = true;
					}
					if (PGG_ChangeRelation2MainCharacter(chr, 0) < 41)
					{
						DeleteAttribute(chr, "AlwaysFriend");
						DeleteAttribute(chr, "Abordage.Enable");
						chr.PGG_Hunter = true;
						chr.PGG_Convoy = true;
						chr.PGG_Grudge = true;
					}
					if (CheckAttribute(pchar, "PGG_killed") && sti(pchar.PGG_killed) > 4 && rand(100) > 50)
					{
						chr.PGG_Hunter = true;
					}
				}
			}
			if (CheckAttribute(chr, "PGG_Hunter"))
			{
				int pgg_hunters = sti(chr.rank)/15 + 1;
				int pgg_hunters_bonus = 1;
				if (MOD_SKILL_ENEMY_RATE == 10)
				{
					pgg_hunters_bonus = 2;
				}
				if (MOD_SKILL_ENEMY_RATE < 4)
				{
					pgg_hunters_bonus = 0;
				}
				chr.mapEnc.Name = chr.mapEnc.Name + " с эскадрой";
				int pgg_hunters_total = pgg_hunters+pgg_hunters_bonus;
				if (!CheckAttribute(pchar, "PGG_grudge") && CheckAttribute(pchar, "PGG_killed") && sti(pchar.PGG_killed) > 4)
				{
					pgg_hunters_total = rand(pgg_hunters_total/2);
				}
				for (int i = 1; i <= pgg_hunters_total; i++)
				{
					sld = GetCharacter(NPC_GenerateCharacter(chr.id+"_hunter_"+i, "", "man", "man", sti(chr.rank), sti(chr.nation), daysQty, true));
					CreatePGG_War(sld, sti(chr.nation), chr);
					SetRandomNameToShip(sld);
					SetBaseShipData(sld);
					SetCrewQuantityFull(sld);
					Fantom_SetCannons(sld, "war"); //fix
					Fantom_SetBalls(sld, "pirate");
					Fantom_SetGoods(sld, "war");
					sld.PGG_companion = true;//Чтобы не было диалога
					SetRandomNameToShip(sld);
					SetFantomParamHunter(sld); //крутые парни
					SetCaptanModelByEncType(sld, "war");//
					SetCharacterRelationBoth(sti(PChar.index), sti(sld.index), RELATION_FRIEND);
					//Положим товар
					iGoods = GOOD_SLAVES + rand(2);
					iSpace = GetCharacterFreeSpace(sld, iGoods);
					iSpace = rand(MakeInt(iSpace/10));
					Fantom_SetCharacterGoods(sld, iGoods, iSpace, 1);

					//SetCompanionIndex(chr, -1, sti(sld.index));
					Group_AddCharacter(sGroup, sld.id);

					if(!CheckAttribute(chr, "PGG_Convoy"))
					{
						sld.AlwaysEnemy = true;
					}
				}
			}
			chr.PGGAi.Task.Target.days = daysQty;
			chr.PGGAi.Task.days = daysQty;
			chr.PGGAi.ActiveQuest = true;
			chr.DeckDialogNode = "Second time";
			if (CheckAttribute(chr, "PGG_warrior"))
			{
				Map_CreateWarrior("", chr.id, daysQty);
				Log_TestInfo(GetHeroName(sti(chr.PGGAi.HeroNum))+" выходит на охоту за ГГ!");
				chr.PGG_Grudge = true;
			}
			else
			{
				DeleteAttribute(chr, "PGG_warrior");
				Map_CreateTrader(chr.PGGAi.location.town, sColony, chr.id, daysQty);
				//Log_TestInfo(GetHeroName(sti(chr.PGGAi.HeroNum))+" отправляется в " +
				//chr.PGGAi.Task.Target + " из "+chr.PGGAi.location.town+", доплывет за " + sti(chr.PGGAi.Task.Target.days) + " дней");
			}
			Fantom_SetBalls(chr, "pirate");
			chr.PGGAi.Task.SetSail =  true;

			int PGGsAtSeaAdded = sti(pchar.PGGsAtSea) + 1;
			pchar.PGGsAtSea = PGGsAtSeaAdded;
			chr.PGGAi.ActiveQuest =  true;
			chr.PGGAi.location = "sea";
			chr.PGGAi.location.town = "";
			SaveCurrentNpcQuestDateParam(chr, "PGGAi.Task.days");

		}
	}
}

//Blackthorn - роспуск эскадры и удаление энкаунтера ПГГ
void PGG_Disband_Fleet(ref chr)
{
	if (CheckAttribute(chr, "PGGAi.Task.SetSail"))
	{
		chr.PGGAi.RemoveEncounter = true;
		wdmEmptyAllDeadQuestEncounter();
		DeleteAttribute(chr, "PGGAi.RemoveEncounter");
		DeleteAttribute(chr, "Abordage.Enable");
		DeleteAttribute(chr, "AlwaysFriend");
		DeleteAttribute(chr, "PGGAi.Task.SetSail");
		int PGGsAtSearemoved = sti(pchar.PGGsAtSea) - 1;
		pchar.PGGsAtSea = PGGsAtSearemoved;
		chr.PGGAi.AfterSail.TaskCounter = 0;
		pchar.LastPgg = chr.id;
		DeleteAttribute(chr, "PGG_Hunter");
		DeleteAttribute(chr, "PGG_Convoy");
	}
}

//качалка для ПГГ
void PGG_UpdateStats(ref chr, string sExpType)
{
	float fMod;
	int iMoney = 0;
	int i, iRnd;
	string sHeroType = chr.HeroParam.HeroType;

	if (!CheckAttribute(chr, "PGGAi.OldRank")) chr.PGGAi.OldRank = chr.Rank;
	if (sExpType == "Daily")
	{
		if (chr.PGGAi.location == "land")
		{
			//время кача в часах

			fMod = MOD_SKILL_ENEMY_RATE + rand(MOD_SKILL_ENEMY_RATE + 12);
			fMod = fMod*EXP_MODIFIER;

			AddCharacterExpToSkill(chr, GetEquipedBladeType(chr), fMod*261);
			AddCharacterExpToSkill(chr, SKILL_PISTOL, fMod*34.444);
			AddCharacterExpToSkill(chr, SKILL_FORTUNE, fMod*11.369);
			AddCharacterExpToSkill(chr, SKILL_LEADERSHIP, fMod*11.759);
			AddCharacterExpToSkill(chr, SKILL_DEFENCE, fMod*11.5);
			//время кача в часах
			fMod = MOD_SKILL_ENEMY_RATE + rand(12);
			fMod = fMod*EXP_MODIFIER;
			AddCharacterExpToSkill(chr, SKILL_COMMERCE, fMod*25.2);
			if(GetCharacterShipClass(chr) > GetCharacterShipClass(PChar) || rand(9) == 0)
			{
				PGG_AddMoneyToCharacter(chr, PGG_AddShipsBattleExp(chr, 1));
			}
		}
		else
		{
			//20 процентов шанс на бой в море за день... для торговца.
			iRnd = 20;
			//для остальных чаще, т.к. основной доход.
			if (sHeroType != "Merchant")
			{
				iRnd += 15;
			}

			if (GetCharacterShipClass(chr) > GetCharacterShipClass(PChar) || rand(100) < iRnd)
			{
				//даю меньше от 1 до 6 кораблей...
				PGG_AddMoneyToCharacter(chr, PGG_AddShipsBattleExp(chr, 1 + rand(makeint(MOD_SKILL_ENEMY_RATE*1.5))));
			}
		}
	}
	if (sExpType == "Task")
	{
		switch (chr.PGGAi.Task)
		{
		case PGG_TASK_WORKONMAYOR:
            AddCharacterExpToSkill(chr, SKILL_LEADERSHIP, 20 + MOD_SKILL_ENEMY_RATE + rand(MOD_SKILL_ENEMY_RATE + 40));
		    AddCharacterExpToSkill(chr, SKILL_COMMERCE, 3 + MOD_SKILL_ENEMY_RATE);
			iMoney = Makeint(sti(chr.rank)*200 + (GetSummonSkillFromNameToOld(chr, SKILL_LEADERSHIP)*30 + 700 + rand(20)*100));
			// + Кач за боевки
			iRnd = rand(100);
			//убить пирата
			if (sti(chr.Ship.Type) != SHIP_NOTUSED && iRnd > 67)
			{
				iMoney = PGG_AddShipsBattleExp(chr, 1);
			}
			//банды, контра
			else
			{
				//TODO: статистика? а нужна она вообще?
			}
			PGG_AddMoneyToCharacter(chr, iMoney);
			break;
		}
		PGG_DebugLog(chr.id + " " + chr.PGGAi.Task + " money: " + iMoney);
	}

	PGG_UpdatePerks(chr);
	PGG_UpdateEquip(chr);
	PGG_UpdateShipEquip(chr);
	//оптимизация.
	if (chr.PGGAi.OldRank != chr.Rank && rand(100) < 40)
	{
		chr.PGGAi.OldRank = chr.Rank;
		DelBakSkillAttr(chr);
		ClearCharacterExpRate(chr);
		RefreshCharacterSkillExpRate(chr);
	}
}

//кач персонажа за бой с кораблями. Возвращает стоимость добычи.
int PGG_AddShipsBattleExp(ref chr, int _shipsNum)
{
	string sDebugStr;
	ref rShip;
	int i, iMoney;
	float fTmp = 0.0;
	float fMod = MOD_SKILL_ENEMY_RATE * EXP_MODIFIER * _shipsNum;

	AddCharacterExpToSkill(chr, SKILL_ACCURACY, fMod*29.4 + FRAND(100.0));
	AddCharacterExpToSkill(chr, SKILL_SAILING, fMod*14.0);
	AddCharacterExpToSkill(chr, SKILL_CANNONS, makefloat((12+rand(80))*12.5));

	sDebugStr = chr.id + " fight with " + _shipsNum + " ships.";
	iMoney = 0;
	//абордаж
	if (rand(100) < 30 && sti(chr.Ship.Type) != SHIP_NOTUSED)
	{
		//взял себе
		rShip = GetRealShip(sti(chr.Ship.Type));
		if (rand(2) == 1)
		{
			fTmp = 1.0;
			//примерная цена за корабль
			iMoney = sti(rShip.Price)*0.2 + rand(sti(sti(rShip.Price)*0.5));
			sDebugStr += " Took them.";
		}
		//утопил
		else
		{
			AddCharacterExpToSkill(chr, SKILL_ACCURACY, fMod * 40);
			AddCharacterExpToSkill(chr, SKILL_SAILING, fMod * 140);
			AddCharacterExpToSkill(chr, SKILL_CANNONS, fMod * 40);
			AddCharacterExpToSkill(chr, SKILL_SAILING, fMod * 200);
			sDebugStr += " Destroyed them.";
		}

		i = sti(chr.ship.crew.quantity);
		AddCharacterExpToSkill(chr, GetEquipedBladeType(chr), fMod*FRAND(233.4));
		AddCharacterExpToSkill(chr, SKILL_PISTOL, fMod*3.48);
		AddCharacterExpToSkill(chr, SKILL_FORTUNE, fMod*(2.4 + fTmp*20));
		AddCharacterExpToSkill(chr, SKILL_LEADERSHIP, fMod*(2.4 + fTmp*25));
		AddCharacterExpToSkill(chr, SKILL_DEFENCE, fMod*(1.0 + fTmp*40));
		AddCharacterExpToSkill(chr, SKILL_COMMERCE, fMod * 50 * fTmp);
		AddCharacterExpToSkill(chr, SKILL_REPAIR, fMod * 60 * fTmp);

		AddCharacterExpToSkill(chr, SKILL_GRAPPLING, ((1+i-rand(i))/3+0.5+110) + (fMod * 120 * fTmp));
		AddCharacterExpToSkill(chr, SKILL_DEFENCE, (1+i-rand(i))/3+0.5+60);
		AddCharacterExpToSkill(chr, SKILL_SAILING, ((1+i-rand(i))/3+0.5+110) + (fMod * 120 * fTmp));
		AddCharacterExpToSkill(chr, SKILL_FORTUNE, (1+i-rand(i))/3+0.5+60);

		//TODO: Что там по деньгам в каюте? Сколько вешать в граммах? :)
		iMoney += 1000 + rand(2000);
		//примерная стоимость товара.
		i = 1;
		//TODO: большой куш!!!! сколько макс можно взять?
		if (rand(100) < 5)
		{
			PGG_DebugLog("PGG BIG LUCK");
			i = 5;
		}
		iMoney += rand(100)*10*i;
	}
	PGG_DebugLog(sDebugStr);
	return iMoney * _shipsNum * 4;
}

//прокачка перков на полученные очки
void PGG_UpdatePerks(ref chr)
{
	//реже будет.... а то часто очень.
	if (!CheckAttribute(chr, "perks.freepoints_self")) chr.perks.freepoints_self = 0;
	if (!CheckAttribute(chr, "perks.freepoints_ship")) chr.perks.freepoints_ship = 0;

	if (sti(chr.perks.freepoints_self) > 0 || sti(chr.perks.freepoints_ship) > 0)
	{
		chr.perks.freepoints_self = 0;
		chr.perks.freepoints_ship = 0;
		LAi_NPC_EquipPerk(chr, "fantom");
		SetCharacterPerk(chr, "Energaiser"); // скрытый перк дает 1.5 к приросту энергии
	}
}

//==================================================
// Equip
//==================================================
//экипируемся, берем другие корабли и все такое...
void PGG_UpdateEquip(ref chr)
{
	ref rShip;
	int aShips[150];
	int i, n, iClass, iType;
	bool bOk, bShipNotUsed;
	string blade;
	int prevClass = 7;

//--------------------------------------------------
// Ship Section
//--------------------------------------------------
	iType = sti(chr.Ship.Type);
	bShipNotUsed = (iType == SHIP_NOTUSED);

	if (!bShipNotUsed)
	{
        rShip = GetRealShip(iType);
		prevClass = sti(rShip.Class);
        //Boyer fix #20170410-01 for in case of missed name
        if (!CheckAttribute(chr, "Ship.Name")) SetRandomNameToShip(chr);
	}
	iClass = PGG_GetAvailableShipClass(chr);

	bOk = !bShipNotUsed && CheckAttribute(rShip, "Class") && (sti(rShip.Class) >= iClass);
	if (bShipNotUsed || bOk)
	{
		n = 0;
		for (i = 0; i <= SHIP_SP_SANFELIPE; i++)
		{
			if (CheckAttribute(&ShipsTypes[i], "Class") && sti(ShipsTypes[i].Class) >= iClass && prevClass > sti(ShipsTypes[i].Class))
			{
				// Log_Info(ShipsTypes[i].name);
				aShips[n] = i;
				n++;
			}
		}

		if (n != 0)
		{
			bOk = false;
			//если корабль уже есть, то не всегда есть возможность его сменить, поэтому 5-50 процентов
			if (!bShipNotUsed)
			{
				if (MOD_SKILL_ENEMY_RATE * sti(chr.rank) > rand(10*MOD_SKILL_ENEMY_RATE)) bOk = true;
			}
			//а нет, так всегда купит, нужда.
			else
			{
				bOk = true;
			}
			if (bOk)
			{
				i = rand(n-1);
				iType = aShips[i];
				i = sti(ShipsTypes[iType].Price) - sti(chr.skill.Commerce) * sti(ShipsTypes[iType].Price) / 500;

				if (sti(chr.Money) > i)
				{
					PGG_DebugLog(chr.id + " Changed ship");
					chr.Ship.Type = GenerateShipExt(iType, rand(1), chr);
					SetBaseShipData(chr);
					if (!CheckAttribute(chr, "Ship.Name")) SetRandomNameToShip(chr);
					PGG_UpdateShipEquip(chr);

					PGG_AddMoneyToCharacter(chr, -i);
					chr.PGGAi.OwnShip = true;
				}
			}
		}
	}
	Fantom_SetBalls(chr, "pirate");
//--------------------------------------------------
// Blade & Gun Section
//--------------------------------------------------
	if (rand(100) < 5)
	{
		if(!CheckAttribute(chr, "DontChangeGun"))	DeleteAttribute(chr, "equip");
		//трем сабли.
		blade = FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE);
		while(blade != "")
		{
			TakeItemFromCharacter(chr, blade);
			blade = FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE);
		}

		blade = LAi_NPC_EquipBladeSelection(sti(chr.rank),false);
		GiveItem2Character(chr, blade);
		EquipCharacterByItem(chr, blade);

		if(rand(1000) < MOD_SKILL_ENEMY_RATE*100)
		{
			TakeNItems(chr, "potion1", rand(makeint(sti(chr.rank)/4 + 0.5))); // даю меньше
			if(GetCharacterItem(chr, "potion1") > 30)	chr.Items.potion1 = 30;
			TakeNItems(chr, "potion2", rand(makeint(sti(chr.rank)/5 + 0.5))); // даю меньше
			if(GetCharacterItem(chr, "potion2") > 10)	chr.Items.potion2 = 10;
		}

		if(rand(1000) < MOD_SKILL_ENEMY_RATE * sti(chr.rank) * 8)
		{
			//трем пистолеты.
			blade = FindCharacterItemByGroup(chr, GUN_ITEM_TYPE);
			while(blade != "")
			{
				TakeItemFromCharacter(chr, blade);
				blade = FindCharacterItemByGroup(chr, GUN_ITEM_TYPE);
			}

			if(rand(100) < 50)
			{
				GiveItem2Character(chr, "pistol1");
				EquipCharacterByItem(chr, "pistol1");
			}
			else
			{
				if(rand(100) < 50)
				{
					GiveItem2Character(chr, "pistol2");
					EquipCharacterByItem(chr, "pistol2");
				}
				else
				{
					if(rand(100) < 50)
					{
						GiveItem2Character(chr, "pistol5");
						EquipCharacterByItem(chr, "pistol5");
					}
					else
					{
						if(rand(100) < 70)
    					{
    						GiveItem2Character(chr, "pistol3");
    						EquipCharacterByItem(chr, "pistol3");
    					}
						else
						{
							if(rand(100) < 80)
        					{
        						GiveItem2Character(chr, "pistol6");
        						EquipCharacterByItem(chr, "pistol6");
        					}
							else
							{
								if (sti(chr.rank) > 9)
								{
									GiveItem2Character(chr, "pistol8");
            						EquipCharacterByItem(chr, "pistol8");
        						}
        					}
    					}
					}
				}
			}
			if (GetCharacterItem(chr, "bullet") < 10)
			{
				TakeNItems(chr, "bullet", 5+rand(15));
			}
			if(GetCharacterItem(chr, "GunPowder") < 10)
			{
				AddItems(chr, "GunPowder", 5+rand(15));
			}
		}
	}
}


//получить тип экипированной сабли.
string GetEquipedBladeType(ref chr)
{
	string sBlade = GetCharacterEquipByGroup(chr, BLADE_ITEM_TYPE);
	if (sBlade != "")
	{
		ref rItm = ItemsFromID(sBlade);
		return rItm.FencingType;
	}
	return SKILL_FENCING;
}

//настроить параметры корабля
void PGG_UpdateShipEquip(ref chr)
{
	if (sti(chr.Ship.Type) == SHIP_NOTUSED) return;

	ref refTown, chref;
	string curTown;

	ProcessHullRepair(chr, 100.0);
	ProcessSailRepair(chr, 100.0);
	SetCrewQuantityFull(chr);

	if (chr.HeroParam.HeroType == "Merchant")
	{
		Fantom_SetCannons(chr, "trade");
		Fantom_SetBalls(chr, "trade");
		Fantom_SetGoods(chr, "trade");
	}
	else
	{
		Fantom_SetCannons(chr, "pirate");
		Fantom_SetBalls(chr, "pirate");
		Fantom_SetGoods(chr, "pirate");
	}

	curTown = chr.PGGAi.location.town;
	if (curTown != "")
	{
		if (rand(5) == 2)   // нулим матросов в городе
		{
			PGG_DebugLog("PGG нулим матросов в городе " + curTown);
			refTown = GetColonyByIndex(FindColony(curTown));
			//SetCrewQuantity(refTown, 0);
			chref = CharacterFromID(curTown + "_tavernkeeper");
			SaveCurrentNpcQuestDateParam(chref, "CrewHired");
			chref.CrewHired.PGGId = chr.id;
		}
		AddRemoveGoodsInStore(curTown, GOOD_BALLS, makeint(195*1.4 + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_BOMBS, makeint(180*1.4 + rand(40)), false);
		AddRemoveGoodsInStore(curTown, GOOD_KNIPPELS, makeint(95*1.4 + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_GRAPES, makeint(75*1.4 + rand(20)), false);

		AddRemoveGoodsInStore(curTown, GOOD_POWDER, makeint(350*1.4 + rand(60)), false);
		AddRemoveGoodsInStore(curTown, GOOD_SAILCLOTH, makeint(5*1.4 + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_PLANKS, makeint(10*1.4 + rand(40)), false);

		AddRemoveGoodsInStore(curTown, GOOD_FOOD, makeint(8*1.4 + rand(50)), false);
		AddRemoveGoodsInStore(curTown, GOOD_WEAPON, makeint(6*1.4 + rand(25)), false);
		AddRemoveGoodsInStore(curTown, GOOD_RUM, makeint(4*1.4 + rand(20)), false);
		AddRemoveGoodsInStore(curTown, GOOD_MEDICAMENT, makeint(4*1.4 + rand(25)), false);
	}
}

//добавить или удалить товары в магазине города.
void AddRemoveGoodsInStore(string _town, int _tradeGoods, int _quantityGoods, bool toAdd)
{
	int iTest;
	ref rStore;
	string sGoodName;

	iTest = FindColony(_town);
	if (iTest != -1)
	{
		rStore = GetColonyByIndex(iTest);
		iTest = sti(rStore.StoreNum);
		makeref(rStore, stores[iTest]);

		sGoodName = Goods[_tradeGoods].Name;
		iTest = sti(rStore.Goods.(sGoodName).Quantity);
		if (toAdd)
		{
			iTest += _quantityGoods;
		}
		else
		{
			iTest -= _quantityGoods;
		}
		if (iTest < 0) iTest = 0;
		rStore.Goods.(sGoodName).Quantity = iTest;
		//TODO: сделать пересчет цены на товар, т.к. его либо мало, либо много.
	}
}

//==================================================
// Locations
//==================================================
//Mett: найти город назначения для персонажа, дописал под новый расчет дистанции между колониями
string PGG_FindTargetTown(ref chr)
{
    string sCurTown = chr.PGGAi.location.town;

    int i = 1;
    string sTargetIsland = GetRandomIslandId();    //получаем рандомный остров
    while (GetCityNameByIsland(sTargetIsland) == "None" || GetCityNameByIsland(sTargetIsland) == "Caiman" || GetCityNameByIsland(sTargetIsland) == "LostShipsCity") //ищем есть ли на острове колония
    {
        i++;
        sTargetIsland = GetRandomIslandId();    //ищем повторно
    }
    string sTargetTown = FindTownOnIsland(sTargetIsland); //присваеваем колонию, в которую направляемся

    chr.PGGAi.Task.Target = sTargetTown;
    chr.PGGAi.Task.Target.days = CalculateColonyDistance(sCurTown, sTargetTown) + rand(2);    //новый расчет дней на дорогу

    Log_TestInfo("PGG_FindTargetTown: для " + chr.id + " выбрана колония " + sTargetTown + ", количество попыток " + i);

    return sTargetTown;
}

//получить ID случайного города по нации. Будет для раскидывания ПГГ в начале игры по миру.
string PGG_FindRandomTownByNation(int _nation)
{
	int i, n;
	string sTowns[MAX_COLONIES];
	n = 0;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		if (sti(Colonies[i].nation) == _nation && Colonies[i].from_sea != "" && Colonies[i].id != "LostShipsCity" && Colonies[i].id != "Caiman") //в сухопутных не будут. //18.06.07 fix ГПК
		{
			sTowns[n] = Colonies[i].id;
			n++;
		}
	}

	if (n == 0)
	{
		trace("ОШИБКА: <PsHero.c>: Не могу найти ни одного города для нации: " + _nation + " Использовать по умолчанию.");
		return "Bridgetown";
	}
	return sTowns[rand(n-1)];
}

//получить ID случайного города по нации. Будет для раскидывания ПГГ в начале игры по миру.
string PGG_FindRandomTown(ref chr)
{

	string town = "";
	if(chr.sex == "skeleton")
	{
		switch (rand(3))
		{
			case 0:
				town = "PortRoyal";
			break;
			case 1:
				town = "BasTer";
			break;
			case 2:
				town = "Havana";
			break;
			case 3:
				town = "Villemstad";
			break;
		}
		return town;
	}
	else
	{
		return PGG_FindRandomTownByNation(sti(chr.nation));
	}
}

//грузить или нет ПГГ в таверну.
void PGG_TavernCheckIsPGGHere()
{
	ref chr;
	int i;
	for (i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);

		if (findsubstr(pchar.location, chr.PGGAi.location.town, 0) != -1 && !LAi_IsDead(chr) && chr.PGGAi.location != "Dead" && !CheckAttribute(chr, "PGGAi.Task.SetSail")) //закрыл дополнительно.
		{
			//квест от ПГГ. Только от одного. И ГГ ещё не занят в квесте.
			if (!CheckAttribute(pchar, "GenQuest.PGG_Quest") && PGG_CheckForQuestOffer(chr)) continue;
			//в таверне или нет.
			if (rand(1) == 1 && chr.sex != "skeleton")
			{
				PGG_PlaceCharacter2Tavern(chr, true);
				PGG_DebugLog("PGG " + chr.id + " in tavern");
			}
			else
			{
				PGG_PlaceCharacter2Tavern(chr, false);
			}
		}
	}
}

void PGG_GraveyardCheckIsPGGHere(ref location)
{
	if(HasSubStr(location.id,"Graveyard"))
	{
		ref chr;
		int i;
		bool hasPGG = false;
		for (i = 1; i <= PsHeroQty; i++)
		{
			chr = CharacterFromID("PsHero_" + i);
			if (findsubstr(pchar.location, chr.PGGAi.location.town, 0) != -1 && !LAi_IsDead(chr) && chr.PGGAi.location != "Dead") //закрыл дополнительно.
			{
				//квест от ПГГ. Только от одного. И ГГ ещё не занят в квесте.
				//if (!CheckAttribute(pchar, "GenQuest.PGG_Quest") && PGG_CheckForQuestOffer(chr)) continue;
				//в таверне или нет.
				if (rand(1) == 1 && !CheckAttribute(chr, "PGGAi.Task.SetSail") && chr.sex == "skeleton")
				{
					PGG_PlaceCharacter2Graveyard(chr, true);
					PGG_DebugLog("PGG " + chr.id + " in graveyard");
					LAi_LocationFightDisable(location, true);
					hasPGG = true;
				}
				else
				{
					PGG_PlaceCharacter2Graveyard(chr, false);
				}
			}
		}
		if (!hasPGG)
		{
			LAi_LocationFightDisable(location, false);
		}
	}
}

void PGG_PlaceCharacter2Graveyard(ref chr, bool _bSet)
{
	int n = 0;
	string sTemp;

	if (_bSet)
	{
		sTemp = PlaceCharacter(chr, "goto", "random_free");
		LAi_SetWarriorType(chr);
	}
	else
	{
		ChangeCharacterAddressGroup(chr, "None", "", "");
		LAi_SetWarriorType(chr);
	}
}
//посадить или убрать ПГГ из таверны.
void PGG_PlaceCharacter2Tavern(ref chr, bool _bSet)
{
	int n = 0;
	string sTemp;

	if (_bSet)
	{
		sTemp = PlaceCharacter(chr, "sit", "random_free");
		n = 0;
		while (findsubstr(sTemp, "sit_front" , 0) != -1 && n < 50)
		{    // чтоб не занимали место напротив
			sTemp = PlaceCharacter(chr, "sit", "random_free");
			n++;
		}
		//Log_TestInfo(sTemp);
		//Log_TestInfo(FloatToString(findsubstr(sTemp, "_" , 0),1));
		if (findsubstr(sTemp, "_bar" , 0) != -1) LAi_SetBarmanType(chr);
		else
		{
			if (findsubstr(sTemp, "sit" , 0) == -1)
			{
				PlaceCharacter(chr, "tables", "random_free");
				switch (rand(2))
				{
					case 0:
						LAi_SetStayType(chr);
					break;
					case 1:
						LAi_SetWarriorType(chr);
						if(chr.model.animation == "man")
						{
							LAi_SetGroundSitType(chr);
						}
					break;
					case 2:
						LAi_SetWarriorType(chr);
					break;
				}
			}
			else
			{
				LAi_SetSitType(chr);
			}
		}
	}
	else
	{
		if (!CheckAttribute(chr, "PGGAi.DontUpdate"))
		{
		ChangeCharacterAddressGroup(chr, "None", "", "");
		LAi_SetWarriorType(chr);
		}
	}
}
//==================================================
// Verifications
//==================================================
//доступны ли услуги NPC, если нет, то возвращает ID ПГГ занявшего NPC
int CheckFreeServiceForNPC(ref NPChar, string ServiceType)
{
	ref chr;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);
		if (chr.PGGAi.location == "land" && chr.PGGAi.location.town == NPChar.City)
		{
			if (CheckAttribute(chr, "PGGAi.LockService") && chr.PGGAi.LockService == ServiceType) return sti(chr.index);
		}
		if (CheckAttribute(chr, "PGGAi.location.town.back") && chr.PGGAi.location.town.back == NPChar.City)
		{
			if (CheckAttribute(chr, "PGGAi.LockService") && chr.PGGAi.LockService == ServiceType) return sti(chr.index);
		}
	}
	return -1;
}

//можно ли взять задание у NPC, если нет, то возвращает ID ПГГ взявшего задание
int CheckAvailableTaskForNPC(ref NPChar, string PGGTask)
{
	ref chr;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);
		if (chr.PGGAi.location == "land" && chr.PGGAi.location.town == NPChar.City)
		{
			if (CheckAttribute (chr, "PGGAi.Task") && chr.PGGAi.Task == PGGTask) return sti(chr.index);
		}
	}
	return -1;
}

//получить доступный класс корабля для персонажа.
int PGG_GetAvailableShipClass(ref chr)
{
	int retVal = 7;
	int iSkill = sti(chr.skill.Sailing);

	if (iSkill > 0) retVal = 6;
	if (iSkill > 24) retVal = 5;
	if (iSkill > 39) retVal = 4;
	if (iSkill > 64) retVal = 3;
	if (iSkill > 79) retVal = 2;
	if (iSkill > 94) retVal = 1;

	return retVal;
}
//==================================================
// Relations
//==================================================
//изменить отношение ПГГ к персонажу
//pgg - ПГГ, которому меняем, chr - отношение к какому персу, _addValue - величина изменения (+/-)
int PGG_ChangeRelation2Character(ref pgg, ref chr, int _addValue)
{
	int curRel = 0;
	string chrID = chr.id;
	if (CheckAttribute(pgg, "PGGAi.relations." + chrID))
	{
		curRel = sti(pgg.PGGAi.relations.(chrID));
	}

	curRel += _addValue;
	if (curRel > 100) curRel = 100;
	if (curRel < 0) curRel = 0;

	PGG_DebugLog(pgg.id + " changed relation to " + chr.id + " " + _addValue);
	pgg.PGGAi.relations.(chrID) = curRel;
	return curRel;
}

//изменить отношение ПГГ к ГГ
//pgg - ПГГ, которому меняем, _addValue - величина изменения (+/-)
int PGG_ChangeRelation2MainCharacter(ref pgg, int _addValue)
{
	return PGG_ChangeRelation2Character(pgg, GetMainCharacter(), _addValue);
}

//уставновит нейтральные отношения ко всем остальным ПГГ и ГГ.
void PGG_SetRelation2OtherAsNormal(ref pgg)
{
	ref chr;
	string chrID;
	for (int i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);
		if (chr.id != pgg.id)
		{
			chrID = chr.id;
			pgg.PGGAi.relations.(chrID) = 50;
		}
	}
	chr = GetMainCharacter();
	chrId = chr.id;
	pgg.PGGAi.relations.(chrID) = 50;
}

//==================================================
// Rumours
//==================================================
//добавить слух о действиях ПГГ
void PGG_AddRumour(ref chr, string _idTmpl)
{
	int iCol;
	//фильтруем действия.. хотя функция AddTemplRumour сейчас защищена от "дурака"
	if (_idTmpl != PGG_TASK_WAITINTAVERN && _idTmpl != PGG_TASK_WORKONMAYOR &&
		_idTmpl != PGG_TASK_SAILTOISLAND && _idTmpl != PGG_TASK_WORKONSTORE &&
		_idTmpl != PGG_TASK_WORKWITHCONTRA) return;

	//нет слухов с пиратских поселений.
	iCol = FindColony(chr.PGGAi.location.town);
	if (iCol != -1 && sti(Colonies[iCol].nation) == PIRATE) return;

	//эти с более низким шансом.
	if (_idTmpl == PGG_TASK_SAILTOISLAND || _idTmpl == PGG_TASK_WORKONSTORE || _idTmpl == PGG_TASK_WAITINTAVERN)
	{
		if (rand(10) != 1) return;
	}

	pchar.questTemp.PGG_Rumour = chr.index;
	AddTemplRumour(_idTmpl, id_counter+1);
	DeleteAttribute(pchar, "questTemp.PGG_Rumour");
}

//---> Иниты шаблонов слухов...
void PGG_OnInit_WaitInTavern(ref rTmpl)
{
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.Town_Name = XI_ConvertString("Colony" + chr.PGGAi.location.town + "Gen");
	ttttstr = GetAssembledString(ttttstr, &oPrm);
}

void PGG_OnInit_WorkOnMayor(ref rTmpl)
{
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	oPrm.PsHero_Name = GetHeroName(sti(chr.PGGAi.HeroNum));
	ttttstr = GetAssembledString(ttttstr, &oPrm);

	rTmpl.City = chr.PGGAi.location.town;
}

void PGG_OnInit_SailToIsland(ref rTmpl)
{
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.To = "в";
	if (chr.PGGAi.Task.Target == "Tortuga") oPrm.To = "на";
	oPrm.Town_Name = XI_ConvertString("Colony" + chr.PGGAi.Task.Target + "Acc");
	ttttstr = GetAssembledString(ttttstr, &oPrm);

	rTmpl.actualtime = sti(chr.PGGAi.Task.Target.days);
}

void PGG_OnInit_WorkOnStore(ref rTmpl)
{
	int iGood;
	object oPrm;
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	iGood = sti(chr.PGGAi.Task.Target.Goods);
	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.Good = XI_ConvertString(Goods[iGood].name);
	oPrm.Town_Name = XI_ConvertString("Colony" + chr.PGGAi.Task.Target + "Acc");
	ttttstr = GetAssembledString(ttttstr, &oPrm);

	rTmpl.actualtime = sti(chr.PGGAi.Task.Target.days);
}

void PGG_OnInit_WorkWithContra(ref rTmpl)
{
	ref chr = GetCharacter(sti(pchar.questTemp.PGG_Rumour));

	rTmpl.City = chr.PGGAi.location.town;
	rTmpl.Var = sti(chr.index);
}

//---> Обработка слухов...
string PGG_Event_WorkWithContra(ref rRum)
{
	int i, iTmp;
	string sSmugglersLocation, sTemp;
	object oPrm;
	ref sld;
	ref chr = GetCharacter(sti(rRum.Var));

	sSmugglersLocation = SelectSmugglingLocation();
	if (!CheckAttribute(pchar, "quest.Contraband.Active") && !CheckAttribute(pchar, "questTemp.PGGContra") && !CheckAttribute(pchar, "GenQuest.contraTravel.active"))
	{
		chr.PGGAi.location.town.back = chr.PGGAi.location.town;
		chr.PGGAi.location.town = "none";
		pchar.questTemp.PGGContra.Loc = sSmugglersLocation;
		pchar.questTemp.PGGContra = chr.index;
		PlaceSmugglersOnShore(sSmugglersLocation);

		ChangeCharacterAddressGroup(chr, sSmugglersLocation, "smugglers", "smugglerload");
		chr.location.from_sea = sSmugglersLocation;

		//подумал я, и решил добавить кампании ;) чтобы веселее было...
		LAi_SetWarriorType(chr);
		LAi_group_MoveCharacter(chr, "PGGTmp");
//		LAi_warrior_SetStay(chr, true); //можно убрать, будет шататься по бухте.

		iTmp = 3 + rand(MakeInt(MOD_SKILL_ENEMY_RATE*0.4));
		for (i = 0; i < iTmp; i++)
		{
			sTemp = "pirate_" + (rand(9)+1);
			sld = GetCharacter(NPC_GenerateCharacter("Rand_Enemy0" + i, sTemp, "man", "man", 5, PIRATE, 3, true));
			ChangeCharacterAddressGroup(sld, sSmugglersLocation, "goto", "goto" + (5 + i));

			LAi_SetWarriorType(sld);
			LAi_group_MoveCharacter(sld, "PGGTmp");
			LAi_warrior_SetCommander(sld, chr);
			SetCharacterTask_FollowCharacter(sld, chr);

			sld.Dialog.FileName = "Quest\sailors_dialog.c";
		}
		LAi_group_SetRelation("PGGTmp", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
		LAi_group_SetRelation("PGGTmp", "Smugglers_1", LAI_GROUP_FRIEND);

		//фича, можно будет стучать мэру, и с солдатами ломиться на облаву :)
		pchar.questTemp.PGGContra.Know = true;
		pchar.PGG_Contra_Current = chr.id;
		SetTimerCondition("PGGContra_close", 0, 0, 3, false);
		ReOpenQuestHeader("Gen_StrangeInfo");
		AddQuestRecord("Gen_StrangeInfo", "1");
		AddQuestUserData("Gen_StrangeInfo", "sName", GetFullName(chr));
		AddQuestUserData("Gen_StrangeInfo", "sLoc", GetConvertStr(sSmugglersLocation, "LocLables.txt"));
	}

	oPrm.PsHero_Name = GetFullName(chr);
	oPrm.ContraPlace = GetConvertStr(sSmugglersLocation, "LocLables.txt");

	return GetAssembledString(rRum.text, &oPrm);
}

//Дебаггер...
void PGG_DebugLog(string _sLog)
{
	if (bBettaTestMode && bPGGLogShow)
	{
		Log_SetStringToLog(_sLog);
		trace("PGG: " + GetQuestBookDataDigit() + " " + _sLog);
	}
}

//==================================================
// Quests
//==================================================
//========== Офицер ===========
void PGG_BecomeHiredOfficer()
{
	ref chr = &Characters[sti(pchar.questTemp.HiringOfficerIDX)];

	AddPassenger(pchar, chr, false);

	chr.location = "None";
	chr.PGGAi.location.town = "Офицер";
	chr.PGGAi.IsPGG = false;
	chr.Dialog.CurrentNode = "hired";
	chr.Dialog.FileName = "Enc_Officer_dialog.c";
	//офицер только раз.
	chr.PGGAi.Officer = 1;
	// учет зп -->
	chr.Money   = 0;
	chr.Payment = true;
	// учет зп <--

	DeleteAttribute(pchar, "questTemp.HiringOfficerIDX");
	PGG_ChangeRelation2MainCharacter(chr, 10);
	SetCharacterTask_FollowCharacter(chr, pchar);
}

void PGG_FireOfficer()
{
	ref chr = &Characters[sti(pchar.questTemp.FiringOfficerIDX)];
	CheckForReleaseOfficer(sti(pchar.questTemp.FiringOfficerIDX));

	RemovePassenger(pchar, chr);
	RemoveCharacterCompanion(pchar, chr);

	LAi_SetWarriorType(chr);

	chr.Dialog.CurrentNode = "Second time";
	chr.Dialog.TempNode = "Second time";
	chr.Dialog.FileName = "PGG_Dialog.c";
	chr.location = "none";
	chr.PGGAi.location.town = GetCurrentTown();
	chr.PGGAi.IsPGG = true;

	DeleteAttribute(Pchar, "quest.TempFiringOfficerIDX");
	PGG_ChangeRelation2MainCharacter(chr, -20);
}

//========== Компаньон ===========
void PGG_CompanionLeave(string qName)
{
	ref chr;
	chrDisableReloadToLocation = true;
	chr = &Characters[sti(pchar.questTemp.PGG_Companion_Leave.index)];
	PlaceCharacter(chr, "goto", "random_must_be_near");

	chr.Dialog.CurrentNode = "companion_leave";
	chr.Dialog.FileName = "PGG_Dialog.c";
	LAi_SetActorType(chr);
	LAi_ActorDialog(chr, pchar, "", 2.0, 0);
	DoQuestCheckDelay("OpenTheDoors", 5.0);   //fix
}

//========== Квестовая проверка ===========
bool PGG_CheckForQuestOffer(ref chr)
{
	bool retVal = false;

	if (rand(2) == 1 || CheckAttribute(chr, "PGGWhisperQuestStart")) return retVal; // пусть сам ГГ тоже подходит, а не только ПГГ достает, второй заход в таврену может вынудить подойти ПГГ
	//не забыть вернуть вероятности и условия
	int iTst = 110 - PGG_ChangeRelation2MainCharacter(chr, 0); //зависит от отношения, лучше = чаще
	// пусть будет честный тест
	if (bBettaTestMode && CheckAttribute(PChar, "PGGAlwaysQuest")) iTst = 10; //в версии нет этого аттрибута

	int iDays = GetQuestPastDayParam("QuestOffer");
	bool bOkWhisper = !CheckAttribute(pchar,"GiantEvilSkeleton") && chr.name == "Виспер";
	bool bOk = PGG_IsQuestAvaible() || bOkWhisper);

	if (sti(chr.Ship.Type) != SHIP_NOTUSED && sti(PChar.Ship.Type) != SHIP_NOTUSED && rand(100) > iTst && iDays > 25 + drand(10))
	{
		if (GetCharacterShipClass(PChar) <= 4 && GetCompanionQuantity(PChar) < COMPANION_MAX && bOk)
		{
			chr.PGGAi.ActiveQuest.QstNumber = 0;
			retVal = true;
			SaveCurrentQuestDateParam("QuestOffer"); // какое-то время ПГГ не будут докучать нам
		}
	}

	if (retVal)
	{
		chrDisableReloadToLocation = true;
		PlaceCharacter(chr, "goto", "random_must_be_near");
		if (!CheckAttribute(chr, "PGGAi.Task.SetSail"))
		{
			PChar.GenQuest.PGG_Quest = 1;
		}
		chr.Dialog.CurrentNode = "quest_onStay";
		chr.PGGAi.ActiveQuest = 1;
		LAi_SetActorType(chr);
		LAi_ActorDialog(chr, pchar, "", 2.0, 0);
		DoQuestCheckDelay("OpenTheDoors", 5.0);      // fix
	}
	return retVal;
}

bool PGG_IsQuestAvaible()
{
	string sTown;
	bool retVal = true;

	sTown = GetCurrentTown();
	if (sTown == "" || sTown == "Panama" || Colonies[FindColony(sTown)].from_sea == "" || CheckAttribute(Colonies[FindColony(sTown)], "HasNoFort")) retVal = false;
	return retVal;
}

//========== Первый квест ===========
//убрать корабль ПГГ при отказе от квеста, чтобы не болтался...
void PGG_Q1RemoveShip(string qName)
{
	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);

	chr.Dialog.CurrentNode = "Second Time";
	DeleteAttribute(chr, "DeckDialogNode");

	Group_SetAddress("PGGQuest", "None", "", "");
	Group_DelCharacter("PGGQuest", chr.id);
//	Group_DeleteGroup("PGGQuest");
	//#20180927-01
	if (CheckAttribute(PChar, "GenQuest.PGG_Quest.Stage") && sti(PChar.GenQuest.PGG_Quest.Stage) < 2 && sti(PChar.GenQuest.PGG_Quest.Stage) >= -1)
	{
		chr.Dialog.CurrentNode = "Second Time";
		DeleteAttribute(chr, "PGGAi.ActiveQuest");
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
		//#20180927-01
		int nRelChange = -10;
		if(sti(PChar.GenQuest.PGG_Quest.Stage) == -1)
            nRelChange = -20;
		PGG_ChangeRelation2MainCharacter(chr, nRelChange);
		if (chr.sex != "woman")CloseQuestHeader("Gen_PGGQuest1");
		else CloseQuestHeader("Gen_PGGQuest1woman");
	}
	DeleteAttribute(chr, "AlwaysFriend");
}
void PGG_Q1AcceptedQuestDeleteFantom(string qName)
{
	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);

	chr.Dialog.CurrentNode = "Second Time";
	DeleteAttribute(chr, "DeckDialogNode");

	Group_SetAddress("PGGQuest", "None", "", "");
	Group_DelCharacter("PGGQuest", chr.id);
//	Group_DeleteGroup("PGGQuest");
	//#20180927-01
}
//после драки на палубе при отказе от квеста.
void PGG_Q1AfterDeckFight()
{
	ref chr, rGroup;
	string sGroup;
	sGroup = PChar.GenQuest.PGG_Quest.GrpID;

	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	chr.AlwaysEnemy = true;
	chr.Coastal_Captain    = true;  // не ругать нации
	chr.Abordage.Enable = false;
	//#20190612-05
	chr.WatchFort = true;
	LAi_SetImmortal(chr, false);


	DelEventHandler("LAi_event_GroupKill", "PGG_Q1AfterDeckFight");
	// опасно тереть на лету LAi_group_Delete(PChar.GenQuest.PGG_Quest.GrpID);
	DeleteAttribute(PChar, "GenQuest.PGG_Quest.GrpID");
	chrDisableReloadToLocation = false;
	string PGGSex = "";
	if(chr.sex == "woman")	PGGSex = "woman";

	if (sti(PChar.GenQuest.PGG_Quest.Stage) < 4)
	{
		AddQuestRecord("Gen_PGGQuest1", "q1_RefuseFight");
		AddQuestUserData("Gen_PGGQuest1", "sSex", GetSexPhrase("ся","ась"));
		AddQuestUserData("Gen_PGGQuest1", "sSex1", GetSexPhrase("ин","на"));
		AddQuestUserData("Gen_PGGQuest1", "sNation", NationNameGenitive(sti(PChar.GenQuest.PGG_Quest.Nation)));
		AddQuestUserData("Gen_PGGQuest1", "sPsName", GetFullName(chr));
		LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], true);
	}
	else
	{
		if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.Ok"))
		{
			AddQuestRecord("Gen_PGGQuest1", "q1_FailNotPay");
			AddQuestUserData("Gen_PGGQuest1", "sSex", GetSexPhrase("","а"));
			AddQuestUserData("Gen_PGGQuest1", "sSex1", GetSexPhrase("ен","на"));
			AddQuestUserData("Gen_PGGQuest1", "sPsName", GetFullName(chr));
		}
		else
		{
			AddQuestRecord("Gen_PGGQuest1"+PGGSex, "q1_OkNotShare");
			AddQuestUserData("Gen_PGGQuest1", "sSex", GetSexPhrase("","а"));
		}
	}
	DoQuestCheckDelay("hide_weapon", 2.0);

	rGroup = Group_FindOrCreateGroup("PGGQuest");
	PChar.Quest.PGGQuest1_AfterDeckFight.win_condition.l1 = "location";
	PChar.Quest.PGGQuest1_AfterDeckFight.win_condition.l1.location = rGroup.Location;
	PChar.Quest.PGGQuest1_AfterDeckFight.function = "PGG_Q1SeaFightAfterDeck";

	CloseQuestHeader("Gen_PGGQuest1"+PGGSex);
}

//выход в море и нападение ПГГ на игрока после боевки на палубе.
void PGG_Q1SeaFightAfterDeck(string qName)
{
	string sTemp = "PGGQuest";
	Group_SetTaskAttackEx(sTemp, PLAYER_GROUP, false);
	Group_LockTask(sTemp);

	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	LAi_SetImmortal(chr, false);

	PChar.Quest.PGGQuest1_AfterSeaFight.win_condition.l1 = "Group_Death";
	PChar.Quest.PGGQuest1_AfterSeaFight.win_condition.l1.group = sTemp;
	PChar.Quest.PGGQuest1_AfterSeaFight.function = "PGG_Q1AfterSeaFight";

	if (sti(PChar.GenQuest.PGG_Quest.Stage) < 4)
	{
		PChar.Quest.PGGQuest1_Runaway.win_condition.l1 = "ExitFromSea";
		PChar.Quest.PGGQuest1_Runaway.function = "PGG_Q1AfterSeaFight";
	}
	else
	{
		PChar.Quest.PGGQuest1_Runaway.win_condition.l1 = "MapEnter";
		PChar.Quest.PGGQuest1_Runaway.function = "PGG_Q1AfterSeaFight";
	}
}

//закрытие квеста при потопление ПГГ или, если игрок свалил...
void PGG_Q1AfterSeaFight(string qName)
{
	ref chr;

	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	chr.Abordage.Enable = true;
	chr.Dialog.CurrentNode = "Second Time";
	if (CheckAttribute(chr, "Nation.Bak")) chr.Nation = chr.Nation.Bak;
	chr.PGGAi.IsPGG = true;
	chr.RebirthPhantom = true;
	if (chr.PGGAi.location.town == "none")
	{
		chr.PGGAi.location.town = PGG_FindRandomTown(chr);
	}

	Group_SetAddress("PGGQuest", "None", "", "");
	Group_DeleteGroup("PGGQuest");

	PChar.Quest.PGGQuest1_AfterSeaFight.Over = "yes";
	PChar.Quest.PGGQuest1_Runaway.Over = "yes";

	ChangeCharacterAddressGroup(chr, "None", "", "");

	DeleteAttribute(chr, "AlwaysEnemy");
	DeleteAttribute(chr, "Coastal_Captain");
	DeleteAttribute(chr, "PGGAi.ActiveQuest");
	DeleteAttribute(PChar, "GenQuest.PGG_Quest");
}

void PGG_Q1PlaceShipsNearIsland()
{
	ref chr;
	string sTmp, sLoc, sType;
	int i, iRnd, iNation, iLifeDay, iRank, iGoods, iSpace;

	sLoc = PChar.GenQuest.PGG_Quest.Island;
	iRank = sti(PChar.Rank);
	iGoods = sti(PChar.GenQuest.PGG_Quest.Goods);
	iNation = sti(PChar.GenQuest.PGG_Quest.Nation);
	iLifeDay = sti(PChar.GenQuest.PGG_Quest.Days);

	iRnd = 3 + rand(2);
	for (i = 0; i < iRnd; i++)
	{
		sTmp = "pirate_" + i;
		chr = GetCharacter(NPC_GenerateCharacter("RandQuestCap_0" + i, sTmp, "man", "man", MakeInt(iRank/2 + rand(iRank)), iNation, iLifeDay, true));
		chr.AlwaysSandbankManeuver = true;
		SetCharacterPerk(chr,PerksChars());
		if (i < 2)
		{
			//Lipsar правки в спавне-->
			CreatePGG_War(chr, iNation, pchar);
			SetRandomNameToShip(chr);
			SetBaseShipData(chr);
			SetCrewQuantityFull(chr);
			Fantom_SetCannons(chr, "war"); //fix
			Fantom_SetBalls(chr, "pirate");
			Fantom_SetGoods(chr, "war");
			iSpace = GetCharacterFreeSpace(chr, iGoods)/rand(10);
			// iSpace = MakeInt(iSpace/(2+rand(1)));
			if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.Goods.Qty")) PChar.GenQuest.PGG_Quest.Goods.Qty = 0;
			PChar.GenQuest.PGG_Quest.Goods.Qty = sti(PChar.GenQuest.PGG_Quest.Goods.Qty) + iSpace;
			Fantom_SetCharacterGoods(chr, iGoods, iSpace, 1);
			SetRandomNameToShip(chr);
			SetFantomParamHunter(chr); //крутые парни
			SetCaptanModelByEncType(chr, "war");//
		}
		else
		{
			CreatePGG_Trade(chr, iNation, pchar);
			SetRandomNameToShip(chr);
			SetBaseShipData(chr);
			SetCrewQuantityFull(chr);
			Fantom_SetCannons(chr, "trade");
			Fantom_SetBalls(chr, "trade");
			iSpace = GetCharacterFreeSpace(chr, iGoods)/(1+rand(1));
			// iSpace = MakeInt(iSpace/2 + rand(iSpace/2));
			if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.Goods.Qty")) PChar.GenQuest.PGG_Quest.Goods.Qty = 0;
			PChar.GenQuest.PGG_Quest.Goods.Qty = sti(PChar.GenQuest.PGG_Quest.Goods.Qty) + iSpace;
			Fantom_SetCharacterGoods(chr, iGoods, iSpace, 1);
//			SetCrewQuantityOverMax()
			SetSeaFantomParam(chr, "trade");
			SetCaptanModelByEncType(chr, "trade");
//			PChar.Quest.(sTmp).win_condition.l1 = "NPC_Death";
		}
		//<--Lipsar правки в спавне
		Group_AddCharacter("PGGQuest", chr.id);
		chr.AlwaysEnemy = true;
	}
	Group_SetGroupCommander("PGGQuest", "RandQuestCap_01");
	Group_SetAddress("PGGQuest", PChar.GenQuest.PGG_Quest.Island, "Quest_Ships", "Quest_Ship_" + (9 + rand(1)));//поправить
	Group_SetTaskNone("PGGQuest");


	PChar.Quest.PGGQuest1_GroupDead.win_condition.l1 = "Group_Death";
	PChar.Quest.PGGQuest1_GroupDead.win_condition.l1.Group = "PGGQuest";
	PChar.Quest.PGGQuest1_GroupDead.function = "PGG_Q1AfterBattle";

	PChar.Quest.PGGQuest1_CheckStartState.win_condition.l1 = "Location";
	PChar.Quest.PGGQuest1_CheckStartState.win_condition.l1.Location = PChar.GenQuest.PGG_Quest.Island;
	PChar.Quest.PGGQuest1_CheckStartState.function = "PGG_Q1CheckStartState";
}

void PGG_Q1PGGDead(string qName)
{
	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	if (sti(PChar.GenQuest.PGG_Quest.Template))
	{
		if (sti(PChar.GenQuest.PGG_Quest.Stage) < 3)
		{
			Group_SetAddress("PGGQuest", "None", "", "");
			Group_DeleteGroup("PGGQuest");

			if (CheckAttribute(chr, "PGGAi.ActiveQuest"))
			{
				if (chr.sex != "woman")AddQuestRecord("Gen_PGGQuest1", "q1_PGGDeadNoShips");
				else AddQuestRecord("Gen_PGGQuest1woman", "q1_PGGDeadNoShips");
			}
			else
			{
				if (chr.sex != "woman")AddQuestRecord("Gen_PGGQuest1", "q1_PGGDeadNoShips_1");
				else AddQuestRecord("Gen_PGGQuest1woman", "q1_PGGDeadNoShips_1");
			}
			PChar.Quest.PGGQuest1_CheckStartState.Over = "yes"; //fix doors
		}
		else
		{
			if (chr.sex != "woman")AddQuestRecord("Gen_PGGQuest1", "q1_PGGDeadShips");
			else AddQuestRecord("Gen_PGGQuest1woman", "q1_PGGDeadShips");
		}
		if (chr.sex != "woman")AddQuestUserData("Gen_PGGQuest1", "sPsName", GetFullName(chr));
		else AddQuestUserData("Gen_PGGQuest1woman", "sPsName", GetFullName(chr));

		PChar.Quest.PGGQuest1_GroupDead.Over = "yes";
		PChar.Quest.PGGQuest1_Time2Late.Over = "yes";
		PChar.Quest.PGGQuest1_Time2Fight.Over = "yes";

		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
	}
	else
	{
		PChar.GenQuest.PGG_Quest.PGGid.Dead = 1;
		if (chr.sex != "woman")AddQuestRecord("Gen_PGGQuest1", "q1_PGGDead");
		else AddQuestRecord("Gen_PGGQuest1woman", "q1_PGGDead");
		if (chr.sex != "woman")AddQuestUserData("Gen_PGGQuest1", "sPsName", GetFullName(chr));
		else AddQuestUserData("Gen_PGGQuest1woman", "sPsName", GetFullName(chr));
	}
	DeleteAttribute(chr, "PGGAi.ActiveQuest");
	PChar.Quest.PGGQuest1_GroupDead.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Late.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Fight.Over = "yes";
	if (chr.sex != "woman")CloseQuestHeader("Gen_PGGQuest1");
	else CloseQuestHeader("Gen_PGGQuest1woman");
}

void PGG_Q1CheckStartState(string qName)
{
	ref chr;
	int i, n, iNum;

	if (CheckAttribute(PChar,"PGG_EnemyPP") && !CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid.Dead") && sti(PChar.GenQuest.PGG_Quest.Template))
	{
	Group_AddCharacter("EnemyPGG", PChar.GenQuest.PGG_Quest.PGGid);
	Group_SetGroupCommander("EnemyPGG", PChar.GenQuest.PGG_Quest.PGGid);
	PGG_SpawnPGG();

	SetTimerCondition("PGGQuest1_Time2Fight", 0, 0, 2, false);

	PChar.Quest.PGGQuest1_ExitBattleField.win_condition.l1 = "ExitFromLocation";
	PChar.Quest.PGGQuest1_ExitBattleField.win_condition.l1.Location = pchar.location;
	PChar.Quest.PGGQuest1_ExitBattleField.function = "PGG_Q1EndClear";
	PChar.Quest.PGGQuest1.SeaBattle = "1";
	}
	else
	{
	PChar.GenQuest.PGG_Quest.Stage = 3;
	PChar.Quest.PGGQuest1_Time2Late.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
	PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";

	if (sti(PChar.GenQuest.PGG_Quest.Template))
	{
		SetTimerCondition("PGGQuest1_Time2Fight", 0, 0, 2, false);
		PChar.Quest.PGGQuest1_Time2Fight.function = "PGG_Q1AfterBattle";
	}
	else
	{
		if (!isLocationFreeForQuests(PChar.GenQuest.PGG_Quest.Island.Shore))
		{
			DeleteAttribute(PChar, "Quest.PGGQuest1_CheckStartState.Over");
			return;
		}
		else
		{
			PGG_Q1FightOnShore();
		}
	}
	PChar.GenQuest.PGG_Quest.StartGoods = 0; //GetSquadronGoods(pchar, sti(PChar.GenQuest.PGG_Quest.Goods)); //считаем весь товар в экскадре!

	n = GetCompanionQuantity(PChar);
	for (i = 0; i < n; i++)
	{
		iNum = GetCompanionIndex(PChar, i);
		if (iNum != -1)
		{
			chr = GetCharacter(iNum);
			if (GetRemovable(chr))
			{
				PChar.GenQuest.PGG_Quest.StartGoods = sti(PChar.GenQuest.PGG_Quest.StartGoods) + GetCargoGoods(chr, sti(PChar.GenQuest.PGG_Quest.Goods));
			}
		}
	}
	}
}

void PGG_Q1AfterBattle(string qName)
{
	//Boyer add
	int cn;
	int nNumShips = 0;

	ref chr;
	aref arOldMapPos;
	int i, n, iNum, iRnd;
	string sLoc;
	bool bLater = false;
	bool bOk = !Group_isDead("PGGQuest") && !bMapEnter;

	if (bOk || !bSeaActive)
	{
		if (IsEntity(worldMap))
		{
//			PChar.Quest.PGGQuest1_Time2Fight.win_condition.l1 = "EnterToSea";
			bLater = true;
		}
		else
		{
			SetTimerConditionParam("PGGQuest1_Time2Fight", "PGGQuest1_Time2Fight", 0, 0, 0, MakeInt(GetHour() + 2), false);
			DeleteAttribute(PChar, "Quest.PGGQuest1_Time2Fight.Over");
			return;
		}
	}

	//теперь весь товар после боя минус то, что было изначально!
	PChar.GenQuest.PGG_Quest.Goods.Taken = 0;
	n = GetCompanionQuantity(PChar);
	for (i = 0; i < n; i++)
	{
		iNum = GetCompanionIndex(PChar, i);
		if (iNum != -1)
		{
			chr = GetCharacter(iNum);
			if (GetRemovable(chr))
			{
				PChar.GenQuest.PGG_Quest.Goods.Taken = sti(PChar.GenQuest.PGG_Quest.Goods.Taken) + GetCargoGoods(chr, sti(PChar.GenQuest.PGG_Quest.Goods));
			}
		}
	}

	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	sLoc = PChar.GenQuest.PGG_Quest.Island.Shore;

	//Boyer fix for log error: RUNTIME ERROR - file: scripts\PsHero.c; line: 1635
	//                  	   missed attribute: startgoods
	if (CheckAttribute(PChar.GenQuest.PGG_Quest, "StartGoods"))
	PChar.GenQuest.PGG_Quest.Goods.Taken = sti(PChar.GenQuest.PGG_Quest.Goods.Taken) - sti(PChar.GenQuest.PGG_Quest.StartGoods);
	PChar.GenQuest.PGG_Quest.GrpLoc = Island_FindNearestLocator2PChar("Quest_Ships");
	PChar.GenQuest.PGG_Quest.GrpLocation = PChar.location;

	if (!bLater)
	{
		if (isLocationFreeForQuests(sLoc))
		{
			PChar.location.from_sea = sLoc;
			Locations[FindLocation(sLoc)].DisableEncounters = true;
			DoReloadFromSeaToLocation(sLoc, "reload", "sea");
		}
		else
		{
			MakeCloneShipDeck(pchar, true); // подмена палубы
			DoReloadFromSeaToLocation("Ship_deck", "goto", "goto5");
   			pchar.quest.Munity = "";
			sLoc = "Ship_deck";
		}
	}
	else
	{
		makearef(arOldMapPos, worldMap.old);
		WdmPrepareMapForAbordage(arOldMapPos);
		MakeCloneShipDeck(pchar, true); // подмена палубы
		LAi_LocationFightDisable(&Locations[FindLocation("Ship_deck")], true);
		DoReloadFromWorldMapToLocation("Ship_deck", "goto", "goto5");
		ReloadFromWMtoL_complete();
		LAi_LockFightMode(pchar, true);
		PChar.Quest.Munity = "";
		sLoc = "Ship_deck";
	}

	PChar.Quest.PGGQuest1_LocationLoaded.win_condition.l1 = "Location";
	PChar.Quest.PGGQuest1_LocationLoaded.win_condition.l1.Location = sLoc;
	PChar.Quest.PGGQuest1_LocationLoaded.function = "PGG_Q1LocationLoaded";


	PChar.Quest.PGGQuest1_PGGDead.Over = "yes";
	PChar.Quest.PGGQuest1_GroupDead.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Fight.Over = "yes";
	PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
	PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";
}
void PGG_Q1LocationLoaded(string qName)
{
	chrDisableReloadToLocation = true;

	ref chr;
	int i, iRnd;
	string attrName;

	//Boyer fix...battle interface stills showing after teleport fix in quests.c:  DoReloadCharacterToLocation
	DeleteBattleInterface();
	//End Boyer fix
	Group_SetAddress("PGGQuest", "", "", "");
	Group_SetAddress("PGGQuestE", "", "", "");
	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	chr.Dialog.CurrentNode = "Quest_1_CheckResult";
	LAi_SetImmortal(chr, true);

	PlaceCharacter(chr, "goto", "random_must_be_near");
	LAi_SetActorType(chr);
	LAi_ActorDialog(chr, pchar, "", 2.0, 0);

	iRnd = 3 + rand(2);
	PChar.GenQuest.PGG_Quest.GrpID.Qty = iRnd;
	chr.location = pchar.location;
	for (i = 1; i < iRnd; i++)
	{
		attrName = "pirate_" + i;
		chr = GetCharacter(NPC_GenerateCharacter(attrName, attrName, "man", "man", 15, PIRATE, 0, true));
		chr.dialog.Filename = "Quest\sailors_dialog.c";
		LAi_SetWarriorType(chr);
		LAi_SetLoginTime(chr, 0.0, 24.0);
		LAi_SetHP(chr, 100.0, 100.0);
		ChangeCharacterAddressGroup(chr, PChar.location, "goto", "goto"+i);
		LAi_group_MoveCharacter(chr, "PGGTmp");
	}
	LAi_group_SetRelation("PGGTmp", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);

	PChar.GenQuest.PGG_Quest.GrpID = "PGGTmp";
	PChar.location.from_sea = PChar.location;
}

void PGG_Q1EndClear(string qName)
{
	if (CheckAttribute(PChar,"PGG_EnemyPP"))
	{
		if(CheckAttribute(PChar,"Quest.PGGQuest1.SeaBattle"))
		{
		Group_SetAddress("PGGQuest","none","none","none");
		LAi_group_Delete("PGGQuest");
		}
		PChar.Quest.PGGQuest1_EndExitSea.Over = "Yes";
		PChar.Quest.PGGQuest1_EndExitLoc.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";
		DeleteAttribute(PChar, "DisableBIFace");
		DeleteAttribute(PChar,"PGG_EnemyPP");
		DeleteAttribute(PChar,"Quest.PGGQuest1.SeaBattle");
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
	}
	else
	{

	ref chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	DeleteAttribute(&Locations[FindLocation(PChar.GenQuest.PGG_Quest.Island.Shore)],"disableencounters");
	chr.Dialog.CurrentNode = "Second Time";
	chr.PGGAi.IsPGG = true;
	chr.RebirthPhantom = true;
	chr.Nation = chr.Nation.Bak;
	LAi_SetImmortal(chr, false);
	LAi_SetWarriorType(chr);
    SetCharacterRemovable(chr, true);

	PChar.Quest.PGGQuest1_EndExitSea.Over = "Yes";
	PChar.Quest.PGGQuest1_EndExitLoc.Over = "Yes";
	PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
	PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";

	LAi_LocationFightDisable(&Locations[FindLocation("Ship_deck")], false);

	//Boyer add
	DeleteAttribute(PChar, "DisableBIFace");
	//End Boyer add
	DeleteAttribute(chr, "PGGAi.ActiveQuest");
	DeleteAttribute(chr, "AlwaysFriend");
	DeleteAttribute(PChar, "GenQuest.PGG_Quest");

	if(!CheckAttribute(chr, "PGGAfterShare"))
	{
		chr.PGGAi.location.town = PGG_FindRandomTown(chr);
	}
	else
	{
		DeleteAttribute(chr, "PGGAfterShare");
	}
	ChangeCharacterAddressGroup(chr, "None", "", "");
	LAi_group_Delete("PGGTmp"); //попробуем потереть в конце всего.
	}
	DeleteAttribute(PChar,"Quest.PGGQuest1.SeaBattle");
	if (CheckAttribute(pchar,"ScamFanActive") && !CheckAttribute(pchar,"ScamDestroyed"))
	{
		chr = CharacterFromID("ScamCharacter");
		LAi_SetActorTypeNoGroup(chr);
		LAi_ActorFollowEverywhere(chr, "", -1);
		chr.chr_ai.tmpl.state = "stay";
	}
}

void PGG_Q1Time2Late(string qName)
{
	int i;
	string attrName;
	ref chr;
	aref arOldMapPos;

	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	if (!bQuestCheckProcessFreeze)
	{
		if (IsEntity(worldMap))
		{
			PChar.Quest.PGGQuest1_Time2Late_01.over = "yes"; // сброс, если уже откладывали
			PChar.Quest.PGGQuest1_Time2Late_02.over = "yes";

			makearef(arOldMapPos, worldMap.old);
			WdmPrepareMapForAbordage(arOldMapPos);
			MakeCloneShipDeck(pchar, true); // подмена палубы
			LAi_LocationFightDisable(&Locations[FindLocation("Ship_deck")], true);
			DoReloadFromWorldMapToLocation("Ship_deck", "goto", "goto7");
			//ReloadFromWMtoL_complete(); - даже не знаю зачем это, наследие К3, проверка острова, тут можно не юзать.
			LAi_LockFightMode(pchar, true);

			ChangeCharacterAddressGroup(chr, "Ship_deck", "goto", "goto2");
			LAi_group_MoveCharacter(chr, "GroupDeck");
			chr.Dialog.CurrentNode = "Quest_1_Time2Late";

			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 3.0, 0); // все работает, смотрите как работают темплейты в АИ актеров. eddy

			// ==> антураж
			for (i=1; i<5; i++)
			{
				attrName = "pirate_" + i;
				Log_TestInfo(attrName);
				chr = GetCharacter(NPC_GenerateCharacter(attrName, attrName, "man", "man", 15, PIRATE, 1, true));
				SetFantomParam(chr);
				chr.dialog.Filename = "Quest\sailors_dialog.c";
				LAi_SetWarriorType(chr);
				LAi_SetLoginTime(chr, 0.0, 24.0);
				LAi_SetHP(chr, 100.0, 100.0);
				ChangeCharacterAddressGroup(chr, "Ship_deck", "goto", "goto"+(i+2));
				LAi_group_MoveCharacter(chr, "GroupDeck");
			}
    		pchar.quest.Munity = "";  // признак выхода с палубы (закрыто, а то можно не говорить)
		}
		else
		{
			if (isLocationFreeForQuests(PChar.location) && !bSeaActive && !LAi_grp_alarmactive) //проверка на бой??
			{
				if (PlaceCharacter(chr, "goto", "random_must_be_near") != "")
				{
					PChar.Quest.PGGQuest1_Time2Late_01.over = "yes";
					PChar.Quest.PGGQuest1_Time2Late_02.over = "yes";

					chrDisableReloadToLocation = true;

					chr.Dialog.CurrentNode = "Quest_1_Time2Late";
					LAi_SetActorType(chr);
					LAi_ActorDialog(chr, pchar, "", 2.0, 0);
				}
				else
				{
					PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1 = "ExitFromLocation";
					PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1.Location = PChar.location;
					PChar.Quest.PGGQuest1_Time2Late_02.function = "PGG_Q1Time2Late";
					DeleteAttribute(PChar, "Quest.PGGQuest1_Time2Late_02.Over");
				}
			}
			else
			{
				PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1 = "ExitFromLocation";
				PChar.Quest.PGGQuest1_Time2Late_02.win_condition.l1.Location = PChar.location;
				PChar.Quest.PGGQuest1_Time2Late_02.function = "PGG_Q1Time2Late";
				DeleteAttribute(PChar, "Quest.PGGQuest1_Time2Late_02.Over");
			}
		}
	}
	else
	{
		// отложим до разморозки (пролет табличек)
		PChar.Quest.PGGQuest1_Time2Late_01.win_condition.l1 = "MapEnter";
		PChar.Quest.PGGQuest1_Time2Late_01.function = "PGG_Q1Time2Late";
	}
}

void PGG_Q1FightOnShore()
{
	ref chr;
	string attrName, sLoc, sLoc_2, relation;
	int i, iRnd, iNation;

	if (CheckAttribute(PChar,"PGG_EnemyPP")) relation = "PGGTemp";
	else relation = LAI_GROUP_PLAYER;
	PChar.PGG_FightOnShore = true;
	sLoc = PChar.GenQuest.PGG_Quest.Island.Shore;
	iNation = sti(PChar.GenQuest.PGG_Quest.Nation);

	chrDisableReloadToLocation = true;
	//our
	if (!CheckAttribute(PChar, "GenQuest.PGG_Quest.PGGid.Dead"))
	{
		chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
		ChangeCharacterAddressGroup(chr,pchar.location, "officers", "sea_1");
		SetCharacterPerk(chr,PerksChars());
		LAi_SetWarriorType(chr);
		LAi_SetImmortal(chr, false);
		if (relation != "PGGTemp") LAi_SetCheckMinHP(chr, 1, true, "PGG_CheckHP");
		LAi_group_MoveCharacter(chr, relation);

		iRnd = 10 + drand(15) + MOD_SKILL_ENEMY_RATE;
		PChar.GenQuest.PGG_Quest.GrpID.Qty = iRnd;
		PChar.GenQuestFort.FarLocator = false;
		sLoc = LAi_FindNPCLocator("officers");
		for (i = 1; i < iRnd; i++)
		{
			if (i % 3 != 0)
			{
			chr = SetFantomDefenceForts("officers", sLoc, PIRATE, relation);
			}
			else
			{
			chr = SetFantomDefenceForts("enc02", "",PIRATE, relation);
			}
			FantomMakeCoolFighterWRankDepend(chr,sti(pchar.rank),25+rand(75),25+rand(75),50);
		}
		Pchar.GenQuestFort.FarLocator = true;
	}

	//enemy
	iRnd = 15 + drand(15) + MOD_SKILL_ENEMY_RATE + GetOfficersQuantity(pchar);
	Pchar.GenQuestFort.FarLocator = true;
	sLoc = LAi_FindNPCLocator("goto");
	sLoc_2 = LAi_FindNPCLocator("smugglers");
	for (i = 1; i < iRnd; i++)
	{
		if (i % 2 == 0)
		{
		chr = SetFantomDefenceForts("goto", sLoc, iNation, "PGGTmp");
		}
		else
		{
			chr = SetFantomDefenceForts("smugglers", sLoc_2, iNation, "PGGTmp");
		}
		chr.id = "pirate_" + i;
		SetCharacterPerk(chr,PerksChars());
		FantomMakeCoolFighterWRankDepend(chr,sti(pchar.rank),25+rand(75),25+rand(75),50);
	}
	//натравим.
	LAi_group_SetHearRadius("PGGTmp", 100.0);
	LAi_group_FightGroupsEx("PGGTmp", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
	LAi_group_FightGroupsEx("PGGTmp", relation, true, Pchar, -1, false, false);

	if(CheckAttribute(PChar,"PGG_EnemyPP"))
	{
	LAi_group_FightGroupsEx(relation, LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
	LAi_group_SetCheck(relation, "PGG_Q1AfterShoreFight");
	}
	else LAi_group_SetCheck("PGGTmp", "PGG_Q1AfterShoreFight");

	LAi_SetFightMode(pchar, true);
	PChar.GenQuest.PGG_Quest.GrpID = "PGGTmp";
}

void PGG_Q1AfterShoreFight()
{
	ref chr, rGroup;
	string sGroup;
	sGroup = PChar.GenQuest.PGG_Quest.GrpID;

	//даю немного целевого товара в любом случае, даже если места нет... пусть разбираются :)
	PChar.GenQuest.PGG_Quest.Goods.Taken = 500 + drand(500) + MakeInt(GetSquadronFreeSpace(PChar, sti(PChar.GenQuest.PGG_Quest.Goods)) / (3 + drand(2)))
	chr = CharacterFromID(PChar.GenQuest.PGG_Quest.PGGid);
	LAi_RemoveCheckMinHP(chr); //fix
	if(!CheckAttribute(PChar,"PGG_EnemyPP"))
	{
	if (!CheckAttribute(PChar, "Quest.PGGQuest1_PGGDead.PGG_Dead"))
	{
		DoQuestCheckDelay("hide_weapon", 2.0);
		PChar.Quest.PGGQuest1_PGGDead.Over = "yes";
		chr.Dialog.CurrentNode = "Quest_1_SharePrise";
		LAi_SetImmortal(chr, false);
		LAi_SetActorType(chr);
		LAi_ActorFollow(chr,pchar, "", 8.0);
		LAi_SetStayType(chr);
		LAi_SetActorType(chr);
		LAi_ActorDialog(chr, pchar, "", 20.0, 0);
	}
	else
	{
		DoQuestCheckDelay("hide_weapon", 2.0);
		ChangeCharacterAddressGroup(chr,pchar.location,"reload", "sea");
		PChar.Quest.PGGQuest1_PGGDead.Over = "yes";
		chr.Dialog.CurrentNode = "Quest_1_SharePrise";
		LAi_SetImmortal(chr, false);
		LAi_SetActorType(chr);
		LAi_ActorFollow(chr,pchar, "", 8.0);
		LAi_SetStayType(chr);
		LAi_SetActorType(chr);
		LAi_ActorDialog(chr, pchar, "", 20.0, 0);
	}
	}
	else
	{
		SetCharacterGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods), sti(PChar.GenQuest.PGG_Quest.Goods.Taken) + GetCargoGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods)));
		chrDisableReloadToLocation = false;
		PChar.Quest.PGGQuest1_EndExitSea.Over = "Yes";
		PChar.Quest.PGGQuest1_EndExitLoc.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_01.Over = "Yes";
		PChar.Quest.PGGQuest1_Time2Late_02.Over = "Yes";
		DeleteAttribute(PChar, "DisableBIFace");
		DeleteAttribute(PChar,"PGG_EnemyPP");
		DeleteAttribute(PChar,"Quest.PGGQuest1.SeaBattle");
		DeleteAttribute(chr, "PGGAi.ActiveQuest");
		DeleteAttribute(chr, "AlwaysFriend");
		DeleteAttribute(PChar, "GenQuest.PGG_Quest");
	}
	DeleteAttribute(PChar,"PGG_FightOnShore");
	DeleteAttribute(PChar,"Quest.PGGQuest1_PGGDead.PGG_Dead");
	DeleteAttribute(loadedLocation,"disableencounters");
}

void PGG_AddMoneyToCharacter(ref chr, int iMoney)
{
	int Norm = 100000 + sti(chr.rank)*10000;
	AddMoneyToCharacter(chr, iMoney);
	if (sti(chr.Money) > Norm) chr.Money = Norm;
}

void PGG_SpawnPGG()
{
	Group_SetAddress("EnemyPGG", PChar.GenQuest.PGG_Quest.Island,"","");
	Group_SetPursuitGroup("EnemyPGG", PLAYER_GROUP);
	Group_SetTaskAttack("EnemyPGG", PLAYER_GROUP);
	Group_LockTask("EnemyPGG");
}

//Выбрать случайного ПГГ из списка живущих.
//Фильтрация по полу и анимации
//Если отправить в поля пустые строки, выбор происходит без фильтрации
string SelectRandomPGG(string sex, string animation)
{
	int heroSelected = 1;
	int chosenHero = rand(PsHeroQty);
	for (i = 0; i < 250; i++)
	{
		if (heroSelected == 0)
		{
			Log_TestInfo("Пройдено итераций: "+ i);
			break;
		}
		if (chosenHero != 0)
		{
			sld = CharacterFromID("PsHero_"+chosenHero);
			if (sld.PGGAi.location == "Dead" || IsCompanion(sld) || IsOfficer(sld))
			{
				chosenHero = rand(PsHeroQty);
			}
			else
			{
				if (sex == "" || sld.sex == sex)
				{
					if (animation == "" || sld.model.animation == animation)
					{
						heroSelected = 0;
						PGG_Disband_Fleet(sld);
						Log_TestInfo("Выбран ПГГ "+ GetFullName(sld));
					}
					else	chosenHero = rand(PsHeroQty);
				}
				else	chosenHero = rand(PsHeroQty);
			}
		}
		else	chosenHero = rand(PsHeroQty);
	}
	if(heroSelected == 0)
	{
		return sld.id;
	}
	else
	{
		Log_TestInfo("ПГГ не выбран.")
		return "";
	}
}

string SelectWhisperPGG()
{
	for (i = 1; i < PsHeroQty; i++)
	{
		sld = CharacterFromID("PsHero_"+i);
		if (sld.name == "Виспер")
		{
			return sld.id;
		}
	}
}

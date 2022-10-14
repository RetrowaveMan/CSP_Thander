// SLiB Новый файл для ВМЛ, все от К3 потерто

int ChangeContrabandRelation(ref pchar, int _val)
{
	pchar.questTemp.Contraband.relation = makeint(pchar.questTemp.Contraband.relation) + _val;
	if (makeint(pchar.questTemp.Contraband.relation) > 99)
		pchar.questTemp.Contraband.relation = 99;
	if (makeint(pchar.questTemp.Contraband.relation) < 0)
		pchar.questTemp.Contraband.relation = 0;

	return makeint(pchar.questTemp.Contraband.relation);
}

// работа с контрабандой
// Jason -- > функция выберет из рандомного интервала в массиве товаров контрабандный товар в текущем магазине, первый в интервале. Если такового не окажется, то вернет -1.
int SelectContrabandGoods(ref _refCharacter)
{
	int i;
	int curStoreIdx = GetCharacterCurrentStore(_refCharacter);
	if (curStoreIdx >= 0)
	{
		for (i = rand(30); i < 31; i++)
		{
			if (GetStoreGoodsType(&Stores[curStoreIdx], i) == TRADE_TYPE_CONTRABAND_NAME)
				return i;
			else
				return -1;
		}
	}
}
//<-- выбор рандомного контрабандного товара

void SetContraGoods(ref _refCharacter, int _Goods, int _Quantity)
{
	string goodsName = Goods[_Goods].name;
	_refCharacter.Goods.(goodsName) = _Quantity;
}

void AddContraGoods(ref _refCharacter, int _Goods, int _Quantity)
{
	string goodsName = Goods[_Goods].name;
	if (CheckAttribute(_refCharacter, "Goods." + goodsName))
	{
		_refCharacter.Goods.(goodsName) = sti(_refCharacter.Goods.(goodsName)) + _Quantity;
	}
	else
	{
		_refCharacter.Goods.(goodsName) = _Quantity;
	}
}

void RemoveContraGoods(ref _refCharacter, int _Goods, int _Quantity)
{
	int curQuantity;
	string goodsName = Goods[_Goods].name;

	curQuantity = sti(_refCharacter.Goods.(goodsName));
	if (curQuantity >= _Quantity)
	{
		_refCharacter.Goods.(goodsName) = curQuantity - _Quantity;
	}
	else
	{
		_refCharacter.Goods.(goodsName) = 0;
	}
}

int GetContraGoodsQuantity(ref _refCharacter, int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	int q = 0;
	if (CheckAttribute(_refCharacter, "Goods." + tmpstr))
		q = sti(_refCharacter.Goods.(tmpstr));
	return q;
}

// установить агенту контриков товары, опираясь на наличие контрабандного товара в соответсвующем магазине
void SetAllContraGoods(ref _refStore, ref _refCharacter)
{
	string tmpstr;
	int qty;

	for (int i = 0; i < GOODS_QUANTITY; i++)
	{
		tmpstr = Goods[i].name;
		if (CheckAttribute(_refStore, "Goods." + tmpstr + ".canbecontraband"))
		{
			qty = sti(_refStore.Goods.(tmpstr).Quantity);
			if (sti(_refStore.Goods.(tmpstr).canbecontraband) == CONTRA_SELL) // можем продать товар  контрикам
			{
				SetContraGoods(_refCharacter, i, 0);
			}
			if (sti(_refStore.Goods.(tmpstr).canbecontraband) == CONTRA_BUY) // можем купить товар у контриков
			{
				SetContraGoods(_refCharacter, i, makeint(qty / 4) + rand(200));
			}
		}
	}
}

void RemoveAllContraGoods(ref _refCharacter)
{
	DeleteAttribute(_refCharacter, "Goods");
}

int FindContrabandGoods(ref _refCharacter)
{
	int i, j = 0;
	int curStoreIdx = GetCharacterCurrentStore(_refCharacter);
	if (curStoreIdx >= 0)
	{
		_refCharacter.FindContrabandGoods.StoreIdx = curStoreIdx;
		for (i = 0; i < GOODS_QUANTITY; i++)
		{
			if (GetStoreGoodsType(&Stores[curStoreIdx], i) == TRADE_TYPE_CONTRABAND_NAME || GetContrabandGoods(&Stores[curStoreIdx], i) == CONTRA_SELL)
			{
				if (GetSquadronGoods(_refCharacter, i) > 0)
				{
					_refCharacter.FindContrabandGoods.GoodsIdx = i;
					j++;
				}
			}
		}
		if (j == 0)
		{
			DeleteAttribute(_refCharacter, "FindContrabandGoods");
			return -1;
		}
	}
	return j;
}

// работа с контрабандой
int FindFirstContrabandGoods(ref _refCharacter)
{
	if (sti(_refCharacter.Ship.Type) == SHIP_NOTUSED)
		return -1;

	int i;
	int curStoreIdx = GetCharacterCurrentStore(_refCharacter);
	if (curStoreIdx >= 0)
	{
		_refCharacter.FindContrabandGoods.StoreIdx = curStoreIdx;
		for (i = 0; i < GOODS_QUANTITY; i++)
		{
			if (GetStoreGoodsType(&Stores[curStoreIdx], i) == TRADE_TYPE_CONTRABAND_NAME)
			{
				if (GetSquadronGoods(_refCharacter, i) > 0)
				{
					_refCharacter.FindContrabandGoods.GoodsIdx = i;
					return i;
				}
			}
			if (GetStoreGoodsType(&Stores[curStoreIdx], i) == TRADE_TYPE_CANNONS_NAME)
			{
				if (GetSquadronGoods(_refCharacter, i) > 0)
				{
					_refCharacter.FindContrabandGoods.GoodsIdx = i;
					return i;
				}
			}
		}
		DeleteAttribute(_refCharacter, "FindContrabandGoods");
		return -1;
	}
	int curLocIdx = FindLocation(_refCharacter.location);
	if (curLocIdx < 0)
		return -1;
	int curIslandIdx = GetIslandIdxByLocationIdx(curLocIdx);
	if (curIslandIdx < 0)
		return -1;
	aref islRef;
	makearef(islRef, Islands[curIslandIdx].Trade.Contraband);
	int n, iMax;
	iMax = GetAttributesNum(islRef); // fix
	for (i = 0; i < iMax; i++)
	{
		n = sti(GetAttributeValue(GetAttributeN(islRef, 0)));
		if (GetSquadronGoods(_refCharacter, n) > 0)
		{
			_refCharacter.FindContrabandGoods.IslandIdx = curIslandIdx;
			_refCharacter.FindContrabandGoods.GoodsIdx = i;
			return n;
		}
	}
	return -1;
}

int FindNextContrabandGoods(ref _refCharacter)
{
	int i, n;
	if (CheckAttribute(_refCharacter, "FindContrabandGoods.StoreIdx"))
	{
		if (CheckAttribute(_refCharacter, "FindContrabandGoods.GoodsIdx"))
		{
			int curStoreIdx = sti(_refCharacter.FindContrabandGoods.StoreIdx);
			if (curStoreIdx >= 0)
			{
				for (i = sti(_refCharacter.FindContrabandGoods.GoodsIdx) + 1; i < GOODS_QUANTITY; i++)
				{
					if (GetStoreGoodsType(&Stores[curStoreIdx], i) == TRADE_TYPE_CONTRABAND_NAME)
					{
						if (GetSquadronGoods(_refCharacter, i) > 0)
						{
							_refCharacter.FindContrabandGoods.GoodsIdx = i;
							return i;
						}
					}
				}
			}
		}
	}
	else
	{
		if (CheckAttribute(_refCharacter, "FindContrabandGoods.IslandIdx"))
		{
			if (CheckAttribute(_refCharacter, "FindContrabandGoods.GoodsIdx"))
			{
				int curIslandIdx = sti(_refCharacter.FindContrabandGoods.IslandIdx);
				if (curIslandIdx >= 0)
				{
					int curGoodsNum = sti(_refCharacter.FindContrabandGoods.GoodsIdx) + 1;
					aref islRef;
					makearef(islRef, Islands[curIslandIdx].Trade.Contraband);
					for (i = curGoodsNum; i < GetAttributesNum(islRef); i++) // это мне сомнительно, походу тут баг, но эта ветка не работает, тк магазин есть, тк разговор в таверне идёт
					{
						n = GetAttributeValue(GetAttributeN(islRef, i));
						if (GetSquadronGoods(_refCharacter, n) > 0)
						{
							_refCharacter.FindContrabandGoods.GoodsIdx = i;
							return n;
						}
					}
				}
			}
		}
	}
	DeleteAttribute(_refCharacter, "FindContrabandGoods");
	return -1;
}

int GetCharacterCurrentStore(ref _refCharacter)
{
	bool bOk2 = CheckAttribute(Pchar, "quest.Contraband.active") && (sti(Pchar.quest.Contraband.active) == true); // в сделке, значит магазин уже ясен
	if (bOk2 && CheckAttribute(Pchar, "Genquest.Contraband.StoreIdx"))
	{
		return sti(Pchar.Genquest.Contraband.StoreIdx);
	}
	int i, idx;
	int curLocNum = FindLocation(_refCharacter.location);
	if (curLocNum < 0)
		return -1;

	idx = -1;
	if (CheckAttribute(&Locations[curLocNum], "fastreload")) // в таверне должно быть всегда
	{
		string locGroupId = Locations[curLocNum].fastreload;
		for (i = 0; i < STORE_QUANTITY; i++)
		{
			if (Stores[i].group == locGroupId)
			{
				idx = i;
				break;
			}
		}
	}
	else
	{
		idx = SHIP_STORE; // нет контры там, всегда пошлют, если вдруг нет fastreload в таверне
	}

	Pchar.Genquest.Contraband.StoreIdx = idx; // запомним, для бухты, когда в сделку пойдем
	return idx;
}
// to_do
int GetCurrentIslandGoodsPrice(int GoodsType)
{
	int curStoreIdx = GetCharacterCurrentStore(GetMainCharacter()); // вернет в бухте из запоминалки
	if (curStoreIdx < 0)
		return 0;
	return GetStoreGoodsPrice(&Stores[curStoreIdx], GoodsType, PRICE_TYPE_SELL, GetMainCharacter(), 1);
}
// выбор куда идти
string SelectSmugglingLocation()
{
	ref CurIsland;
	int n;
	string TargetLocation

			n = GetCharacterCurrentIsland(Pchar);
	if (n < 0)
	{
		TargetLocation = "None";
	}
	else
	{
		CurIsland = GetIslandByIndex(n);
		TargetLocation = GetIslandRandomShoreId(CurIsland.id);
	}
	Pchar.Quest.SelectedSmugglingLocation = TargetLocation;
	return TargetLocation;
}

////////////////////////////////////////////////////////////////////////////////
//                Контрабандисты НАЧАЛО
////////////////////////////////////////////////////////////////////////////////
void PlaceSmugglersOnShore(string LocationId)
{
	ref Smuggler, player;
	int RandCounter;
	int i;

	player = GetMainCharacter();

	string Model;

	player.GenQuest.Smugglers_Group = "Smugglers_1";
	LAi_group_Register(player.GenQuest.Smugglers_Group);

	for (i = 1; i <= 3 + makeint(MOD_SKILL_ENEMY_RATE / 2); i++)
	{
		Model = "pirate_" + (rand(9) + 1);
		Smuggler = GetCharacter(NPC_GenerateCharacter("Rand_Smug0" + i, Model, "man", "man", 5, PIRATE, 4, true)); // 4 дня, потом сами пропадут
		SetFantomParam(Smuggler);
		LAi_SetWarriorTypeNoGroup(Smuggler);
		LAi_warrior_DialogEnable(Smuggler, true);
		LAi_warrior_SetStay(Smuggler, true);
		Smuggler.Dialog.Filename = "Smuggler_OnShore_dialog.c";
		Smuggler.Dialog.CurrentNode = "First time";
		Smuggler.greeting = "Gr_Smuggler_OnShore";
		LAi_group_MoveCharacter(Smuggler, player.GenQuest.Smugglers_Group);
		if (i < 3)
			ChangeCharacterAddressGroup(Smuggler, LocationID, "Smugglers", "Smuggler0" + i);
		else
		{
			int rando = rand(3);
			switch (rando)
			{
			case 0:
				rando = 1;
				break;
			case 1:
				rando = 1;
				break;
			case 2:
				rando = 2;
				break;
			case 3:
				rando = 3;
				break;
			}
			ChangeCharacterAddressGroup(Smuggler, LocationID, "Smugglers", "Smuggler0" + rando);
		}
	}

	AddGeometryToLocation(LocationID, "smg");

	LAi_group_SetRelation(LAI_GROUP_PLAYER, player.GenQuest.Smugglers_Group, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(player.GenQuest.Smugglers_Group, "CoastalGuards", LAI_GROUP_NEITRAL); // патруль на берегу

	SetTimerCondition("Rand_Smuggling", 0, 0, 4, false); // через 4 дня всех убрать
	// задание на завал контры
	if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))
	{
		pchar.quest.KillSmugglers.win_condition.l1 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l1.character = "Rand_Smug01";
		pchar.quest.KillSmugglers.win_condition.l2 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l2.character = "Rand_Smug02";
		pchar.quest.KillSmugglers.win_condition.l3 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l3.character = "Rand_Smug03";
		pchar.quest.KillSmugglers.win_condition = "KillSmugglers_after";
	}
}

void RemoveSmugglersFromShore()
{
	ref Smuggler;
	int RandCounter, i, cn;
	string sLoc;

	DeleteAttribute(pchar, "quest.Contraband.active");
	CloseQuestHeader("Gen_Contraband");

	if (CheckAttribute(pchar, "Quest.SelectedSmugglingLocation"))
	{
		sLoc = pchar.Quest.SelectedSmugglingLocation;
	}
	// navy --> PGG
	else
	{
		if (CheckAttribute(pchar, "questTemp.PGGContra"))
		{
			sLoc = pchar.questTemp.PGGContra.Loc;
			Smuggler = &Characters[sti(pchar.questTemp.PGGContra)];
			ChangeCharacterAddressGroup(Smuggler, "None", "", "");
			Smuggler.location.from_sea = "";
			Smuggler.PGGAi.location.town = Smuggler.PGGAi.location.town.back;
			DeleteAttribute(pchar, "questTemp.PGGContra");
			DeleteAttribute(Smuggler, "PGGAi.location.town.back");
			CloseQuestHeader("Gen_StrangeInfo");
			// LAi_group_Delete("PGGTmp");
			pchar.Quest.PGG_WithSmugglers_Dead.Over = "yes";
			CloseQuestHeader("Gen_StrangeInfo");
		}
	}
	// navy <--
	RemoveGeometryFromLocation(sLoc, "smg");

	pchar.quest.KillSmugglers_outShore.win_condition.l1 = "ExitFromLocation";
	pchar.quest.KillSmugglers_outShore.win_condition.l1.location = pchar.location;
	pchar.quest.KillSmugglers_outShore.win_condition = "KillSmugglers_outShore";
	for (i = 1; i <= 3 + makeint(MOD_SKILL_ENEMY_RATE / 2); i++) // eddy
	{
		cn = GetCharacterIndex("Rand_Smug0" + i);
		if (cn != -1)
		{
			characters[cn].lifeDay = 0;
		}
	}
	LAi_group_SetRelation(LAI_GROUP_PLAYER, "CoastalGuards", LAI_GROUP_NEITRAL);
}

void SetCoastalGuardPursuit()
{
	ref Smuggler;
	int i;
	int iNation = sti(pchar.Genquest.Contraband.GuardNation); // Нация патруля
	string Model;
	if (CheckCharacterPerk(pchar, "UnlimitedContra") && IsNoCompanions() && sti(RealShips[sti(pchar.ship.type)].Class) > 2)
	{
		Log_TestInfo("Џерк спас от патрулЯ");
	}
	else
	{
		if (CheckAttribute(pchar, "SmugglingFlag"))
			Flag_Change(PIRATE);
		pchar.ContraInterruptWaiting = true;
		for (i = 1; i <= 3; i++)
		{
			Model = "off_" + NationShortName(iNation) + "_" + (rand(1) + 1);
			Smuggler = GetCharacter(NPC_GenerateCharacter("Coastal_Captain0" + i, Model, "man", "man", 5, iNation, 3, true)); // 3 дня, потом сами пропадут
			SetFantomParam(Smuggler);
			SelectCoastalGuardShip(Smuggler);
			DeleteAttribute(Smuggler, "OZG");
			// 1.2.3 хардкор! Smuggler.Coastal_Captain = true; // если по нему палить, НЗГ не будет
			Smuggler.AlwaysEnemy = true;
			Smuggler.DontRansackCaptain = true;
			Smuggler.AlwaysSandbankManeuver = true;
			Group_addCharacter("Coastal_Guards", Smuggler.id);
			SetCharacterRelationBoth(sti(Smuggler.index), GetMainCharacterIndex(), RELATION_ENEMY);
			if (makeint(Pchar.rank) < 6 && i == 1 && IsNoCompanions())
				break;
			if (makeint(Pchar.rank) < 9 && i == 2 && GetCompanionQuantity(Pchar) < 3)
				break;
		}
		Group_SetGroupCommander("Coastal_Guards", "Coastal_Captain01");

		Group_SetPursuitGroup("Coastal_Guards", PLAYER_GROUP);
		Group_SetAddress("Coastal_Guards", Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
		Group_SetTaskAttack("Coastal_Guards", PLAYER_GROUP);
		Group_LockTask("Coastal_Guards");
	}
}

void StopCoastalGuardPursuit()
{
	ref rCap;
	int i, cn;

	for (i = 1; i <= 3; i++)
	{
		cn = GetCharacterIndex("Coastal_Captain0" + i);
		if (cn != -1)
		{
			rCap = GetCharacter(cn);
			if (rCap.location == Pchar.location && bSeaActive)
				return; // если в море погоня, то не трем, только на выход на карту.
			ChangeCharacterAddressGroup(rCap, "None", "", "");
			setCharacterShipLocation(rCap, "None");
		}
	}
	group_DeleteGroup("Coastal_Guards");
}

void SelectCoastalGuardShip(ref rCharacter)
{
	SetShipHunter(rCharacter);
}

int GetContrabandGoodsPrice(ref _refStore, int _Goods, int _PriceType, ref chref, int _qty)
{
	float _TradeSkill = GetSummonSkillFromNameToOld(chref, SKILL_COMMERCE); // 0..10.0
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	int basePrice = MakeInt(Goods[_Goods].Cost);
	if (!CheckAttribute(_refStore, "Goods." + tmpstr))
		return 0;
	makearef(refGoods, _refStore.Goods.(tmpstr));
	int tradeType = MakeInt(refGoods.TradeType);
	int Type;
	if (CheckAttribute(refGoods, "Type"))
	{
		Type = MakeInt(refGoods.Type);
	}

	float tradeModify = 1.0;
	float priceModify = 1.0;
	float costModify = 1.0;

	ref mc = GetMainCharacter();

	switch (tradeType)
	{
	case TRADE_TYPE_NORMAL:
		tradeModify = 1.00 + stf(refGoods.RndPriceModify);
		break;
	case TRADE_TYPE_EXPORT:
		tradeModify = 0.80 + stf(refGoods.RndPriceModify);
		break;
	case TRADE_TYPE_IMPORT:
		tradeModify = 1.20 + stf(refGoods.RndPriceModify);
		break;
	case TRADE_TYPE_CONTRABAND:
		tradeModify = 1.00 + stf(refGoods.RndPriceModify);
		break;
	case TRADE_TYPE_AMMUNITION:
		tradeModify = 1.00 + stf(refGoods.RndPriceModify);
		break;
	case TRADE_TYPE_CANNONS:
		tradeModify = 1.00 + stf(refGoods.RndPriceModify);
		break;
	case TRADE_TYPE_AGGRESSIVE:
		tradeModify = 1.00 + stf(refGoods.RndPriceModify);
		break;
	}

	float skillModify;
	float cModify = 1.0;

	if (_PriceType == PRICE_TYPE_BUY) // цена покупки товара игроком
	{
		skillModify = 1.325 - _TradeSkill * 0.005;
		if (tradeType == TRADE_TYPE_CANNONS)
			cModify = 2.0 - MOD_SKILL_ENEMY_RATE / 20.0;
		if (CheckCharacterPerk(chref, "HT2"))
		{
			if (CheckOfficersPerk(chref, "AdvancedCommerce"))
			{
				skillModify -= 0.20;
			}
			else
			{
				if (CheckOfficersPerk(chref, "BasicCommerce"))
				{
					skillModify -= 0.15;
				}
				else
					skillModify -= 0.05;
			}
		}
		else
		{
			if (CheckOfficersPerk(chref, "Trader"))
			{
				skillModify -= 0.05;
			}
			if (CheckOfficersPerk(chref, "AdvancedCommerce"))
			{
				skillModify -= 0.20;
			}
			else
			{
				if (CheckOfficersPerk(chref, "BasicCommerce"))
				{
					skillModify -= 0.10;
				}
			}
		}

		costModify = 1.05;
	}
	else // цена продажи товара игроком
	{
		skillModify = 0.675 + _TradeSkill * 0.005;
		if (tradeType == TRADE_TYPE_CANNONS)
			cModify = 2.0 - MOD_SKILL_ENEMY_RATE / 20.0;
		if (CheckCharacterPerk(chref, "HT2"))
		{
			if (CheckOfficersPerk(chref, "AdvancedCommerce"))
				skillModify += 0.20;
			else
			{
				if (CheckOfficersPerk(chref, "BasicCommerce"))
				{
					skillModify += 0.15;
				}
				else
					skillModify += 0.05;
			}
		}
		else
		{
			if (CheckOfficersPerk(chref, "Trader"))
			{
				skillModify += 0.05;
			}
			if (CheckOfficersPerk(chref, "AdvancedCommerce"))
				skillModify += 0.20;
			else
			{
				if (CheckOfficersPerk(chref, "BasicCommerce"))
				{
					skillModify += 0.10;
				}
			}
		}
		costModify = 0.85;
	}

	// boal 23.01.2004 -->
	if (MakeInt(basePrice * tradeModify * skillModify * costModify + 0.5) < 1)
		return 1;
	// boal 23.01.2004 <--

	switch (Type)
	{
	case TRADE_TYPE_NORMAL:
		priceModify = 1.0;
		break;
	case TRADE_TYPE_AMMUNITION:
		priceModify = 1.0;
		break;
	case TRADE_TYPE_EXPORT:
		priceModify = 2.0;
		break;
	case TRADE_TYPE_IMPORT:
		priceModify = 3.0;
		break;
	case TRADE_TYPE_CANNONS:
		priceModify = 1.0;
		break;
	}

	return MakeInt(priceModify * basePrice * tradeModify * skillModify * _qty * costModify * cModify + 0.5);
}

void ContrabandInterruptionSetStatus(bool status)
{
	for (i = 1; i <= 3 + makeint(MOD_SKILL_ENEMY_RATE / 2); i++)
	{
		ref randSmug = &characters[GetCharacterIndex("Rand_Smug0" + i)];
		if (status)
		{
			randSmug.ContrabandInterruption = true;
		}
		else
		{
			DeleteAttribute(randSmug, "ContrabandInterruption");
		}
	}
}

bool Smugglers_IsContraband()
{
	return CheckAttribute(PChar, "quest.Contraband.active") && PChar.quest.Contraband.active;
}

bool Smugglers_Contraband_ShipRequirements()
{
	if (!Smugglers_IsNoCompanions())
	{
		Log_QuestInfo("Есть компаньон, отказ в контрабанде");
		return false;
	}

	if (sti(RealShips[sti(pchar.ship.type)].Class) <= 2)
	{
		Log_QuestInfo("Класс корабля выше 3, отказ в контрабанде");
		return false;
	}

	return true;
}

bool Smugglers_Contraband_IsEnoughRelations(ref NPChar)
{
	if (ChangeContrabandRelation(pchar, 0) < 70)
	{
		Log_QuestInfo("Плохая репутация у контрабандистов");
		return false;
	}

	return true;
}

bool Smugglers_IsTransportation()
{
	return CheckAttribute(PChar, "quest.Transportation.active") && PChar.quest.Transportation.active;
}

bool Smugglers_Transportation_Payed()
{
	if (!CheckAttribute(PChar, "quest.Transportation.payed") || !PChar.quest.Transportation.payed)
	{
		Log_QuestInfo("Мы не оплатили перевозку");
		return false;
	}

	return true;
}

bool Smugglers_Transportation_Requirements()
{
	if (sti(pchar.ship.type) != SHIP_NOTUSED)
	{
		Log_QuestInfo("Есть собственный корабль, отказ в перевозке");
		return false;
	}

	if (IsWithCompanions())
	{
		Log_QuestInfo("У компаньона есть корабль, отказ в перевозке");
		return false;
	}

	if (GetPassengersQuantity(pchar) != 0)
	{
		Log_QuestInfo("У нас есть пассажиры, отказ в перевозке");
		return false;
	}

	return true;
}

bool Smugglers_Transportation_Available()
{
	int nRel = ChangeContrabandRelation(pchar, 0);

	if (Smugglers_IsUnlimitedContra())
		return true;

	if (nRel <= 0 || Statistic_AddValue(PChar, "contr_TransportatationKill", 0) >= 20)
	{
		Log_QuestInfo("Контрабандисты нас не любят, отказ в перевозке");
		return false;
	}

	return true;
}

bool Smugglers_IsUnlimitedContra()
{
	if (CheckCharacterPerk(pchar, "UnlimitedContra"))
	{
		Log_QuestInfo("Сработал перк заядлый контрабандист");
		return true;
	}

	return false;
}

bool Smugglers_Transportation_IsAgentApprove(ref NPChar)
{
	if (bPauseContrabandMetro)
	{
		Log_QuestInfo("Включена пауза перевозок, транспортировка невозможна");
		return false;
	}

	if (!CheckNPCQuestDate(NPChar, "Transportation_Talk"))
	{
		Log_QuestInfo("Сегодня уже разговаривали, нужно подождать день");
		return false;
	}

	return true;
}

bool Smugglers_IsGoodPlaceFor() {
	if (Pchar.quest.().CurrentPlace == "None")
	{
		trace("ОШИБКА: Не нашли доступную бухту для контрабандистов");
		return false;
	}

	return true;
}

bool Smugglers_IsNoCompanions()
{
	if (IsWithCompanions())
	{
		Log_QuestInfo("Есть компаньон, отказ");
		return false;
	}

	return true;
}

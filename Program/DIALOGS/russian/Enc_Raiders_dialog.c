void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, Diag;
	int i;
	string sGroup;
	bool bOk, bOk1;
	int iMoney;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
	int iTemp = sti(npchar.EncQty);
	string sTemp = "Gang" + locations[FindLocation(npchar.location)].index + "_";

	switch(Dialog.CurrentNode)
	{
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
		break;

		case "exit_Robbed":
			ChangeCharacterReputation(pchar, -1);
			//int iMoney = makeint(makeint(Pchar.money)/20)*10;
			iMoney = sti(NPChar.moneyfee);
			AddMoneyToCharacter(pchar, -iMoney);
			AddMoneyToCharacter(npchar, iMoney);
			npchar.SaveItemsForDead   = true; // сохранять на трупе вещи
			npchar.DontClearDead = true;  // не убирать труп через 200с
			AddSimpleRumour(LinkRandPhrase("Вы слышали, у местного грабителя " + GetFullName(npchar) + " появилась очередная жертва. Один капитан струсил и откупился. По слухам отвалил " + FindRussianMoneyString(iMoney) + ", хи-хи... ",
				"Да, капитан " + GetFullName(pchar) + ", я уже слышал, что вам пришлось отдать " + FindRussianMoneyString(iMoney) + ", чтобы откупиться от местного бандита " + GetFullName(npchar) + ". Ну, что я могу сказать - не повезло! Ха-ха-ха!",
				"Вы не слышали о том, что местные грабители напали на какого-то капитана? Так вот, его ограбили на целых " + FindRussianMoneyString(iMoney) + "!"), sti(npchar.nation), 5, 1);
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				sld.LifeDay = 0;
				LAi_SetWarriorTypeNoGroup(sld);
				sld.Dialog.CurrentNode = "OnceAgain";
				LAi_SetCheckMinHP(sld, LAi_GetCharacterHP(sld)-1, true, "LandEnc_RaidersBeforeDialog");
			}
			DialogExit();
		break;

		case "exit_fight":
			sGroup = "RaidersGroup_" + locations[FindLocation(npchar.location)].index;
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_SetWarriorTypeNoGroup(sld);
				LAi_group_MoveCharacter(sld, sGroup);
				LAi_RemoveCheckMinHP(sld);
				LAi_SetImmortal(sld, false);
			}
			LAi_group_SetRelation(sGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups(sGroup, LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck(sGroup, "LandEnc_RaidersAfrer");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "exit_RunFight":
			AddSimpleRumour(LinkRandPhrase(RandPhraseSimple("Вы слышали, наконец-то и на этого бандита " + GetFullName(npchar) + "управа нашлась. Он с дружками хотел ограбить "+ GetSexPhrase("какого-то капитана, но тот оказался","какую-то морячку, но та оказалась") +" им не по зубам. В итоге наш отважный грабитель сам наложил в штаны, ха-ха! Вот и поделом, это ему не из местных фермеров мелочь вытряхивать.",
				"Вот спасибо вам, капитан " + GetFullName(pchar) + ", что вы наконец-то разогнали шайку этого бандита " + GetFullName(npchar) + ". А то совсем житья не стало от этих проходимцев. Иной раз боишься посыльного на ферму за молоком отправить, того и гляди - встретят да деньги и отберут."),
				"Вы слышали, "+ GetSexPhrase("наконец-то какой-то капитан дал","какая-то морячка дала") +" достойный отпор этому головорезу " + GetFullName(npchar) + ", что орудовал в джунглях и наводил страх на местных фермеров. Интересно, надолго ли это отобьёт у них охоту разбойничать.",
				"Вы не слышали о том, что местный бандит " + GetFullName(npchar) + " со своими прихвостнями пытался ограбить "+ GetSexPhrase("какого-то капитана","какую-то девушку-капитана") +"? Так вот, не тут то было - капитан оказал"+ GetSexPhrase("ся","ась") +" крепким орешком. Говорят, он"+ GetSexPhrase("","а") +" гонял"+ GetSexPhrase("","а") +" этих горе-грабителей по джунглям, как акула стаю сардин. Спасибо Господу, что есть ещё люди, способные постоять за нас в такое время, когда власти бездействуют, слишком занятые своими делишками..."), sti(npchar.nation), 5, 1);
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_type_actor_Reset(sld);
				LAi_ActorAfraid(sld, Pchar, true);
				sld.Dialog.CurrentNode = "GetTheHellOut";
			}
			DialogExit();
		break;

		case "exit_noFight":
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				sld.LifeDay = 0;
				LAi_SetWarriorTypeNoGroup(sld);
				LAi_SetCitizenTypeNoGroup(sld);
				//#20200315-01
                sld.chr_ai.type.checkFight = 999999.0;
				sld.Dialog.CurrentNode = "GetLost";
				LAi_SetCheckMinHP(sld, LAi_GetCharacterHP(sld)-1, true, "LandEnc_RaidersBeforeDialog");
			}
			DialogExit();
		break;

		case "First Time":
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_type_actor_Reset(sld);
				LAi_RemoveCheckMinHP(sld);
				LAi_SetImmortal(sld, false);
			}
			bool bRape = GetOfficersQuantity(pchar) == 0 || rand(4) == 0;
			if (pchar.sex == "woman" && bRape)
			{
				dialog.text = RandPhraseSimple(LinkRandPhrase("Ну надо же, какая красотка бродит по джунглям! ",
				"Вот это мне повезло сегодня, наконец-то с бабой перепадёт! ",
				"Не так быстро, дорогуша, одно дело к тебе есть! ")+
				LinkRandPhrase("Надеюсь, ты не против, если мы с тобой немного развлечёмся? Скажу сразу, отказов я не принимаю.",
				"Давай по-хорошему: раздевайся, сделаем дело и пойдёшь себе дальше.",
				"Ты же не думала, что сможешь просто так мимо меня пройти? Сначала тебе придётся познакомиться с моим маленьким другом!"),

				LinkRandPhrase("Лопните мой глаза! Дамочка, а вам известно, что вы сегодня выходите замуж?  Нечаянно и ненадолго...",
				"Стой, милаха, торопиться никуда не надо, свою судьбу ты уже нашла. Моему дружку ты понравилась, если глупить не будешь понравишься и мне тоже... Бросай железяку, начнём знакомиться поближе, кроха.",
				"Ну что, цыпа, от тоски что ли в лес подалась? Мы веселить умеем! Кто последний тот и папа, парни!"))

				Link.l1 = "Ладно, делай своё дело, сволочь."
				Link.l1.go = "Rape";
				if (GetOfficersQuantity(pchar) > 0)
				{
					Link.l1 = LinkRandPhrase("Ну уж нет, отдаваться я тебе не стану, да и моя охрана этого не позволит. ",
					"Ты бы завязал с этими грязными мыслишками, мои абордажники всё равно в обиду меня не дадут. ",
					"Ты мог бы попробовать меня взять, но подумай, стоит ли рисковать ради такого своей жизнью в бою с моими офицерами? ")+
					LinkRandPhrase("Давай лучше я заплачу тебе, деньгами, и мы разойдёмся без кровопролития.",
					"Взамен, как насчёт небольшой денежной суммы, чтоб мы разошлись мирно?",
					"Сколько денег ты хочешь чтобы пропустить нас?")
					Link.l1.go = "Node_2_woman";
				}
				Link.l2 = RandPhraseSimple("Не на ту напал, мразь. Я тебе не хрупкая девчонка, могу и постоять за себя.",
				"Повтори-ка это в дуло моего пистолета, похотливый павиан!",
				"Да ты никак танцором стать решил, но кое-что мешает? Ну подходи, мой клинок это мигом исправит.");
				Link.l2.go = "CheckSkills";
				break;
			}
			dialog.text = LinkRandPhrase(LinkRandPhrase("Стой! Бросай оружие! Выкладывай золото, или подохнешь на месте!",
				"Ни с места, "+ GetSexPhrase("путник","девчонка") +"! Оружие не трогай и не вздумай шутить, если жизнь дорога!",
				"Стой, "+ GetSexPhrase("путник","девчонка") +"! Это платная дорога! Хочешь пройти - давай пошлину."),
				LinkRandPhrase("Эй, "+ GetSexPhrase("приятель","подруга") +"! Не так быстро! Хочу взглянуть, не слишком ли тяжёл твой кошель.",
				"Подойди-ка поближе, любезн"+ GetSexPhrase("ый","ая") +". Хочу услышать, как позвякивает золото в твоём кошеле.",
				"Погоди-ка, "+ GetSexPhrase("приятель","подруга") +". Мы тут с дружками поспорили, много ли золота влезет в твой кошель. Теперь нужно бы проверить, пока до драки не дошло."),
				"Не торопись, "+ GetSexPhrase("прохожий","дорогуша") +"! Хочу потолковать с тобой по душам.");
			Link.l1 = LinkRandPhrase(LinkRandPhrase("Что ты несёшь?","В чем дело?", "О чём это ты?"), "Что ты имеешь в виду?", "К чему это ты?");
			Link.l1.go = "Node_2";

			Link.l2 = LinkRandPhrase(LinkRandPhrase("Никак разбойники?! Вот сейчас и разомнёмся!",
				"Разбойники, что ль?! Небось, от правосудия бегаете? А вот правосудие к вам само пришло...",
				"А разве тебя не учили, что нехорошо приставать к "+ GetSexPhrase("незнакомым","девушкам") +" с разными глупостями? Придётся преподать урок..."),
				LinkRandPhrase("Ах ты, наглец! Надеюсь, твой клинок столь же остр, как и язык.",
				"Вижу, язык у тебя острый. Надеюсь, клинком владеешь так же мастерски?",
				"А-а, разбойнички! Вешают вас, вешают, а вы всё не унимаетесь..."),
				LinkRandPhrase(RandPhraseSimple("А-а, разбойники! У меня с такими, как вы разговор короткий!",
				"Ну, наглец! От тебя воняет за версту, а ты к прилично"+ GetSexPhrase("му человеку","й девушке") +" с разговорами лезешь."),
				"С такими как ты, обычно говорит мой пистолет!",
				"Опять разбойники! Ну, виселицу я вам не обещаю, а по паре дырок в брюхе проделать могу!"));
			Link.l2.go = "CheckSkills";
			Link.l3 = LinkRandPhrase("А тебе известно, что за мною по пятам идёт патрульный отряд? Стоит мне только свистнуть, и вы покойники.",
				RandPhraseSimple("Не боишься, что я патруль позову? Они как раз здесь неподалёку вас ищут.",
				"За вами уже патрульный отряд послали, он с минуты на минуту здесь будет, так что вы очень рискуете."),
				RandPhraseSimple("Вам бы лучше побыстрее уносить ноги. Сюда направляется патрульный отряд, я только что разговаривал"+ GetSexPhrase("","а") +" с их офицером.",
				"Я бы с радостью продолжил"+ GetSexPhrase("","а") +" разговор, но боюсь, патрульный отряд, который я только что встретил"+ GetSexPhrase("","а") +", не даст перерасти нашему знакомству в тесную дружбу."));
			Link.l3.go = "Node_3";
		break;

		case "Node_2":
			iMoney = 100*MOD_SKILL_ENEMY_RATE*sti(NPChar.EncQty)+makeint(100 * sti(PChar.rank))+makeint(100 * sti(NPChar.rank)*sti(NPChar.EncQty));
			NPChar.moneyfee = iMoney;
			dialog.text = LinkRandPhrase(LinkRandPhrase("Не прикидывайся простофилей! Выкладывай своё золото, тогда, может, и отпущу!",
				"Ты что, о дорожных издержках не слышал"+ GetSexPhrase("","а") +"? Не расстанешься со своим золотом - расстанешься с головой!",
				"Хех, это обыкновенное дорожное приключение, которое будет стоить тебе кошелька... это, если я не рассержусь."),
				RandPhraseSimple("Всё очень просто - ты добровольно отдаёшь нам золото и уходишь, или ты остаёшься здесь, а мы уходим с твоим золотом, но второй вариант тебе вряд ли понравится, хе-хе.",
				"А ты будто не смекаешь. Кошель отвязывай! если не хочешь, чтобы я его с твоего трупа снимал!"),
				"Поясняю, раз ты так"+ GetSexPhrase("ой","ая") +" непонятлив"+ GetSexPhrase("ый","ая") +" - выкладывай все деньги, если жить хочешь!");
				dialog.text = dialog.text+ "\nДумаю ты не обеднеешь, отдав "+iMoney+" пиастров. Примерно по "+makeint(iMoney/sti(NPChar.EncQty))+" на брата.";
			//Link.l1 = "Проклятье! Мерзавец! У меня с собой только " + makeint(makeint(Pchar.money)/20)*10 + " пиастров.";
			if(sti(pchar.money) >= sti(NPChar.moneyfee))
			{
				Link.l1 = "Проклятье! Мерзавец! Забирай свои деньги!";
			}
			else
			{
				Link.l1 = "У меня нет столько денег.";
			}
			Link.l1.go = "CheckMoney";
			Link.l2 = LinkRandPhrase(LinkRandPhrase("Денег захотел? Так подойди и возьми, а я погляжу, сколько пенсов ты стоишь.",
				"Да как ты посмел, невежа! Придётся дать тебе урок хороших манер!",
				"Какая самонадеянность! Что ж, проверим, по зубам ли сухопутным крысам морск"+ GetSexPhrase("ой офицер","ая волчица") +"!"),
				LinkRandPhrase("Да вас бы плетьми отходить за такую дерзость!",
				"Ах вы канальи! Ну, тогда молитесь своим чертям!",
				"Вас, висельников, следовало бы вздёрнуть на площади! Да видать придётся шпагу марать!"),
				RandPhraseSimple("А с чего ты решил, что я стану перед тобой кошелем трясти?",
				"А тебя не смущает, что я неплохо вооружён"+ GetSexPhrase("","а") +" и в джунгли не спросонок выбежал"+ GetSexPhrase("","а") +"?"));
			Link.l2.go = "CheckSkills";
		break;

		case "Node_2_woman":
			iMoney = 100*MOD_SKILL_ENEMY_RATE*sti(NPChar.EncQty)+makeint(100 * sti(PChar.rank))+makeint(100 * sti(NPChar.rank)*sti(NPChar.EncQty));
			NPChar.moneyfee = iMoney;
			dialog.text = "Думаю ты не обеднеешь, отдав "+iMoney+" пиастров. Примерно по "+makeint(iMoney/sti(NPChar.EncQty))+" на брата.";
			//Link.l1 = "Проклятье! Мерзавец! У меня с собой только " + makeint(makeint(Pchar.money)/20)*10 + " пиастров.";
			if(sti(pchar.money) >= sti(NPChar.moneyfee))
			{
				Link.l1 = "Проклятье! Мерзавец! Забирай свои деньги!";
			}
			else
			{
				Link.l1 = "У меня нет столько денег.";
			}
			Link.l1.go = "CheckMoney";
			Link.l2 = RandPhraseSimple("Сколько? Ну это уже наглость, нам придётся тебя за это проучить.",
				"А я ведь надеялась на твоё благоразумие. С такими расценками нам гораздо проще тебя убить!");
			Link.l2.go = "CheckSkills";
		break;

		case "Node_3":
			bOk = makeint(pchar.reputation) < 11 || makeint(pchar.reputation) > 90;
			if(bOk || GetSummonSkillFromName(pchar, SKILL_LEADERSHIP) == 100 || CheckCharacterPerk(pchar, "Trustworthy") || CheckOfficersPerk(pchar, "SeaDogProfessional"))
			{
				dialog.text = RandPhraseSimple("Вот чёрт! Ладно, бывай. И не вздумай шум поднимать.",
					"На этот раз тебе повезло, но в следующий - будешь долж"+ GetSexPhrase("ен","на") +" вдвойне! Не забудь напомнить, хе-хе.");
				Link.l1 = RandPhraseSimple("Ну, как можно?", "Давай-давай, беги, пока цел.");
				Link.l1.go = "Exit_NoFight";
			}
			else
			{
				bOk1 = makeint(pchar.reputation) < 51 && makeint(pchar.reputation) > 41;
				if(!bOk || GetSummonSkillFromName(pchar, SKILL_LEADERSHIP) < 35)
				{
					dialog.text = RandPhraseSimple("А мы тебя тихонько прирежем, даже пикнуть не успеешь.",
						"Вот чёрт! Придётся прирезать тебя по-быстрому, пока шуму не поднял"+ GetSexPhrase("","а") +".");
					Link.l1 = "Ну, как знаете. Я вас предупредил"+ GetSexPhrase("","а") +".";
					Link.l1.go = "Exit_Fight";
				}
				else
				{
					dialog.text = RandPhraseSimple("А что мне твой патруль? У меня здесь всё схвачено и за всё заплачено. Так что выворачивай кошель и поменьше языком телипай.",
						"Ты кого напугать решил"+ GetSexPhrase("","а") +"?! Я в этом лесу сам патруль для таких, как ты. Ещё ни один без пошлины не ушёл!");
					Link.l1 = "Проклятье! Мерзавец! У меня с собой только " + makeint(makeint(Pchar.money)/20)*10 + " пиастров.";
					Link.l1.go = "CheckMoney";
					Link.l2 = RandPhraseSimple(LinkRandPhrase("Хм, не по чину тебе мною командовать.",
						"За подобную дерзость я сделаю в твоей башке пару дырок. Чтоб мозги проветрились.",
						"Такая наглость не может остаться безнаказанной!"),
						"Эх, не за своё дело ты взялся...");
					Link.l2.go = "CheckSkills";
				}
			}
		break;

		case "CheckSkills":
			bool z_ok = (GetCharacterSkillToOld(Pchar, "Fencing") >= 7) && (makeint(Pchar.Rank) >= 8) && (Makeint(PChar.reputation) <= 30);
			if (z_ok || CheckOfficersPerk(pchar, "SeaDogProfessional"))
			{
				Diag.TempNode = "GetLost";
				dialog.text = LinkRandPhrase(RandPhraseSimple("Эй, не нервничай так, "+ GetSexPhrase("приятель","подруга") +"! Я пошутил! Иди своей дорогой!",
					"Ладно, я передумал. Можешь идти, раз так"+ GetSexPhrase("ой грозный","ая грозная") +"."),
					"Что, опять "+ GetSexPhrase("'отважный капитан'","'бойцовская девка'") +"? Ладно, иди с миром...",
					"Потише, "+ GetSexPhrase("приятель","подруга") +". Не видишь, обознались мы"+ GetSexPhrase(" - за купца тебя приняли","") +". Иди от греха подальше.");
				Link.l1 = LinkRandPhrase(LinkRandPhrase("Ну что же, тогда до встречи!",
					"Бросил бы ты это дело, пока шкура цела.",
					"Ну смотрите, не попадайтесь мне больше."),
					RandPhraseSimple("В следующий раз смотри, на кого рот открываешь.",
					"Благодарите Бога, что я сегодня добр"+ GetSexPhrase("ый","ая") +"."),
					RandPhraseSimple("Такой промысел вас до добра не доведёт. Прощайте.",
					"Что ж, это мудрое решение. Счастливо оставаться!"));
				Link.l1.go = "Exit_NoFight";
				Link.l99 = LinkRandPhrase("Нет уж, приятель. Придётся поучить тебя хорошим манерам.",
					"Никак струсил?! Но за свои поступки нужно отвечать! Не правда ли?",
					"Нет уж! Теперь я не успокоюсь, пока не испорчу тебе шкуру.");
				Link.l99.go = "GetTheHellOut";
			}
			else
			{
				dialog.text = LinkRandPhrase("А ведь мог"+ GetSexPhrase("","ла") +" бы уйти с миром! Теперь пеняй на себя!",
								"Зря ты это затеял"+ GetSexPhrase("","а") +", каналья! Теперь подохнешь, как собака!",
								"Ах так! Я порву тебя на куски! Вырежу печень и скормлю собакам!");
				Link.l1 = RandPhraseSimple("Я заткну эти слова тебе в глотку!","Ты кому угрожаешь, отребье?!");
				Link.l1.go = "Exit_Fight";
			}
		break;

		case "CheckMoney":
			if(sti(pchar.money) >= sti(NPChar.moneyfee))
			{
				Diag.TempNode = "OnceAgain";
				dialog.text = LinkRandPhrase(RandPhraseSimple("Хорошо! Давай их сюда и проваливай!",
					"Негусто, но лучше, чем ничего. Приятно иметь дело с умным человеком! Можешь идти."),
					"Вот - это другое дело! Как говорил один мой знакомый - приятно послушать умного человека, а с дураком можно и поговорить, - хе-хе.",
					"В обмен на твоё золото дам совет - не стоит шляться по джунглям, "+ GetSexPhrase("если ты такой слабак! Сиди в портовой таверне, и пей свой ром - целее будешь","девушка. А то ведь не только ограбить могут") +".");
				Link.l1 = "Проклятые висельники!";
				Link.l1.go = "Exit_Robbed";
			}
			else
			{
				dialog.text = "Сдаётся, ты хочешь меня надуть! Но ничего, сейчас я тебя пощекочу - глядишь, чего и вывалится.";
				Link.l1 = "Проклятье!";
				Link.l1.go = "Exit_Fight";
			}
		break;

		case "Rape":
			AddCharacterExpToSkill(pchar, "Leadership", -100);
            AddCharacterExpToSkill(pchar, "Fencing", 50);
            AddCharacterExpToSkill(pchar, "Pistol", 50);
            AddCharacterHealth(pchar, -10);
			Diag.TempNode = "OnceAgainRape";
			AddDialogExitQuest("PlaySex_1");
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				sld.LifeDay = 0;
				LAi_SetWarriorTypeNoGroup(sld);
				sld.Dialog.CurrentNode = "OnceAgainRape";
				LAi_SetCheckMinHP(sld, LAi_GetCharacterHP(sld)-1, true, "LandEnc_RaidersBeforeDialog");
			}
			DialogExit();
		break;

		case "OnceAgainRape":
			Diag.TempNode = "OnceAgainRape";
			dialog.text = LinkRandPhrase("Мы закончили, можешь идти, куда шла.",
				"Надеюсь, мы стобой ещё увидимся, хе-хе. А пока, можешь идти.",
				"Если не хочешь на второй заход, лучше вали подобру-поздорову.");
			Link.l1 = "Уже ухожу.";
			Link.l1.go = "Exit";
			Link.l2 = "Ну уж нет, я уйду только после того, как ты сдохнешь, тварь!";
			Link.l2.go = "Exit_Fight";
		break;

		case "OnceAgain":
			Diag.TempNode = "OnceAgain";
			dialog.text = LinkRandPhrase("Снова ты? Давай, беги домой, к маме! А то ведь я могу и рассердиться.",
				"Что? Не всё золото отдал"+ GetSexPhrase("","а") +"? Ха-ха-ха!",
				"Слушай, "+ GetSexPhrase("приятель","подруга") +", ты мне надоел"+ GetSexPhrase("","а") +"! Убирайся с глаз долой, пока жив"+ GetSexPhrase("","а") +"!");
			Link.l1 = "Да, я уже ухожу.";
			Link.l1.go = "Exit";
			Link.l2 = "Я подумал"+ GetSexPhrase("","а") +", что так будет несправедливо. Вот, решил"+ GetSexPhrase("","а") +" уравнять издержки...";
			Link.l2.go = "Exit_Fight";
		break;

		case "GetLost":
			Diag.TempNode = "GetLost";
			dialog.text = LinkRandPhrase("Что тебе нужно? Мы же договорились, что ты уходишь!",
				"Отстань от меня, "+ GetSexPhrase("парень","деваха") +". У меня свои дела, у тебя свои, нам не стоит продолжать знакомство!",
				"Уходи, "+ GetSexPhrase("приятель","подруга") +"! Чёрт возьми, неужели ты всё-таки хочешь отдать свои деньги?");
			Link.l1 = "Да, я уже ухожу.";
			Link.l1.go = "Exit";
			Link.l2 = "Я вспомнил"+ GetSexPhrase("","а") +", что вы не попрощались! Так что давайте... прощайтесь!";
			Link.l2.go = "Exit_Fight";
		break;
        //#20200315-01
        case "CitizenNotBlade":
            npchar.chr_ai.type.checkFight = 999999.0;
            Diag.TempNode = "GetTheHellOut";
			dialog.text = LinkRandPhrase("Я всё-таки решил"+ GetSexPhrase("","а") +" наказать вас за ваши злые дела!",
				"Отстань! Убирайся! Я хочу жить!",
				"Помогите! "+ GetSexPhrase("Он маньяк","Она маньячка") +"! Не убивай меня!");
			Link.l1 = "Пощади! Я всего лишь скромный разбойник! Я слишком молод, чтобы умереть!";
			Link.l1.go = "Exit";
        break;
		case "GetTheHellOut":
			Diag.TempNode = "GetTheHellOut";
			dialog.text = LinkRandPhrase("Отстань! Убирайся! Я хочу жить!",
				"Помогите! "+ GetSexPhrase("Он маньяк","Она маньячка") +"! Не убивай меня!",
				"Пощади! Я всего лишь скромный разбойник! Я слишком молод, чтобы умереть!");
			Link.l1 = LinkRandPhrase(LinkRandPhrase("Сидел бы дома - жил бы дольше!",
				"Раньше нужно было думать!",
				"Ты знал, на что идёшь!"),
				"Рано или поздно это должно было случиться.",
				"Поздно каяться, защищайтесь!");
			Link.l1.go = "exit_Fight";
		break;
	}
}

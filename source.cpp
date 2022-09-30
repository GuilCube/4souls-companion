#include <iostream>
#include <time.h>
#include <cstdlib>
#include <string>
#define NOTFOUND -1
using namespace std;

enum Action
{
    BACKTOCARDCHOOSE,
    ROLLDICE,
    DONTROLLDICE
};

class Card   //Карточки
{
private:
    string Effect, CardName_str;
    int SuccessRoll;
    bool onlineStatus;
    string activationTime;
    int ID;

public:
    static int ID_static;


    //перегрузка конструктора класса для создание массивов
    Card()
    {
        onlineStatus =false, SuccessRoll = 0;
        Effect = activationTime = CardName_str = "";
    }
    Card(string Effect, int SuccessRoll)
    {
        this->Effect = Effect;
        this->SuccessRoll = SuccessRoll;
        onlineStatus = false, activationTime="";
        ID = ++ID_static;
    }
    Card(string Effect, string activationTime)
    {
        this->Effect = Effect;
        this->activationTime = activationTime;
        onlineStatus = false, SuccessRoll = 0;
        ID = ++ID_static;
    }

    //перегрузка операторов для сравнения и приравнивания карточек
    bool operator !=(const Card& Other)
    {
        return!(this->CardName_str == Other.CardName_str);
    }
    bool operator ==(const Card& Other)
    {
        return(this->CardName_str == Other.CardName_str);
    }
    Card& operator =(const Card& Other)
    {
        this->CardName_str = Other.CardName_str;
        this->Effect = Other.Effect;
        this->SuccessRoll = Other.SuccessRoll;
        this->onlineStatus = Other.onlineStatus;
        this->activationTime = Other.activationTime;
        this->ID = Other.ID;
        return *this;
    }

    //геттеры
    string GetActivationTime()
    {
        return activationTime;
    }
    string GetEffect()
    {
        return Effect;
    }
    string GetCardName()
    {
        return CardName_str;
    }
    bool isOnline()
    {
        return onlineStatus;
    }
    int GetSuccessRoll()
    {
        return SuccessRoll;
    }
    int GetID()
    {
        return ID;
    }
    string RollCheck(const int Roll);

    //сеттеры
    void SetEffect(string Effect)
    {
        this->Effect = Effect;
    }
    void SetOnlineStatus(bool onlineStatus)
    {
        this->onlineStatus = onlineStatus;
    }
    void SetCardName(string newCardName)
    {
        this->CardName_str = newCardName;
    }

};


int Card::ID_static = 0;

//Запись названий из массива в класс
void NameCards(Card* Destination, string* NameStorage, const int size)
{
    for (int x = 0; x != size; x++)
        Destination[x].SetCardName(NameStorage[x]);
}

/*##############################################################################
                            Всё, что связано с кубиком
##############################################################################*/

//павезло или не?)
string Card::RollCheck(const int Roll)
{
    if (!isOnline() || GetSuccessRoll() == 0)
        return "No Effect";
    if (Roll == GetSuccessRoll())
        return GetEffect();
    else
        return "Fail";
}

/* Запрос на бросок кубика:
 1: кинуть кубик
 0: завершить ход
 -1: перейти к выбору карт */
Action RequestDiceRoll()
{
    string answer;
    cout << "\nКинуть кубик?(0|1|2) ";
    cin >> answer;
    if (answer == "1" || answer == "y" || answer == "Y")
        return Action::ROLLDICE;
    if (answer == "2" || answer == "N" || answer == "n")
        return Action::BACKTOCARDCHOOSE;
    if (answer == "0" || answer == "Stop")
        return Action::DONTROLLDICE;
    cout << "Неправильно! Попробуй ещё раз :) " << endl;
    return RequestDiceRoll();
}

//Бросок кубика
int DiceRollResult()
{
    return rand() % 6 + 1;
}

/*##############################################################################
                             Работа со столом
##############################################################################*/

//добавляет одну карту на стол
void AddCard(Card*& Reciever, Card CardAdded, int& size)
{
    int CardAddedID = CardAdded.GetID();
    Card* UpdatedTable = new Card[size + 1];
    bool wasSorted = false;
    for (int i = 0, j = 0; i != size;)
    {
        if (Reciever[j].GetID() > CardAddedID && !wasSorted)
        {
            UpdatedTable[i++] = CardAdded;
            wasSorted = true;
            ++size;
            continue;
        }
        UpdatedTable[i++] = Reciever[j++];
    }
    if (!wasSorted)
        UpdatedTable[size++] = CardAdded;

    delete[] Reciever;
    Reciever = UpdatedTable;
}

//убирает одну карту со стола
void RemoveCard(Card*& OnTable, Card CardRemoved, int& size)
{
    // CardRemoved.SetOnlineStatus(false);
    Card* UpdatedTable = new Card[size - 1];
    for (int i = 0, j = 0; j != size; )
    {
        if (OnTable[j] == CardRemoved)
        {
            ++j;
            continue;
        }
        UpdatedTable[i++] = OnTable[j++];
    }
    --size;
    delete[] OnTable;
    OnTable = UpdatedTable;
}

//очищает стол
void ClearTable(Card*& OnTable, int& size, Card*& CardList)
{
    for (int i = 0; i < size; i++)
        CardList[OnTable[i].GetID()-1].SetOnlineStatus(false);
    Card* EmptyTable = new Card[0];
    delete[] OnTable;
    OnTable = EmptyTable;
    size = 0;
}

//проверяет, есть ли такая карта; вернёт 0, если такой карты нету
int FindCard(Card* Array, const int size, int InputSearchByID)
{
    if (--InputSearchByID < size && InputSearchByID >= 0)
        return InputSearchByID;
    return NOTFOUND;
}

//проверяет, есть ли такая карта; вернёт 0, если такой карты нету
int FindCard(Card* Array, const int size, string InputSearchByName)
{
    //если получили число, то  
    int InputSearchByID = atoi(InputSearchByName.c_str());
    if (InputSearchByID != 0)
        return FindCard(Array, size, InputSearchByID);

    //поиск соответсвующей вводу карты (название)
    for (int i = 0; i < size; i++)
    {
        if (Array[i].GetCardName() == InputSearchByName)
            return i;
    }
    return NOTFOUND;
}

//есть ли на столе ходовые карты
bool isTurnBasedCardsOnline(Card* Array, const int size)
{
    if (Array[size - 1].GetSuccessRoll() == 0)
        return true;
    return false;
}

/*##############################################################################
                                    Вывод эффектов
##############################################################################*/

//Вывод полной инфы всех активных карт.
void WriteOnlineCardsInfo(Card* Array, const int size)
{
    //вывод названия, айдишек и значения для срабатывания эффекта всех активных карт
    cout << "\nКарты на столе: ";
    for (int x = 0; x != size; x++)
    {
        if (Array[x].GetSuccessRoll() != 0)
        {
            cout << "id[" << Array[x].GetID() << "]" << Array[x].GetCardName()
                + "(" << Array[x].GetSuccessRoll() << ")" << (x == size - 1 ? "" : ", ");
        }
        else
        {
            cout << "id[" << Array[x].GetID() << "]" << Array[x].GetCardName()
                + "(" << Array[x].GetActivationTime() << ")" << (x == size - 1 ? "" : ", ");
        }
    }
    cout << ".\n\n";
}

//Вывод эффектов активных карт
void WriteSuccessEffect(Card* Array, const int size, const int Roll)
{
    cout << "Выпало: " << Roll << endl;
    bool effectShown = false, isBasicCardsOnline = false;
    string RollEffect; //для того, чтобы не вызывать функцию 3 раза
    for (int i = 0; i != size; ++i)
    {
        if (Array[i].GetSuccessRoll() == 0)
        {
            break;
        }
        RollEffect = Array[i].RollCheck(Roll);
        isBasicCardsOnline = true;
        //вывод эффекта при удачном броске
        if (!(RollEffect == "Fail"))
        {
            cout << RollEffect << endl;
            effectShown = true;
        }
    }
    //если ничего не прокнулось 
    if (!isBasicCardsOnline)
    {
        cout << "На столе нету подходящих карт\n";
        return;
    }
    if (!effectShown)
        cout << "Не повезло :(\n";
}

//выводит эффекты карт, зависящих от хода
void WriteTurnBasedCardsEffect(Card* Array, const int size)
{
    bool endTurn = false;
    int ActiveStartTurnCard = 0, FirstEndTurnCardID = 0;

    //проверка на наличие соответствующих карт на столе
    cout << "Завершить ход? (0|1) ";
    cin >> endTurn;
    if (!endTurn)
        return;
    for (int i = size - 1; i >= 0; --i)
    {
        if (Array[i].GetSuccessRoll() != 0)
        {
            FirstEndTurnCardID = i;
            break;
        }
    }
    for (int i = FirstEndTurnCardID; i != size; ++i)
    {
        if (Array[i].GetActivationTime() == "end")
        {
            cout << endl << Array[i].GetEffect() << endl;
            FirstEndTurnCardID = i;
        }
        else
            ActiveStartTurnCard = i;
    }
    cout << "Ход окончен!\n";

    if (ActiveStartTurnCard != 0)
    {
        for (int i = ActiveStartTurnCard; i != size; ++i)
        {
            if (Array[i].GetActivationTime() == "start")
                cout << endl << Array[i].GetEffect() << endl;
        }
    }
}

/*##############################################################################
                                   Ведущие функции
##############################################################################*/

/*Активация карт через консоль вводя числа или названия карт*/
void CardManagement(Card* Array, const int size, Card*& OnTable, int& AmmountOfActivatedCards)
{
    string inputName;
    int inputID;
    //Card RequestedCard;
    while (true)
    {
        cout << "Какие карты на столе? ";
        cin >> inputName;
        if (inputName == "Clear" || inputName == "clear" || inputName == "c")
        {
            ClearTable(OnTable, AmmountOfActivatedCards, Array);
            cout << "Стол очищен!" << endl;
            continue;
        }
        //выход с функции с последующим запросом на бросок кубика
        if (inputName == "0")
            break;

        inputID = FindCard(Array, size, inputName);

        if (inputID == NOTFOUND)
        {
            cout << "Неправильно! Попробуй ещё раз :) " << endl;
            continue;
        }

        if (inputID == 18 || inputID == 19)
        {
            //добавление или изъятие карт со стола
            Array[18].SetOnlineStatus(!Array[18].isOnline());
            Array[19].SetOnlineStatus(!Array[19].isOnline());

            Array[18].isOnline() ? AddCard(OnTable, Array[18], AmmountOfActivatedCards),
                AddCard(OnTable, Array[19], AmmountOfActivatedCards) :
                RemoveCard(OnTable, Array[18], AmmountOfActivatedCards),
                RemoveCard(OnTable, Array[19], AmmountOfActivatedCards);

            //показать в консоль
            string a = Array[18].isOnline() ? "добавлены.\n\n" : "убраны со стола\n\n";
            cout << "Карты №18 и №19 (Cursed Eye) " + a;
        }
        else
        {
            //Card& RequestedCard = *(Array+inputID); //шо б 100 раз не обращаться к массиву

                //добавление или изъятие карт со стола 
            Array[inputID].SetOnlineStatus(!Array[inputID].isOnline());

            Array[inputID].isOnline() ? AddCard(OnTable, Array[inputID], AmmountOfActivatedCards) :
                RemoveCard(OnTable, Array[inputID], AmmountOfActivatedCards);

            //показать в консоль
            string a = Array[inputID].isOnline() ? " добавленa.\n\n" : " убранa со стола\n\n";
            cout << "Картa №" << inputID + 1 << ": " << Array[inputID].GetCardName() + a;
            //Array[inputID] = Array[inputID];
        }
    }
}


//Запуск помощника
void LoadDiceCardReminder(Card* Array, int size)
{
    int AmmountOfActivatedCards = 0, FirstTBCardID = 0;
    Action action;

    //массив карточек на столе
    Card* OnTable = new Card[AmmountOfActivatedCards];

    //первоначальный ввод айди карточек затем бросок кубика
    for (CardManagement(Array, size, OnTable, AmmountOfActivatedCards);;)
    {
        //переход на режим вкл/выкл карт (Пустой стол)
        if (AmmountOfActivatedCards == 0)
        {
            cout << "\nСтол пустой, как твоя голова!\n";
            CardManagement(Array, size, OnTable, AmmountOfActivatedCards);
            continue;
        }

        action = RequestDiceRoll();

        //  переход на режим вкл/выкл карт (Кинуть кубик=2)
        if (action == Action::BACKTOCARDCHOOSE)
        {
            CardManagement(Array, size, OnTable, AmmountOfActivatedCards);
            continue;
        }

        // вывод эффектов
        if (action == Action::ROLLDICE)
        {
            WriteOnlineCardsInfo(OnTable, AmmountOfActivatedCards);
            WriteSuccessEffect(OnTable, AmmountOfActivatedCards, DiceRollResult());
            continue;
        }

        /*условие завершения хода с ходовыми карточками(Кинуть кубик = 0),
            если таковых нету, то переход на режим вкл/выкл */
        if (isTurnBasedCardsOnline(OnTable, AmmountOfActivatedCards))
            WriteTurnBasedCardsEffect(OnTable, AmmountOfActivatedCards);
        else
            CardManagement(Array, size, OnTable, AmmountOfActivatedCards);
    }
    delete[] OnTable;
}

/*##############################################################################

##############################################################################*/


int main()
{
    setlocale(LC_ALL, "Russian");
    //обьявление карточек

    string pref = "Когда любой игрок выбрасывает ";
    Card Finger("(Палец): " + pref + "2, ты можешь украсть предмет у игрока, который выбросил 2, но затем отдай ему свой предмет.", 2);
    Card TarotCloth("(Сукно для таро): " + pref + "4, этот игрок должен отдать тебе 1 добычу на свой выбор.", 4);
    Card SackOfPennies("(Мешочек с мелочью): " + pref + "1, ты можешь перезарядить этот предмет.", 1);
    Card SpiderMod("(Паучий модуль): " + pref + "5, сбрось активного монстра, которого не атакуют, но затем замени его топ деком.", 5);
    Card DadsLostCoin("(Папина потеряная монета):" + pref + "1, ты можешь заставить этого игрока перебросить кубик.", 1);
    Card GameBreakingBug("(Ломающий игру баг): " + pref + "1, ты можешь сбросить предмет этого игрока, но затем замени его топ деком.", 1);
    Card DeadBird("(Дохлая птичка): " + pref + "3, ты можешь посмотреть его руку и украсть 1 добычу.", 3);
    Card CheeseGrater("(Сыротёрка): " + pref + "6, покажи топ дек любой колоды всем игрокам, сбрось или положи её обратно.", 6);
    Card MomsBox("(Мамина шкатулка): " + pref + "4, ты можешь взять 1 добычу, но затем сбрось 1 добычу.", 4);
    Card EyeOfGreed("(Око алчности): " + pref + "5, получи 3с.", 5);
    Card TheMissingPage("(Недостающая страница): " + pref + "5, ты можешь заставить этого игрока перебросить кубик.", 5);
    Card HourGlass("(Песочные часы): " + pref + "2, ты можешь деактевировать любую активку.", 2);
    Card TheRelic("(Реликвия): " + pref + "1, возьми 1 добычу.", 1);
    Card SacredHeart("(Священное сердце): Ты можешь изменить этот результат броска на 6. ", 1);
    Card MomsRazor("(Мамина бритва): " + pref + "6, ты можешь нанести ему 1 урона.", 6);
    Card TheD10("(D10):" + pref + "3, ты можешь положить топ дек колоды монстров в активный слот, который не атакуют.", 3);
    Card ChargedBaby("(Заряженный малыш): " + pref + "2, ты можешь перезарядить любой предмет.", 2);
    Card CursedEye1("(Проклятое око): При броске атаки на 6 ты получаешь + 3 урона к этому броску атаки.", 6);
    Card CursedEye2("(Проклятое око): При броске атаки на 1 в свой ход, отмени атаку и заверши ход.", 1);
    Card Infamy("(Позор): При броске атаки на 1 ты не получаешь урон.", 1);
    Card DarkArts("(Тёмные искусства): " + pref + "6, получи 2с.", 6);
    Card Ipecac("(Рвотный корень): Про броске атаки на 6, ты наносишь 1 урона всем игрокам.", 6);
    Card MomsKnife("(Мамин нож): Когда ты наносишь урон монсру при броске атаки на 6, ты также наносишь 1 урон всем другим активным монстрам. ", 6);
    Card Test1("(Test1): Some effect", "end");
    Card Test2("(Test2): Yet another effect", "start");

    int AmmountOfAllCards = Card::ID_static;
    //занесения карточек в массив
    Card* CardList = new Card[AmmountOfAllCards]
    { Finger,TarotCloth, SackOfPennies,SpiderMod, DadsLostCoin,GameBreakingBug,DeadBird,CheeseGrater,MomsBox,
      EyeOfGreed,TheMissingPage,HourGlass,TheRelic,SacredHeart,MomsRazor,TheD10,ChargedBaby,
      CursedEye1,CursedEye2,Infamy,DarkArts,Ipecac,MomsKnife,Test1,Test2 };

    //массив с названиями карточек
    string* CardNamesList = new string[AmmountOfAllCards]
    { "Finger","TarotCloth", "SackOfPennies","SpiderMod", "DadsLostCoin","GameBreakingBug","DeadBird","CheeseGrater",
      "MomsBox","EyeOfGreed","TheMissingPage","HourGlass","TheRelic","SacredHeart","MomsRazor","TheD10","ChargedBaby",
      "CursedEye1","CursedEye2","Infamy","DarkArts","Ipecac","MomsKnife","Test1","Test2" };

    srand(time(NULL));
    //занесения названий в классы
    NameCards(CardList, CardNamesList, AmmountOfAllCards);
    delete[] CardNamesList;

    //непосредственно код 
    LoadDiceCardReminder(CardList, AmmountOfAllCards);
    // cout << CardList[22].GetID();
    delete[] CardList;
}
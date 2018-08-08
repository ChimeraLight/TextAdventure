// Text Adventure.cpp
// Ben Gross, CISP 360
// 8/3/18

// core.h
    #include <iostream>
    #include <limits>
    #include <ctime>
    #include <cstdlib>

    namespace Core {

        struct Location {
            int x, y;
        };

        struct Room
        {
            std::string    name;
            std::string    description;
            Core::Location id;
            int            occupiedCode;
        };

        void validateInt(int& n, int min, int max)
        {
            using namespace std;

            while(cin.fail() || n < min || n > max)
            {
                cout << "ERROR (Invalid input): Please enter a correct int value" << endl;
                cout << "---> ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin >> n;
            }
        }

    }

// gameBoard.h
    #include <string>
    #include <vector>
    //#include <core.h>


    namespace Game {

        enum class DifficultySetting {
            Easy = 2,
            Medium = 3,
            Hard = 4
        };

        std::string DifficultySettingToString(DifficultySetting setting)
        {
            switch(setting)
            {
                case DifficultySetting::Easy:   return "EASY";
                case DifficultySetting::Medium: return "MEDIUM";
                case DifficultySetting::Hard:   return "HARD";
                Default:                        return "Unknown Difficulty";
            }
        }



        class Board {
        public:
            using GameSpace = std::vector<std::vector<Core::Room*>>;

                              Board(DifficultySetting difficulty);

            DifficultySetting GetSetting() const { return m_setting; }
            std::string       GetSettingName() const { return DifficultySettingToString(m_setting); }

            Core::Room&       GetRoom(Core::Location loc);
            const Core::Room& GetRoom(Core::Location loc) const;


        private:
            // construct a room based on location
            Core::Room       *makeRoom(Core::Location loc, int num) const { 
                switch(num) {
                    case 1: return new Core::Room{"Home Village", "Your home town, nice and cozy with friendly people.", {loc}, 0};
                    case 2: return new Core::Room{"Farm Outskirts", "The outskirts of the farmland.", {loc}, 1};
                    case 3: return new Core::Room{"Forest", "The forest has been known to contain many myths about mysterious creatures and curses.", {loc}, 2};
                    case 4: return new Core::Room{"Mountains", "The outskirts of the farmland.", {loc}, 2};
                    case 5: return new Core::Room{"Lord's Castle", "The outskirts of the farmland.", {loc}, 5};
                    case 6: return new Core::Room{"Burnt Outlands", "The outskirts of the farmland.", {loc}, 3};
                    case 7: return new Core::Room{"Heart of the Demon Legions", "The outskirts of the farmland.", {loc}, 4};
                    case 8: return new Core::Room{"Smouldering Wastes", "The outskirts of the farmland.", {loc}, 4};
                    case 9: return new Core::Room{"Demon Lord's Castle", "The outskirts of the farmland.", {loc}, 6};
                    default : std::cout << "ERROR: rand num generator error." << std::endl; 
                }
            }

        private:
            const int         MAX_RAND = 8,
                            MIN_RAND = 2;
            
            GameSpace         m_board;
            DifficultySetting m_setting;
        };
    }

    namespace Game {
    Board::Board(DifficultySetting difficulty) {
        int randInt;

        m_setting = difficulty;

        for(int i = 0; i < (static_cast<int>(difficulty) - 1);  i++)
        {
            m_board.push_back(std::vector<Core::Room* > ((static_cast<int>(difficulty) - 1)));
        }
        for (int i = 0; i < (static_cast<int>(difficulty) - 1); i++)
        {
            for (int j = 0; j < (static_cast<int>(difficulty) - 1); j++)
            {
                // Specification A8 - Start Room Is Always Same
                // Specification A9 - Boss Room is Always Same
                if (i == 0 && j == 0)
                    randInt = 1;
                else if (i == (static_cast<int>(difficulty) - 1) && j == (static_cast<int>(difficulty) - 1))
                    randInt = 9;
                else
                    randInt = rand() % (MAX_RAND - MIN_RAND) + MIN_RAND;

                m_board[i].push_back(makeRoom({i, j}, randInt));
            }
        }
    }

    }

// entities.h

    #include <iomanip>
    #include <string>
    #include <vector>
    //#include "core.h"


    namespace Entities {

        struct Stats {
            std::string name;
            int         health,
                        maxHealth,
                        strength,
                        exp,
                        maxExp,
                        level;
        };

        struct Item
        {
            std::string name;
            int    statMod,
                amountOwned,
                maxAmount;
            size_t pos;
        }potion = {"Health Potion", 50, 0, 10},
        manaPotion = {"Mana Potion", 50, 0, 10};

        struct Weapon
        {
            std::string name;
            int         dmgMod;
            size_t      pos;
        }starterSword = {"Starter Sword", 1},
        longSword = {"Long Sword", 10},
        claymore = {"Claymore", 15},
        club = {"club", 5};


        class Entity {
        public: 
            using CarriedItems = std::vector<Item>;
            using Weapons = std::vector<Weapon>;


            int            getTotalCombatStr() const {return m_stats.strength * m_equipedWeapon.dmgMod;}
            Weapon         getEquipedWeapon() const {return m_equipedWeapon;}
            Stats          getStats() const {return m_stats;}
            Core::Location getLocation() const {return m_location;}

            void displayStats();
            void   takeDmg(int);
            bool   isAlive();
            int    attack();

        protected:
            Weapon         m_equipedWeapon;
            Stats          m_stats;
            Core::Location m_location;
        };


        class Player : public Entity {
        public: 

                        Player();

            void gainExp(int);
            void levelUp();
            void displayItems();
            void displayWeapons();
            void addItem(Item, int); 
            void move();

        private: 
            void populateItems();
            
        private: 
            Weapons      m_playerWeapons;
            CarriedItems m_items;
        };


        class Monster : public Entity {
        public: 

                        Monster(int);

        };


        class NPC : public Entity {
        public: 
            using shopItems = std::vector<Entities::Item>;
            using shopWeapons = std::vector<Entities::Weapon>; 

            struct TypeShop {
                std::string name;
                shopItems wares;
                shopWeapons wares_weapons;
            }villageShop = {"Village Shop"},
            castleShop = {"Castle Shop"};
        
        public: 
                        NPC(int);
            void displayItems();
            void displayWeapons();

        private: 
            TypeShop m_thisShop;
        };

    }
    //Entity Methods
    namespace Entities {
    void Entity::displayStats() {
        using namespace std;

        cout << m_stats.name << endl;
        cout << "==" << setw(m_stats.name.length() - 2) << "==" << endl;
        cout << "Level:    " << m_stats.level << endl; 
        cout << "Health:   " << m_stats.health << endl;
        cout << "Strength: " << m_stats.strength << endl;
        cout << "Location: " << m_location.x << ", " << m_location.y << endl;
        cout << endl;
        cout << "Equiped Weapon: " << m_equipedWeapon.name << endl;
        cout << endl;
        cout << "Total Combat Strength: " << m_stats.strength * m_equipedWeapon.dmgMod << endl;
        cout << "==" << setw(m_stats.name.length() - 2) << "==" << endl;
        cout << endl;
    }
    void Entity::takeDmg(int dmg) {
        m_stats.health = m_stats.health - dmg;
        if(m_stats.health < 0)
            m_stats.health = 0;
    }
    bool Entity::isAlive(){
    if(m_stats.health > 0)
        return true;
    else
        return false;
    }
    int Entity::attack() {
        int randAttack = rand() % (m_equipedWeapon.dmgMod - (m_equipedWeapon.dmgMod - 10)) + (m_equipedWeapon.dmgMod - 10);
        std::cout << "You attack for " << m_stats.strength * m_equipedWeapon.dmgMod + randAttack << " damage!\n" << std::endl;
        return m_stats.strength * m_equipedWeapon.dmgMod + randAttack;
    }
    }
    //Player Methods
    namespace Entities {
    Player::Player() {
        m_location.x = 0;
        m_location.y = 0;
        m_equipedWeapon = starterSword;
        m_playerWeapons.push_back(starterSword);
        populateItems();
        addItem(potion, 1);
        m_stats.health = 100;
        m_stats.maxHealth = 100;
        m_stats.strength = 10;
        m_stats.exp = 0;
        m_stats.maxExp = 1000;
        m_stats.level = 1;

        // Specification C2 - Player Name
        std::cout << "Please enter you desired player name: ";
        std::cin.ignore();
        getline(std::cin, m_stats.name);
        std::cout << std::endl;
    }
    void Player::gainExp(int xp) {
        m_stats.exp = m_stats.exp + xp;

        if(m_stats.exp < m_stats.maxExp) {
            ;
        }else {
            m_stats.exp = m_stats.exp - m_stats.maxExp;
            levelUp();
        }
    }
    void Player::levelUp(){
        using namespace std;

        if(m_stats.level > 100) 
            cout << "You are at the max level and cannot level up anymore," << endl;
        else {
            m_stats.level++;
            m_stats.maxExp = m_stats.maxExp + (m_stats.maxExp / 4);
            m_stats.strength = m_stats.strength + (m_stats.strength / 2);
            m_stats.health = m_stats.maxHealth + (m_stats.maxHealth / 4);
            m_stats.maxHealth = m_stats.maxHealth + (m_stats.maxHealth / 2);
            cout << endl;
        }

        cout << "Congratulations, You have leveled up!" << endl 
            << "Here are your new statuses: " << endl;
        displayStats();
        cout << endl;
    }
    void Player::displayItems() {
        using namespace std;
        cout << "Items" << endl;
        cout << "=====" << endl;
        for (size_t i = 0; i < m_items.size(); ++i)
            cout << i + 1 << ". " << left << setw(15) << m_items[i].name << ": " << m_items[i].amountOwned << endl;
        cout << endl;
    }
    void Player::displayWeapons() {
        using namespace std;
        
        cout << "Weapons" << endl;
        cout << "=======" << endl;
        for (size_t i = 0; i < m_playerWeapons.size(); ++i)
            cout << i + 1 << ". " << m_playerWeapons[i].name << endl;
        cout << endl;
    }
    void Player::populateItems() {
        potion.pos = m_items.size();
        m_items.push_back(potion);

        manaPotion.pos = m_items.size();
        m_items.push_back(manaPotion);
    } 
    void Player::addItem(Item item, int num) {

    }
    void Player::move() {

    }
    }
    //NPC Methods
    namespace Entities {
    NPC::NPC (int occCode) {
        if (occCode == 1)
        {
            m_thisShop = villageShop;
            villageShop.wares.at(potion.pos).pos = villageShop.wares.size();
            villageShop.wares.push_back(potion);
        }else 
        {
            m_thisShop = castleShop;
            castleShop.wares.at(potion.pos).pos = castleShop.wares.size();
            castleShop.wares.push_back(potion);
            castleShop.wares.at(manaPotion.pos).pos = castleShop.wares.size();
            castleShop.wares.push_back(manaPotion);

            castleShop.wares_weapons.at(longSword.pos).pos = castleShop.wares_weapons.size();
            castleShop.wares_weapons.push_back(longSword);
            castleShop.wares_weapons.at(claymore.pos).pos = castleShop.wares_weapons.size();
            castleShop.wares_weapons.push_back(claymore);
        }    
    }
    void NPC::displayItems() {
        using namespace std;

        cout << "Items" << endl;
        cout << "=====" << endl;
        for (size_t i = 0; i < m_thisShop.wares.size(); ++i)
            cout << i + 1 << ". " << left << setw(15) << m_thisShop.wares[i].name 
            << ": " << m_thisShop.wares[i].amountOwned << endl;
        cout << endl;
    }
    void NPC::displayWeapons() {
        using namespace std;

        cout << "Weapons" << endl;
        cout << "=======" << endl;
        for (size_t i = 0; i < m_thisShop.wares_weapons.size(); ++i)
            cout << i + 1 << ". " << m_thisShop.wares_weapons[i].name << endl;
        cout << endl;
    }
    }

Game::DifficultySetting initializeGameBoard();

void mainMenu();
void startGame(int);
void actionMenu();
void takeAction(int);
bool shouldPlayAgain();

Game::Board *gameBoard;
Entities::Player *player;

int main() {
    srand(time(NULL));
    std::cout << "This program is a text based adventure game." << std::endl;
    std::cout << "The balance could very well be terrible and I made up the game rules on a whim, so don't expect too much of actual game mechanics.\n" 
        << std::endl;

     do
    {
        gameBoard = new Game::Board(initializeGameBoard());
        player = new Entities::Player;

        startGame(1);

        delete gameBoard;
        delete player;

    }while (shouldPlayAgain());

    return 0;
}

//#inclide <gameBoard.h>
//#include <entities.h>

Game::DifficultySetting initializeGameBoard() {
    using namespace std;
    using difficulty = Game::DifficultySetting;

    int input;
    cout << endl;
    cout << "Select difficulty" << "\n=================\n";
    cout << "1 - " << Game::DifficultySettingToString(difficulty::Easy) <<  endl;
    cout << "2 - " << Game::DifficultySettingToString(difficulty::Medium) << endl;
    cout << "3 - " << Game::DifficultySettingToString(difficulty::Hard) << endl;
    cout << "\n---> ";
    cin >> input;

    cout << endl;

    Core::validateInt(input, 1, 3);

    switch(input) {
        case static_cast<int>(difficulty::Easy):   return Game::DifficultySetting::Easy;
        case static_cast<int>(difficulty::Medium): return Game::DifficultySetting::Medium;
        case static_cast<int>(difficulty::Hard):   return Game::DifficultySetting::Hard;
    };

}

void mainMenu() {
    using namespace std;

    int input;

    cout << "\nWelcome to the Super Awesome Adventure Game!\n";
    cout << "============================================\n" << endl;
    cout << "Please select an option below by typing the number preceding the option." << endl;
    cout << "1. Start Your Adventure" << endl;
    cout << "2. Explain Game" << endl;
    cout << "3. Quit Game" << endl;
    cout << "\n---> ";
    cin >> input;

    Core::validateInt(input, 1, 3);

    startGame(input);
}

void startGame(int input) {
    using namespace std;

    if(input == 1)
    {
        cout << "Game set to " << gameBoard->GetSettingName() << endl;

        cout << endl;
        cout << "Your journey begins in your home town, your goal, to beat the Demon Lord and bring peace to the land." << endl;
        cout << endl;
        player->displayStats();
        player->displayItems();
        player->displayWeapons();

        actionMenu();
    }
    else if(input == 2)
    {
        cout << "\nIn this game, you can buy items at the village or castle, or fight monsters to level up." << endl;
        cout << "The goal of the game is to beat the Demon Lord who resides at the furthest point from the village," << endl;
        cout << "Each time you play the game, the enviorment will be different, making for a new adventure each time.\n" << endl;
        mainMenu();
    }else 
    {
        cout << "Quitting game." << endl;
        exit(0);
    }
}

void actionMenu() {
    using namespace std;

    int input;

    cout << "What action would you like to take?" << endl;
    cout << "===================================" << endl;
    cout << "1. Move" << endl;
    cout << "2. Display Status" << endl;
    cout << "3. Look Around" << endl;
    cout << "4. Stay" << endl;
    cout << "\n---> ";
    cin >> input;

    Core::validateInt(input, 1, 4);

    takeAction(input);
}

void takeAction(int input) {
    using namespace std;
}

bool shouldPlayAgain() {
    using namespace std;

    bool status;
    char input;

    cout << "Would you like keep playing? (y/n)" << endl;
    cout << "---> ";
    cin >> input;
    cout << endl;
    input = tolower(input);
    if( input == 'y')
        status = true;
    else
        status = false;

    return status;
}




#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#include <stdint.h>
#include <bits/stdc++.h>

#define PLAYER_COUNT 2

using namespace std;

typedef uint8_t Loc;

typedef uint16_t CardType;
typedef uint8_t SuperType;
typedef uint8_t CardColor;


typedef uint32_t EvergreenKeyword;




const EvergreenKeyword DEATHTOUCH (1<<0);
const EvergreenKeyword DEFENDER (1<<1);
const EvergreenKeyword FIRST_STRIKE (4);
const EvergreenKeyword DOUBLE_STRIKE (8);
const EvergreenKeyword FLASH (16);
const EvergreenKeyword FLYING (32);
const EvergreenKeyword HASTE (64);
const EvergreenKeyword HEXPROOF (128);
const EvergreenKeyword INDESTRUCTIBLE (256);
const EvergreenKeyword LIFELINK (512);
const EvergreenKeyword MENACE (1024);
const EvergreenKeyword REACH (2048);
const EvergreenKeyword TRAMPLE (1 << 12);
const EvergreenKeyword VIGILANCE (1 << 13);



typedef struct {
    int mana[7];
    bool x;
    bool tapself;
} Cost;





const Loc BATTLEFIELD (1);
const Loc GRAVEYARD (2);
const Loc HAND (4);
const Loc CONTROLLED (8);
const Loc NOT_CONTROLLED (16);
const Loc OWNED (32);
const Loc NOT_OWNED (64);
const Loc STACK (128);

enum Color {
    WHITE = 0,
    BLUE = 1,
    BLACK = 2,
    RED = 3,
    GREEN = 4,
    COLORLESS = 5,
    ANY_COLOR = 6,
};


const CardType CREATURE (1);
const CardType ARTIFACT (2);
const CardType LAND (4);
const CardType ENCHANTMENT (8);
const CardType PLANESWALKER (16);
const CardType INSTANT (32);
const CardType SORCERY (64);
const CardType TOKEN (128);

const SuperType BASIC (1);
const SuperType LEGENDARY (2);
const SuperType SNOW (4);


enum TurnPhase {
    UntapStep,
    UpkeepStep,
    DrawStep,
    PrecombatMainPhase,
    BeginningOfCombat,
    DeclareAttackers,
    DeclareBlockers,
    FirstStrikeDamageStep,
    DamageStep,
    EndOfCombat,
    PostcombatMainPhase,
    EndStep,
    CleanupStep
};



typedef struct {
    
    int mana_production[6];
    int target;
    uint16_t target_type_constraint;
    uint8_t target_localisation_constraint;



} Effect;

typedef struct {
    bool triggered;
    bool passive;
    bool mana;
    Cost cost;
    Effect effect;



} Ability;

typedef struct CardTemplate {
    int strength;
    int endurance;
    string name;
    CardType type;
    string secondary_type;
    SuperType supertype; 
    CardColor color;

    vector<Ability> abilities;
    int owner;

    Cost cost;

    EvergreenKeyword kw;

    int count;

    string repr();    

} CardTemplate;



typedef struct Card {
    int card_id;
    int ctempl_id;
    int hp;
    int basic_counters[4];
    vector<Card*> attached;

} Card; 

Card from_card_template(int ctempl_id, CardTemplate * ctempl);


typedef struct  {
    Card spell;
    Ability ability;

    bool copy;

} SpellOrAbility;


typedef struct {
    vector<Card> battlefield[PLAYER_COUNT];
    vector<Card> graveyard[PLAYER_COUNT];
    vector<Card> exile[PLAYER_COUNT];
    vector<Card> library[PLAYER_COUNT];
    vector<Card> hand[PLAYER_COUNT];
    vector<SpellOrAbility> stack;

    vector<CardTemplate> cards;

} Board;


typedef struct {
    Board board;
    int priority;
    int phase;
} GameState;


CardTemplate new_basic_creature(int strength, int endurance,
    string name, string secondary_type, int owner, Cost cost,
    EvergreenKeyword kw);


CardTemplate new_basic_land(string name, int owner, int color_id);
Board board_example();

Board * board_ptr();


void draw_random_cards(Board * board, int player, int amount);






#include "magic.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
// mt19937
void cardtype_repr(CardType ct, stringstream * stream) {
    const string names[8] = {
        "Creature",
        "Artifact",
        "Land",
        "Enchantment",
        "Planeswalker",
        "Instant",
        "Sorcery",
        "Token"
    } ;

    bool b = false;
    CardType mask (1);
    for (int i = 0; i < 8; ++i) {
        if (ct & mask) {
            if (b) {
                *stream << " and ";
            } else {
                b = true;
            }

            *stream << names[i];
        }
        mask += mask;

    }
}
void evergreen_kw_repr(EvergreenKeyword ct, stringstream * stream) {
    const string names[14] = {
        "DEATHTOUCH",
        "DEFENDER",
        "FIRST_STRIKE",
        "DOUBLE_STRIKE",
        "FLASH",
        "FLYING",
        "HASTE",
        "HEXPROOF",
        "INDESTRUCTIBLE",
        "LIFELINK",
        "MENACE",
        "REACH",
        "TRAMPLE",
        "VIGILANCE",
    } ;

    EvergreenKeyword mask (1);
    for (int i = 0; i < 14; ++i) {
        if (ct & mask) {    

            *stream << names[i] << "\n";
        }
        mask += mask;

    }
}

bool mana_repr(int * mana, stringstream * stream) {
    const char * COL_S = "WUBRGC";
    bool b = false;
    for (int i = 0; i<= 5; ++i) {
        if (mana[i]) b = true;
        for (int j = 0; j < mana[i]; ++j) {
            *stream << '{' << COL_S[i] << '}';
        }
    }
    return b;
}

void manacost_repr(Cost cost, stringstream * stream) {

    if (cost.mana[ANY_COLOR] > 0) {
        *stream << '{' << cost.mana[ANY_COLOR] << '}';
    }
    bool b = mana_repr(cost.mana, stream);

    if (cost.tapself) {
        if (b) {
            *stream << ", ";
        }
        *stream << "{T}";
    }
}

string CardTemplate::repr()
{

    stringstream s ("");
    s << '[' << name;
    manacost_repr(cost, &s);
    s << ']';

    s << '\n';
    cardtype_repr(type, &s);
    if (!secondary_type.empty()) {
        s << " - " << secondary_type << "\n";
    }
    evergreen_kw_repr(kw, &s);

    for (size_t i = 0; i < abilities.size(); ++i) {
        Ability ability = abilities[i];
        manacost_repr(ability.cost, &s);
        s << ": ";
        stringstream s2 ("");
        bool b = mana_repr(ability.effect.mana_production, &s2);
        if (b) {
            s << "Add " << s2.str() << " to your mana pool.";
        }

    }


    if (type & CREATURE) {
        s << strength << "/" << endurance << "\n";
    }

    return s.str();
}

Card from_card_template(int ctempl_id, CardTemplate *ctempl)
{
    Card card = {0,};
    card.ctempl_id = ctempl_id;
    card.hp = ctempl->endurance;
    return card;
}

CardTemplate new_basic_creature(int strength, int endurance,
                                string name, string secondary_type, int owner, Cost cost,
                                EvergreenKeyword kw)
{
    CardTemplate ans = {0,};
    ans.strength = strength;
    ans.endurance = endurance;
    ans.name = name;
    ans.secondary_type = secondary_type;
    ans.type |= CREATURE;
    ans.owner = owner;
    ans.cost = cost;
    ans.kw = kw;
    return ans;

}

CardTemplate new_basic_land(string name, int owner, int color_id)
{
    CardTemplate ans = {0,};
    ans.name = name;
    ans.secondary_type = "basic";
    ans.type |= LAND;
    ans.owner = owner;
    Cost cost = {0,};
    ans.cost = cost;

    Ability ability = {0,};
    ability.cost = {.mana = {0,}, .x = false, .tapself = true};
    ability.mana = true;
    Effect eff = {0,};
    eff.mana_production[color_id] = 1;
    ability.effect = eff;
    ans.abilities = {ability};

    ans.color = COLORLESS;

    return ans;
}


Board board_example() {
    vector<CardTemplate> cards = {};
    cards.push_back(new_basic_land(string("Island"), 0, BLUE));
    cards.push_back(new_basic_land(string("Forest"), 0, GREEN));
    cards.push_back(new_basic_land(string("Island"), 1, BLUE));
    cards.push_back(new_basic_land(string("Swamp"), 1, BLACK));

    Card card;
    Board board;

    for (int i=0; i < 20; ++i) {
        card = from_card_template(0, &cards[0]);
        board.library[0].push_back(card);
        card = from_card_template(3, &cards[3]);
        board.library[1].push_back(card);
    }

    for (int i=0; i < 10; ++i) {
        card = from_card_template(1, &cards[1]);
        board.library[0].push_back(card);
        card = from_card_template(2, &cards[2]);
        board.library[1].push_back(card);
    }

    board.cards = cards;
    return board;
}

Board _board = board_example();
Board * board_ptr() {
    return &_board;
}







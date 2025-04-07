
#include "magic.h"
#include <stdio.h>
#include <iostream>
#include <sstream>

#include <random>
// mt19937
void cardtype_repr(CardTemplate * ctempl, stringstream * stream) {
    CardType ct = ctempl->type;


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

    const string supertypes[3] {
        "Basic",
        "Legendary",
        "Snow"
    };

    SuperType m_ (1);
    for (int i = 0; i < 3; ++i) {
        if (ctempl->supertype & m_) {
            *stream << supertypes[i] << " ";
            m_ += m_;
        }
    }

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
    s  << name << '|';
    manacost_repr(cost, &s);
    s << '|';

    cardtype_repr(this, &s);
    if (!secondary_type.empty()) {
        s << " - " << secondary_type;
    }
    s << '|';

    evergreen_kw_repr(kw, &s);

    for (size_t i = 0; i < abilities.size(); ++i) {
        Ability ability = abilities[i];
        manacost_repr(ability.cost, &s);
        s << ": ";
        stringstream s2 ("");
        bool b = mana_repr(ability.effect.mana_production, &s2);
        if (b) {
            s << "Add " << s2.str() << " to your mana pool.\n";
        }

    }
    s << '|';

    if (type & CREATURE) {
        s << strength << "/" << endurance;
    }

    s << '|';
    if (color & 1) s << "W";
    if (color & 2) s << "U";
    if (color & 4) s << "B";
    if (color & 8) s << "R";
    if (color & 16) s << "G";


    return s.str();
}
int _card_id = 0;

Card from_card_template(int ctempl_id, CardTemplate *ctempl)
{   
    
    Card card = {0,};
    card.ctempl_id = ctempl_id;
    card.hp = ctempl->endurance;
    card.card_id = ++_card_id;
    return card;
}

CardColor color_from_manacost(Cost * cost) {
    CardColor mask (1);
    CardColor ans (0);

    for (int i = 0; i < 5; ++i) {
        if (cost->mana[i] > 0) {
            ans |= mask;
        }

        mask += mask;
    }

    return ans;

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
    ans.color = color_from_manacost(&cost);

    return ans;

}

CardTemplate new_basic_land(string name, int owner, int color_id)
{
    CardTemplate ans = {0,};
    ans.name = name;
    ans.secondary_type = name;
    ans.supertype = BASIC; 
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

    ans.color = 0;

    return ans;
}


Board board_example() {
    cout << "BOARD EXAMPLE\n";

    vector<CardTemplate> cards = {};
    cards.push_back(new_basic_land(string("Island"), 0, BLUE));
    cards.push_back(new_basic_land(string("Forest"), 0, GREEN));
    cards.push_back(new_basic_land(string("Island"), 1, BLUE));
    cards.push_back(new_basic_land(string("Swamp"), 1, BLACK));

    Cost cost = {0,};
    //cost.mana[BLUE] = 2;
    cost.mana[WHITE] = 2;
    cost.mana[ANY_COLOR] = 3;
    cards.push_back(new_basic_creature(4, 4, "Serra Angel", "Angel", 0, cost, VIGILANCE | FLYING));
    cards.push_back(new_basic_creature(4, 4, "Serra Angel", "Angel", 1, cost, VIGILANCE | FLYING));

    Card card;
    Board board = {};
    board.cards = cards;

    board.max_hand_size[0] = 7;
    board.max_hand_size[1] = 7;

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

    for (int i = 0; i < 5; ++i) {
        card = from_card_template(0, &cards[0]);
        board.battlefield[0].push_back(card);

        card = from_card_template(2, &cards[2]);
        board.battlefield[1].push_back(card);
    }

    card = from_card_template(4, &cards[4]);
    board.battlefield[0].push_back(card);
    

    card = from_card_template(5, &cards[5]);
    board.battlefield[1].push_back(card);
    
    board.draw_random_cards(0, 5);
    board.draw_random_cards(1, 7);


    return board;
}

GameState gs;

Board * board_ptr() {
    return &gs.board;
}

GameState *gs_ptr()
{
    return &gs;
}

mt19937 prng (12);



void Board::draw_random_cards(int player, int amount)
{ 
    uint32_t lib_size = library[player].size();
    Card * data = library[player].data();
    for (int i = 0; i < amount; ++i) {

        uint32_t card_id = prng() % lib_size;
        Card card = data[card_id];
        data[card_id] = data[lib_size - 1];
        library[player].pop_back();

        hand[player].push_back(card);
    }
}

void init_gamestate()
{
    gs.phase = UntapStep;
    gs.player = 0;
    gs.priority = -1;
    gs.board = board_example();
}

void GameState::untap_step()
{
    phase = UntapStep;
    uint32_t b_size = board.battlefield[player].size();
    Card * data = board.battlefield[player].data();

    for (int i = 0; i < b_size; ++i) {
        untap(data + i);
    }

    upkeep_step();
}

void GameState::untap(Card *card)
{
    card->tapped = false;
}

void GameState::pass_priority(int player)
{
    if (pass_counter >= 2) {
        pass_counter = 0;
        priority = -1;
    } else {
        priority = player;
        action_request = priority;
    }
}

void GameState::upkeep_step()
{
    phase = UpkeepStep;
    pass_priority(player);
    
}

void GameState::draw_step()
{
    phase = DrawStep;
    board.draw_random_cards(player, 1);

    pass_priority(player);

}

void GameState::precombat_mainphase()
{
    phase = PrecombatMainPhase;
    pass_priority(player);
}

void GameState::beginning_of_combat()
{
    phase = BeginningOfCombat;
    pass_priority(player);
}

void GameState::declare_attackers()
{
    phase = DeclareAttackers;
    action_request = player;
}

void GameState::declare_blocker()
{
    phase = DeclareBlockers;
    action_request = 1 - player;
}

void GameState::first_strike_damage_step()
{
    phase = FirstStrikeDamageStep;
    // resoudre les degats
    pass_priority(player);
}

void GameState::damage_step()
{
    phase = DamageStep;
    // resoudre les degats
    pass_priority(player);
}

void GameState::end_of_combat()
{
    phase = EndOfCombat;
    pass_priority(player);
}

void GameState::post_combat_main_phase()
{
    phase = PostcombatMainPhase;
    pass_priority(player);
}

void GameState::end_step()
{
    phase = EndStep;
    pass_priority(player);
}

void GameState::cleanup_step()
{
    phase = CleanupStep;
    action_request = player; // que si on doit discard, 
    // sinon terminer le tour

}

bool GameState::is_stack_empty()
{
    return board.stack.empty();
}

void GameState::possible_actions(vector<Action> &buffer)
{   


    bool is_passing_possible = true;


    if (priority == -1) {
        switch (phase)
        {
        case DeclareAttackers:
            assert(action_request == player);
            int creature_count = board.battlefield[player].size();
            assert(creature_count < 10); // pour le moment
            for (int i = 0; i < (1 << creature_count); ++i) {
                Action action = {.action_type = Attack, .target = {i, 0}, .action_data = nullptr};
                buffer.push_back(action);
            }
            break;
        case DeclareBlockers:
            assert(action_request == 1 - player);

            int creature_count = board.battlefield[1-player].size();
            assert(creature_count < 10); // pour le moment
            for (int i = 0; i < (1 << creature_count); ++i) {
                Action action = {.action_type = Block, .target = {i, 0}, .action_data = nullptr};
                buffer.push_back(action);
            }
            break;
        default:
            break;
        }
    } else if (priority == player && 
        (phase == PrecombatMainPhase || phase == PostcombatMainPhase) && is_stack_empty()) {
            assert(action_request == player);
            int hand_size = board.hand[player].size();
            Card * data = board.hand[player].data();
            for (int i = 0; i < hand_size; ++i) {
                Action action = {.action_type = PlayCard, .target = {i, 0}, .action_data = nullptr};
                buffer.push_back(action); // pour le moment, on ne verifie pas les cout
                // ou la limite du nombre de terrains par tours; tous les sorts coutent 0 mana
            }

    } else {
        assert(action_request == priority);
        int hand_size = board.hand[priority].size();
        Card * data = board.hand[priority].data();
        for (int i = 0; i < hand_size; ++i) {
            int ctempl_id = data[i].ctempl_id;
            CardTemplate * ctempl = board.cards.data() + i;
            if (ctempl->kw & FLASH || ctempl->type & INSTANT) {
                Action action = {.action_type = PlayCard, .target = {i, 0}, .action_data = nullptr};
                buffer.push_back(action);
            }
        }
    }


    if (is_passing_possible) {
        Action pass = {.action_type = Pass, .target = {0, 0}, .action_data = NULL};
        buffer.push_back(pass);    
    }

}

int GameState::play(Action action)
{
    action_request = -1;

    // faire les actions de jeu, et vérifier la validité du coup

    if (priority != -1) {
        
        
        if (action.action_type == Pass) { // si l'action est "Pass"
            ++pass_counter;
            pass_priority(1 - priority);
        } else if (action.action_type == PlayCard 
                || action.action_type == ActivateAbility) {
            pass_counter = 0;
            pass_priority(priority);

        }
    }
    

    if (priority == -1) {
        if (is_stack_empty()) {
            go_to_next_step();
        } else {
            board.stack.pop_back();
            pass_priority(player);
        }
    }
    return action_request;
}

void GameState::go_to_next_step()
{
    pass_counter = 0;
    switch (phase)
    {
    case UntapStep:
        upkeep_step();
        break;
    case UpkeepStep:
        draw_step();
        break;
    case DrawStep:
        precombat_mainphase();
        break;
    case PrecombatMainPhase:
        beginning_of_combat();
        break;
    case BeginningOfCombat:
        declare_attackers();
        break;
    case DeclareAttackers:
        declare_blocker();
        break;
    case DeclareBlockers:
        first_strike_damage_step();
        break;
    case FirstStrikeDamageStep:
        damage_step();
        break;
    case DamageStep:
        end_of_combat();
        break;
    case EndOfCombat:
        post_combat_main_phase();
        break;    
    case PostcombatMainPhase:
        end_step();
        break;

    case EndStep:
        cleanup_step();
        break;
    case CleanupStep:
        player = 1 - player;
        untap_step();
        break;

    default:
        break;
    }
}

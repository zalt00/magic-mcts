#include <iostream>
#include "magic.h"
using namespace std;



extern "C" {
    #include <string>

    void init_board() {
        init_gamestate();
    }

    int print_cardtemplate(CardTemplate * ct, char * buffer, int max_size) {
        return snprintf(buffer, max_size, "%s", ct->repr().c_str());
    }

    int print_library(int owner, char * buffer, int max_size) {
        Card * library = board_ptr()->library[owner].data();
        int lib_size = board_ptr()->library[owner].size();
        CardTemplate * cards = board_ptr()->cards.data();

        int cms = max_size;
        int w;
        for (size_t i = 0; i < lib_size; ++i) {
            int id = library[i].ctempl_id;
            cards[id].count++;
        }
        for (size_t i = 0; i < lib_size; ++i) {
            int id = library[i].ctempl_id;

            if (cards[id].count > 0) {
                w = snprintf(buffer, max_size, "%d#", cards[id].count);
                max_size -= w;
                buffer += w;

                w = print_cardtemplate(&cards[id], buffer, max_size);
                max_size -= w;
                buffer += w;
                
                w = snprintf(buffer, max_size, ";;");
                max_size -= w;
                buffer += w;

                cards[id].count = 0;

            }
        }

        return cms - max_size;
    }



    int print_cardvector(vector<Card> * v, char * buffer, int max_size) {
        int cms = max_size;
        int w;

        CardTemplate * cards = board_ptr()->cards.data();

        for (size_t i = 0; i < v->size(); ++i) {
                int id = (*v)[i].ctempl_id;

                w = print_cardtemplate(&cards[id], buffer, max_size);
                max_size -= w;
                buffer += w;

                w = snprintf(buffer, max_size, "$%d", (*v)[i].card_id);
                max_size -= w;
                buffer += w;

                w = snprintf(buffer, max_size, ";;");
                max_size -= w;
                buffer += w;
        }
        return cms - max_size;
    }
    int print_battlefield(int owner, char * buffer, int max_size) {
        return print_cardvector(&board_ptr()->battlefield[owner], buffer, max_size);
    }

    int print_hand(int owner, char * buffer, int max_size) {
        return print_cardvector(&board_ptr()->hand[owner], buffer, max_size);
    }

    int print_graveyard(int owner, char * buffer, int max_size) {
        return print_cardvector(&board_ptr()->graveyard[owner], buffer, max_size);
    }
    
    int action_pass() {


        Action action = {.action_type = Pass, .target = {0,}, .action_data = NULL};
        int ans = gs_ptr()->play(action);
        cout << "after action, phase: " << gs_ptr()->phase << ", priority: " << gs_ptr()->priority << ", player: " << gs_ptr()->player << '\n';
        return ans;
    }

}


int main() {
    init_board();
    cout << board_ptr()->hand[0].size();

    return 0;
}



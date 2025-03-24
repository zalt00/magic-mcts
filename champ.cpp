#include <iostream>
#include "magic.h"
using namespace std;


int main() {
    cout << "Hello world\n";
    Cost cost = {0,};
    cost.mana[WHITE] = 2;
    cost.mana[ANY_COLOR] = 3;
    CardTemplate v = new_basic_creature(4, 4, "Ange de Serra", "Ange", 0, cost, FLYING | VIGILANCE);

    cout << v.repr() << "\n";
    string x;
    cin >> x;
    CardTemplate land = new_basic_land("Island", 0, BLUE);
    cout << land.repr() << "\n";

    return 0;
}

extern "C" {
    #include <string>

    int print_cardtemplate(CardTemplate * ct, char * buffer, int max_size) {
        return snprintf(buffer, max_size, "%s", ct->repr().c_str());
    }

    int print_library(int owner, char * buffer, int max_size) {
        vector<Card> library = board_ptr()->library[owner];
        vector<CardTemplate> cards = board_ptr()->cards;

        int cms = max_size;
        int w;
        for (size_t i = 0; i < library.size(); ++i) {
            int id = library[i].ctempl_id;
            cards[id].count++;
        }
        for (size_t i = 0; i < library.size(); ++i) {
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

    int print_battlefield(int owner, char * buffer, int max_size) {
        int cms = max_size;
        int w;

        vector<Card> bfield = board_ptr()->battlefield[owner];
        vector<CardTemplate> cards = board_ptr()->cards;

        for (size_t i = 0; i < bfield.size(); ++i) {
                int id = bfield[i].ctempl_id;

                w = print_cardtemplate(&cards[id], buffer, max_size);
                max_size -= w;
                buffer += w;
                
                w = snprintf(buffer, max_size, ";;");
                max_size -= w;
                buffer += w;
        }
        return cms - max_size;

    }

}


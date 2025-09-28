#ifndef HAND_H
#define HAND_H

#include <vector>
#include <string>

namespace blackjack {
    class Hand {
    public:
        Hand();

        void addCards(int count);
        int handValue() const;
        bool isSoft17() const;
        bool checkBlackjack() const;
        bool bust() const;
        size_t size() const;
        
        std::vector<std::string> getCards() const;
        static int getValue(const std::string &card);
        
        private:
            std::vector<std::string> cards;
    };
}

#endif
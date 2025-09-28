#include "hand.h"
#include "util.cpp"
#include <algorithm>
#include <iostream>

using namespace blackjack;

Hand::Hand() {}

void Hand::addCards(int count) {
    for (int i = 0; i < count; i++) {
        int num = rand() % 13;
        std::string card = deckCards[num];
        cards.push_back(card);
    }
}

int Hand::getValue(const std::string &card) {
    if (card == "K" || card == "Q" || card == "J") {
        return FACEVAL;
    } else if (card == "A") {
        return 11;
    } else {
        return std::stoi(card);
    }
}

int Hand::handValue() const {
    int aces = 0;
    int sum = 0;
    
    for (const std::string &card : cards) {
        if (card == "A") {
            aces++;
        }
        
        sum += getValue(card);
    }
    
    while (sum > BJVAL && aces > 0) {
        sum -= 10;
        aces--;
    }
    
    return sum;
}

bool Hand::isSoft17() const {
    int aces = 0;

    for (const std::string &card : cards) {
        if (card == "A") {
            aces += 1;
        }
    }

    return (handValue() == 17 && aces > 0);
}

bool Hand::checkBlackjack() const {
    return handValue() == BJVAL;
}

bool Hand::bust() const {
    return handValue() > BJVAL;
}

std::vector<std::string> Hand::getCards() const {
    return cards;
}

size_t Hand::size() const {
    return cards.size();
}
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <chrono>
#include <thread>
#include "hand.h"
#include "util.cpp"

using namespace blackjack;

/*
    TODO: split, do this with std::vectors of std::vectors, also side bets and insurance, and count
    TODO: Also being able to play multiple hands, but that is the same as splitting
    TODO: Add so that if the bet is 0, it does not play the hand, in case players bet wrong
    TODO: Make sentences have correct plurals and singulars (grammar)
    The rules of this blackjack is that dealer hits on soft 17, 
    or S17 blackjack. Can choose to play up to 3 hands simultaneously.
    Is implemented so that bet is deducted from money as soon as bet is placed
*/ 
    
void transformCase(std::string &word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
}

void checkPlay(bool &play) {
    std::cout << "Play \n"
            "1: Yes 2: No \n";

    std::string input;
    
    while (true) {
        getline(std::cin, input);
        transformCase(input);
    
        if (input == "yes" || input == "1") {
            play = true;
            break;
        } else if (input == "no" || input == "2") {
            play = false;
            break;
        } else {
            std::cout << "Try again \n";
        }
    }
}

int getAmount() {
    int money;

    while(true) {
        std::cin >> money;
    
        if (!std::cin || money <= 0) {
            std::cout << "Please enter a valid money amount: ";

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return money;
        }
    }
}

bool playerBlackjack(Hand &hand, int &money, int bet) {
    if (hand.checkBlackjack()) {
        money += bet + static_cast<int>(1.5 * bet);

        std::cout << "Blackjack! \n";

        return true;
    }

    return false;
}

bool playerBust(Hand &hand) {
    if (hand.bust()) {
        std::cout << bustMsg;
        return true;
    }

    return false;
}

void printPlayerHands(std::vector<Hand> hands, std::vector<int> bets) {
    std::cout << "\n\t" << "Player's" << " Hands: \n\t";
    
    for (int i = 0; i < hands.size(); i++) {
        std::vector<std::string> cards = hands[i].getCards();

        std::cout << "\n\t" << "Hand " << i + 1 << ": \n\t";
        
        for (std::string card : cards) std::cout << "+---+ ";
        std::cout << "\n\t";
        
        for (std::string card : cards) {
            std::cout << "| " << std::setw(1) << card << " | ";
        }
        std::cout << "\n\t";
        
        for (std::string card : cards) std::cout << "+---+ ";
        std::cout << "\n\t";
        
        std::cout << "Bet: " << bets[i] << "\n\n";
    }
}

void printDealerHidden(Hand &hand) {
    std::cout << "\n\t" << "Dealer's" << " Hand:\n\t";
    
    std::vector<std::string> cards = hand.getCards();
    
    for (std::string card : cards) std::cout << "+---+ ";
    std::cout << "\n\t";
    
    int count = 0;
    for (std::string card : cards) {
        if (count == 0) {
            card = "?";
        }
        std::cout << "| " << std::setw(1) << card << " | ";
        count++;
    }
    std::cout << "\n\t";
    
    for (std::string card : cards) std::cout << "+---+ ";
    std::cout << "\n";
}

void printDealer(Hand &hand) {
    std::cout << "\n\t" << "Dealer's" << " Hand:\n\t";
    
    std::vector<std::string> cards = hand.getCards();
    
    for (std::string card : cards) std::cout << "+---+ ";
    std::cout << "\n\t";
    
    for (std::string card : cards) {
        std::cout << "| " << std::setw(1) << card << " | ";
    }
    std::cout << "\n\t";
    
    for (std::string card : cards) std::cout << "+---+ ";
    std::cout << "\n";
}

void printHandsHidden(
    Hand &dealerHand, 
    std::vector<Hand> playerHands,
    std::vector<int> bets
) {
    printDealerHidden(dealerHand);
    printPlayerHands(playerHands, bets);
}

void printHands(
    Hand &dealerHand,
    std::vector<Hand> playerHands,
    std::vector<int> bets
) {
    printDealer(dealerHand);
    printPlayerHands(playerHands, bets);
}

void initialCards(
    Hand &dealerHand,
    std::vector<Hand> &playerHands, 
    std::vector<int> bets,
    int hands
) {
    for (int i = 0; i < hands; i++) {
        Hand hand;
        hand.addCards(2); 
        playerHands.push_back(hand);
    }  
    
    dealerHand.addCards(2); 
    
    printHandsHidden(dealerHand, playerHands, bets);
}

int getHands() {
    std::cout << "How many hands would you like to play at once? "
            "The maximum is 3. \n";

    int hands;

    while(true) {
        std::cout << "Hands: ";

        std::cin >> hands;
    
        if (!std::cin || hands <= 0 || hands > 3) {
            std::cout << "Please enter a valid hand amount \n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return hands;
        }
    }
}

std::vector<int> getBets(int &money, int hands) {
    int bet = 0;
    std::vector<int> bets;

    for (int i = 0; i < hands; i++) {
        bet = 0;

        while (!bet) {
            std::cout << "Place your bet for hand " << i + 1 << ": ";

            bet = getAmount();
    
            if (bet > money || bet <= 0) {
                std::cout << "Invalid bet amount \n";

                bet = 0;
            }
        }

        money -= bet;
        bets.push_back(bet);
    }
    
    return bets;
}

bool instantBlackjack(
    Hand &dealerHand, 
    std::vector<Hand> playerHands,
    int &money,
    std::vector<int> bets,
    int &pushHands
) {
    if (dealerHand.checkBlackjack()) {
        int handsCount = playerHands.size();

        for (int i = 0; i < handsCount; i++) {
            if (playerHands[i].checkBlackjack()) {
                money += bets[i];
                pushHands++;
            }
        }

        printHands(dealerHand, playerHands, bets);

        std::cout << "The dealer has gotten a blackjack instantly, you lost. \n"; 

        return true;
    }

    return false;
}

bool hit(
    Hand &dealerHand,
    std::vector<Hand> &playerHands, 
    int &money, 
    std::vector<int> &bets,
    int numHand
) {
    Hand &hand = playerHands[numHand]; 
    int &bet = bets[numHand];

    hand.addCards(1); 

    std::cout << "Hit! \n";
    
    printHandsHidden(dealerHand, playerHands, bets);
    
    if (playerBlackjack(hand, money, bet) || 
        playerBust(hand)
    ) {
        return true;
    } 

    return false;
}

bool doubleDown(
    Hand &dealerHand,
    std::vector<Hand> &playerHands, 
    int &money, 
    std::vector<int> &bets,
    int numHand
) {
    Hand &hand = playerHands[numHand]; 
    int &bet = bets[numHand];
    int left = money - bet;

    if (left < 0) {
        std::cout << "Not enough money for double down \n";
        return false;
    }

    hand.addCards(1); 

    std::cout << "Double down! \n";

    money -= bet;
    bet *= 2;

    printHandsHidden(dealerHand, playerHands, bets);

    playerBlackjack(hand, money, bet);
    playerBust(hand);

    return true;
}

void playerAction(
    Hand &dealerHand, 
    std::vector<Hand> &playerHands,
    int &money,
    std::vector<int> &bets,
    int numHand
) {
    bool firstMove = true;
    bool stand = false;
    std::string action;

    while (true) {
        if (firstMove) {
            std::cout << playerFirstActions;
        } else {
            std::cout << playerActions;
        }

        getline(std::cin, action);
        transformCase(action);

        if (action == "hit" || action == "1") {
            if (hit(dealerHand, playerHands, money, bets, numHand)) {
                return;
            }
        } else if (action == "stand" || action == "2") {
            std::cout << "Stand! \n";

            return;
        // } else if ((action == "split" || action == "3") && firstMove) {
            
        } else if ((action == "double" || action == "4") && firstMove) {
            if (doubleDown(dealerHand, playerHands, money, bets, numHand)) {
                return;
            }

        } else {
            std::cout << "Not a valid action \n";

            continue;
        }

        firstMove = false;
    }
}

void dealerAction(
    Hand &dealerHand, 
    std::vector<Hand> &playerHands,
    std::vector<int> bets
) {
    while(true) {
        printHands(dealerHand, playerHands, bets);
        
        if (dealerHand.handValue() < 17 || dealerHand.isSoft17()){
            std::cout << "\n --------Dealer draws a card-------- \n";

            dealerHand.addCards(1); 
        } else {
            return;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

bool dealerBust(
    Hand &dealerHand,
    std::vector<Hand> playerHands, 
    int &money, 
    std::vector<int> bets,
    int &won
) {
    if (dealerHand.bust()) {
        for (int i = 0; i < playerHands.size(); i++) {
            if (!playerHands[i].bust()) {
                money += 2 * bets[i];
                won++;
            }
        }

        int lost = playerHands.size() - won;

        std::cout << "Dealer has gone bust! \n";

        return true;
    }
    
    return false;
}

bool playerAllBust(std::vector<Hand> playerHands) {
    bool allBust = true;
    for (int i = 0; i < playerHands.size(); i++) {
        if (!playerHands[i].bust()) {
            allBust = false;
        }
    }

    if (allBust) {
        std::cout << "Sorry! You have busted all of your hands! \n";

        return true;
    }
 
    return false;
}

void roundEnd(
    int &money, 
    bool &play,
    int won,
    int lost,
    int pushHands
) {
    std::cout << "You won " << won << " hands, lost " << lost << 
        " hands, and pushed " << pushHands << " hands. \n"; 

    std::cout << "Money: " << money << std::endl;

    if (!money) {
        std::cout << "You are out of money! Would you like to start again? \n";

        checkPlay(play);
        std::cout << "Please enter your money amount: ";

        money = getAmount();
    } else {
        checkPlay(play);
    }
}

int main() {
    srand(time(0));

    std::cout << "Hello! Welcome to Blackjack. \n";

    bool play;
    checkPlay(play);

    std::cout << "Please enter your initial starting value: ";

    int money = getAmount();

    int hands = getHands();

    while (play) {
        int bet;
        std::vector<int> bets = getBets(money, hands);

        std::vector<Hand> playerHands;
        Hand dealerHand;
        initialCards(dealerHand, playerHands, bets, hands);

        int won = 0;
        int lost = 0;
        int pushHands = 0;
        
        if (instantBlackjack(dealerHand, playerHands, money, bets, pushHands)) {
            roundEnd(money, play, 0, hands - pushHands, pushHands);
            continue;
        }
        
        for (int i = 0; i < hands; i++) {
            std::cout << "Currently playing hand: " << i + 1 << std::endl;

            if (playerBlackjack(playerHands[i], money, bets[i])) {
                continue;
            }

            playerAction(dealerHand, playerHands, money, bets, i);
        }

        if (playerAllBust(playerHands)) {
            roundEnd(money, play, 0, hands, 0);
            continue;
        }

        dealerAction(dealerHand, playerHands, bets);

        if (dealerBust(dealerHand, playerHands, money, bets, won)) {
            roundEnd(money, play, won, hands - won, 0);
            continue;
        }

        for (int i = 0; i < hands; i++) {
            if (playerHands[i].bust()) {
                std::cout << "You busted on hand " << i + 1 << ". \n";
                continue;
            }

            if ((playerHands[i].checkBlackjack() && dealerHand.checkBlackjack()) ||
                (playerHands[i].handValue() == dealerHand.handValue())) {
                money += bets[i];
                pushHands++;

                std::cout << "You pushed hand " << i + 1 << "! \n";
            } else if (playerHands[i].checkBlackjack()) {
                money += bets[i] + static_cast<int>(1.5 * bets[i]);
                won++;

                std::cout << "You got a blackjack on hand " << i + 1 << "! \n";
            } else if (playerHands[i].handValue() > dealerHand.handValue()) {
                money += 2 * bets[i];
                won++;

                std::cout << "You scored higher than the dealer without busting "
                        "on hand " << i + 1 << "! \n";
            } else {
                lost++;

                std::cout << "The dealer has scored higher than you "
                        "on hand " << i + 1 << ". \n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        roundEnd(money, play, won, lost, pushHands);
    }

    std::cout << "Thank you. Exiting \n";

    return 0;
}
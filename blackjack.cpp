#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::this_thread;
using namespace chrono_literals;

/*
    TODO: split, do this with vectors of vectors, also side bets and insurance, and count
    TODO: Also being able to play multiple hands, but that is the same as splitting
    TODO: Add so that if the bet is 0, it does not play the hand, in case players bet wrong
    TODO: Make sentences have correct plurals and singulars (grammar)
    The rules of this blackjack is that dealer hits on soft 17, 
    or S17 blackjack. Can choose to play up to 3 hands simultaneously.
    Is implemented so that bet is deducted from money as soon as bet is placed
*/ 

const string deckCards[] = {"A", "2", "3", "4", "5", "6", "7",
    "8", "9", "10", "J", "Q", "K"};
    
const string playerFirstActions = "Player actions: \n"
"1: Hit "
"2: Stand "
"3: Split "
"4: Double \n";

const string playerActions = "Player actions: \n"
"1: Hit "
"2: Stand \n";

const string bustMsg = "You have bust this hand! \n";

const int faceVal = 10;
const int bjVal = 21;
    
void transformCase(string &word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
}

void checkPlay(bool &play) {
    cout << "Play \n"
            "1: Yes 2: No \n";

    string input;
    
    while (true) {
        getline(cin, input);
        transformCase(input);
    
        if (input == "yes" || input == "1") {
            play = true;
            break;
        } else if (input == "no" || input == "2") {
            play = false;
            break;
        } else {
            cout << "Try again \n";
        }
    }
}

int getAmount() {
    int money;

    while(true) {
        cin >> money;
    
        if (!cin || money <= 0) {
            cout << "Please enter a valid money amount: ";

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return money;
        }
    }
}

void addCards(vector<string> &hand, int cards) {
    int num;

    for (int i = 0; i < cards; i++) {
        num = rand() % 13; 
        string card = deckCards[num];
        hand.push_back(card);
    }
}


int getValue(string card) {
    if (card == "K" || card == "Q" || card == "J") {
        return faceVal;
    } else{
        return stoi(card);
    }
}

int handValue(vector<string> hand) {
    int aces = 0;
    int sum = 0;
    
    for (string card: hand) {
        if (card == "A") {
            aces++;
        } else {
            sum += getValue(card);
        }
    }
    
    if (aces) {
        int oneCount = 0;
        int currSum;
        
        while (aces) {
            currSum = sum + aces*11 + oneCount; 
            
            if (currSum <= 21) {
                return currSum;
            }
            
            oneCount++;
            aces--;
        }
    } 
    
    return sum;
}

bool checkBlackjack(vector<string> hand) {
    return handValue(hand) == bjVal;
}

bool playerBlackjack(vector<string> hand, int &money, int bet) {
    if (checkBlackjack(hand)) {
        money += bet + static_cast<int>(1.5 * bet);

        cout << "Blackjack! \n";

        return true;
    }

    return false;
}

bool bust(vector<string> hand) {
    return handValue(hand) > 21;
} 

bool playerBust(vector<string> hand) {
    if (bust(hand)) {
        cout << bustMsg;
        return true;
    }

    return false;
}

void printPlayerHands(vector<vector<string>> hands, vector<int> bets) {
    cout << "\n\t" << "Player's" << " Hands: \n\t";
    
    for (int i = 0; i < hands.size(); i++) {
        vector<string> hand = hands[i];

        cout << "\n\t" << "Hand " << i + 1 << ": \n\t";
        
        for (string card : hand) cout << "+---+ ";
        cout << "\n\t";
        
        for (string card : hand) {
            cout << "| " << setw(1) << card << " | ";
        }
        cout << "\n\t";
        
        for (string card : hand) cout << "+---+ ";
        cout << "\n\t";
        
        cout << "Bet: " << bets[i] << "\n\n";
    }
    
}

void printDealerHidden(vector<string> hand) {
    cout << "\n\t" << "Dealer's" << " Hand:\n\t";
    
    for (string card : hand) cout << "+---+ ";
    cout << "\n\t";
    
    int count = 0;
    for (string card : hand) {
        if (count == 0) {
            card = "?";
        }
        cout << "| " << setw(1) << card << " | ";
        count++;
    }
    cout << "\n\t";
    
    for (string card : hand) cout << "+---+ ";
    cout << "\n";
}

void printDealer(vector<string> hand) {
    cout << "\n\t" << "Dealer's" << " Hand:\n\t";
    
    for (string card : hand) cout << "+---+ ";
    cout << "\n\t";
    
    for (string card : hand) {
        cout << "| " << setw(1) << card << " | ";
    }
    cout << "\n\t";
    
    for (string card : hand) cout << "+---+ ";
    cout << "\n";
}

void printHandsHidden(
    vector<string> dealerHand, 
    vector<vector<string>> playerHands,
    vector<int> bets
) {
    printDealerHidden(dealerHand);
    printPlayerHands(playerHands, bets);
}

void printHands(
    vector<string> dealerHand,
    vector<vector<string>> playerHands,
    vector<int> bets
) {
    printDealer(dealerHand);
    printPlayerHands(playerHands, bets);
}

void initialCards(
    vector<string> &dealerHand,
    vector<vector<string>> &playerHands, 
    vector<int> bets,
    int hands
) {
    for (int i = 0; i < hands; i++) {
        vector<string> hand;
        addCards(hand, 2);
        playerHands.push_back(hand);
    }  
    
    addCards(dealerHand, 2);
    
    printHandsHidden(dealerHand, playerHands, bets);
}

int getHands() {
    cout << "How many hands would you like to play at once? "
            "The maximum is 3. \n";

    int hands;

    while(true) {
        cout << "Hands: ";

        cin >> hands;
    
        if (!cin || hands <= 0 || hands > 3) {
            cout << "Please enter a valid hand amount \n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return hands;
        }
    }
    
}

vector<int> getBets(int &money, int hands) {
    int bet = 0;
    vector<int> bets;

    for (int i = 0; i < hands; i++) {
        bet = 0;

        while (!bet) {
            cout << "Place your bet for hand " << i + 1 << ": ";

            bet = getAmount();
    
            if (bet > money || bet <= 0) {
                cout << "Invalid bet amount \n";

                bet = 0;
            }
        }

        money -= bet;
        bets.push_back(bet);
    }
    
    return bets;
}

bool instantBlackjack(
    vector<string> dealerHand, 
    vector<vector<string>> playerHands,
    int &money,
    vector<int> bets
) {
    if (checkBlackjack(dealerHand)) {
        int pushHands = 0;
        int hands = playerHands.size();

        for (int i = 0; i < hands; i++) {
            vector<string> currHand = playerHands[i];

            if (checkBlackjack(currHand)) {
                money += bets[i];
                pushHands++;
            }
        }

        printHands(dealerHand, playerHands, bets);
        int lost = hands - pushHands;

        cout << "The dealer has a Blackjack, you lost " << lost 
                << " hands and pushed " << pushHands << " hands. \n"; 

        return true;
    }

    return false;
}

bool hit(
    vector<string> dealerHand,
    vector<vector<string>> playerHands, 
    int &money, 
    vector<int> &bets,
    int numHand
) {
    vector<string> &hand = playerHands[numHand]; 
    int &bet = bets[numHand];

    addCards(hand, 1);

    cout << "Hit! \n";
    
    printHandsHidden(dealerHand, playerHands, bets);
    
    if (playerBlackjack(hand, money, bet) || 
        playerBust(hand)
    ) {
        return true;
    } 

    return false;
}

void doubleDown(
    vector<string> dealerHand,
    vector<vector<string>> playerHands, 
    int &money, 
    vector<int> &bets,
    int numHand
) {
    vector<string> &hand = playerHands[numHand]; 
    int &bet = bets[numHand];

    addCards(hand, 1);

    cout << "Double down! \n";

    money -= bet;
    bet *= 2;

    printHandsHidden(dealerHand, playerHands, bets);

    playerBlackjack(hand, money, bet);
    playerBust(hand);
}

void playerAction(
    vector<string> &dealerHand, 
    vector<vector<string>> &playerHands,
    int &money,
    vector<int> &bets,
    int numHand
) {
    bool firstMove = true;
    bool stand = false;
    string action;

    while (true) {
        if (firstMove) {
            cout << playerFirstActions;
        } else {
            cout << playerActions;
        }

        getline(cin, action);
        transformCase(action);

        if (action == "hit" || action == "1") {
            if (hit(dealerHand, playerHands, money, bets, numHand)) {
                return;
            }
        } else if (action == "stand" || action == "2") {
            cout << "Stand! \n";

            return;
        // } else if ((action == "split" || action == "3") && firstMove) {
            
        } else if ((action == "double" || action == "4") && firstMove) {
            doubleDown(dealerHand, playerHands, money, bets, numHand);

            return;
        } else {
            cout << "Not a valid action \n";

            continue;
        }

        firstMove = false;
    }
}

void dealerAction(
    vector<string> &dealerHand, 
    vector<vector<string>> &playerHands,
    vector<int> bets
) {
    while(true) {
        printHands(dealerHand, playerHands, bets);
        
        if (handValue(dealerHand) < 17){
            cout << "\n --------Dealer draws a card-------- \n";

            addCards(dealerHand, 1);
        } else {
            return;
        }
        
        sleep_for(2000ms);
    }
}

bool dealerBust(
    vector<string> dealerHand,
    vector<vector<string>> playerHands, 
    int &money, 
    vector<int> bets
) {
    if (bust(dealerHand)) {
        for (int i = 0; i < playerHands.size(); i++) {
            vector<string> currHand = playerHands[i];

            if (!bust(currHand)) {
                money += 2 * bets[i];
            }
        }
    }

    return false;
}

bool playerAllBust(vector<vector<string>> playerHands) {
    bool allBust = true;
    for (int i = 0; i < playerHands.size(); i++) {
        vector<string> currHand = playerHands[i];

        if (!bust(currHand)) {
            allBust = false;
        }
    }

    if (allBust) {
        cout << "Sorry! You have busted all of your hands! \n";

        return true;
    }
 
    return false;
}


int main() {
    srand(time(0));

    cout << "Hello! Welcome to Blackjack. \n";

    bool play;
    checkPlay(play);

    cout << "Please enter your initial starting value: ";

    int money = getAmount();

    int hands = getHands();

    while (play) {
        int bet;
        vector<int> bets = getBets(money, hands);

        vector<vector<string>> playerHands;
        vector<string> dealerHand;
        initialCards(dealerHand, playerHands, bets, hands);

        if (instantBlackjack(dealerHand, playerHands, money, bets)) {
            checkPlay(play);
            continue;
        }
        
        for (int i = 0; i < hands; i ++) {
            vector<string> currHand = playerHands[i];
            
            cout << "Currently playing hand: " << i + 1 << endl;

            if (playerBlackjack(currHand, money, bets[i])) {
                continue;
            }

            playerAction(dealerHand, playerHands, money, bets, i);
        }

        if (playerAllBust(playerHands)) {
            continue;
        }

        dealerAction(dealerHand, playerHands, bets);

        if (dealerBust(dealerHand, playerHands, money, bets)) {
            checkPlay(play);
            continue;
        }

        int pushHands = 0;
        int won = 0;
        int lost = 0;
        for (int i = 0; i < hands; i ++) {
            vector<string> currHand = playerHands[i];

            if (bust(currHand)) {
                cout << "You busted on hand " << i + 1 << ". \n";
                continue;
            }

            if ((checkBlackjack(currHand) && checkBlackjack(dealerHand)) ||
                (handValue(currHand) == handValue(dealerHand))) {
                money += bets[i];
                pushHands++;

                cout << "You pushed hand " << i + 1 << "! \n";
            } else if (checkBlackjack(currHand)) {
                money += bets[i] + static_cast<int>(1.5 * bets[i]);
                won++;

                cout << "You got a blackjack on hand " << i + 1 << "! \n";
            } else if (handValue(currHand) > handValue(dealerHand)) {
                money += 2 * bets[i];
                won++;

                cout << "You scored higher than the dealer without busting "
                        "on hand " << i + 1 << "! \n";
            } else {
                lost++;

                cout << "The dealer has scored higher than you "
                        "on hand " << i + 1 << ". \n";
            }

            sleep_for(1000ms);
        }

        cout << "You won " << won << " hands, lost " << lost << 
        " hands, and pushed " << pushHands << " hands. \n"; 

        cout << "Money: " << money << endl;

        if (!money) {
            cout << "You are out of money! Would you like to start again? \n"
                    "1: Yes 2: No \n";

            checkPlay(play);
            cout << "Please enter your money amount: ";

            money = getAmount();
        } else {
            checkPlay(play);
        }
    }

    cout << "Thank you. Exiting \n";

    return 0;
}   

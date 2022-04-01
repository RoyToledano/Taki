#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <stdbool.h>

#define RED 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define CARD_LEN 6
#define CARD_WID 9
#define NAME_LEN 20
#define MAX_CARDS 14
#define PLUS 10
#define STOP 11
#define CHANGE_DIRECT 12
#define TAKI 13
#define CHANGE_COLOR 14
#define NO_WINNER 1
#define REGULAR_CARDS_NUM 9
#define START_CARD 4

typedef struct Card
/* Types are symbold by:
* 1-9 : the regular numbers.
* 10: plus (+).
* 11: stop.
* 12: change direction (<->).
* 13: Taki.
* 14: change color.
* 
*  Colors are symbols by:
* 
* 0: No color
* 1: Red
* 2: Yellow
* 3: Green
* 4: Blue
*/
{
	int cardType;
	int cardColor; // for the statistics we use the card color to count the card frequency
	bool stop; // in case card stop remain in the top of the played cards.
	bool direct; // in case card change direction remain in the top of the played cards.
}CARD;

typedef struct Player
{
	char playerName[NAME_LEN];
	int numCards;
	int cardPhySize;
	CARD* playerCards;
}PLAYER;

void resetStats(CARD* stats);
void checkAlocPlayer(PLAYER* players);
void checkAlocCard(CARD* card);
void printWelcome();
void putInteger(int* num);
void getPlayersNames(PLAYER* players, int numPlayers);
void runGame(PLAYER players[], int numPlayers, CARD* stats);
void giveFourCars(PLAYER players[], int playerNum, CARD* stats);
CARD getCard(CARD* stats);
void printCard(CARD card);
void printCardType(int type);
void printCardColor(int color);
void freeCards(PLAYER* players, int numPlayers);
void printTurn(PLAYER* players, int playerNum, int numCards);
int printTurnMessage(int numCards);
bool makeTurn(PLAYER* players, int playerNum, int choice, CARD* upperCard, CARD* stats);
CARD* newSizeArr(CARD* cards, int size);
void copyArr(CARD* cards1, CARD* cards2, int size);
void printError();
void arrangeCards(CARD* cards, int choice, int size);
void cardCases(PLAYER* players, int playerNum, int typeChoice, CARD* uppercard, CARD* stats);
void turnPlus(PLAYER* players, int player, CARD* upperCard, CARD* stats);
void turnChangeColor(CARD* upperCard);
int printTurnTakiMessage(int numCards);
void turnTaki(PLAYER* players, int player, CARD* upperCard, CARD* stats);
void caseStop(int turnDrict, int numPlayers, int* player);
void makeTurnTaki(PLAYER* players, int* choice, int playerNum, CARD* upperCard);
void printStats(CARD* stats);
void sortStats(CARD* stats, int size);
void mergeArr(CARD* a1, int size1, CARD* a2, int size2, CARD* res);




void main()
{
	srand(time(NULL));
	CARD stats[MAX_CARDS];
	int numPlayers;
	PLAYER* players;
	resetStats(stats);
	printWelcome();
	putInteger(&numPlayers);
	players = (PLAYER*)malloc(numPlayers * sizeof(PLAYER));
	checkAlocPlayer(players);
	getPlayersNames(players, numPlayers);
	runGame(players, numPlayers, stats);
	printStats(stats);
	freeCards(players, numPlayers);
	free(players);
}

void printWelcome()
/* This function prints the first lines of the program.
*/
{
	printf("************ Welcome to TAKI game !!! ************\n");
	printf("Please enter the number of players:\n");
}

void resetStats(CARD* stats)
{
	int i;
	for (i = 0; i < MAX_CARDS; i++)
	{
		stats[i].cardType = i + 1;
		stats[i].cardColor = 0;
	}
}

void putInteger(int* num)
/* This function gets a number's pointer.
*  The function gets an input from the user and send the input to the given pointer.
*/
{
	int i;
	scanf("%d", &i);
	*num = i;
}

void getPlayersNames(PLAYER* players, int numPlayers)
/* This function gets an array of players and the number of players.
*  The function gets names input from the user and insert the name and the player number to each player in the array.
*/
{
	int i;
	for (i = 0; i < numPlayers; i++)
	{
		printf("Please enter the first name of player #%d:\n", i + 1);
		scanf("%s", players[i].playerName);
	}
}

void runGame(PLAYER players[], int numPlayers, CARD* stats)
/* This function gets an array of players and the number of players.
*  The function run the turns of the game.
*/
{
	bool moveTurn = true; 
	CARD upperCard; // Upper card is an arry of cards with only one upper card so the program will be able to change the card.
	int player, choice, turnDirec = 1;
	//Dealing 4 cards to each one of the players:
	for (player = 0; player < numPlayers; player++)
	{
		players[player].playerCards = (CARD*)malloc(START_CARD * sizeof(CARD));
		checkAlocCard(players[player].playerCards);
		giveFourCars(players, player, stats);
	}
	//Printing upper card:
	upperCard = getCard(stats);
	while (upperCard.cardType > REGULAR_CARDS_NUM) // if the upper card is not regular card, fixing the statistics also.
	{
		stats[upperCard.cardType - 1].cardColor--;
		upperCard = getCard(stats);
	}
	printf("Upper card:");
	printCard(upperCard);

	//Starting turns:
	player = 0;
	while (NO_WINNER) // run the while loop until we have a winner and break.
	{
		if (player >= numPlayers) // if last player put change direction
			player = 0;
		else if (turnDirec < 0 && player < 0) // if first player put change direction
			player = numPlayers - 1;
		printTurn(players, player, players[player].numCards);
		choice = printTurnMessage(players[player].numCards);
		while (!makeTurn(players, player, choice, &upperCard, stats)) // If the choice is invalid the program prints the turn again.
		{
			choice = printTurnMessage(players[player].numCards);
		}
		if (upperCard.cardType == STOP && upperCard.stop) 
		{
			if (players[player].numCards == 0 && numPlayers == 2) // in case there are only two players and a player put stop card last card
			{
				players[player].playerCards[0] = getCard(stats);
				players[player].numCards += 1;
				upperCard.stop = false;
			}
			else
			{
				caseStop(turnDirec, numPlayers, &player);
				upperCard.stop = false;
				moveTurn = false;
			}
		}
		if (players[player].numCards == 0)
			break;
		if (upperCard.cardType == CHANGE_DIRECT && numPlayers >= 3 && upperCard.direct) // case of changing direction
		{
			turnDirec *= (-1);
			player += turnDirec;
			upperCard.direct = false;
		}
		else if (moveTurn) // for stop card so the turn wont be moved foward in this function
		{
			player += turnDirec;
		}
		else
			moveTurn = true;
		printf("\nUpper card:");
		printCard(upperCard);
	}
	printf("\nThe winner is... %s! Congratulations!\n", players[player].playerName);

	
}

void giveFourCars(PLAYER players[], int playerNum, CARD* stats)
/* This function gets an array of players and the number of one player.
*  The function puts 4 random cards in the player's card array.
*/
{
	int card = 0;
	while (card < START_CARD)
	{
		players[playerNum].playerCards[card] = getCard(stats);
		card++;
	}
	players[playerNum].numCards = START_CARD;
	players[playerNum].cardPhySize = START_CARD;
}

CARD getCard(CARD* stats)
/* This function gets an array of the card's statistics.
*  The function creates a random card and return the card.
*  The function also adds the random card to the statistics.
*/
{
	CARD card;
	card.cardType = 1 + rand() % 14;
	if (card.cardType != CHANGE_COLOR)
		card.cardColor = 1 + rand() % 4;
	else
		card.cardColor = 0;
	stats[card.cardType - 1].cardColor++;
	return card;
}

void printCard(CARD card)
/* This function gets a card.
*  The function prints the card.
*/
{
	int row;
	printf("\n*********\n");
	for (row = 1; row < CARD_LEN-1; row++)
	{
		printf("*");
		if (row == 2)
			printCardType(card.cardType);
		else if (row == 3)
			printCardColor(card.cardColor);
		else
			printf("       ");
		printf("*\n");
	}
	printf("*********\n");
}

void printCardType(int type)
/* This function gets a card's type.
*  The function printing specific lane in the card printing duo to the card's type.
*/
{
	switch (type)
	{
	case PLUS:
		printf("   +   ");
		break;
	case STOP:
		printf("  STOP ");
		break;
	case CHANGE_COLOR:
		printf(" COLOR ");
		break;
	case CHANGE_DIRECT:
		printf("  <->  ");
		break;
	case TAKI:
		printf("  TAKI ");
		break;
	}
	
	if (type < 10)
		printf("   %d   ", type);
}

void printCardColor(int color)
/* This function gets a card's color.
*  The function printing specific lane in the card printing duo to the card's color.
*/
{
	switch (color)
	{
	case RED:
		printf("   R   ");
		break;
	case YELLOW:
		printf("   Y   ");
		break;
	case GREEN:
		printf("   G   ");
		break;
	case BLUE:
		printf("   B   ");
		break;
	case 0:
		printf("       ");
		break;
	}
}

void printTurn(PLAYER* players, int playerNum, int numCards)
/* This function gets a players array, specific player number and the amount of cards he has.
*  The function printing the player's cards.
*/
{
	int i;
	printf("%s's turn:\n", players[playerNum].playerName);
	for (i = 0; i < numCards; i++)
	{
		printf("\nCard #%d:\n", i + 1);
		printCard(players[playerNum].playerCards[i]);
	}
	printf("\n");
}

int printTurnMessage(int numCards)
/*  The function printing the turn steps and return the choice input.
*/
{
	int choice;
	printf("Please Enter 0 if you want to take a card from the deck\nor 1-%d if you want to put one of your cards in the middle: \n",numCards);
	putInteger(&choice);
	return choice;
}

bool makeTurn(PLAYER* players, int playerNum, int choice, CARD* upperCard, CARD* stats)
/* This function gets a players array, specific player number and the player's choice .
*  The function making the player's move duo to TAKI rules.
*  The function return false if the turn has faild, otherwise return true.
*/
{	
	CARD tempUpper = *upperCard;
	int size = players[playerNum].numCards;
	int tempChoice;
	if (choice == 0)   // taking card from the deck
	{
		if (players[playerNum].cardPhySize == size)
		{
			players[playerNum].playerCards = newSizeArr(players[playerNum].playerCards, size);
			players[playerNum].cardPhySize = size * 2;
		}
		players[playerNum].playerCards[size] = getCard(stats);
		players[playerNum].numCards = size + 1;
		return true;
	}
	else
	{
		if (players[playerNum].playerCards[choice - 1].cardColor != 0)
		{
			if (players[playerNum].playerCards[choice - 1].cardColor != tempUpper.cardColor && players[playerNum].playerCards[choice - 1].cardType != tempUpper.cardType)
				// Checking for not matching color choose or the number is not the same.
			{
				printError();
				return false;
			}
		}
		if (players[playerNum].playerCards[choice - 1].cardType < REGULAR_CARDS_NUM + 1) // if the card is regular number
		{
			*upperCard = players[playerNum].playerCards[choice - 1];
			arrangeCards(players[playerNum].playerCards, choice, size);
			players[playerNum].numCards = size - 1;
			return true;
		}
		else
		{
			tempChoice = players[playerNum].playerCards[choice - 1].cardType; // saving the card type befor the arrangeCards.
			*upperCard = players[playerNum].playerCards[choice - 1];
			arrangeCards(players[playerNum].playerCards, choice, size);
			players[playerNum].numCards = size - 1;
			cardCases(players, playerNum, tempChoice, upperCard, stats);
			return true;
		}
	}
	
}

CARD* newSizeArr(CARD* cards, int size)
/* This function gets a cards array and its size.
*  The function extending the array's size.
*/
{
	CARD* cardsTemp;
	cardsTemp = (CARD*)malloc((size * 2) * sizeof(CARD));
	checkAlocCard(cardsTemp);
	copyArr(cardsTemp,cards,size);
	free(cards);
	return cardsTemp;
}

void copyArr(CARD* cards1, CARD* cards2, int size)
/* This function gets two cards arrays and their size.
*  The function coping the information from cards2 to cards1.
*/
{
	int i;
	for (i = 0; i < size; i++)
	{
		cards1[i] = cards2[i];
	}
}

void printError()
// Prints an error message
{
	printf("Invalid card! Try again\n\n");
}

void arrangeCards(CARD* cards, int choice, int size)
/* This function gets an array of cards, its size and an index
*  The function swaping between the card in the given index and the last card.
*/
{
	CARD tempCard;
	tempCard = cards[choice - 1];
	cards[choice - 1] = cards[size-1];
	cards[size - 1] = tempCard;
}

void cardCases(PLAYER* players, int playerNum, int typeChoice ,CARD* uppercard, CARD* stats)
/* This function gets an array of players, the player number, the type of the card that was choosen, pointer to the upper card and the statistcs array.
*  The function helps to arrange the functions for every special card turn.
*/
{
	switch (typeChoice)
	{
	case PLUS:
		turnPlus(players, playerNum, uppercard, stats);
		break;
	case STOP:
		(*uppercard).stop = true;
		break;
	case CHANGE_COLOR:
		turnChangeColor(uppercard);
		break;
	case CHANGE_DIRECT: 
		(*uppercard).direct = true;
		break;
	case TAKI:
		turnTaki(players, playerNum, uppercard, stats);
		break;
	}
}

void turnPlus(PLAYER* players, int player, CARD* upperCard, CARD* stats)
/* This function gets an array of players, the player number, pointer to the upper card and the statistcs array.
*  The function making the Plus turn according to the Taki rules.
*/
{
	if (players[player].numCards == 0) // if last card is plus and the player has no more cards.
	{
		players[player].playerCards[0] = getCard(stats);
		players[player].numCards += 1;
	}
	int choice;
	printf("\nUpper card:");
	printCard(*upperCard);
	printTurn(players, player, players[player].numCards);
	choice = printTurnMessage(players[player].numCards);
	while (!makeTurn(players, player, choice, upperCard, stats))
	{
		choice = printTurnMessage(players[player].numCards);
	}
}

void turnChangeColor(CARD* upperCard)
/* This function get a pointer to the upper card
*  The function changing the upper card's color according to the player's pick.
*/
{
	int choice;
	printf("Please enter your color choice:\n1 - Red\n2 - Yellow\n3 - Green\n4 - Blue\n");
	putInteger(&choice);
	switch (choice)
	{
	case RED:
		(*upperCard).cardColor = RED;
		break;
	case YELLOW:
		(*upperCard).cardColor = YELLOW;
		break;
	case GREEN:
		(*upperCard).cardColor = GREEN;
		break;
	case BLUE:
		(*upperCard).cardColor = BLUE;
		break;
	}
}

void freeCards(PLAYER* players, int numPlayers)
/* This function gets a players array and its size .
*  The function free the cards arrays for each player.
*/
{
	int i;
	for (i = 0; i < numPlayers; i++)
		free(players[i].playerCards);
}

void checkAlocCard(CARD* card)
// Check allocation for the cards array
{
	if (card == NULL)
	{
		printf("Error in allocation\n");
		exit(1);
	}
}

void checkAlocPlayer(PLAYER* players)
// Check allocation for the players array
{
	if (players == NULL)
	{
		printf("Error in allocation\n");
		exit(1);
	}
}

void turnTaki(PLAYER* players, int player, CARD* upperCard, CARD* stats) 
/* This function gets an array of players, the player number, pointer to the upper card and the statistcs array
*  The function making the Taki turn according to the Taki rules.
*/ 
{
	CARD tempUpper;
	int choice;
	printf("\nUpper card:");
	printCard(*upperCard);
	printTurn(players, player, players[player].numCards);
	choice = printTurnTakiMessage(players[player].numCards);
	while (choice != 0)
	{
		if (players[player].playerCards[choice - 1].cardType == CHANGE_COLOR) // in case player put change color while turn taki.
		{
			turnChangeColor(upperCard);
			arrangeCards(players[player].playerCards, choice, players[player].numCards);
			players[player].numCards -= 1;
			break;
		}
		makeTurnTaki(players, &choice, player, upperCard);
		if (players[player].numCards == 0) // if the player finish all of his cards while turn taki.
			break;
		if (choice == 0) // in case the player input 0 after the error message.
			break;
		printCard(*upperCard);
		printTurn(players, player, players[player].numCards);
		choice = printTurnTakiMessage(players[player].numCards);
	}
	tempUpper = *upperCard;
	// checking the last card
	if (tempUpper.cardType == PLUS && players[player].numCards == 0) // if last card is plus and the player has no more cards.
	{
		players[player].playerCards[0] = getCard(stats);
		players[player].numCards += 1;
	}
		
}

int printTurnTakiMessage(int numCards)
/* This function gets the number of cards the player have
*  The function prints the Taki turn message and get an input of the choice
*  The function returns the input.
*/
{
	int choice;
	printf("Please Enter 0 if you want to finish your turn\nor 1-%d if you want to put one of your cards in the middle: \n", numCards);
	putInteger(&choice);
	return choice;
}

void caseStop(int turnDrict, int numPlayers, int* player)
/* This function gets the turn direction (1 go foward, -1 go backwards), the number of players and a pointer to the player's number
*  The function stop the next player's turn by cases.
*/
{
	if (numPlayers == 2) // if there are only 2 players
	{
		if (*player == 1)
			*player = 1;
		else
			*player = 0;
	}
	else // case of 3 players and more
	{
		if (*player == (numPlayers - 1))
		{
			if (turnDrict<0)
				*player += 2 * turnDrict;
			else
				*player = 1;
		}
		else
			*player += 2 * turnDrict;
	}
}

void makeTurnTaki(PLAYER* players, int* choice, int playerNum, CARD* upperCard)
/* This function gets an array of players, a pointer to the player's choice, the player number and a pointer to the upper card.
*  This function helps "turnTaki" by creating a loop in case the player does not put the same color as the Taki card.
*/
 {
	CARD tempUpper = *upperCard;
	while (players[playerNum].playerCards[(*choice) - 1].cardColor != tempUpper.cardColor && (*choice) != 0 && players[playerNum].playerCards[(*choice) - 1].cardType != CHANGE_COLOR)
		// check after the error if the card has the same color or player picked change color or player input 0.
	{
		printError();
		(*choice) = printTurnTakiMessage(players[playerNum].numCards);
	}
	if ((*choice) != 0)
	{
		*upperCard = players[playerNum].playerCards[(*choice) - 1];
		arrangeCards(players[playerNum].playerCards, (*choice), players[playerNum].numCards);
		players[playerNum].numCards -= 1;
	}
	else if (players[playerNum].playerCards[(*choice) - 1].cardType == CHANGE_COLOR) // in case player put change color after the error message.
	{
		turnChangeColor(upperCard);
		arrangeCards(players[playerNum].playerCards, (*choice), players[playerNum].numCards);
		players[playerNum].numCards -= 1;
	}
	
}

void printStats(CARD* stats)
/* This function gets an array of the card's statistics (arranged in the Card struct, the color present the card frequency during the game).
*  The function sort the array and prints the array in the end of the game.
*/
{
	int i;
	printf("************ Game Statistics ************\n");
	printf("Card # | Frequency\n_________________");
	sortStats(stats,MAX_CARDS);
	for (i = MAX_CARDS-1; i >= 0; i--)
	{
		if (stats[i].cardType < 10)
			printf("\n   %d   |    %d     ", stats[i].cardType, stats[i].cardColor);
		else
		{
			switch (stats[i].cardType)
			{
			case PLUS:
				printf("\n   +   |    %d     ", stats[i].cardColor);
				break;
			case STOP:
				printf("\n STOP  |    %d     ", stats[i].cardColor);
				break;
			case CHANGE_COLOR:
				printf("\n COLOR |    %d     ", stats[i].cardColor);
				break;
			case CHANGE_DIRECT:
				printf("\n  <->  |    %d     ", stats[i].cardColor);
				break;
			case TAKI:
				printf("\n TAKI  |    %d     ", stats[i].cardColor);
				break;
			}
		}
	}
	printf("\n");
}

void sortStats(CARD* stats, int size)
/* This function gets an array of statistics and its size.
*  The function sort the array be the card amount of appearance.
*/
{
	CARD* tempArr = NULL;
	if (size <= 1)
		return;
	sortStats(stats, size / 2);
	sortStats(stats + (size / 2), size - (size / 2));
	tempArr = (CARD*)malloc(size * sizeof(CARD));
	if (tempArr)
	{
		mergeArr(stats, size / 2, stats + (size / 2), size - (size / 2), tempArr);
		copyArr(stats, tempArr, size);
		free(tempArr);
	}
	else
	{
		printf("Error in allocation\n");
		exit(1);
	}
		
	
}

void mergeArr(CARD* a1, int size1, CARD* a2, int size2, CARD* res)
/* This function gets two arrays of cards statistics, their size and an empty array.
*  The function helps "sortStats" by coping the arranged cards from the two arrays to the empty one by their frequency (the number in the color)
*/
{
	int index1, index2, indexRes;
	index1 = index2 = indexRes = 0;
	while (index1 < size1 && index2 < size2)
	{
		if (a1[index1].cardColor < a2[index2].cardColor)
		{
			res[indexRes] = a1[index1];
			index1++;
			indexRes++;
		}
		else
		{
			res[indexRes] = a2[index2];
			index2++;
			indexRes++;
		}
	}
	while (index1 < size1)
	{
		res[indexRes] = a1[index1];
		index1++;
		indexRes++;
	}
	while (index2 < size2)
	{
		res[indexRes] = a2[index2];
		index2++;
		indexRes++;
	}
}
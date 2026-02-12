#pragma once
#include "Game.h"

class Connect4 : public Game 
{
public:
    Connect4();
    ~Connect4();

    // Required virtual methods from Game base class
    void        setUpBoard() override;
    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override {return false;}
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override {return false;}
    void        stopGame() override;

    // AI methods
    void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid* getGrid() override { return _grid; }

private:
    // Constants for piece types
    static const int EMPTY = 0;
    static const int RED_PIECE = 1;
    static const int YELLOW_PIECE = 2;

    // Player constants
    static const int RED_PLAYER = 0;
    static const int YELLOW_PLAYER = 1;

    // Helper methods
    int winnerAlg(const std::string& s) const;
    Bit*        PieceForPlayer(int playerNumber);
    bool        getBoardPosition(BitHolder &holder, int &x, int &y) const;
    ChessSquare* lowestEmptyInColumn(int col) const;
    Player*     ownerAt(int x, int y) const;

    int dropRow(const std::string& s, int col) const;
    int makeMove(std::string& s, int col, int player) const;
    void undoMove(std::string& s, int col, int row) const;

    int eval(const std::string& s, int player) const;
    int negamax(std::string& s, int player, int depth, int alpha, int beta);

    // Board representation
    Grid*        _grid;

    // Layout
    float _squareSize;

    // Game state
    int         _redPieces;
    int         _yellowPieces;
};
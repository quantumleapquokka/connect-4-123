#include "Connect4.h"
#include "Bit.h"



Connect4::Connect4() : Game() 
{
    _grid = new Grid(7, 6);
    _squareSize = 80.0f;
    _redPieces = 0;
    _yellowPieces = 0;
}

Connect4::~Connect4()
{
    delete _grid;
    _grid = nullptr;
}

Bit* Connect4::PieceForPlayer(int playerNumber)
{
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == RED_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber));

    return bit;
}

void Connect4::setUpBoard(){
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(_squareSize, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();

}

bool Connect4::actionForEmptyHolder(BitHolder &holder) 
{
    int col = 0;
    int row = 0;
    if (!getBoardPosition(holder, col, row)) {
        return false;
    }

    ChessSquare* dst = lowestEmptyInColumn(col);
    if (!dst) {
        return false;
    }

    const int currentPlayer = getCurrentPlayer()->playerNumber();
    Bit* bit = PieceForPlayer(currentPlayer);
    if (!bit) return false;

    ImVec2 finalPos = dst->getPosition();
    ChessSquare* top = _grid->getSquare(col, 0);

    if (top) {
        ImVec2 startPos = top->getPosition();
        startPos.y -= _squareSize * 0.9f;
        bit->setPosition(startPos);
        dst->setBit(bit);
        bit->moveTo(finalPos);
    } else {
        bit->setPosition(finalPos);
        dst->setBit(bit);
    }

    endTurn();
    return true;
}

void Connect4::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Connect4::checkForWinner()
{
    int w = winnerAlg(stateString());
    if (w == -1) return nullptr;
    return getPlayerAt(w);
}

bool Connect4::checkForDraw()
{
    bool full = true;
    _grid->forEachSquare([&](ChessSquare* square, int /*x*/, int /*y*/) {
        if (!square->bit()) {
            full = false;
        }
    });
    return full;
}

std::string Connect4::initialStateString()
{
    return std::string(7 * 6, '0');
}

std::string Connect4::stateString()
{
    std::string s(7 * 6, '0');
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit* bit = square->bit();
        if (bit && bit->getOwner()) {
            s[y * 7 + x] = char('0' + (bit->getOwner()->playerNumber() + 1));
        }
    });
    return s;
}

void Connect4::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int idx = y * 7 + x;
        if (idx < 0 || idx >= (int)s.size()) {
            square->destroyBit();
            return;
        }

        char c = s[idx];
        if (c == '1') {
            square->setBit(PieceForPlayer(RED_PLAYER));
        } else if (c == '2') {
            square->setBit(PieceForPlayer(YELLOW_PLAYER));
        } else {
            square->setBit(nullptr);
        }
    });
}

Player* Connect4::ownerAt(int x, int y) const
{
    ChessSquare* square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

bool Connect4::getBoardPosition(BitHolder& holder, int& x, int& y) const
{
    bool found = false;
    _grid->forEachSquare([&](ChessSquare* square, int sx, int sy) {
        if (!found && square == &holder) {
            x = sx;
            y = sy;
            found = true;
        }
    });
    return found;
}

ChessSquare* Connect4::lowestEmptyInColumn(int col) const
{
    for (int y = 6 - 1; y >= 0; --y) {
        ChessSquare* square = _grid->getSquare(col, y);
        if (square && !square->bit()) {
            return square;
        }
    }
    return nullptr;
}

int Connect4::winnerAlg(const std::string& s) const
{
    auto at = [&](int x, int y) -> char { return s[y * 7 + x]; };

    for (int y = 0; y < 6; y++) {
        for (int x = 0; x <= 7 - 4; x++) {
            char a = at(x,y);
            if (a != '0' && a == at(x+1,y) && a == at(x+2,y) && a == at(x+3,y))
                return (a - '1');
        }
    }

    for (int x = 0; x < 7; x++) {
        for (int y = 0; y <= 6 - 4; y++) {
            char a = at(x,y);
            if (a != '0' && a == at(x,y+1) && a == at(x,y+2) && a == at(x,y+3))
                return (a - '1');
        }
    }

    for (int x = 0; x <= 7 - 4; x++) {
        for (int y = 0; y <= 6 - 4; y++) {
            char a = at(x,y);
            if (a != '0' && a == at(x+1,y+1) && a == at(x+2,y+2) && a == at(x+3,y+3))
                return (a - '1');
        }
    }

    for (int x = 0; x <= 7 - 4; x++) {
        for (int y = 3; y < 6; y++) {
            char a = at(x,y);
            if (a != '0' && a == at(x+1,y-1) && a == at(x+2,y-2) && a == at(x+3,y-3))
                return (a - '1');
        }
    }

    return -1;
}

int Connect4::dropRow(const std::string& s, int col) const
{
    for (int row = 6 - 1; row >= 0; row--) {
        if (s[row * 7 + col] == '0') return row;
    }
    return -1;
}

int Connect4::makeMove(std::string& s, int col, int player) const
{
    int row = dropRow(s, col);
    if (row < 0) return -1;
    s[row * 7 + col] = char('1' + player);
    return row;
}

void Connect4::undoMove(std::string& s, int col, int row) const
{
    if (row >= 0) s[row * 7 + col] = '0';
}

int Connect4::eval(const std::string& s, int player) const
{
    const char me  = char('1' + player);
    const char opp = char('1' + (1 - player));

    auto at = [&](int x, int y) -> char { return s[y * 7 + x]; };

    auto scoreWindow = [&](char a, char b, char c, char d) -> int {
        int m=0,o=0,e=0;
        char w[4] = {a,b,c,d};
        for (char v : w) {
            if (v == me) m++;
            else if (v == opp) o++;
            else e++;
        }

        if (m == 4) return 100000;
        if (m == 3 && e == 1) return 50;
        if (m == 2 && e == 2) return 10;

        if (o == 4) return -100000;
        if (o == 3 && e == 1) return -80;
        if (o == 2 && e == 2) return -10;

        return 0;
    };

    int score = 0;

    int center = 7 / 2;
    for (int y = 0; y < 6; y++) {
        if (at(center,y) == me) score += 6;
        else if (at(center,y) == opp) score -= 6;
    }

    for (int y = 0; y < 6; y++)
        for (int x = 0; x <= 7 - 4; x++)
            score += scoreWindow(at(x,y), at(x+1,y), at(x+2,y), at(x+3,y));

    for (int x = 0; x < 7; x++)
        for (int y = 0; y <= 6 - 4; y++)
            score += scoreWindow(at(x,y), at(x,y+1), at(x,y+2), at(x,y+3));

    for (int x = 0; x <= 7 - 4; x++)
        for (int y = 0; y <= 6 - 4; y++)
            score += scoreWindow(at(x,y), at(x+1,y+1), at(x+2,y+2), at(x+3,y+3));

    for (int x = 0; x <= 7 - 4; x++)
        for (int y = 3; y < 6; y++)
            score += scoreWindow(at(x,y), at(x+1,y-1), at(x+2,y-2), at(x+3,y-3));

    return score;
}

int Connect4::negamax(std::string& s, int player, int depth, int alpha, int beta)
{
    int w = winnerAlg(s);
    if (w != -1) {
        return (w == player) ? (1000000 + depth) : (-1000000 - depth);
    }

    bool full = true;
    for (char c : s) if (c == '0') { full = false; break; }
    if (full || depth == 0) return eval(s, player);

    static const int order[7] = {3,2,4,1,5,0,6};

    int best = -1000000000;
    bool anyMove = false;

    for (int i = 0; i < 7; i++) {
        int col = order[i];

        int row = makeMove(s, col, player);
        if (row < 0) continue;
        anyMove = true;

        int score = -negamax(s, 1 - player, depth - 1, -beta, -alpha);

        undoMove(s, col, row);

        if (score > best) best = score;
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;
    }
    if (!anyMove) return 0;

    return best;
}

void Connect4::updateAI()
{
    if (checkForWinner() != nullptr || checkForDraw()) return;
    if (!getCurrentPlayer() || !getCurrentPlayer()->isAIPlayer()) return;

    int turn = getCurrentPlayer()->playerNumber();
    std::string s = stateString();

    const int depth = 4;

    int bestScore = -1000000000;
    int bestCol = -1;

    static const int order[7] = {3,2,4,1,5,0,6};

    for (int i = 0; i < 7; i++) {
        int col = order[i];

        int row = makeMove(s, col, turn);
        if (row < 0) continue;

        int score = -negamax(s, 1 - turn, depth - 1, -1000000000, 1000000000);

        undoMove(s, col, row);

        if (score > bestScore) {
            bestScore = score;
            bestCol = col;
        }
    }

    if (bestCol < 0) return;

    ChessSquare* choice = _grid->getSquare(bestCol, 0);
    if (choice) actionForEmptyHolder(*choice);
}

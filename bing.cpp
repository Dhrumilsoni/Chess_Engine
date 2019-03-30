#include<bits/stdc++.h>
using namespace std;

char board[8][8];

struct ChessBoard {
    enum class Turn {white, black} turn;
    enum class Piece {king, queen, white_pawn, black_pawn, rook, bishop, knight};
    static map<Piece,int> pieceValues;
    bool show_coordinates = false;
 
    struct Pos{
        int x,y;
        Pos(const Pos &p, int dx=0, int dy=0){ *this = p; x+=dx; y+=dy;}
        Pos(int _x, int _y){  x=_x; y=_y; }
        bool operator<(const Pos & p) const { return (x < p.x) || (x==p.x && y < p.y); }
        bool operator==(const Pos & p) const { return x==p.x && y==p.y; }
        Pos(){x=-1;y=-1;}};
 
    map<Pos,Piece> white_pieces, black_pieces;
    map<Pos,Piece> & moverPieces(){ return turn == Turn::white ? white_pieces : black_pieces; }
    map<Pos,Piece> & opponentPieces(){ return turn == Turn::white ? black_pieces : white_pieces; }


    void reset(){
        turn = Turn::black;
        white_pieces.clear();
        black_pieces.clear();
        for (int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                if (board[i-1][j-1] == 'p') {
                    black_pieces[Pos(i,j)] = Piece::black_pawn;
                }
                if (board[i-1][j-1] == 'P') {
                    white_pieces[Pos(i,j)] = Piece::white_pawn;
                }

                if (board[i-1][j-1] == 'r') {
                    black_pieces[Pos(i,j)] = Piece::rook;
                }
                if (board[i-1][j-1] == 'R') {
                    white_pieces[Pos(i,j)] = Piece::rook;
                }

                if (board[i-1][j-1] == 'n') {
                    black_pieces[Pos(i,j)] = Piece::knight;
                }
                if (board[i-1][j-1] == 'N') {
                    white_pieces[Pos(i,j)] = Piece::knight;
                }

                if (board[i-1][j-1] == 'b') {
                    black_pieces[Pos(i,j)] = Piece::bishop;
                }
                if (board[i-1][j-1] == 'B') {
                    white_pieces[Pos(i,j)] = Piece::bishop;
                }

                if (board[i-1][j-1] == 'q') {
                    black_pieces[Pos(i,j)] = Piece::queen;
                }
                if (board[i-1][j-1] == 'Q') {
                    white_pieces[Pos(i,j)] = Piece::queen;
                }

                if (board[i-1][j-1] == 'k') {
                    black_pieces[Pos(i,j)] = Piece::king;
                }
                if (board[i-1][j-1] == 'K') {
                    white_pieces[Pos(i,j)] = Piece::king;
                }

            }
        }
    }
 
    void flipTurn(){ turn = turn == Turn::white? Turn::black : Turn::white; }
 
    bool makeMove(Pos from, Pos to){
        vector<Pos> allowed = possibleMoves(from);
        if(find(allowed.begin(), allowed.end(), to) == allowed.end())
            return false;
        opponentPieces().erase(to);
        moverPieces()[to]=moverPieces()[from];
        moverPieces().erase(from);
        if((moverPieces()[to]==Piece::white_pawn || moverPieces()[to]==Piece::black_pawn) && (to.y == 1 || to.y == 8))
            moverPieces()[to]=Piece::queen;
        flipTurn();
        return true;}
 
    vector<Pos> possibleMoves(const Pos & from){
        vector<Pos> moves;
        auto isOwn = [&] (int dx, int dy) -> bool { return moverPieces().count(Pos(from,dx,dy));};
        auto isOpponent = [&] (int dx, int dy) -> bool { return opponentPieces().count(Pos(from,dx,dy));};
        auto isInsideBoard = [&] (int dx, int dy) -> bool { Pos p(from,dx,dy); return p.x < 9 && p.x > 0 && p.y < 9 && p.y > 0; };
        auto isFree = [&] (int dx, int dy) -> bool { return !isOwn(dx,dy) && isInsideBoard(dx,dy) && !isOpponent(dx,dy); };

        auto addMove = [&] (int dx, int dy) -> bool {
            if(isFree(dx,dy) || isOpponent(dx,dy)){
                moves.push_back(Pos(from,dx,dy));
                return true;
            }
            return false;
        };

        if(!isOwn(0,0))
            return moves;

        auto moving_piece = moverPieces()[from];
        switch(moving_piece){
            case Piece::white_pawn:
                if(isFree(0,-1)) addMove(0,-1);
                if(isFree(0,-1) && isFree(0,-2) && from.y==7) addMove(0,-2);
                if(isOpponent(-1,-1)) addMove(-1,-1);
                if(isOpponent(1,-1)) addMove(1,-1); 
                break;

            case Piece::black_pawn:
                if(isFree(0,1)) addMove(0,1);
                if(isFree(0,1) && isFree(0,2) && from.y==2) addMove(0,2);
                if(isOpponent(-1,1)) addMove(-1,1);
                if(isOpponent(1,1)) addMove(1,1);
                break;

            // case Piece::white_pawn:
            //     if(isFree(-1,0)) addMove(-1,0);
            //     if(isFree(-1,0) && isFree(-2,0) && from.y==7) addMove(-2,0);
            //     if(isOpponent(-1,-1)) addMove(-1,-1);
            //     if(isOpponent(-1,1)) addMove(-1,1);
            //     break;

            // case Piece::black_pawn:
            //     if(isFree(1,0)) addMove(1,0);
            //     if(isFree(1,0) && isFree(2,0) && from.y==2) addMove(2,0);
            //     if(isOpponent(1,-1)) addMove(1,-1);
            //     if(isOpponent(1,1)) addMove(1,1);
                break;

            case Piece::knight:
                addMove(-2,-1); addMove(-2,1); addMove(2,-1); addMove(2,1);
                addMove(-1,-2); addMove(-1,2); addMove(1,-2); addMove(1,2);
                break;

            case Piece::king:
                for(auto dy : {-1,0,1})
                for(auto dx : {-1,0,1})
                addMove(dy,dx);
                break;

            case Piece::queen:
            case Piece::rook:
                for(int n=1; n < 9 && addMove(0,n) && !isOpponent(0,n); ++n);
                for(int n=1; n < 9 && addMove(0,-n) && !isOpponent(0,-n); ++n);
                for(int n=1; n < 9 && addMove(n,0) && !isOpponent(n,0); ++n);
                for(int n=1; n < 9 && addMove(-n,0) && !isOpponent(-n,0); ++n);
                if(moving_piece != Piece::queen)
                    break;

            case Piece::bishop:
                for(int n=1; n < 9 && addMove(n,n) && !isOpponent(n,n); ++n);
                for(int n=1; n < 9 && addMove(n,-n) && !isOpponent(n,-n); ++n);
                for(int n=1; n < 9 && addMove(-n,n) && !isOpponent(-n,n); ++n);
                for(int n=1; n < 9 && addMove(-n,-n) && !isOpponent(-n,-n); ++n);
                break;
        }

        return moves;
    }
 
    int score(){
        int sumWhite = 0;
        for(auto & p : white_pieces)
            sumWhite += pieceValues[p.second];
        int sumBlack = 0;
        for(auto & p : black_pieces)
            sumBlack += pieceValues[p.second];
        return sumWhite-sumBlack;}
 
    struct Move{
        Pos from,to;
        int score;};
 
    Move minimax(int depth, bool minimize){
        Move best_move;
        best_move.score = -1000000 + 2000000*minimize;
        if(0 == depth){
            best_move.score = score();
            return best_move;
        }
 
        for(auto & from : moverPieces()){
            for(auto & to : possibleMoves(from.first)){
                ChessBoard branch = *this;
                branch.makeMove(from.first,to);
                Move option = branch.minimax(depth-1, !minimize);
                if((option.score > best_move.score && !minimize) || (option.score < best_move.score && minimize)){
                    best_move.score = option.score;
                    best_move.from = from.first;
                    best_move.to = to;
                }
            }
        }
        return best_move;
    }
 
    void AIMove(){
        bool minimize = turn == Turn::black ? true : false;
        Move m = minimax(2,minimize);
        //cout << board[m.from.x - 1][m.from.y - 1];
        //cout << board[m.to.x - 1][m.to.y - 1];
        //cout << m.from.x << m.from.y << m.to.x << m.to.y << "\n"; 

        int reti = 9 - m.from.y;
        int retj = 9 - m.from.x - 1;
        retj = retj + 'A';

        int retx = 9 - m.to.y;
        int rety = 9 - m.to.x - 1;
        rety = rety + 'A';


        ofstream file;
        file.open("inp.in");  
        file << char(retj);
        file << reti;
        file << char(rety);
        file << retx;
        file.close();
        cerr << char(retj) << reti << char(rety) << retx;
    }
};
 
map<ChessBoard::Piece,int> ChessBoard::pieceValues {{ChessBoard::Piece::king, 10000},
{ChessBoard::Piece::queen, 9}, {ChessBoard::Piece::black_pawn, 1}, {ChessBoard::Piece::white_pawn, 1},
{ChessBoard::Piece::bishop, 3},{ChessBoard::Piece::knight, 3},{ChessBoard::Piece::rook, 5},};



int main(){
	ifstream fi;
	fi.open("curr_board.in");
	string line;
	fi>>line;
	fi.close();
	int len = line.length();
	int i=0,j=0;
    char board1[8][8];
	for(int k=0;k<len;k++){
		if(i==7 && j==8){
			break;
		}else if(line[k]=='/'){
			i++;
			j=0;
		}else if(line[k]>='0' && line[k]<='9'){
			int n = line[k]-'0';
			while(n--) board1[i][j++]='-';
		}else board1[i][j++]=line[k];
	}
    for (int ii = 0, ri = 0; ii < 8; ii++, ri++) {
        for (int jj = 0, rj = 7; jj < 8; jj++, rj--) {
            board[rj][ri] =  board1[ii][jj];
        }   
    }





    ChessBoard game;
    game.reset();
    game.AIMove();
	return 0;
}

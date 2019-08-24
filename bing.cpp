#include<bits/stdc++.h>
using namespace std;
#define engine 2
#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)
char board[9][9];
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
        turn = Turn::white;

        white_pieces.clear();
        black_pieces.clear();
        for (int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                if (board[i][j] == 'p') {
                    //cout<<"b";
                    black_pieces[Pos(i,j)] = Piece::black_pawn;
                }
                if (board[i][j] == 'P') {
                    //cout<<"w";
                    white_pieces[Pos(i,j)] = Piece::white_pawn;
                }

                if (board[i][j] == 'r') {
                    black_pieces[Pos(i,j)] = Piece::rook;
                }
                if (board[i][j] == 'R') {
                    white_pieces[Pos(i,j)] = Piece::rook;
                }

                if (board[i][j] == 'n') {
                    black_pieces[Pos(i,j)] = Piece::knight;
                }
                if (board[i][j] == 'N') {
                    white_pieces[Pos(i,j)] = Piece::knight;
                }

                if (board[i][j] == 'b') {
                    black_pieces[Pos(i,j)] = Piece::bishop;
                }
                if (board[i][j] == 'B') {
                    white_pieces[Pos(i,j)] = Piece::bishop;
                }

                if (board[i][j] == 'q') {
                    black_pieces[Pos(i,j)] = Piece::queen;
                }
                if (board[i][j] == 'Q') {
                    white_pieces[Pos(i,j)] = Piece::queen;
                }

                if (board[i][j] == 'k') {
                    black_pieces[Pos(i,j)] = Piece::king;
                }
                if (board[i][j] == 'K') {
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
            if((isFree(dx,dy) || isOpponent(dx,dy)) && isInsideBoard(dx, dy)){
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
                if(isFree(-1,0)) addMove(-1,0);
                if(isFree(-1,0) && isFree(-2,0) && from.x==7) addMove(-2,0);
                if(isOpponent(-1,-1)) addMove(-1,-1);
                if(isOpponent(-1,1)) addMove(-1,1); 
                break;

            case Piece::black_pawn:
                if(isFree(1,0)) addMove(1,0);
                if(isFree(1,0) && isFree(2,0) && from.x==2) addMove(2,0);
                if(isOpponent(1,-1)) addMove(1,-1);
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
            //    break;

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
    /*
    int occupied() {
        int white_occupied = 0, black_occupied = 0;
        int board1[9][9];
        if(turn!=Turn::white){
            flipTurn();
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                board1[i][j] = 0;
            }
        }
        for(auto &from : white_pieces) {
            board1[from.first.x][from.first.y]=1;
            for(auto &to : possibleMoves(from.first)) {
                   board1[to.x][to.y] = 1;
            }
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                //cout<<board1[i][j]<<" ";
                if(board1[i][j]){
                    white_occupied++;
                }
            }
            //cout<<"\n";
        }
        flipTurn();
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                board1[i][j] = 0;
            }
        }
        for(auto &from : black_pieces) {
            board1[from.first.x][from.first.y]=1;
            for(auto &to : possibleMoves(from.first)) {
                    board1[to.x][to.y] = 1;
            }
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                if(board1[i][j])
                    black_occupied++;
            }
        }
        //cout<<white_occupied<<" "<<black_occupied<<"\n";
        return white_occupied - black_occupied;
    }
*/
    int bishop_score(){
        int cnt=0;
        for(auto &p : white_pieces){
            if(p.second==Piece::bishop || p.second==Piece::queen){
                cnt++;
            }
        }
        return cnt;
    }

    int opponent_bishop_score(){
        int cnt=0;
        for(auto &p : black_pieces){
            if(p.second==Piece::bishop || p.second==Piece::queen){
                cnt++;
            }
        }
        return cnt;
    }

    int king_safety(){
        int cnt=0;
        if(turn==Turn::black)
            flipTurn();
        for(auto &p : white_pieces){
            if(p.second==Piece::king){
                for(auto &to : possibleMoves(p.first)) {
                   cnt++;
                }
            }
        }
        return cnt;
    }

    int opponent_king_safety(){
        int cnt=0;
        if(turn==Turn::white)
            flipTurn();
        for(auto &p : black_pieces){
            if(p.second==Piece::king){
                for(auto &to : possibleMoves(p.first)) {
                   cnt++;
                }
            }
        }
        return cnt;
    }


    int occupied() {
        int white_occupied = 0, black_occupied = 0;
        int board1[9][9];
        if(turn!=Turn::white){
            flipTurn();
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                board1[i][j] = 0;
            }
        }
        for(auto &from : white_pieces) {
            board1[from.first.x][from.first.y]=1;
            for(auto &to : possibleMoves(from.first)) {
                   board1[to.x][to.y] = 1;
            }
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                //cout<<board1[i][j]<<" ";
                if(board1[i][j]){
                    white_occupied++;
                }
            }
            //cout<<"\n";
        }
        flipTurn();
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                board1[i][j] = 0;
            }
        }
        for(auto &from : black_pieces) {
            board1[from.first.x][from.first.y]=1;
            for(auto &to : possibleMoves(from.first)) {
                    board1[to.x][to.y] = 1;
            }
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                if(board1[i][j])
                    black_occupied++;
            }
        }
        //cout<<white_occupied<<" "<<black_occupied<<"\n";
        return white_occupied - black_occupied;
    }

    int woccupied() {
        int white_occupied = 0, black_occupied = 0;
        int board1[9][9];
        if(turn!=Turn::white){
            flipTurn();
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                board1[i][j] = 0;
            }
        }
        for(auto &from : white_pieces) {
            board1[from.first.x][from.first.y]=1;
            for(auto &to : possibleMoves(from.first)) {
                   board1[to.x][to.y] = 1;
            }
        }
        for(int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; ++j) {
                //cout<<board1[i][j]<<" ";
                if(board1[i][j]){
                    white_occupied++;
                }
            }
            //cout<<"\n";
        }
        return white_occupied;
    }

/*
    int bishop_score(){
        int cnt=0;
        for(auto &p : white_pieces){
            if(p.second==Piece::bishop || p.second==Piece::queen){
                cnt++;
            }
        }
        return cnt;
    }
    
    int opponent_bishop_score(){
        int cnt=0;
        for(auto &p : black_pieces){
            if(p.second==Piece::bishop || p.second==Piece::queen){
                cnt++;
            }
        }
        return cnt;
    }

    int king_safety(){
        int cnt=0;
        if(turn==Turn::black)
            flipTurn();
        for(auto &p : white_pieces){
            if(p.second==Piece::king){
                for(auto &to : possibleMoves(p.first)) {
                   cnt++;
                }
            }
        }
        return cnt;
    }

    int opponent_king_safety(){
        int cnt=0;
        if(turn==Turn::white)
            flipTurn();
        for(auto &p : black_pieces){
            if(p.second==Piece::king){
                for(auto &to : possibleMoves(p.first)) {
                   cnt++;
                }
            }
        }
        return cnt;
    }
*/
    int score(){
        int sumWhite = 0;
        for(auto & p : white_pieces){
            sumWhite += pieceValues[p.second];
        }
        int sumBlack = 0;
        for(auto & p : black_pieces)
            sumBlack += pieceValues[p.second];
        //cout<<"sw"<<sumWhite<<" sb"<<sumBlack<<endl;
        return sumWhite-sumBlack;
    }
    
    int scorewp(){
        int sumWhite = 0;
        for(auto & p : white_pieces){
            if(pieceValues[p.second]!=1)
                sumWhite += pieceValues[p.second];
        }
        int sumBlack = 0;
        for(auto & p : black_pieces)
            if(pieceValues[p.second]!=1)
                sumBlack += pieceValues[p.second];
        //cout<<"sw"<<sumWhite<<" sb"<<sumBlack<<endl;
        return sumWhite-sumBlack;
    }

    double get_for_that(double alp,vector<int> a, int sec, vector<int> u){
        //vector a(a,sec),b(u,1);
        if(engine==1){
            double pr;
            for(int i=0;i<a.size();i++){
                pr+=a[i]*u[i];
            }
            return alp*(sec)*(double)pr;
        }else{
            double pr;
            for(int i=0;i<a.size();i++){
                pr+=(a[i]-u[i])*(a[i]-u[i]);
            }
            int gamma=1;
            double finally=exp(-0.5*gamma*pr);
            return alp*(sec)*(double)finally;
        }
    }

    double predict(vector<int> u){
        double zz=0;
        //u.print();
        if(engine==1){
            ifstream in;
            in.open("svmsv.csv");
            string get_row;
            for(int i=0;i<158;i++){
                //std::cout<<alphas[i]<<" ";
                get_row.clear();
                getline(in,get_row);
                stringstream s(get_row);
                double alp,sec;
                string temp;
                //in>>temp;
                getline(s,temp,',');
                //cout<<"temp->"<<temp<<endl;
                alp=stod(temp);
                getline(s,temp,',');
                vector<int> a;
                for(int i=0;i<7;i++){
                    getline(s,temp,',');
                    a.push_back(stoi(temp));
                }
                getline(s,temp,',');
                sec=stoi(temp);
                zz+=get_for_that(a[i],a,sec,u);
                //std::cout<<alphas[i]<<" "<<data[i].val.second<<" "<<kernel(data[i],u)<<"\n";
            }
            string temp;
            in>>temp;
            zz-=stod(temp);
        }else{
            ifstream in;
            in.open("rbfsvmsv.csv");
            string get_row;
            for(int i=0;i<248;i++){
                //std::cout<<alphas[i]<<" ";
                get_row.clear();
                getline(in,get_row);
                stringstream s(get_row);
                double alp,sec;
                string temp;
                //in>>temp;
                getline(s,temp,',');
                //cout<<"temp->"<<temp<<endl;
                alp=stod(temp);
                getline(s,temp,',');
                vector<int> a;
                for(int i=0;i<7;i++){
                    getline(s,temp,',');
                    a.push_back(stoi(temp));
                }
                getline(s,temp,',');
                sec=stoi(temp);
                zz+=get_for_that(a[i],a,sec,u);
                //std::cout<<alphas[i]<<" "<<data[i].val.second<<" "<<kernel(data[i],u)<<"\n";
            }
            string temp;
            in>>temp;
            zz-=stod(temp);
        }
        //std::cout<<std::endl<<b<<std::endl;
        //std::cout<<"\n"<<zz<<" "<<b<<"\n";
        //std::cout<<zz<<std::endl;
        return zz;
    }

    double final_score(){
        vector<int> s;
        s.push_back(score());
        s.push_back(scorewp());
        s.push_back(occupied());
        s.push_back(bishop_score());
        s.push_back(opponent_bishop_score());
        s.push_back(king_safety());
        s.push_back(opponent_king_safety());
        return (predict(s)/*+0.01*woccupied()*/);
    }
 
    struct Move{
        Pos from,to;
        double score;};

    Move minimax(int depth, bool minimize,long long alpha,long long beta){//TODO: alpha beta pruning
    	
        Move best_move;
        best_move.score = -1000000 + 2000000*minimize;
        if(0 == depth){
            best_move.score = final_score();
            return best_move;
        }
 
        for(auto & from : moverPieces()){
        	bool flag=0;
            for(auto & to : possibleMoves(from.first)){
                ChessBoard branch = *this;
                branch.makeMove(from.first,to);
                if(alpha >= beta){
                	flag = 1;
                	break;
                }
                Move option = branch.minimax(depth-1, !minimize ,alpha ,beta);
                if((option.score > best_move.score && !minimize) || (option.score < best_move.score && minimize)){
                    best_move.score = option.score;
                    best_move.from = from.first;
                    best_move.to = to;
                }else if(option.score == best_move.score){
                    srand(time(NULL));
                    int x=rand()%2;
                    if(x==0){
                        best_move.score = option.score;
                        best_move.from = from.first;
                        best_move.to = to;
                    }
                }
                if(!minimize){
                	alpha = MAX(alpha,best_move.score);
                }else{
                	beta = MIN(beta,best_move.score);
                }               
            }
            if(flag) break;
        }
        //cout<<depth<<" "<<minimize<<"\n";
        return best_move;
    }
 
    void AIMove(){
        bool minimize = turn == Turn::black ? true : false;
        /*double score = final_score();
        Move m;
        if(score==1){
            m = minimax(5,minimize,INT_MIN,INT_MAX);
        }else if(score==-1){
            m = minimax(6,minimize,INT_MIN,INT_MAX);
        }else{
            m = minimax(5,minimize,INT_MIN,INT_MAX);
        }*/
        Move m = minimax(5,minimize,INT_MIN,INT_MAX);
        
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
        //cerr << char(retj) << reti << char(rety) << retx;
    }
};
 
map<ChessBoard::Piece,int> ChessBoard::pieceValues {{ChessBoard::Piece::king, 100},
{ChessBoard::Piece::queen, 9}, {ChessBoard::Piece::black_pawn, 1}, {ChessBoard::Piece::white_pawn, 1},
{ChessBoard::Piece::bishop, 5},{ChessBoard::Piece::knight, 3},{ChessBoard::Piece::rook, 4},};

void gen_board(string line) {
    int len = line.length();
    int i = 1, j = 1;
    for(int k = 0; k < len; k++) {
        if(i==8 && j==9){
            break;
        } else if(line[k] == '/') {
            i++;
            j = 1;
        } else if(line[k] >= '0' && line[k] <= '9') {
            int n = line[k] - '0';
            while(n--) board[i][j++] = '-';
        } else board[i][j++] = line[k];
    }
    /*for(int i=1;i<9;i++){
        for(int j=1;j<9;j++){
            cout<<board[i][j]<<" ";
        }cout<<endl;
    }*/
}



int main(){
	ifstream fi;
	fi.open("curr_board.in");
	string line;
	fi>>line;
	fi.close();
	gen_board(line);
    /*int len = line.length();
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
    }*/
    ChessBoard game;
    game.reset();
    game.AIMove();
	return 0;
}

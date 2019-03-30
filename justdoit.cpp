#include<bits/stdc++.h>
using namespace std;

char board[8][8];

int dur(int m, int n){
	int cnt=0;
	while(m>0 && n<7){
		m--;n++;
		cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	cerr<<"donedur";
	return cnt;
}
int dul(int m, int n){
	int cnt=0;
	while(m>0 && n>0){
		m--;n--;
		cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	cerr<<"donedul";
	return cnt;
}
int dll(int m, int n){
	int cnt=0;
	while(m<7 && n>0){
		m++;n--;cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	cerr<<"donedll";
	return cnt;
}
int dlr(int m, int n){
	int cnt=0;
	while(m<7 && n<7){
		m++;n++;cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	cerr<<"donedlr";
	return cnt;
}
int hu(int m, int n){
	int cnt=0;

	while(m>0){
		m--;cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	return cnt;
}
int hr(int m, int n){
	int cnt=0;
	cout<<"entered";
	while(n<7){
		n++;cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	cerr<<"donehr";
	return cnt;
}
int hl(int m, int n){
	int cnt=0;
	while(n>0){
		n--;cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	cerr<<"donehl";
	return cnt;
}
int hd(int m, int n){
	int cnt=0;
	while(m<7){
		m++;cnt++;
		if(board[m][n]>='A' && board[m][n]<='Z')
			return cnt+1;
		if(board[m][n]!='-'){
			return cnt;
		}
	}
	cerr<<"donehd";
	return cnt;
}


int main(){
	ifstream fi;
	fi.open("curr_board.in");
	
	string line;
	fi>>line;
	fi.close();
	cin.ignore();
	int len = line.length();
	int i=0,j=0;
	for(int k=0;k<len;k++){
		if(i==7 && j==8){
			break;
		}else if(line[k]=='/'){
			i++;
			j=0;
		}else if(line[k]>='0' && line[k]<='9'){
			int n = line[k]-'0';
			while(n--) board[i][j++]='-';
		}else board[i][j++]=line[k];
	}
	/*
	for (int p = 0; p < 8; p++) {
		for (int q = 0; q < 8; ++q)
		{
			cout << board[p][q] << " ";
		} 
		cout << "\n";
	}*/
	//legal moves
	vector<pair<int, int> > a[8][8];
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++) {
			vector<pair<int, int> > temp1;
			vector<int> temp;
			for(int k=0;k<8;k++){
				temp1.push_back(make_pair(0,0));
			}
			a[i][j]=temp1;
			
			if(board[i][j]=='r' || board[i][j]-32=='r'){
				a[i][j].push_back(make_pair(0,0));
				a[i][j].push_back(make_pair(0,0));
				a[i][j].push_back(make_pair(0,0));
				a[i][j].push_back(make_pair(0,0));
				temp[4]=hu(i,j);
				a[i][j].push_back(make_pair(i-temp[4],j));
				temp[5]=hr(i,j);
				a[i][j].push_back(make_pair(i,j+temp[5]));
				temp[6]=hd(i,j);
				a[i][j].push_back(make_pair(i+temp[6],j));
				temp[7]=hl(i,j);
				a[i][j].push_back(make_pair(i,j-temp[7]));
			}else if(board[i][j]=='n' || board[i][j]-32=='n'){

			}/*else if(board[i][j]=='b' || board[i][j]-32=='b'){
				temp[0]=dur(i,j);
				a[i][j][0]=make_pair(i-temp[0],j+temp[0]);
				temp[1]=dul(i,j);
				a[i][j][1]=make_pair(i-temp[1],j-temp[1]);
				temp[2]=dll(i,j);
				a[i][j][2]=make_pair(i+temp[2],j-temp[2]);
				temp[3]=dlr(i,j);
				a[i][j][3]=make_pair(i+temp[3],j+temp[3]);
			}else if(board[i][j]=='q' || board[i][j]-32=='q'){
				temp[0]=dur(i,j);
				a[i][j][0]=make_pair(i-temp[0],j+temp[0]);
				temp[1]=dul(i,j);
				a[i][j][1]=make_pair(i-temp[1],j-temp[1]);
				temp[2]=dll(i,j);
				a[i][j][2]=make_pair(i+temp[2],j-temp[2]);
				temp[3]=dlr(i,j);
				a[i][j][3]=make_pair(i+temp[3],j+temp[3]);
				temp[4]=hu(i,j);
				a[i][j][4]=make_pair(i-temp[4],j);
				temp[5]=hr(i,j);
				a[i][j][5]=make_pair(i,j+temp[5]);
				temp[6]=hd(i,j);
				a[i][j][6]=make_pair(i+temp[6],j);
				temp[7]=hl(i,j);
				a[i][j][7]=make_pair(i,j-temp[7]);
			}else if(board[i][j]=='k' || board[i][j]-32=='k'){

			}else if(board[i][j]=='p' || board[i][j]-32=='p'){

			}*/
			//cerr << i << " " << j << "\n";
		}
	}
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			cout<<i<<" "<<j<<endl;
			for(int k=0;k<8;k++){
				cout<<a[i][j][k].first<<","<<a[i][j][k].second<<" ";
			}
			cout<<endl;
		}
	}
	return 0;
}

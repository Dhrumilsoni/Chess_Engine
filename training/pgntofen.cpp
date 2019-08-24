#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;
int main(){
	fstream in;
	in.open("allgamespgn.txt");
	string s;
	in>>s;
	vector<string> fens;
	while(1){
		if(s=="1/2-1/2"){
			ofstream ofs1;
			ofs1.open("moves.txt", std::ofstream::out | std::ofstream::trunc);
			ofs1.close();

			ofstream out;
			out.open("moves.txt");
			out<<s;
			out.close();

			ofstream ofs;
			ofs.open("status.txt", std::ofstream::out | std::ofstream::trunc);
			ofs.close();
			
			check1:
			ifstream status;
			status.open("status.txt");
			int x=0;
			status>>x;
			
			if(x==0){
				usleep(50);
				status.close();
				goto check1;
			}
		}else if(s=="1-0"){
			ofstream ofs1;
			ofs1.open("moves.txt", std::ofstream::out | std::ofstream::trunc);
			ofs1.close();

			ofstream out;
			out.open("moves.txt");
			out<<s;
			out.close();

			ofstream ofs;
			ofs.open("status.txt", std::ofstream::out | std::ofstream::trunc);
			ofs.close();
			
			check2:
			ifstream status;
			status.open("status.txt");
			int x=0;
			status>>x;
			
			if(x==0){
				usleep(50);
				status.close();
				goto check2;
			}
		}else if(s=="0-1"){
			ofstream ofs1;
			ofs1.open("moves.txt", std::ofstream::out | std::ofstream::trunc);
			ofs1.close();

			ofstream out;
			out.open("moves.txt");
			out<<s;
			out.close();

			ofstream ofs;
			ofs.open("status.txt", std::ofstream::out | std::ofstream::trunc);
			ofs.close();
			
			check3:
			ifstream status;
			status.open("status.txt");
			int x=0;
			status>>x;
			
			if(x==0){
				usleep(50);
				status.close();
				goto check3;
			}
		}else if(s[0]>='0' && s[0]<='9'){
			int fl=0;
			string st,en;
			for(int i=0;i<s.length();i++){
				if(fl){
					st+=s[i];
				}
				if(s[i]=='.'){
					fl=1;
				}
			}
			/*int flag=0;
			while(st[0]=='O'){
				in>>st;
				flag=1;
			}
			if(flag){
				s=st;
				continue;
			}*/
			in>>en;/*
			while(en[0]=='O'){
				in>>en;
				flag=1;
			}
			if(flag){
				s=st;
				continue;
			}*/
			//cout<<st<<" "<<en<<endl;
			if(en=="1/2-1/2" || en=="1-0" || en=="0-1"){
				s=en;
				continue;
			}
			ofstream ofs1;
			ofs1.open("moves.txt", std::ofstream::out | std::ofstream::trunc);
			ofs1.close();

			ofstream out;
			out.open("moves.txt");
			out<<st<<" "<<en;
			out.close();

			ofstream ofs;
			ofs.open("status.txt", std::ofstream::out | std::ofstream::trunc);
			ofs.close();
			
			check:
			ifstream status;
			status.open("status.txt");
			int x=0;
			status>>x;
			
			if(x==0){
				usleep(50);
				status.close();
				goto check;
			}

			//break;
		}
		in>>s;
		cout<<"s->"<<s<<endl;
	}
}
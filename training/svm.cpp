/*
Author: Dhrumil Soni
*/
#include<iostream>
#include<vector>
#include<utility>
#include<iomanip>
#include<fstream>
#include"SMO.h"
//using namespace std;

int main(){
	int size,dimension;std::cin>>size>>dimension;
	std::fstream in;
	in.open("datasvm.csv");
	std::vector<vector> data;
	std::string get_row;
	for(int i=0;i<size;i++){
		get_row.clear();
		std::getline(in,get_row);
		/*if(get_row=="")
			continue;*/
		std::stringstream s(get_row);
		std::string temp;
		std::vector<int> point(dimension);
		for(int i=0;i<dimension;i++){
			std::getline(s,temp,',');
			//std::cout<<temp<<std::endl;
			point[i]=stoi(temp);
		}
		std::string st;
		std::getline(s,st,',');
		if(st=="1"){
			vector v(point, 1.0);
			data.push_back(v);
		}else if(st=="-1"){
			vector v(point, -1.0);
			data.push_back(v);
		}
		//data[data.size()-1].print();
	}
	std::cout<<"Done with input"<<std::endl;
	/*
	for(int i=0;i<data.size();i++){
		for(int j=i+1;j<data.size();j++){
			int fl=0;
			for(int k=0;k<data[i].val.first.size();k++){
				if(data[i].val.first[k]!=data[j].val.first[k]){
					fl=1;
 				}
			}
			if(fl==0){
				std::cout<<"same reported";
			}
		}
	}*/
	/*
	for(int i=0;i<size;i++){
		std::vector<double> point(dimension);
		for(int i=0;i<dimension;i++){
			std::cin>>point[i];
		}
		double group;
		std::cin>>group;
		vector v(point,group);
		data.push_back(v);
	}*/
	/*for(int i=0;i<data.size();i++){
		data[i].print();
	}*/
	double c=1.0,tol=1e-3;
	int max_passes=10;
	compute obj(c,tol,max_passes);
	obj.pass_data(data,size);
	obj.get_result();
	//obj.print();
	/*int cor=0,mis=0;
	for(int i=0;i<40;i++){
		std::vector<double> pdata(dimension,0);
		get_row.clear();
		std::getline(in,get_row);
		std::stringstream s(get_row);
		std::string temp;
		for(int i=0;i<dimension;i++){
			std::getline(s,temp,',');
			pdata[i]=stod(temp);
		}
		std::string st;
		std::getline(s,st,',');
		double predicted_value=(obj.predict(pdata,dimension));
		//std::cout<<"predicted class "<<predicted_value<<"\n";
		if((predicted_value>0.0 && st=="Iris-setosa") || (predicted_value<0.0 && st=="Iris-versicolor")){
			cor++;
		}else{
			mis++;
		}
		/*
		std::cout<<"actual class ";
		if(st=="Iris-setosa"){
			std::cout<<"1.0\n";
		}else{
			std::cout<<"-1.0\n";
		}*/
		/*
	}
	std::cout<<"Accuracy "<<(double)cor/(cor+mis)<<"\n";*/
}
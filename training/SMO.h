/*
Author: Dhrumil Soni
*/
#include<iostream>
#include<vector>
#include<utility>
#include<iomanip>
#include<cmath>
#include<fstream>
#include"vector.h"
double pos(double x){
	if(x<0)
		return -x;
	return x;
}

class compute{
   public:
	std::vector<double> alphas;
	std::vector<vector> data;
	std::vector<int> err;
	int passes, max_passes, size;
	double b, tol, c;
	
	compute(double c, double tol, int max_passes){
		b=0;
		passes=0;
		this->c=c;
		this->tol=tol;
		this->max_passes=max_passes;
		//data.resize(size);
	}
	void pass_data(std::vector<vector> data, int size){
		alphas.resize(size);
		err.resize(size);
		this->data=data;
		this->size=size;
	}
	double get_function_value_at(vector a){
		double val=0;
		for(int i=0;i<size;i++){
			val+=alphas[i]*data[i].val.second*kernel(data[i], a);
		}
		val-=b;
		return val;
	}
	int trythis(int i,int j){
		int flag=0;
		for(int it=0;it<data[i].val.first.size();it++){
			if(data[i].val.first[it]!=data[j].val.first[it]){
				flag=1;
			}
		}
		if(!flag)
			return 0;
		//if(data[i].val.first==data[j].val.first && data[i].val.second==data[j].val.second)
		//	return 0;
		double s=data[i].val.second*data[j].val.second;
		double L, H;
		if(data[i].val.second!=data[j].val.second){
			L=std::max((double)0, alphas[j]-alphas[i]);
			H=std::min(c,c+alphas[j]-alphas[i]);
		}else{
			L=std::max((double)0,alphas[j]+alphas[i]-c);
			H=std::min(c,alphas[i]+alphas[j]);
		}
		if(L==H)
			return 0;
		double eta=kernel(data[i],data[i])+kernel(data[j],data[j])-2*kernel(data[i],data[j]);
		double a1,a2;
		if(eta>0){
			a2=alphas[j]+data[j].val.second*((get_function_value_at(data[i])-data[i].val.second)-(get_function_value_at(data[j])-data[j].val.second))/eta;
			if(a2<L)
				a2=L;
			else if(a2>H)
				a2=H;
		}else{
			double f1=data[i].val.second*(get_function_value_at(data[i])-data[i].val.second+b)-alphas[i]*kernel(data[i],data[i])-s*alphas[j]*kernel(data[i],data[j]);
			double f2=data[j].val.second*(get_function_value_at(data[j])-data[j].val.second+b)-s*alphas[i]*kernel(data[i],data[j])-alphas[j]*kernel(data[j],data[j]);
			double l=alphas[i]+s*(alphas[j]-L);
			double h=alphas[i]+s*(alphas[j]-H);
			double lobj=l*f1+l*f2+0.5*l*l*kernel(data[i],data[i])+0.5*L*L*kernel(data[j],data[j])+s*L*l*kernel(data[i],data[j]);
			double hobj=h*f1+H*f2+0.5*h*h*kernel(data[i],data[i])+0.5*H*H*kernel(data[j],data[j])+s*H*h*kernel(data[i],data[j]);
			if(lobj<hobj-tol)
				a2=L;
			else if(lobj>hobj+tol)
				a2=H;
			else
				a2=alphas[j];
		}
		if(abs(a2-alphas[j])<tol*(a2+alphas[j]+tol))
			return 0;
		a1=alphas[i]+s*(alphas[j]-a2);
		double b1=(get_function_value_at(data[i])-data[i].val.second)+data[i].val.second*(a1-alphas[i])*kernel(data[i],data[i])+data[j].val.second*(a2-alphas[j])*kernel(data[i],data[j])+b;
		double b2=(get_function_value_at(data[j])-data[j].val.second)+data[i].val.second*(a1-alphas[i])*kernel(data[i],data[j])+data[j].val.second*(a2-alphas[j])*kernel(data[j],data[j])+b;
		if(a1>0 && a1<c){
			b=b1;
		}else if(a2>0 && a2<c){
			b=b2;
		}else if(L!=H){
			b=(b1+b2)/2;
		}
		//update weight vector later on
		for(int i=0;i<size;i++){
			err[i]=get_function_value_at(data[i])-data[i].val.second;
		}
		alphas[i]=a1;
		alphas[j]=a2;
		//std::cout<<"Changed"<<std::endl;
		return 1;
	}
	int examine(vector x, double alpha, int ind1){
		double error=get_function_value_at(x)-x.val.second;
		double r2=error*x.val.second;
		if((r2<-tol && alpha<c) || (r2>tol && alpha>0)){
			int non_zeros=0,fl=0;
			for(int i=0;i<size;i++){
				if(alphas[i]!=0 && alphas[i]!=c){
					non_zeros++;
					if(non_zeros>1){
						fl=1;
						break;
					}
				}
			}
			int ind2=0;
			if(fl==1){
				if(error>0){
					double mn=INT_MAX;
					for(int i=0;i<size;i++){
						if(mn>err[i]){
							mn=err[i];
							ind2=i;
						}
					}
				}else if(error<0){
					double mx=INT_MIN;
					for(int i=0;i<size;i++){
						if(mx<err[i]){
							mx=err[i];
							ind2=i;
						}
					}
				}
				if(trythis(ind1,ind2))
					return 1;
			}
			int st=rand()%size;
			for(int i=st;i<size;i++){
				if(alphas[i]!=0 && alphas[i]!=c)
					if(trythis(ind1,i))
						return 1;
			}
			for(int i=0;i<st;i++){
				if(alphas[i]!=0 && alphas[i]!=c)
					if(trythis(ind1,i))
						return 1;
			}
			st=rand()%size;
			for(int i=st;i<size;i++){
				if(trythis(ind1,i))
					return 1;
			}
			for(int i=0;i<st;i++){
				if(trythis(ind1,i))
					return 1;
			}
		}
		return 0;
	}

	//Final SMO (Not simplified)
	void get_result(){
		//int tot=0;
		
		srand(time(NULL));

		for(int i=0;i<size;i++){
			err.push_back(get_function_value_at(data[i])-data[i].val.second);
		}
		for(int pass=0;pass<1;pass++){
			int numchanged=0, examineall=1;
			while(numchanged>0 || examineall){
				numchanged=0;
				if(examineall){
					for(int i=0;i<size;i++){
						numchanged+=examine(data[i],alphas[i],i);
					}
				}else{
					for(int i=0;i<size;i++){
						if(alphas[i]!=0 && alphas[i]!=c)
							numchanged+=examine(data[i],alphas[i],i);
					}
				}
				//std::cout<<"numchanged "<<numchanged<<std::endl;
				if(examineall==1)
					examineall=0;
				else if(numchanged==0)
					examineall=1;
			}
		}
		
		/*
		srand(time(NULL));
		while(passes<max_passes){
			int num_changed_alphas=0;
			for(int i=0;i<size;i++){
				//tot+=1;
				double Ei=get_function_value_at(data[i])-data[i].val.second;
				if((data[i].val.second*Ei<-tol && alphas[i]<c) || (data[i].val.second*Ei>tol && alphas[i]>0)){
					int j=rand()%size;
					while(j==i)
						j=rand()%size;
					double Ej=get_function_value_at(data[j])-data[j].val.second;
 					double L, H;
					if(data[i].val.second!=data[j].val.second){
						L=std::max((double)0, alphas[j]-alphas[i]);
						H=std::min(c,c+alphas[j]-alphas[i]);
					}else{
						L=std::max((double)0,alphas[j]+alphas[i]-c);
						H=std::min(c,alphas[i]+alphas[j]);
					}
					if(L==H){
						continue;
					}
					double eta=2*kernel(data[i],data[j])-kernel(data[i],data[i])-kernel(data[j],data[j]);
					if(eta>=0){
						continue;
					}
					double alphai=alphas[i],alphaj=alphas[j];
					alphas[j]=(alphas[j]-(data[j].val.second*(Ei-Ej))/eta);
					if(alphas[j]>H){
						alphas[j]=H;
					}
					else if(alphas[j]<L){
						alphas[j]=L;
					}
					if(pos(alphas[j]-alphaj)<1e-5)
						continue;
					alphas[i]=alphas[i]+data[i].val.second*data[j].val.second*(alphaj-alphas[j]);
					double b1=b-Ei-data[i].val.second*(alphas[i]-alphai)*kernel(data[i],data[i])-data[j].val.second*(alphas[j]-alphaj)*kernel(data[i],data[j]);
					double b2=b-Ej-data[i].val.second*(alphas[i]-alphai)*kernel(data[i],data[j])-data[j].val.second*(alphas[j]-alphaj)*kernel(data[j],data[j]);
					if(alphas[i]>0 && alphas[i]<c){
						b=b1;
					}else if(alphas[j]>0 && alphas[j]<c){
						b=b2;
					}else{
						b=(b1+b2)/2;
					}
					num_changed_alphas+=1;
				}
			}
			if(num_changed_alphas==0)
				passes+=1;
			else
				passes=0;
		}

		//std::cout<<"tot "<<tot<<"\n";
		/*for(int i=0;i<size;i++){
			if(alphas[i]!=0){
				data[i].print();
				std::cout<<alphas[i]<<std::endl;
			}
		}*/
		std::ofstream out;
		out.open("rbfsvmsv.csv");
		for(int i=0;i<size;i++){
			//std::cout<<alphas[i]<<std::endl;
			if(alphas[i]!=0){
				
				out<<alphas[i]<<",";
				for(int j=0;j<data[i].val.first.size();j++){
					out<<data[i].val.first[j]<<",";
				}
				out<<data[i].val.second<<std::endl;
			}
		}
		out<<b<<std::endl;
		out.close();
	}

	double predict(vector u){
		double zz=0;
		//u.print();
		for(int i=0;i<size;i++){
			//std::cout<<alphas[i]<<" ";
			zz+=alphas[i]*data[i].val.second*kernel(data[i],u);
			//std::cout<<alphas[i]<<" "<<data[i].val.second<<" "<<kernel(data[i],u)<<"\n";
		}
		//std::cout<<std::endl<<b<<std::endl;
		//std::cout<<"\n"<<zz<<" "<<b<<"\n";
		//std::cout<<zz<<std::endl;
		zz-=b;
		return zz;
	}

	void print(){
		for(int i=0;i<alphas.size();i++){
			std::cout<<alphas[i]<<" ";
		}std::cout<<"\n";
		std::cout<<b<<"\n";
	}
};
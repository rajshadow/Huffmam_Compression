#include <algorithm>
#include <functional>
#include <string>
#include <map>
#include <queue>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#define pb push_back
#define ll long long
using namespace std;
ifstream fin;
ofstream fout;

void clear(priority_queue<int> &q){
	priority_queue<int> empty;
	swap(q, empty);
}

struct node{
	static int height;
	char c;
	ll wt;
	node *l, *r, *p;
}*root;

int node::height = 0;
vector<bool> bin;
ll size=0,size_before=0;
int pow2[16] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
template<typename T> void print(vector<T> &buff)
{
	for (auto i : buff)
		cout << i;
	cout << endl;
}

map<unsigned char, vector<bool>> binVal;
map<pair<int,int>, unsigned char> byteVal;
char byteValue[1000000][30] = { NULL };
void assign(node *x){
	if (x->c != NULL)
	{
		binVal[x->c] = bin;
		//byteVal[bin] = x->c;
		size += bin.size()*x->wt;
		size_before += 8 * x->wt;
		return;
	}
	bin.push_back(0);
	assign(x->l);
	bin.pop_back();
	bin.push_back(1);
	assign(x->r);
	bin.pop_back();
}

int intVal(vector<bool> v)
{
	int i, len = v.size(), ans = 0;
	for (i = 0; i < len;++i)
	{
		if (v[i])
			ans += pow2[len - i - 1];
	}
	return ans;
}

void consReverseMap()
{
	vector<bool> temp;
	unsigned int h = 0;
	for (auto i : binVal)
	{
		//cout << i.first << " " << intVal(i.second)<<endl;
		h = h > i.second.size() ? h : i.second.size();
		temp = i.second;
		byteVal[make_pair(intVal(temp),i.second.size())] = i.first;
		byteValue[intVal(temp)][i.second.size()] = i.first;
	}
	root->height = h;
}
struct custComp {
	bool operator() (node *a,node *b){
		return a->wt > b->wt; //calls your operator
	}
};
typedef priority_queue<node *, vector<node *>, custComp> PQ;

node * create_node(node *a, node *b){
	node *temp = new node;
	temp->wt = a->wt + b->wt;
	temp->l = a;
	temp->r = b;
	temp->c = NULL;
	a->p = b->p = temp;
	return temp;
}

vector<bool> byteToBits(unsigned char byte)
{
	unsigned char temp;
	vector<bool> bits;
	int n = 8;
	while (n--)
	{
		temp = byte & (unsigned char)pow2[n];
		bits.pb(temp == 0 ? false : true);
	}
	return bits;
}

unsigned char bitsToByte(vector<bool> bits){
	int p = 7;
	unsigned char byte=0;
	for (auto b : bits)
	{
		byte += b == true ? (unsigned char)pow(2, p) : 0;
		--p;
	}
	return byte;
}

void getFrequency(string fname, ll *cnt){
	fin.open(fname);
	string str;
	while (getline(fin, str)){
		for (auto c : str)
			cnt[(int)c]++;
		if (!fin.eof())
			cnt[10]++;
	}
	fin.clear();
	fin.seekg(0);
}

void makePQ(ll *cnt, PQ &pq)
{
	for (int i = 0; i < 256;++i){
		if (cnt[i] == 0)
			continue;
		node *temp = new node;
		temp->c = (unsigned char)i;
		temp->wt = cnt[i];
		temp->l = temp->r = temp->p = NULL;
		pq.push(temp);
	}
}

void makeTree(PQ &pq)
{
	while (pq.size()>1){
		node *a, *b, *temp;
		temp = new node;
		a = pq.top();
		pq.pop();
		b = pq.top();
		pq.pop();
		pq.push(create_node(a, b));
	}
	root = pq.top();
}

struct cont
{
	unsigned char buff;
	int filled;
};

string original = "";
char buffer[1024];
int pos=0;
cont packAndWrite(cont byte, vector<bool> bits)
{
	int len = bits.size(),i;
	for (i = 0; i < len; ++i)
	{
		if (byte.filled == 8)
		{
			//fout.write((char *)&byte.buff,sizeof(unsigned char));
			buffer[pos++] = byte.buff;
			//cout << byte.buff;
			original += byte.buff;
			//print(byteToBits(byte.buff));
			byte.filled = 0;
			byte.buff = 0;
		}
		if (pos == 1024)
		{
			fout.write((char *)&buffer, sizeof(buffer));
			pos = 0;
		}
		if (bits[i])
			byte.buff |= (unsigned char)pow2[7 - byte.filled];
		byte.filled++;
	}
	return byte;
}

void compress(){
	string str;
	cont byte;
	byte.buff = 0;
	byte.filled = 0;
	fout.open("R:\\compressed.bin",ios::binary);
	//cout << "WHILE WRITING:\n";
	bool flag = false;
	while (getline(fin, str)){
		for (auto c : str)
		{
			byte = packAndWrite(byte, binVal[c]);
		}
		if (!fin.eof())
			byte = packAndWrite(byte, binVal[10]);		
	}
	//cout << buffer << " " << pos << " "<<byte.filled<< endl;
	if (byte.filled != 0)
		buffer[pos++] = byte.buff;
	/*for (int i = 0; i < pos; ++i)
	{
		cout << buffer[i] << ": ";
		print(byteToBits(buffer[i]));
	}
	_getch();*/
	fout.write((char *)buffer, sizeof(unsigned char)*pos);
	/*if (byte.filled != 0)
	{
		fout.write((char *)&byte.buff, sizeof(unsigned char));
		original += byte.buff;
	}*/
	fout.close();
	fin.close();
}

void decode()
{
	fin.open("R:\\compressed.bin",ios::binary);
	fout.open("R:\\uncompressed.txt");
	unsigned char c;
	int temp=0, cnt=0;
	//cout << "\nWHILE READING:\n";
	while (fin.read((char *)&c, sizeof(unsigned char)))
	{
		//cout << c<< ": ";
		//print(byteToBits(c));
		for (auto i : byteToBits(c))
		{
			temp <<= 1; cnt++;
			if (i)
				temp += 1;
			/*pair<int, int> p = make_pair(temp,cnt);
			if (byteVal.find(p) != byteVal.end())
			{
				fout << byteVal[p];
				//cout << byteVal[p];
				temp = cnt = 0;
							
			}*/
			if (byteValue[temp][cnt] != NULL)
			{
				//cout << temp << " " << cnt << " " << byteValue[temp][cnt] << endl;
				fout << byteValue[temp][cnt];
				temp = cnt = 0;
			}
		}		
	}
	fin.close();
	fout.close();
}

void decode(string str)
{
	cout << "\nWHILE READING FROM STRING:\n";
	int temp = 0, cnt = 0;
	for (auto c : str)
	{
		cout << c << ": ";
		print(byteToBits(c));
		for (auto i : byteToBits(c))
		{
			temp <<= 1;
			cnt++;
			if (i)
				temp += 1;
			pair<int, int> p = make_pair(temp, cnt);

			if (byteVal.find(p) != byteVal.end())
			{
				fout << byteVal[p];
				cout << byteVal[p];
				temp = cnt = 0;
			}
		
		}

	}

}
int main()
{
	ll cnt[256];
	memset(cnt, 0, sizeof(cnt));
	string fname = "R:\\sample.txt";
	PQ pq;
	getFrequency(fname,cnt);
	makePQ(cnt, pq);
	makeTree(pq);
	assign(root);
	consReverseMap();
	cout << "Frequencies:\n";
	for (auto i : byteVal)
	{
		cout << i.first.first << " " << i.first.second << " "<<i.second<< ": ";
		print(binVal[i.second]);
	}
	_getch();
	//system("cls");
	compress();
	cout << "Compression DONE\n";
	_getch();
	decode();
	//decode(original);
	/*fin.open("R:\\compressed.txt");
	string str;
	getline(fin, str);
	for (auto c : str)
	{
		cout << c << ": ";
		print(byteToBits(c));
	}*/
	//cout << "Size of file: " << size_before << " bits\n";
	//cout << "Size of compressed file: " << size << " bits";
	//fin.close();
	//print(byteToBits('a'));
	cout << "uncompressed.";
	_getch();
}

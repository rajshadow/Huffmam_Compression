#include <algorithm>
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
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

struct node{
	static int height;
	static ll FILE_CHAR_COUNT;
	char c;
	ll wt;
	node *l, *r, *p;
}*root;

int node::height = 0;
ll node::FILE_CHAR_COUNT = 0;

vector<bool> bin;
ll size=0,size_before=0;
int pow2[16] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
template<typename T> void print(vector<T> &buff)
{
	for (auto i : buff)
		cout << i;
	cout << endl;
}
unordered_map<unsigned char, vector<bool>> binVal;

void assign(node *x){
	if (x->c != NULL)
	{
		binVal[x->c] = bin;
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


struct custComp {
	bool operator() (node *a,node *b){
		return a->wt > b->wt; //calls your operator
	}
};

typedef priority_queue<node *, vector<node *>, custComp> PQ;
void clear(priority_queue<int> &q){
	priority_queue<int> empty;
	swap(q, empty);
}

node * create_node(node *a, node *b)
{
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
	while (n--){
		temp = byte & (unsigned char)pow2[n];
		bits.pb(temp == 0 ? false : true);
	}
	return bits;
}

unsigned char bitsToByte(vector<bool> bits)
{
	int p = 7;
	unsigned char byte=0;
	for (auto b : bits){
		byte += b == true ? (unsigned char)(1<<p) : 0;
		--p;
	}
	return byte;
}

void getFrequency(string fname, ll *cnt)
{
	fin.open(fname);
	string str;
	while (getline(fin, str)){
		for (auto c : str){
			cnt[(int)c]++;
			++node::FILE_CHAR_COUNT;
		}
		if (!fin.eof()){
			++node::FILE_CHAR_COUNT;
			cnt[10]++;
		}
	}
	fin.clear();
	fin.seekg(0);
}

void makePQ(ll *cnt, PQ &pq)
{
	node *temp;
	for (int i = 0; i < 256;++i){
		if (cnt[i] == 0)
			continue;
		temp = new node;
		temp->c = (unsigned char)i;
		temp->wt = cnt[i];
		temp->l = temp->r = temp->p = NULL;
		pq.push(temp);
	}
	//to create dummy node in the case of single character files
	if (pq.size() == 1){
		node *root = pq.top();
		temp = new node;
		temp->c = (root->c + 1) % 256;
		temp->wt = root->wt + 1;
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

cont packAndWrite(cont byte, vector<bool> bits, int& pos)
{
	int len = bits.size(),i;
	for (i = 0; i < len; ++i){
		if (byte.filled == 8){
			//fout.write((char *)&byte.buff,sizeof(unsigned char));
			buffer[pos++] = byte.buff;
			original += byte.buff;
			byte.filled = 0;
			byte.buff = 0;
		}
		if (pos == 1024){
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
	int pos = 0;
	fout.open("R:\\compressed.bin",ios::binary);
	bool flag = false;
	while (getline(fin, str)){
		for (auto c : str)
			byte = packAndWrite(byte, binVal[c],pos);
		if (!fin.eof())
			byte = packAndWrite(byte, binVal[10],pos);		
	}
	if (byte.filled != 0)
		buffer[pos++] = byte.buff;
	fout.write((char *)buffer, sizeof(unsigned char)*pos);
	fout.close();
	fin.close();
}

void decode()
{
	fin.open("R:\\compressed.bin",ios::binary);
	fout.open("R:\\uncompressed.txt");
	unsigned char c;
	node *treeNode = root;
	int cnt=0;
	while (fin.read((char *)&c, sizeof(unsigned char))){
		for (auto i : byteToBits(c)){
			if (i)
				treeNode = treeNode->r;
			else
				treeNode = treeNode->l;
			if (treeNode->l == NULL){
				if (cnt < node::FILE_CHAR_COUNT)
					fout << treeNode->c;
				++cnt;
				treeNode = root;
			}
		}
	}
	cout << cnt << endl;
	fin.close();
	fout.close();
}

void decode(string str)
{
	cout << "\nWHILE READING FROM STRING:\n";
	node *treeNode = root;
	int cnt = 0;
	for (auto c : str){
		for (auto i : byteToBits(c)){
			if (i)
				treeNode = treeNode->r;
			else
				treeNode = treeNode->l;
			if (treeNode->l == NULL){
				if (cnt < node::FILE_CHAR_COUNT)
					fout << treeNode->c;
				++cnt;
				treeNode = root;
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
	cout << node::FILE_CHAR_COUNT << endl;
	cout << "Frequencies:\n";
	for (auto i : binVal){
		cout << i.first << " ";
		print(i.second);
	}
	cout << "PRESS ANY KEY TO START COMPRESSION.\n";
	_getch();
	compress();
	cout << "Compression DONE\n";
	cout << "PRESS ANY KEY TO START DECODING.";
	_getch();
	decode();
	cout << "uncompressed.";
	_getch();
}

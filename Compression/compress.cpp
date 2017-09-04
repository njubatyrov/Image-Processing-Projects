/**
    Create by Nursultan on 4/11/2016

*/

#include <fstream>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <vector>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iterator>


using namespace std;
using namespace cv;

#define fi first
#define se second
#define mp make_pair
#define sz(x) ((int)(x).size())
#define re return
#define pb(x) push_back(x)
#define pf(x) push_front(x)
#define abs(x) ((x) < 0 ? -(x) : (x))
#define INF 2000000000
#define sqr(x) ((x) * (x))
#define all(x) x.begin(), x.end()
#define fname "a"
#define MOD 1000000007

Mat img, bits[8];
long long start, finish;
int m, n;

struct node {
    double freq;
    int ch;
    node *l, *r;
    
    node(double fr, int c) {
        ch = c;
        freq = fr;
        l = r = NULL;
    }
};
typedef node * pnode;

struct comp {
    bool operator() (const pnode l, const pnode r) const{
        return l->freq > r->freq;
    }
};

priority_queue< pnode, vector<pnode>, comp> t;
vector < bool > codes[511111], current;
pnode root = NULL;
int k = -1;

void buildTree(vector < double > &p, vector < int> & symbols) {
    
    for(int i = 0; i < sz(symbols); i++) {
        pnode link = new node(p[i], symbols[i]);
        t.push(link);
    }
    
    while(sz(t) > 1) {
        pnode fi = t.top();
        t.pop();
        pnode se = t.top();
        t.pop();
        
        pnode par = new node(fi->freq + se->freq, -1);
        par->l = fi;
        par->r = se;
        t.push(par);
    }
    root = t.top();
    t.pop();
}

void printTree(pnode t) {
    if(t == NULL) return;
    printTree(t->l);
    if(t->ch) cout << t->ch << " " << t->freq << endl;
    printTree(t->r);
}

void free(pnode t) {
    if(t == NULL) return;
    free(t->l);
    codes[t->ch].clear();
    free(t->r);
}

void buildDictionary(pnode t, vector < bool > & current, map < int, pair < int, int > > & dict) {
    if(t == NULL) return;
    
    if(t->ch != -1) {
        int res = 0;
        for(int j = 0; j < sz(current); j++)
            res |= (1 << j) * current[j];
        dict[t->ch] = mp(sz(current), res);
        //for(int j = 0; j < k; j++)
          //  current.pop_back();
        return;
    }
    
    current.pb(false);
    buildDictionary(t->l, current, dict);
    current.pop_back();
    current.pb(true);
    buildDictionary(t->r, current, dict);
    current.pop_back();
}

vector < int > encodeRunLength(Mat &img) {
    vector < int > res;
    uchar *p1;
    uchar prev = 0, cur;
    int cnt = 0;
    for(int i = 0; i < m; i++) {
        p1 = img.ptr<uchar>(i);
        for(int j = 0; j < n; j++) {
            cur = *p1++;
            
            if(cur != prev) {
                res.pb(cnt);
                cnt = 1;
            }
            else
                cnt++;
            prev = cur;
        }
    }
    res.pb(cnt);
    return res;
}

vector < int > encodeRunLength2(vector < bool > & source) {
    vector < int > res;
    bool prev = 0, cur;
    int cnt = 0;
    for(int i = 0; i < sz(source); i++) {
        cur = source[i];
        if(cur != prev) {
            res.pb(cnt);
            cnt = 1;
        }
        else
            cnt++;
        prev = cur;
    }
    res.pb(cnt);
    return res;
}

vector < bool > encodeHuffman(vector < int > & symbols, map < int, pair < int, int > > & dict) {
    vector < bool > res;
    for(int i = 0; i < sz(symbols); i++) {
        int tmp = symbols[i];
        pair < int, int > s = dict[tmp];
        
        for(int j = 0; j < s.fi; j++)
            if((1 << j) & s.se) res.pb(true); else res.pb(false);
    }
    return res;
}

vector < int > decodeHuffman(vector < bool > & code) {
    pnode t = root;
    vector < int > res;
    bool cur;
    for(int i = 0; i < sz(code); i++) {
        cur = code[i];
        if(cur == false)
            t = t->l;
        else
            t = t->r;
        
        if(t->ch != -1) {
            res.pb(t->ch);
            t = root;
        }
    }
    return res;
}

Mat decodeRunLength(vector < int > & symbols) {
    
    Mat res = img.clone();
    
    uchar *p1 = res.ptr<uchar>(0);
    uchar cur = 0;
    for(int i = 0; i < sz(symbols); i++) {
        
        for(int j = 0; j < symbols[i]; j++)
            *p1++ = (!cur ? 0 : 255);
        cur = (cur + 1) % 2;
    }
    return res;
    
}
int getVal(int pos, vector < bool > & codes) {
    int res = 0;
    for(int i = 0; i < 8; i++) {
        res += (1 << i) * codes[pos + i];
    }
    return res;
}

int readInt(int &i, vector < char > & buff ) {
    int res = 0;
    while(buff[i] != '\n' && buff[i] != ' ') {
        res = res * 10 + (buff[i++] - '0');
    }
    i++;
    return res;
}
int main() {
    
    printf("Please enter the file name of an image...\n");
    string s;
    cin >> s;

    freopen("comprssedNJ.pku", "w", stdout);

    srand(time(NULL));
    start = clock();
    img = imread(s, CV_LOAD_IMAGE_UNCHANGED);
    
    for(int i = 0; i < 8; i++)
        bits[i] = img.clone();
    
    m = img.rows;
    n = img.cols;
    printf("%d %d\n", m, n);
    uchar *p1, *p2;
    uchar tmp;
    long long total = 0;
    for(int mask = 0; mask < 8; mask++) {
        
        for(int i = 0; i < m; i++){
            p1 = img.ptr<uchar>(i);
            p2 = bits[mask].ptr<uchar>(i);
            for(int j = 0; j < n; j++) {
                tmp = *p1++;
                *p2++ = (1 << mask) & tmp;
            }
        }
    }
    int o = 0;
    for(int mask = 0; mask < 8; mask++) {
        
        set < int > q;
        map < int, int > pp;
        vector < int > symbols;
        vector < int > rlcode = encodeRunLength(bits[mask]);
    
        map < int, pair < int, int > > dict;
        vector < double > p;
        
        for(int i = 0; i < sz(rlcode); i++) {
            int tmp = rlcode[i];
            if(pp.find(tmp) == pp.end()) {
                pp[tmp] = 1;
            }
            else
                pp[tmp]++;
            q.insert(tmp);
        }
    
        for(set < int > :: iterator it = q.begin(); it != q.end(); it++) {
            symbols.pb(*it);
        }

        for(int i = 0; i < sz(symbols); i++)
            p.pb((double)pp[symbols[i]] / sz(rlcode));
        
        current.clear();
        buildTree(p, symbols);
        buildDictionary(root, current, dict);
        
        printf("%d\n", sz(dict));
        for(map < int, pair < int, int > > :: iterator it = dict.begin(); it != dict.end(); it++) {

            pair < int , int > s = it->second;
            o = max(o, s.fi);
            printf("%d %d %d\n", it->first, s.fi, s.se);
            total += 33 + 33 + 33;
        }
        vector < bool > huffenc = encodeHuffman(rlcode, dict);
        int k = (sz(huffenc) % 8 == 0 ? 0 : 8 - sz(huffenc) % 8);
        for(int j = 0; j < k; j++)
            huffenc.pb(false);
        printf("%d\n", sz(huffenc) / 8);
        for(int i = 0; i < sz(huffenc); i += 8) {
            printf("%c", (char)getVal(i, huffenc));
        }
        total += 33 + 33 + 33 + sz(huffenc) + 33;
    }
    fclose(stdout);
    finish = clock();

    cerr << "Compressed time is: "<< double(finish - start) / CLOCKS_PER_SEC << " seconds.\n";
    cerr << "Redundant data is: " << (1 - ((double)total / n / m / 8)) * 100 << "%\n" << endl;
    
   
    return 0;
}
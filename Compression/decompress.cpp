//
//  main.cpp
//  Test
//
//  Created by Nursultan on 3/11/16.
//  Copyright © 2016 Nursultan. All rights reserved.
//

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
uchar *p1;
long long start, finish;
int m, n;
int readInt(int &i, vector < char > & buff ) {
    int res = 0;
    while(buff[i] != '\n' && buff[i] != ' ') {
        res = res * 10 + (buff[i++] - '0');
    }
    i++;
    return res;
}
int main() {
    
    int dictLength, val, codeLength, codeVal, valLength;
    start = clock();
    ifstream input("comprssedNJ.pku", std::ios::binary );
    
    vector<char> buffer((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
    input.close();

    int pos = 0;
    int k;
    int m = readInt(pos, buffer);
    int n = readInt(pos, buffer);

    Mat img2 = Mat(m, n, CV_8UC1);
        p1 = img2.ptr<uchar>(0);
    for(int i = 0; i < n * m; i++)
        *p1++ = 0;
    p1 = img2.ptr<uchar>(0);
    
    for(int mask = 0; mask < 8; mask++) {
        
        dictLength = readInt(pos, buffer);
        map <pair< int, int> , int > dict;
        
        //cerr << dictLength << endl;
        for(int i = 0; i < dictLength; i++) {
            val = readInt(pos, buffer);
            codeLength = readInt(pos, buffer);
            codeVal = readInt(pos, buffer);
            dict[mp(codeLength, codeVal)] = val;
        }
        
        vector < bool > huffcode;
        valLength = readInt(pos, buffer);
        for(int j = 0; j < valLength; j++) {
            k = buffer[pos++];
            k = (k + 256) % 256;
            for(int h = 0; h < 8; h++)
                if((1 << h) & k) huffcode.pb(true); else huffcode.pb(false);
        }
        int cur = 0, curl = 0;
        vector < int > rlvals;
        
        for(int j = 0; j < valLength * 8; j++) {
            cur |= (1 << curl) * huffcode[j];
            curl++;
            if(dict.find(mp(curl, cur)) != dict.end()) {
                rlvals.pb(dict[mp(curl, cur)]);
                cur = 0;
                curl = 0;
            }
        }
        int curc = 0;
        p1 = img2.ptr<uchar>(0);
        for(int i = 0; i < sz(rlvals); i++) {
            
            for(int j = 0; j < rlvals[i]; j++) {
                *p1++ |= ((1 << mask) * curc);
            }
            curc = (curc + 1) % 2;
        }
    }
    finish = clock();
    cerr <<"Decompression time is: "  << double(finish - start) / CLOCKS_PER_SEC << " seconds.\n" << endl;
    
    namedWindow("", 1);
    imshow("", img2);
    waitKey(0);
    
    return 0;
}
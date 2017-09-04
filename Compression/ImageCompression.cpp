#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <dirent.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <conio.h>  
#include <stdio.h>
#include <vector>
#include <ctime>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <map>
#define INF 2000000000

using namespace cv;
using namespace std;
string INPUT_IMAGE_PATH = "D:\\study\\images\\";
int windowsCounter = 0;
void showImage(Mat image) {
	string name = "Image" + to_string(++windowsCounter);
	namedWindow(name, WINDOW_AUTOSIZE);
	imshow(name, image);
	waitKey(0);
}
void encode(vector<pair<int, int> > &vp, unordered_map<int, vector<bool> > &res, int cntr) {
	if (vp.size() == 2){
		int idx1 = vp[0].second;
		int idx2 = vp[1].second;
		vector<bool> v1, v2;
		v1.push_back(false);
		v2.push_back(true);
		res[idx1] = v1;
		res[idx2] = v2;
	}
	else {
		sort(vp.begin(), vp.end());
		reverse(vp.begin(), vp.end());
		int last = (int)vp.size() - 1;
		int idx1 = vp[last - 1].second;
		int idx2 = vp[last].second;
		int sum = vp[last].first + vp[last - 1].first;
		vp.pop_back();
		vp.pop_back();
		vp.push_back(make_pair(sum, cntr));
		encode(vp, res, cntr + 1);
		vector<bool> v1(res[cntr]), v2(res[cntr]);
		v1.push_back(false);
		v2.push_back(true);
		res[idx1] = v1;
		res[idx2] = v2;
	}
}
unordered_map<int, vector<bool> > encode(vector<pair<int, int> > &vp) {
	unordered_map<int, vector<bool> > res;
	encode(vp, res, 256);
	return res;
}
void compressHuffman(String imagePath) {
	Mat image = imread(imagePath, 0);
	int width = image.cols;
	int height = image.rows;
	cout << width << " " << height << endl;
	unordered_map<int, int> cnt;
	vector<uchar> mat;
	mat.assign(image.datastart, image.dataend);
	cout << "started reading" << endl;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			cnt[mat[i*width + j]]++;
		}
	}
	cout << "finished reading" << endl;
	
	vector<pair<int, int> > vp;
	for (int i = 0; i < 256; i++) {
		if (cnt[i] > 0)
			vp.push_back(make_pair(cnt[i], i));
	}
	unordered_map<int, vector<bool> > code = encode(vp);
	int res = 0;
	for (int i = 0; i < 256; i++) {
		res += cnt[i] * code[i].size();
	}
	cout << res << " " << width*height*8 << endl;
}
void push_back(vector<bool> &v, int x) {
	for (int i = 0; i < 8; i++) {
		v.push_back((x >> (7 - i))&1);
	}
}
void compressLZW(string imagePath, string outputPath) {
	Mat mat = imread(imagePath, 0);
	resize(mat, mat, Size(300, 400));
	int width = mat.cols;
	int height = mat.rows;
	vector<uchar> image;
	image.assign(mat.datastart, mat.dataend);
	cout << image.size() << endl;
	unordered_map<vector<bool>, int> dict;
	vector<short> encoding;
	int cntr = 0;
	for (int i = 0; i < 256; i++) {
		vector<bool> temp;
		push_back(temp, i);
		dict[temp] = cntr++;
	}
	int i = 0;
	int iters = 0;
	while (i < image.size()) {
		if (i%10000 == 0) cout << i << endl;
		vector<bool> temp;
		push_back(temp, image[i]);
		i++;
		while (true) {
			iters+= temp.size();
			if (i >= image.size()) {
				encoding.push_back(dict[temp]);
				break;
			}
			int prev = dict[temp];
			push_back(temp, image[i]);
			if (dict.find(temp) == dict.end()) {
				encoding.push_back(prev);
				dict[temp] = cntr++;
				break;
			}
			i++;
		}
	}
	cout << iters << endl;
	cout << encoding.size() << " " << width*height << endl;
	int mx = 0;
	for (int i = 0; i < encoding.size(); i++) {
		mx = max(mx, encoding[i]);
	}
	int sz = 0;
	while (mx > 0) {
		mx /= 2;
		sz++;
	}
	cout << sz << endl;
	imwrite("D:\\input.bmp", mat);
	
	
	vector<bool> toWrite;
	for (int i = 0; i < 8; i++) {
		toWrite.push_back((sz >> (7 - i)) & 1);
	}
	for (int i = 0; i < encoding.size(); i++) {
		for (int j = 0; j < sz; j++) {
			toWrite.push_back((encoding[i] >> (7-j))&1);
		}
	}
	while (toWrite.size() % 8 != 0) {
		toWrite.push_back(false);
	}
	vector<uchar> final;
	for (int i = 0; i < toWrite.size(); i += 8) {
		uchar x = 0;
		for (int j = i; j < i + 8; j++) {
			x = x * 2 + toWrite[j];
		}
		final.push_back(x);
	}
	FILE* pFile;
	pFile = fopen(outputPath.c_str(), "wb");
	fwrite(&final[0], sizeof(vector<uchar>::value_type), final.size(), pFile);
	fclose(pFile);
	return;
}
void decompressLZW(string inputFile, string outputFile) {
	ifstream input(inputFile, ios::binary);
	vector<char> buffer((
		istreambuf_iterator<char>(input)),
		(istreambuf_iterator<char>()));
	int sz = ((uchar)buffer[0]);
	vector<int> encoding;
	int curr = 0, bits = 0;
	for (int i = 1; i < buffer.size(); i++) {
		for (int j = 0; j < 8; j++) {
			curr = curr * 2 + (int)((((int)((uchar)buffer[i])) >> (7 - j))&1);
			bits++;
			if (bits == sz) {
				//if (curr < 0 || curr > 255) cout << "curr: " << curr << endl;
				encoding.push_back(curr);
				curr = 0;
				bits = 0;
			}
		}
	}
	cout << encoding[0] << endl;
	unordered_map<int, vector<uchar> > dict;
	vector<uchar> decoding;
	int cntr = 0;
	for (int i = 0; i < 256; i++) {
		vector<uchar> temp;
		temp.push_back((uchar)i);
		dict[cntr++] = temp;
	}
	int j = 0;
	int pw, cw;
	
	for (int i = 0; i < encoding.size(); i++) {
		cw = encoding[i];
		if (dict.find(cw) != dict.end()) {
			decoding.insert(decoding.end(), dict[cw].begin(), dict[cw].end());
			if (i > 0) {
				vector<uchar> temp;
				temp.insert(temp.end(), dict[pw].begin(), dict[pw].end());
				temp.push_back(dict[cw][0]);
				dict[cntr++] = temp;
			}
		}
		else {
			vector<uchar> temp;
			temp.insert(temp.begin(), dict[pw].begin(), dict[pw].end());
			temp.push_back(dict[cw][0]);
			decoding.insert(decoding.end(), temp.begin(), temp.end());
			dict[cntr++] = temp;
		}
		pw = cw;
	}
	cout << "out size: " << decoding.size() << endl;
	
}
int main() {
	for (int i = 1; i <= 1; i++) {
		string path = INPUT_IMAGE_PATH + to_string(i) + ".bmp";
		decompressLZW("D:\\output.pku", "D:\\dec.bmp");
	}
	/*clock_t begin = clock();
	map<int, int> x;
	for (int i = 0; i < 3000; i++) {
		for (int j = 0; j < 4000; j++) {
			x[i] = j;
		}
	}*/
	/*clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << elapsed_secs << endl;*/
	//showImage(inputImage);
	_getch();
	return 0;
}




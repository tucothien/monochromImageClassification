#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>

using namespace std;

#define SIZE 50
//size 14
struct BitmapFileHeader{
	unsigned short align;		//bien nay them vao chi de xep hang		
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
};

//size 40
struct BitmapInfoHeader {
	unsigned int biSize ;
	unsigned int biWidth;
	unsigned int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	unsigned int biXPelsPerMeter;
	unsigned int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
};

struct RGB {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

double* bitmapReader(char* filePath);
void writeToFile(double* imVector, char* dataFilePath);
void int2str(unsigned int number, char aNum[10]);
double* train(char* directory, int numOfSample);
double compare2Vect(double* vect1, double* vect2);
int predict(char* filePath, double* meanVect1, double* meanVect2);
main() {
	// training
	double* meanVect8 = train(".//imData//8", 10);
	double* meanVect7 = train(".//imData//7", 10);
	
	//testing

	if (predict("t.bmp", meanVect7, meanVect8)) {
		cout<<"this is number 8";
	} else {
		cout<<"this is number 7";
	}
}





int predict(char* filePath, double* meanVect1, double* meanVect2) {
	// return 1 if test file belongs to class 2
	// else return 0
	double* testFile = bitmapReader(filePath);
	writeToFile(testFile, 0);
	double dis1 = compare2Vect(testFile, meanVect1);
	double dis2 = compare2Vect(testFile, meanVect2);
	delete testFile;
	return dis2 < dis1;
}

double* train(char* directory, int numOfSample) {
	// Read all the files in the directory and calculate the mean of features
	// all the files must be name from 0.bmp to numOfSamble.bmp
	// all the files must be monochrome bitmap with size SIZExSIZE
	double* meanVector = new double[SIZE*SIZE];
	for (int j = 0; j<numOfSample; j++) {
		meanVector[j] = 0;
	}

	for (int i=0; i<numOfSample; i++) { // examine all the images in the folder
		// generate the file Path
		char fileName[100];
		int2str(i, fileName); // i
		strcat(fileName, ".bmp"); // i.bmp
		char filePath[100];
		strcpy(filePath, directory); // filePath
		strcat(filePath, "//"); // filePath//
		strcat(filePath, fileName); // filePath//i.bmp
		
		// read file and update the meanVector
		double* imVector = bitmapReader(filePath);
		for (int j = 0; j<SIZE*SIZE; j++) {
			meanVector[j] += imVector[j] / numOfSample;
		}
	}
	return meanVector;
}

double compare2Vect(double* vect1, double* vect2) {
	// calculate mean of square of euclidean distance between 2 vector
	double distance = 0;
	for (int i = 0; i< SIZE*SIZE; i++) {
		distance += (vect1[i] - vect2[i]) * (vect1[i] - vect2[i]);
	}
	return distance / (SIZE*SIZE);
}


double* bitmapReader(char* filePath) {
	BitmapFileHeader fileheader;
	BitmapInfoHeader infoheader;
	fstream file(filePath,ios::in | ios::binary);
	if(!file) cout<<"\nbitmap file_error: " <<filePath;
	file.read((char*)&fileheader.bfType,14);
	file.read((char*)&infoheader,40);
#if 0
	cout<<"\n\n"<<"        FILEHEADER:";
	cout<<endl<<"bfType           : "<<fileheader.bfType;
	cout<<endl<<"size all         : "<<fileheader.bfSize<<"*";
	cout<<endl<<"reserved1        : "<<fileheader.bfReserved1;	
	cout<<endl<<"reserved2        : "<<fileheader.bfReserved2;	
	cout<<endl<<"offset to data   : "<<fileheader.bfOffBits<<"*";
	cout<<"\n\n"<<"        INFOHEADER:";
	cout<<endl<<"info header size : "<<infoheader.biSize<<"*";
	cout<<endl<<"image width      : "<<infoheader.biWidth<<"*";	
	cout<<endl<<"image height     : "<<infoheader.biHeight<<"*";	
	cout<<endl<<"biPlanes         : "<<infoheader.biPlanes;	
	cout<<endl<<"bit Count        : "<<infoheader.biBitCount<<"*";	
	cout<<endl<<"is compression   : "<<infoheader.biCompression;	
	cout<<endl<<"data size        : "<<infoheader.biSizeImage<<"*";
	cout<<endl<<"biXPelsPerMeter  : "<<infoheader.biXPelsPerMeter;
	cout<<endl<<"biYPelsPerMeter  : "<<infoheader.biYPelsPerMeter;
	cout<<endl<<"biClrUsed        : "<<infoheader.biClrUsed;	
	cout<<endl<<"biClrImportant   : "<<infoheader.biClrImportant;
#endif
	//xuat data

	int hang,cot;
	hang = (infoheader.biWidth/8) + !!(infoheader.biWidth%8);	//tinh so byte cua mot hang
	hang = 4*(hang/4) + 4*(!!(hang%4));							//so byte tren mot hang phai chia het cho 4
	cot = infoheader.biHeight;
	char* ptem  = new char(0);
	int i,j,k;
	int dem;
	file.seekg(fileheader.bfOffBits);
	
	double* imVector;
	imVector = new double[50*50];
	int index = 0;
	for(i=cot-1;i>=0;i--){
		dem = 0;
		for(j=1;j<=hang;j++){
			file.read(ptem,1);
			*ptem = ~*ptem;
			for(k=7;k>=0;k--){
				dem++;
				if(dem<=infoheader.biWidth){
					int num = (*ptem>>k)&1;
					imVector[index++] = (double)num;
				}
			}
		}
	}
	return imVector;
}

void writeToFile(double* imVector, char* dataFilePath) {
	// print imageVector to file or screen
	// dataFilePath = 0 -> print to screen
	if (dataFilePath == 0) {
		for (int i = 0; i < SIZE*SIZE; i++) {
			printf("|%.0f", (imVector[i] + 0.0000001));
			if ((i+1) % SIZE == 0) {
				cout<<endl;
			}	
		}
	} else {
		fstream dataFile(dataFilePath,ios::out|ios::binary);
		if(dataFile) cout<<"\ndata file_ok";
		else cout<<"\ndata file_error";
		cout<<endl;
		for (int i = 0; i < SIZE*SIZE; i++) {
			char num = (int)imVector[i];
			dataFile.put(num | 0x30);
			if ((i+1) % SIZE == 0) {
				dataFile<<"\n";
			}	
		}
		dataFile.close();
	}
}


void int2str(unsigned int number, char aNum[10]) {
	// convert integer to string
	if (number == 0) {
		aNum[0] = '0';
		aNum[1] = '\0';
		return;
	}
	int index = 0;
	int num = number;
	char temp[10];
	while (num != 0) {
		char a = ((char)(num % 10)) | 0x30;
		
		temp[index++] = a;
		num = num/10;
	}
	for (int i = 0; i < index; i++) {
		aNum[i] = temp[index - i -1];
	}
	aNum[index] = '\0';
}

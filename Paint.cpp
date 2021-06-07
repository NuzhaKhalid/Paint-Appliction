#include <fstream>
#include "graphics.h"
#include <iostream>
#include <string>
#include <math.h>

using namespace std;

struct Point {
	double x, y;

	Point() {
		x = 0, y = 0;
	}
	Point(int a, int b) {
		x = a, y = b;
	}
};

class Shape {

protected:

	Point* vertices;
	int color;
	int np;

public:
	Shape() {
		color = 0;
		vertices = nullptr;
		np = 0;
	}
	Shape(Point* v, int c, int n)
		:color(c), np(n)
	{
		vertices = new Point[n];
		for (int i = 0; i < n; i++) {
			vertices[i].x = v[i].x;
			vertices[i].y = v[i].y;
		}
	}

	virtual void draw() = 0;
	virtual bool containspoint(int a, int b) = 0;
	virtual void print(ofstream& out) = 0;
	virtual void fill(int x, int y, int fillclr) = 0;

	virtual ~Shape() {
		cout << "Shape destructor called" << endl;
		if (vertices != nullptr)
			delete[] vertices;
	}

};

void floodFill(int x, int y, int fillclr, int boundary) {

	if (getpixel(x, y) != boundary && getpixel(x, y) != fillclr && x > 0 && x < getwindowwidth() && y > 0 && y<getwindowheight()) {
		putpixel(x, y, fillclr);
		floodFill(x + 1, y, fillclr, boundary);
		floodFill(x, y + 1, fillclr, boundary);
		floodFill(x - 1, y, fillclr, boundary);
		floodFill(x, y - 1, fillclr, boundary);
	}
}
void floodFillLines(int x, int y, int fillclr, int boundary) {

	if (getpixel(x, y) == boundary && getpixel(x, y) != fillclr && x > 0 && x < getwindowwidth() && y > 0 && y<getwindowheight()) {
		putpixel(x, y, fillclr);
		floodFillLines(x + 1, y, fillclr, boundary);
		floodFillLines(x, y + 1, fillclr, boundary);
		floodFillLines(x - 1, y, fillclr, boundary);
		floodFillLines(x, y - 1, fillclr, boundary);
	}
}

class Polyg : public Shape {

protected:

	int fillcolor;

public:
	Polyg() {
		vertices = 0;
		color = BLACK;
		np = 0;
		fillcolor = BLACK;
	}
	Polyg(Point* v, int c, int n, int fc)
		: Shape(v, c, n), fillcolor(fc)
	{}

	void draw() {

		setcolor(color);
		for (int i=0; i < np; i++ ) 
			if(i==np-1)
				line(vertices[i].x, vertices[i].y, vertices[0].x, vertices[0].y);
			else
				line(vertices[i].x, vertices[i].y, vertices[i+1].x, vertices[i+1].y);
	}

	bool containspoint(int a, int b) {
		
		int j = np - 1;
		bool point = false;

		for (int i = 0; i < np; i++)
		{
			if (vertices[i].y < b && vertices[j].y >= b ||
				vertices[j].y < b && vertices[i].y >= b)
			{
				if (vertices[i].x +
					(b - vertices[i].y) / (vertices[j].y - vertices[i].y)*(vertices[j].x - vertices[i].x) < a)
				{
					point = !point;
				}
			}
			j = i;
		}
		return point;
	}

	virtual void print(ofstream& out) {

		out << "Polygon" << endl;
		out << np << endl;
		for (int i = 0; i < np; i++) {
			out << vertices[i].x << endl;
			out << vertices[i].y << endl;
		}
		out << color << endl;
		out << fillcolor << endl;
	}
	void fill(int x, int y, int fillclr){
		
		fillcolor = fillclr;
		floodFill(x, y, fillcolor, color);
	}
};

class Circle : public Shape {

	int fillcolor;
	double rad;

public:

	Circle(Point* v, int c, int n, int fc, double r)
		: Shape(v, c, n), fillcolor(fc), rad(r)
	{}
	bool containspoint(int a, int b) {

		int x = (vertices[0].x - a) * (vertices[0].x - a);
		int y = (vertices[0].y - b) * (vertices[0].y - b);
		int dist = sqrt(x + y);

		if (dist <= rad)
			return true;
		else
			return false;

	}
	void draw() {
		setcolor(color);
		circle(vertices[0].x, vertices[0].y, rad);
	}

	virtual void print(ofstream& out) {

		out << "Circle" << endl;
		out << np << endl;
		for (int i = 0; i < np; i++) {
			out << vertices[i].x << endl;
			out << vertices[i].y << endl;
		}
		out << rad << endl;
		out << color << endl;
		out << fillcolor << endl;
	}
	void fill(int x, int y, int fillclr) {

		fillcolor = fillclr;
		floodFill(x, y, fillcolor, color);
	}

};

class Rect : public Polyg {

public:
	Rect() {
		vertices = nullptr;
		color = 0;
		np = 0;
		fillcolor = 0;
	}
	Rect(Point* v, int c, int n, int fc)
		: Polyg(v,c,n,fc)
	{}

	bool containspoint(int a, int b) {
		
		if (a >= vertices[0].x && a <= vertices[1].x && b >= vertices[0].y && b <= vertices[1].y)
			return true;
		else
			return false;
	}
	void draw(){}
	void fill(int x, int y, int fillclr){
		fillcolor = fillclr;
		floodFill(x, y, fillcolor, color);
	}

};

class Text : public Shape {    

	string str = "";

public:

	Text(Point* v, int c, int n, string s)
		:Shape(v, c, n)
	{
		str = s;
	}


	void draw() {
		

		
	}
	bool containspoint(int a, int b) {
		
		if (a >= vertices[0].x && a <= vertices[1].x && b >= vertices[0].y && b <= vertices[1].y)
			return true;
		else
			return false;
	}
	virtual void print(ofstream& out) {

		out << "Text" << endl;
		out << np << endl;
		for (int i = 0; i < np; i++) {
			out << vertices[i].x << endl;
			out << vertices[i].y << endl; 
		}
		out << str;
		out << endl;
		out << color << endl;
	}
	void fill(int x, int y, int fillclr) {
		floodFillLines(x, y, fillclr, color);
	}

};

class OpenShapes : public Shape {

	bool style;

public:

	OpenShapes(Point* v, int c, int n, bool s)
		: Shape(v, c, n), style(s)
	{}

	void draw() {

		setcolor(color);
		int a = 0, b = 0, i = 0;
		setlinestyle(style, 0, 3);
		while (i < np-1) {
			
			line(vertices[i].x, vertices[i].y, vertices[i+1].x, vertices[i+1].y);
			i++;
		}
		
	}
	bool containspoint(int a , int b) {

		bool flag;
		int i = 0;
		
		if (np == 2) {
			double dx = vertices[1].x - vertices[0].x;
			double dy = vertices[1].y - vertices[0].y;
			double slope = dy / dx;
			double c = vertices[1].y - slope*vertices[1].x;
			if (abs(b - (slope*a + c)) <= 2)
				return true;
			else
			return false;
		}
		else if (np > 2) {

			for (int i = 0; i < np - 1; i++) {
				double dx = vertices[i+1].x - vertices[i].x;
				double dy = vertices[i+1].y - vertices[i].y;
				double slope = dy / dx;
				double c = vertices[i+1].y - slope*vertices[i+1].x;
				if (abs(b - (slope*a + c)) <= 2)
					return true;
			}
			return false;
		}
	}
	virtual void print(ofstream& out) {

		out << "Openshapes" << endl;
		out << np << endl;
		for (int i = 0; i < np; i++) {
			out << vertices[i].x << endl << vertices[i].y << endl;
		}
		out << color << endl;
		out << style << endl;
	}
	void fill(int x, int y, int fillclr) {

		floodFillLines(x, y, fillclr, color);
	}

};

bool containspanel(int x, int y) {

	Point v[2];
	v[0].x = 2, v[0].y = 2;
	v[1].x = 80, v[1].y = 103;
	Rect temp1(v, 0, 2, 0);

	v[0].x = 2, v[0].y = 103;
	v[1].x = 41, v[1].y = 135;
	Rect temp2(v, 0, 2, 0);

	if (temp1.containspoint(x, y) || temp2.containspoint(x,y))
		return true;
	else
		return false;
}

bool containscolor(int x, int y) {

	Point v[2];
	v[0].x = 614, v[0].y = 2;
	v[1].x = 650, v[1].y = 215;

	Rect temp(v, 0, 2, 0);

	if (temp.containspoint(x, y))
		return true;
	else
		return false;
}
bool containscanvas(int x, int y) {

	Point v[2];
	v[0].x = 2, v[0].y = 2;
	v[1].x = 80, v[1].y = 103;
	Rect temp1(v, 0, 2, 0);

	v[0].x = 2, v[0].y = 103;
	v[1].x = 41, v[1].y = 135;
	Rect temp2(v, 0, 2, 0);

	v[0].x = 614, v[0].y = 2;
	v[1].x = 650, v[1].y = 215;
	Rect temp3(v, 0, 2, 0);

	if (temp1.containspoint(x, y) || temp2.containspoint(x, y) || temp3.containspoint(x, y))
		return false;
	else
		return true;

}

void PrintScreen() {


	
	setlinestyle(0, 1, 3);
	setfillstyle(1, 5);
	bar3d(2, 2, 80, 103, 0, 1);
	line(41, 2, 41, 100);                      //verticle line
	line(2, 36, 79, 36);                      // 1st line
	line(2, 68, 79, 68);                     // 2nd line
											 //	line(2, 94, 80, 94);                    //3rd line
	
	setlinestyle(0, 1, 3);
	circle(21, 19, 10);                    //circle

	int poly2[10] = { 60,9,71,19,60,30,49,20,60,9 };
	drawpoly(5, poly2);                        //polygon
	line(11, 50, 32, 50);

	//paint brush
	line(54, 55, 70, 44);
	setfillstyle(1, 15);
	fillellipse(52, 57, 3, 3);

	//Text
	settextstyle(9, HORIZ_DIR, 0);
	setusercharsize(1, 1, 1, 1);
	setbkcolor(5);
	outtextxy(9, 70, "T");

	//save file
	rectangle(50, 78, 70, 94);
	setcolor(5);
	line(76, 82, 62, 74);
	line(76, 85, 62, 77);
	setcolor(15);
	int poly[8] = { 62,78,71,85,62,85, 62,78 };
	drawpoly(4, poly);

	//Eraser
	setfillstyle(1, 5);
	bar3d(2, 103, 41, 135, 0, 1);
	rectangle(10, 114, 33, 124);
	setfillstyle(1, 15);
	bar3d(27, 114, 33, 124, 0, 1);

	//dotted line
	setfillstyle(1, 5);
	bar3d(41, 103, 80 , 135, 0, 1);
	setcolor(15);
	setlinestyle(DOTTED_LINE, 1, THICK_WIDTH);
	line(48, 123, 73, 123);
	line(48, 116, 73, 116);


	//COLORS display

	setlinestyle(0, 1, 3);
	setfillstyle(1, BLUE);
	bar3d(614, 2, 650, 29, 0, 1);
	
	setfillstyle(1, GREEN);
	bar3d(614, 29, 650, 55, 0, 1);
	
	setfillstyle(1, RED);
	bar3d(614, 55, 650, 82, 0, 1);
	
	setfillstyle(1, BLACK);
	bar3d(614, 82, 650, 109, 0, 1);
	
	setfillstyle(1, YELLOW);
	bar3d(614, 109, 650, 134, 0, 1);
	
	setfillstyle(1, MAGENTA);
	bar3d(614, 134, 650, 163, 0, 1);
	
	setfillstyle(1, COLOR(255, 100, 0));
	bar3d(614, 163, 650, 190, 0, 1);
	
	setfillstyle(1, WHITE);
	bar3d(614, 190, 650, 215, 0, 1);

	setbkcolor(0);
}

int main() {

	// PANEL 
	int wid1 = initwindow(660, 500, "Paint");
	PrintScreen();
	setbkcolor(0);
	int color = 15;
	int fillclr = 0;

	// SHAPES BUTTON
	Point* P = new Point[2];
	P[0].x = 2, P[0].y = 2;        //left , top
	P[1].x = 41, P[1].y = 36;    //right bottom
	Rect C(P, 15, 2, 5);
	P[0].x = 41, P[0].y = 2;
	P[1].x = 80, P[1].y = 36;
	Rect p(P, 15, 2, 5);
	P[0].x = 2, P[0].y = 36;
	P[1].x = 41, P[1].y = 68;
	Rect L(P, 15, 2, 5);
	P[0].x = 41, P[0].y = 36;
	P[1].x = 80, P[1].y = 68;
	Rect B(P, 15, 2, 5);
	P[0].x = 2, P[0].y = 68;
	P[1].x = 41, P[1].y = 103;
	Rect T(P, 15, 2, 5);
	P[0].x = 41, P[0].y = 68;
	P[1].x = 80, P[1].y = 103;
	Rect S(P, 15, 2, 5);
	P[0].x = 2, P[0].y = 103;
	P[1].x = 41, P[1].y = 135;
	Rect E(P, 15, 2, 5);
	P[0].x = 41, P[0].y = 103;
	P[1].x = 80, P[1].y = 135;
	Rect DL(P, 15, 2, 5);



	//COLORS BUTTON
	P[0].x = 615, P[0].y = 2;
	P[1].x = 650, P[1].y = 29;
	Rect Blue(P, 15, 2, 5);
	P[0].x = 614, P[0].y = 29;
	P[1].x = 650, P[1].y = 55;
	Rect Green(P, 15, 2, 5);
	P[0].x = 614, P[0].y = 55;
	P[1].x = 650, P[1].y = 82;
	Rect Red(P, 15, 2, 5);
	P[0].x = 614, P[0].y = 82;
	P[1].x = 650, P[1].y = 109;
	Rect Black(P, 15, 2, 5);
	P[0].x = 614, P[0].y = 109;
	P[1].x = 650, P[1].y = 134;
	Rect Yellow(P, 15, 2, 5);
	P[0].x = 614, P[0].y = 134;
	P[1].x = 650, P[1].y = 163;
	Rect Magenta(P, 15, 2, 5);
	P[0].x = 614, P[0].y = 163;
	P[1].x = 650, P[1].y = 190;
	Rect Orange(P, 15, 2, 5);
	P[0].x = 614, P[0].y = 190;
	P[1].x = 650, P[1].y = 215;
	Rect White(P, 15, 2, 5);


	int x = 0, y = 0;
	double rad = 0;
	Shape** allShapes;
	int cap = 0, size = 0;
	int c = 0;
	int select = 0;

	int wid2 = initwindow(360, 250, "LOAD FILE");
	setcurrentwindow(wid2);
	outtextxy(3, 3, "Press 1 to load file otherwise 0 to continue.  ");
	char ch[2];
	ch[1] = 0;
	ch[0] = getch();
	
	if (ch[0] == '1')                   //load file
	{
		char chr[2];
		chr[1] = 0;
		outtextxy(3, 25, "Enter the file name:  ");
		char file[100];
		int i = 0;
		int x = 3, y = 45;
		for (i = 0;; i++) {

			chr[0] = getch();
			if (chr[0] == 13) {
				break;
			}
			file[i] = chr[0];

			outtextxy(x, y, chr);
			x += textwidth(chr);
		}
		file[i] = 0;
		closegraph(wid2);
		setcurrentwindow(wid1);
		ifstream load;
		
		load.open(file);
		if (!load) {
			cout << "ERROR";
		}
		int np;
		int clr;
		int fill;
		int x0, y0;
		string lol;
		while (load) {
		
			load >> lol;
			cout << lol;
			if (lol == "Circle") {
				Point* P = new Point[1];
				double rad = 0;
				load >> np;
				load >> P[0].x;
				load >> P[0].y;
				load >> rad;
				load >> clr;
				load >> fill;
				cout << fill;
			
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new Circle(P, clr, np, fill, rad);
					size = 1;
					cap = 1;
					c++;
					allShapes[0]->draw();
				}
				else if (size < cap) {
					allShapes[c] = new Circle(P, clr, 1, fill, rad);
					size++;
					allShapes[c]->draw();
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                   //pointer at stack
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new Circle(P, clr, 1, fill, rad);
					size++;
					delete[] allShapes;
					allShapes = newarr;
					allShapes[c]->draw();
					c++;
				}
			}
			else if (lol == "Polygon") {
				cout << "POLy";
				load >> np;
				cout << np;
				Point* P = new Point[np];
				for (int i = 0; i < np; i++) {
					load >> P[i].x;
					load >> P[i].y;
				}
				load >> clr;
				load >> fill;
			//	p = load.peek();
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new Polyg(P, clr, np, fill);
					allShapes[0]->draw();
					size = 1;
					cap = c = 1;
				}
				else if (size < cap) {
					allShapes[c] = new Polyg(P, clr, np, fill);
					allShapes[c]->draw();
					size++;
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                   //pointer at stack
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new Polyg(P, clr, np, fill);
					size++;
					c++;
					delete[] allShapes;
					allShapes = newarr;
					allShapes[size-1]->draw();
				}
				
			}
			else if (lol == "Openshapes") {
				load >> np;
				Point* P = new Point[np];
				for (int i = 0; i < np; i++) {
					load >> P[i].x;
					load >> P[i].y;
				}
				load >> clr;
				load >> fill;
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new OpenShapes(P, clr,np, SOLID_LINE);
					allShapes[0]->draw();
					size = 1;
					cap = c = 1;
				}
				else if (size < cap) {
					allShapes[c] = new OpenShapes(P, clr, np, SOLID_LINE);
					allShapes[c]->draw();
					size++;
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                   //pointer at stack
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new OpenShapes(P, clr, np, SOLID_LINE);
					size++;
					c++;
					delete[] allShapes;
					allShapes = newarr;
					allShapes[size - 1]->draw();
				}
			}
			if (lol == "Text") {
				load >> np;
				Point* P = new Point[2];
				for (int i = 0; i < np; i++) {
					load >> P[i].x;
					load >> P[i].y;
				}
				string str;
				load >> str;
				load >> str;
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new Text(P, color, 2,str);
					size = 1;
					cap = c = 1;
				}
				else if (size < cap) {
					allShapes[c] = new Text(P, color, 2,str);
					size++;
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                   //pointer at stack
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new Text(P, color, 2,str);
					size++;
					c++;
					delete[] allShapes;
					allShapes = newarr;
				}
				int i = 0;
				char crap[2];
				crap[1] = 0;
				while (i<str.length()) {
					crap[0] = str[i];
					outtextxy(x, y, crap);
					x += textwidth(crap);
				}

			}
		}
		load.close();
	}
	else {
		closegraph(wid2);
		setcurrentwindow(wid1);
	}

	x = 0, y = 0;
	while (true) {

		//cout << select << endl;
		if (select == 0) {

			if (ismouseclick(WM_LBUTTONDOWN)) {
				getmouseclick(WM_LBUTTONDOWN, x, y);
			}
		}
		if (select == 2) {
			//	delay(1000);
			if (ismouseclick(WM_LBUTTONDOWN)) {

				getmouseclick(WM_LBUTTONDOWN, x, y);
				if (containscanvas(x, y)) {
					cout << "lolo";
					for (int i = 0; i < c; i++) {
						if (allShapes[i]->containspoint(x, y)) {
							allShapes[i]->fill(x, y, fillclr);
						}
					}
				}
			}
		}
		
		
		if (C.containspoint(x, y)) {             //circle button

			cout << "circle " << endl;
			if (ismouseclick(WM_LBUTTONDOWN)) {
				getmouseclick(WM_LBUTTONDOWN, x, y);
				if (!containscanvas(x, y)) {
					select = 1;
					break;
				}
				Point*P = new Point[1];
				P[0].x = x;                        //point
				P[0].y = y;
				cout << "  x  " << x << "  y  " << y << endl;
				delay(1000);

				getmouseclick(WM_LBUTTONDOWN, x, y);
				if (!containscanvas(x, y)) {
					select = 1;
					break;
				}
				cout << " next x  " << x << "  next y  " << y << endl;

				int a = (P[0].x - x) * (P[0].x - x);
				int b = (P[0].y - y) * (P[0].y - y);
				rad = sqrt(a + b);
				cout << "rad" << " " << rad << endl;
	
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new Circle(P, color, 1, 0, rad);
					size = 1;
					cap = 1;
					c++;
					allShapes[0]->draw();
				}
				else if (size < cap) {
					allShapes[c] = new Circle(P, color, 1, 0, rad);
					size++;
					allShapes[c]->draw();
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                   //pointer at stack
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new Circle(P, color, 1, 0, rad);
					size++;
					delete[] allShapes;
					allShapes = newarr;
					allShapes[c]->draw();
					c++;
				}
				select = 0;
			}
		}
		else if (p.containspoint(x, y)) {       //polygon button

		//	cout << "poly " << endl;
			int i = 0;
			Point*P = new Point[3];
			delay(500);
			int control = 0;
			while (true) {

					//cout << "loool";
					//delay(1000);
					if (ismouseclick(WM_RBUTTONDOWN)) {
						getmouseclick(WM_RBUTTONDOWN, x, y);
						select = 0;
						break;
					}
					else if (ismouseclick(WM_LBUTTONDOWN)) {
						getmouseclick(WM_LBUTTONDOWN, x, y);

							
						if (!containscanvas(x, y)) {
							select = 1;
							cout << "break";
							break;
						}
						cout << "x  (" << i << ")  " << x << "y  (" << i << ")  " << y << endl;
						if (i > 2) {
							Point* Pnew = new Point[i + 1];
							for (int k = 0; k < i; k++) {
								Pnew[k] = P[k];
							}
							Pnew[i].x = x;
							Pnew[i].y = y;
							delete P;
							P = new Point[i + 1];
							for (int k = 0; k < i + 1; k++) {
								P[k] = Pnew[k];
							}
							delete Pnew;
						}
						else {
							P[i].x = x;
							P[i].y = y;
						}
						i++;
					}
			}
			if (select != 1 && i > 1) {
				Polyg temp(P, color, i, 0);
				temp.draw();
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new Polyg(P, color, i, 0);
					size = 1;
					cap = c = 1;
				}
				else if (size < cap) {
					allShapes[c] = new Polyg(P, color, i, 0);
					size++;
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                   //pointer at stack
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new Polyg(P, color, i, 0);
					size++;
					c++;
					delete[] allShapes;
					allShapes = newarr;
				}
				select = 0;
			}
		}
		else if (L.containspoint(x, y)) {       //Line or openshape button

			cout << "line" << endl;
			Point* P = new Point[2];
			int i = 0;
			while (true) {
				if (ismouseclick(WM_RBUTTONDOWN)) {
					getmouseclick(WM_RBUTTONDOWN , x, y);
					select = 0;
					break;
				}
				else if (ismouseclick(WM_LBUTTONDOWN)) {
					getmouseclick(WM_LBUTTONDOWN, x, y);
					if (!containscanvas(x, y)) {
						select = 1;
						break;
					}
					if (i > 1) {
						Point* Pnew = new Point[i + 1];
						for (int k = 0; k < i; k++) {
							Pnew[k] = P[k];
						}
						Pnew[i].x = x;
						Pnew[i].y = y;
						delete P;
						P = new Point[i + 1];
						for (int k = 0; k < i + 1; k++) {
							P[k] = Pnew[k];
						}
						delete Pnew;
					}
					else {
						P[i].x = x;
						P[i].y = y;
					}
					i++;
				}
			}
			if (select != 1) {
				OpenShapes L(P, color, i, SOLID_LINE);
				L.draw();
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new OpenShapes(P, color, i, SOLID_LINE);
					size = 1;
					cap = c = 1;
				}
				else if (size < cap) {
					allShapes[c] = new OpenShapes(P, color, i, SOLID_LINE);
					size++;
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new OpenShapes(P, color, i, SOLID_LINE);
					size++;
					c++;
					delete[] allShapes;
					allShapes = newarr;
				}
				select = 0;
			}
		}
		else if (B.containspoint(x, y)) {       //brush

			//cout << "brushh";
			select = 1;
				if (ismouseclick(WM_LBUTTONDOWN)) {
					getmouseclick(WM_LBUTTONDOWN, x, y);

					cout << x << "  " << y << endl;
					if (Blue.containspoint(x, y) && select == 1) {
						cout << "lol";
						fillclr = BLUE;
						select = 2;
						//cout << select;
					}
					else if (Green.containspoint(x, y) && select == 1) {
						fillclr = GREEN;
						select = 2;
					}

					else if (Red.containspoint(x, y) && select == 1) {
						fillclr = RED;
						select = 2;
					}

					else if (Black.containspoint(x, y) && select == 1) {
						fillclr = BLACK;
						select = 2;
					}

					else if (Yellow.containspoint(x, y) && select == 1) {
						fillclr = YELLOW;
						select = 2;
					}

					else if (Magenta.containspoint(x, y) && select == 1) {
						fillclr = MAGENTA;
						select = 2;
					}

					else if (Orange.containspoint(x, y) && select == 1) {
						fillclr = COLOR(255, 100, 0);
						select = 2;
					}

					else if (White.containspoint(x, y) && select == 1) {
						fillclr = WHITE;
						select = 2;
					}
					
				}	
		}
		else if (DL.containspoint(x, y)) {       //dotted openshapes

			cout << "line" << endl;
			Point* P = new Point[2];
			int i = 0;
			while (true) {
				if (ismouseclick(WM_RBUTTONDOWN)) {
					getmouseclick(WM_RBUTTONDOWN, x, y);
					select = 0;
					break;
				}
				else if (ismouseclick(WM_LBUTTONDOWN)) {
					getmouseclick(WM_LBUTTONDOWN, x, y);
					if (!containscanvas(x, y)) {
						select = 1;
						break;
					}
					if (i > 1) {
						Point* Pnew = new Point[i + 1];
						for (int k = 0; k < i; k++) {
							Pnew[k] = P[k];
						}
						Pnew[i].x = x;
						Pnew[i].y = y;
						delete P;
						P = new Point[i + 1];
						for (int k = 0; k < i + 1; k++) {
							P[k] = Pnew[k];
						}
						delete Pnew;
					}
					else {
						P[i].x = x;
						P[i].y = y;
					}
					i++;
				}
			}
			if (select != 1) {
				OpenShapes L(P, color, i, DOTTED_LINE);
				L.draw();
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new OpenShapes(P, color, i, DOTTED_LINE);
					size = 1;
					cap = c = 1;
				}
				else if (size < cap) {
					allShapes[c] = new OpenShapes(P, color, i, DOTTED_LINE);
					size++;
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new OpenShapes(P, color, i, DOTTED_LINE);
					size++;
					c++;
					delete[] allShapes;
					allShapes = newarr;
				}
				select = 0;
			}

		}

		else if (T.containspoint(x, y)) {      // text

			cout << "text";
			delay(1000);

			if (ismouseclick(WM_LBUTTONDOWN))
			{
				getmouseclick(WM_LBUTTONDOWN, x, y);

				int x0 = x;
				int y0 = y;
				string str;
				char ch[2];
				setbkcolor(0);
				int h = x;
				setcolor(color);
				settextstyle(3, HORIZ_DIR, 2);
				ch[1] = 0;
				int i = 0, j;
				char lol;
				while (true)                  //keep displaying
				{
					ch[0] = getch();
					if (ch[0] == 13)
						break;
					if (x > getwindowwidth() - 27) {
						y += textheight(ch);
						x = h;
					}
					outtextxy(x, y, ch);
					lol = ch[0];
					str += lol;
					x += textwidth(ch);
					i++;
				}

				cout << str;
				Point* yay = new Point[2];
				yay[0].x = x0;
				yay[0].y = y0;
				yay[1].x = x;
				yay[1].y = y;

				Text T(yay, color, 2,str);
				T.draw();
				if (cap == 0) {
					allShapes = new Shape*[1];
					allShapes[0] = new Text(yay, color, 2,str);
					size = 1;
					cap = c = 1;
				}
				else if (size < cap) {
					allShapes[c] = new Text(yay, color, 2,str);
					size++;
					c++;
				}
				else if (size == cap) {
					cap = cap * 2;
					Shape** newarr;                   //pointer at stack
					newarr = new Shape*[cap];
					for (int i = 0; i < size; i++) {
						newarr[i] = allShapes[i];
					}
					newarr[size] = new Text(yay, color, 2,str);
					size++;
					c++;
					delete[] allShapes;
					allShapes = newarr;
				}
			}
		}
		else if (S.containspoint(x, y)) {       // save 

			ofstream output;
			output.open("Paint.txt");
			for (int i = 0; i < size; i++)
			{
				allShapes[i]->print(output);
			}
		}
		

		else if (Blue.containspoint(x, y) && select ==0) {
			color = BLUE;
		}
			
		else if (Green.containspoint(x, y) && select == 0)
			color = GREEN;

		else if (Red.containspoint(x, y) && select == 0)
			color = RED;

		else if (Black.containspoint(x, y) && select == 0)
			color = BLACK;

		else if (Yellow.containspoint(x, y) && select == 0)
			color = YELLOW;

		else if (Magenta.containspoint(x, y) && select == 0)
			color = MAGENTA;

		else if (Orange.containspoint(x, y) && select == 0)
			color = COLOR(255, 100, 0);

		else if (White.containspoint(x, y) && select == 0)
			color = WHITE;

		else if (E.containspoint(x, y)) {

			if (ismouseclick(WM_LBUTTONDOWN)) {
				getmouseclick(WM_LBUTTONDOWN, x, y);

				cout << "yay";
				for (int i = 0; i < c; i++) {
					if (allShapes[i]->containspoint(x, y)) {

						Shape** newarr = new Shape*[c - 1];
						for (int j = 0; j < i; j++)
							newarr[j] = allShapes[j];

						for (int j = i + 1; j < c; j++)
							newarr[j - 1] = allShapes[j];

						delete[] allShapes[i];
						delete[] allShapes;
						allShapes = newarr;
						c--;
						size--;
						break;
					}
				}
				cleardevice();
				PrintScreen();
				cout << "C" << c << endl;
				for (int i = 0; i < c; i++) {

					allShapes[i]->draw();
				}
			}
		}
	}

	//system("pause");

	return 0;
}
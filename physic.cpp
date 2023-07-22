#include<graphics.h>
#include<windows.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<cmath>
//#include<vector>
//#include<EasyX.h>

#define high 800//画布的高度
#define width 1200//画布的宽度
#define ECOLLIDE 0.02//碰撞恢复系数
#define EFRICTION 0.38//动摩擦因数
#define G 0.5//重力加速度
#define GND 7.0;
const double WHEELR= 29.0;

IMAGE gmod;//图标
IMAGE facebook;
IMAGE lenovo;
IMAGE andriod;
IMAGE zfb;
IMAGE ae;
IMAGE app;
IMAGE bluecircle;
IMAGE bluecircle2;
IMAGE laser;
IMAGE hand;
IMAGE body;
IMAGE shield;
IMAGE bullet1;
IMAGE bullet2;
IMAGE bullet3;
IMAGE bluestar;
IMAGE bluetick;
IMAGE earth;
IMAGE garbage;
IMAGE greencircle;
IMAGE greencircle2;
IMAGE greentick;
IMAGE txhy;
IMAGE warning;



IMAGE background;
IMAGE black;
IMAGE blue;
IMAGE car;
IMAGE rcar;
IMAGE carz;
IMAGE rcarz;
IMAGE imgwheel;
IMAGE rwheel;
IMAGE imgwheelz;
IMAGE rwheelz;
IMAGE imgwheel2;
IMAGE rwheel2;
IMAGE imgwheel2z;
IMAGE rwheel2z;
IMAGE myhead;
IMAGE rmyhead;
IMAGE myheadt;
IMAGE rmyheadt;
IMAGE fuel;
IMAGE fuelt;
MOUSEMSG m;

struct vector {
	double x;
	double y;
};

struct circles {
	double xg, yg, st;//质心坐标,角度
	double r;
	vector v;//xy速度
	vector a;//xy加速度
	double wv, wa;//角速度，角加速度
	vector f;//合力
	double fm;//合力矩,力矩为正时，逆时针旋转
	double m;//质量
	double i;//转动惯量

	double collidewv;
	vector collidev;//这两项是碰撞判定后在phyupd中更新的量，任何时候当一个collide开始的时候，都应该先把这两项重置为零
};

struct dbx {//多边形   定义绝对坐标时一定要绕质心顺时针设定，否则将产生严重的bug!!!
	double xg, yg,st;//质心坐标,角度
	int dotnum;//该多边形有几个边节点
	double xcon[20], ycon[20];//边节点的xy坐标(相对质心的参考系)，一个多边形最多存在20个边节点,这20个节点的值在模拟中不能做修改，否则将改变碰撞体积的形状
	//注：假如想让一个点在质心的右上方（20,70）处，则在初始化时，
	double xact[20], yact[20];//边节点在模拟中的实际坐标
	vector v;//xy速度
	vector a;//xy加速度
	double wv, wa;//角速度，角加速度
	vector f;//合力
	double fm;//合力矩,力矩为正时，逆时针旋转
	double m;//质量
	double i;//转动惯量

	double collidewv;
	vector collidev;//这两项是碰撞判定后在phyupd中更新的量，任何时候当一个collide开始的时候，都应该先把这两项重置为零
	double havedonesep;//怎么说呢，不好说，草拟吗的傻逼东西，
};

struct camera {
	double x, y;
};

struct ground {//铺设宽度为2000
	double x[500];//一个ground最多拥有500个节点
	double y[500];
};

//struct car {
//	circles backwheel;
//	circles frontwheel;
//	dbx carbody;
//};

dbx ply;
dbx rec;
dbx dimian;
dbx temp;
dbx temp2;
dbx fuelblock;
circles wheel;
circles wheel2;
circles head;
camera camera;
ground gnd;
double pregnd[500];

int preleft=0;
vector rp;
int test;
double fueltank = 100;
double fuelx, fuely;



//判断两条线段是否相交的函数（好像最后并没有用这个函数来的）
int cross(double Ax1, double Ay1, double Ax2, double Ay2, double Bx1, double By1, double Bx2, double By2)
{
	if (
		(max(Ax1, Ax2) >= min(Bx1, Bx2) && min(Ax1, Ax2) <= max(Bx1, Bx2)) &&  //判断x轴投影
		(max(Ay1, Ay2) >= min(By1, By2) && min(Ay1, Ay2) <= max(By1, By2))    //判断y轴投影
		)
	{
		if (
			((Bx1 - Ax1) * (Ay2 - Ay1) - (By1 - Ay1) * (Ax2 - Ax1)) *          //判断B是否跨过A
			((Bx2 - Ax1) * (Ay2 - Ay1) - (By2 - Ay1) * (Ax2 - Ax1)) <= 0 &&
			((Ax1 - Bx1) * (By2 - By1) - (Ay1 - By1) * (Bx2 - Bx1)) *          //判断A是否跨过B
			((Ax2 - Bx1) * (By2 - By1) - (Ay2 - By1) * (Bx2 - Bx1)) <= 0
			)
		{
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

//地形生成
void perlinnoise(double a[500]) {//制造地形缓冲
	int i, j, k,l,p;
	k = 2;
	double b[500];
	while (k * 2 - 1 <= 150) {
		j = 0;
		for (i = 0;i <= k-1;i++) {
			b[j] = a[i];	
			j++;
			b[j] = a[i]+rand()%50*(a[i + 1] - a[i])/100 + 25 * (a[i + 1] - a[i]) / 100;
			j++;
		}
		k = k * 2 - 1;
		for (l = 0;l <= k - 1;l++) {
			a[l] = b[l];
		}
	}
	
}

//以下是有关向量运算的函数.......
vector setvector(double a, double b) {
	vector pl;
	pl.x = a;
	pl.y = b;
	return pl;
}

vector fillvector(double x1,double y1,double x2, double y2) {//创建从1指向2的向量
	vector pl;
	pl.x = x2-x1;
	pl.y = y2-y1;
	return pl;
}

vector muti(vector a, double r) {//向量数乘
	vector pl;
	pl.x = a.x * r;
	pl.y = a.y * r;
	return pl;
}

vector plus(vector a, vector b) {//两向量相加
	vector pl;
	pl.x = a.x + b.x;
	pl.y = a.y + b.y;
	return pl;
}

vector normalize(vector a) {//将一个向量单位化
	vector pl;
	pl.x = a.x / sqrt(a.x * a.x + a.y * a.y);
	pl.y = a.y / sqrt(a.x * a.x + a.y * a.y);
	return pl;
}

double dmuti(vector a,vector b) {//两向量点乘
	return a.x * b.x + a.y * b.y;
}

double cmuti(vector a, vector b) {//两向量a叉乘b,由于z轴无意义所以返回double
	return a.y * b.x - a.x * b.y;
}

vector vertical(vector a) {//求与a垂直的单位向量，方向为a逆时针旋转90度
	vector pl;
	pl.x = a.y;
	pl.y = -1 * a.x;
	pl = normalize(pl);
	return pl;
}
//以上是有关向量运算的函数.......

//以下是有关地面更新的函数.......
ground groundinit(ground gnd) {//初始化
	int i;
	for (i = 0;i <= 499;i++) {
		gnd.x[i] = i*GND;
		gnd.y[i] = 800;
	}
	return gnd;
}

ground groundupdate(ground gnd,dbx ply) {//当玩家的x坐标大于gnd.x[250]时重复执行该函数直到玩家坐标小于它
	int i;
	
	if (preleft == 90) {
		preleft = 0;
		pregnd[0] = gnd.y[499];
		pregnd[1] = 300+rand() % 1000;
		perlinnoise(pregnd);
	}
	
	
	while (gnd.x[250] <= ply.xg) {
		if (preleft == 90) {
			preleft = 0;
			pregnd[0] = gnd.y[499];
			pregnd[1] = 300 + rand() % 1000;
			perlinnoise(pregnd);
		}
		

		for (i = 0;i <= 498;i++) {
			gnd.x[i] = gnd.x[i] + GND;
			gnd.y[i] = gnd.y[i + 1];
		}
		gnd.x[499] = gnd.x[499] + GND;
		gnd.y[499] = pregnd[preleft];
		preleft++;
	}
	
	return gnd;
}


//以下是用来绘制刚体碰撞体积与地形的函数
void drawdbx(dbx a) {
	int i;
	for (i = 0;i <= a.dotnum - 2;i++) {
		line(a.xact[i] - camera.x, a.yact[i] - camera.y, a.xact[i + 1] - camera.x, a.yact[i + 1] - camera.y);
	}
	line(a.xact[a.dotnum - 1] - camera.x, a.yact[a.dotnum - 1] - camera.y, a.xact[0] - camera.x, a.yact[0] - camera.y);
}

void drawcircles(circles a) {
	circle(a.xg-camera.x, a.yg-camera.y, a.r);
}

void drawground(ground gnd){
	setlinecolor(GREEN);
	setfillcolor(RGB(110,50,10));
	setlinestyle(PS_SOLID, 12);
	int i;
	for (i = 0;i <= 498;i++) {
		
		bar(gnd.x[i] - camera.x, gnd.y[i] - camera.y, gnd.x[i] + 7 - camera.x, 1600 - camera.y);
	}
	for (i = 0;i <= 498;i++) {
		line(gnd.x[i] - camera.x, gnd.y[i] - camera.y, gnd.x[i + 1] - camera.x, gnd.y[i + 1] - camera.y);
		
	}
	setfillcolor(RGB(100,40,10));
	for (i = 0;i <= 498;i++) {

		bar(gnd.x[i] - camera.x, gnd.y[i]+250 - camera.y, gnd.x[i] + 7 - camera.x, 1600 - camera.y);
	}
	setlinecolor(YELLOW);
	setlinestyle(PS_SOLID, 3);
}


//显示函数
void show() {
	TCHAR p[20];
	putimage(0,0, 1250, 850, &black, 0, 0);
	putimage(0, 0, 1250, 850, &blue, 0, 0);
	putimage(0-camera.x, 400-camera.y, &background);
	
	


	drawground(gnd);
	drawdbx(temp);
	
	if (fuelblock.xg - camera.x >= 1200) {
		putimage(1100, fuelblock.yg - 50 - camera.y, 100, 100, &fuelt, 0, 0, NOTSRCERASE);
		putimage(1100, fuelblock.yg - 50 - camera.y, 100, 100, &fuel, 0, 0, SRCINVERT);
		settextstyle(40, 0, _T("Arial Black"));
		settextcolor(0);
		_stprintf_s(p, _T("%.0f m"), (fuelblock.xg - ply.xg) / 60);
		outtextxy(1098, fuelblock.yg + 50 - camera.y + 2, p);
		settextcolor(RGB(220, 220, 220));
		outtextxy(1100, fuelblock.yg + 50 - camera.y, p);
		settextstyle(80, 0, _T("Arial Black"));
	}
	else {
		putimage(fuelblock.xg - 50 - camera.x, fuelblock.yg - 50 - camera.y, 100, 100, &fuelt, 0, 0, NOTSRCERASE);
		putimage(fuelblock.xg - 50 - camera.x, fuelblock.yg - 50 - camera.y, 100, 100, &fuel, 0, 0, SRCINVERT);
		settextstyle(40, 0, _T("Arial Black"));
		settextcolor(0);
		_stprintf_s(p, _T("%.0f m"), abs((fuelblock.xg - ply.xg) / 60));
		outtextxy(fuelblock.xg - 50 - camera.x-2, fuelblock.yg + 50 - camera.y + 2, p);
		settextcolor(RGB(220, 220, 220));
		outtextxy(fuelblock.xg - 50 - camera.x, fuelblock.yg + 50 - camera.y, p);
		settextstyle(80, 0, _T("Arial Black"));
	}

	putimage(fuelblock.xg - 50 - camera.x, fuelblock.yg - 50 - camera.y, 100, 100, &fuelt, 0, 0, NOTSRCERASE);
	putimage(fuelblock.xg - 50 - camera.x, fuelblock.yg - 50 - camera.y, 100, 100, &fuel, 0, 0, SRCINVERT);
	
	
	setfillcolor(RGB(100, 100, 100));
	bar(dimian.xg - 150-camera.x, dimian.yg - 300-camera.y, dimian.xg + 150 - camera.x, dimian.yg + 300 - camera.y);
	settextstyle(40, 0, _T("华文琥珀"));
	outtextxy(dimian.xg-70 - camera.x, dimian.yg - camera.y, _T("按W开始"));
	settextstyle(80, 0, _T("Arial Black"));
	bar(rec.xg - 30 - camera.x, rec.yg - 2000 - camera.y, rec.xg + 30 - camera.x, rec.yg + 2000 - camera.y);
	drawdbx(dimian);
	drawdbx(rec);

	rotateimage(&rcar, &car, ply.st,WHITE);
	rotateimage(&rcarz, &carz, ply.st,BLACK);
	putimage(ply.xg - 150 - camera.x, ply.yg - 150 - camera.y,  &rcarz,  NOTSRCERASE);
	putimage(ply.xg - 150 - camera.x, ply.yg - 150 - camera.y,  &rcar,  SRCINVERT);

	rotateimage(&rwheel, &imgwheel, wheel.st,WHITE);
	rotateimage(&rwheelz, &imgwheelz, wheel.st, BLACK);
	putimage(wheel.xg - WHEELR - camera.x, wheel.yg - WHEELR - camera.y, 2 * WHEELR, 2 * WHEELR, &rwheelz, 0, 0,NOTSRCERASE);
	putimage(wheel.xg - WHEELR - camera.x, wheel.yg - WHEELR - camera.y, 2 * WHEELR, 2 * WHEELR, &rwheel, 0, 0, SRCINVERT);
	rotateimage(&rwheel2, &imgwheel2, wheel2.st, WHITE);
	rotateimage(&rwheelz, &imgwheelz, wheel2.st, BLACK);
	putimage(wheel2.xg - WHEELR - camera.x, wheel2.yg - WHEELR - camera.y, 2 * WHEELR, 2 * WHEELR, &rwheelz, 0, 0, NOTSRCERASE);
	putimage(wheel2.xg - WHEELR - camera.x, wheel2.yg - WHEELR - camera.y, 2 * WHEELR, 2 * WHEELR, &rwheel2, 0, 0, SRCINVERT);
	rotateimage(&rmyhead, &myhead, head.st, WHITE);
	rotateimage(&rmyheadt, &myheadt, head.st, BLACK);
	putimage(head.xg - 28 - camera.x, head.yg - 28 - camera.y, 56, 56, &rmyheadt, 0, 0, NOTSRCERASE);
	putimage(head.xg - 28 - camera.x, head.yg - 28 - camera.y, 56, 56, &rmyhead, 0, 0, SRCINVERT);
	
	
	settextcolor(0);
	_stprintf_s(p, _T("%.0f m"),ply.xg/60);
	outtextxy(46,34, p);
	
	settextcolor(RGB(220, 220, 220));
	outtextxy(50, 30, p);

	settextstyle(40, 0, _T("Arial Black"));
	settextcolor(0);
	outtextxy(43, 9, _T("DISTANCE"));
	settextcolor(RGB(220, 220, 220));
	outtextxy(46, 6, _T("DISTANCE"));
	settextstyle(80, 0, _T("Arial Black"));

	setfillcolor(BLACK);
	bar(42, 116, 210, 144);
	setfillcolor(RGB(255-(fueltank*1.35), 255 * sqrt(fueltank / 100), 0));
	bar(46, 120, 46+1.6*fueltank, 140);
	if (fueltank < 20) {
		settextcolor(RGB(245, 20, 20));
		settextstyle(40, 0, _T("Arial Black"));
		_stprintf_s(p, _T("LOW FUEL"));
		outtextxy(46, 140, p);
		settextcolor(RGB(220, 220, 220));
		settextstyle(80, 0, _T("Arial Black"));
	}
	/*_stprintf_s(p, _T("%f"),ply.yg);
	outtextxy(10, 50, p);
	
	outtextxy(10, 90, p);
	_stprintf_s(p, _T("%f"),wheel.wv);
	outtextxy(10, 130, p);
	_stprintf_s(p, _T("%f"), ply.v.y);
	outtextxy(10, 150, p);*/

	FlushBatchDraw();
	Sleep(1);
	Sleep(1);
}
//图片读取
void load() {
	loadimage(&background, _T("./images/background.jpg"), 1000, 400, true);
	loadimage(&black, _T("./images/black.jpg"), 1250, 850, true);
	loadimage(&facebook, _T("./images/facebook.jpg"), 50, 50, true);
	loadimage(&blue, _T("./images/blue1.jpg"), 1250, 850, true);
	loadimage(&gmod, _T("./images/gmod.jpg"), 50, 50, true);
	loadimage(&car, _T("./images/car.jpg"), 300, 300, true);
	loadimage(&carz, _T("./images/carz.jpg"), 300, 300, true);
	loadimage(&imgwheel, _T("./images/wheel.jpg"), 2*WHEELR, 2 * WHEELR, true);
	loadimage(&imgwheel2, _T("./images/wheel.jpg"), 2 * WHEELR, 2 * WHEELR, true);
	loadimage(&imgwheelz, _T("./images/wheelz.jpg"), 2 * WHEELR, 2 * WHEELR, true);
	loadimage(&myhead, _T("./images/myhead.jpg"), 56, 56, true);
	loadimage(&myheadt, _T("./images/myheadt.jpg"), 56, 56, true);
	loadimage(&fuel, _T("./images/fuel.jpg"), 100, 100, true);
	loadimage(&fuelt, _T("./images/fuelt.jpg"), 100, 100, true);
}



dbx initxyact(dbx a) {//根据dbx的质心坐标与边节点绝对坐标，将边节点的实际坐标进行初始化至角度st=0的位置
	int i;
	for (i = 0;i <= a.dotnum - 1;i++) {
		a.xact[i] = a.xg + a.xcon[i];
		a.yact[i] = a.yg + a.ycon[i];
	}

	return a;
}

dbx dbxrotate(dbx ply) {//利用相对质心坐标系，质心坐标与当前角度，计算出边节点的实际坐标
	int i;
	for (i = 0;i <= ply.dotnum - 1;i++) {
		ply.xact[i] = (ply.xcon[i]) * cos(-ply.st) - (ply.ycon[i]) * sin(-ply.st)+ ply.xg;
		ply.yact[i] = (ply.xcon[i]) * sin(-ply.st) + (ply.ycon[i]) * cos(-ply.st)+ ply.yg;
	}
	return ply;
}

void camerachange(double x,double y) {//相机对准哪个对象，其坐标就是对象坐标-600-400，在show绘图运算中，所有坐标都要减去
	camera.x = x-600;
	camera.y = y-400;
}

void smocamerachange(double x,double y) {//相机对准哪个对象，其坐标就是对象坐标-600-400，在show绘图运算中，所有坐标都要减去
	camera.x = camera.x+0.2*(x-600-camera.x);
	camera.y = camera.y + 0.2 * (y - 400 - camera.y);
}



//以下是碰撞判定与赋值函数

void sepaxis(dbx *a, dbx *b) {//WOW WOW WOW 一个非常成功的碰撞解决函数就此诞生！！！！！
	//此函数用于两个活动刚体的碰撞赋值，同时改变两个dbx
	vector ra;//a的质心到a的当前研究边节点的向量
	ra = setvector(0, 0);
	vector rafa;//ra的法向量
	vector rb;//b的质心到a的当前研究边节点的向量
	vector rbfa;//rb的法向量
	vector line;//当前研究边向量
	vector n;//b的当前研究边向量的单位法向量
	vector r;//b的当前研究边向量的单位方向向量
	vector s;//从b的研究边起始点指向a的当前研究边节点的向量
	vector dotspeeda;//a的当前研究边节点对于a质心的速度向量
	vector dotspeedb;//a的当前研究边节点的投影对于b质心的速度向量
	vector temp1;
	vector temp2;
	vector temp3;
	vector templine;vector templiner;vector tempn;double tex=0;double tey=0; double distance = 10000;int type=0;//type=0是a，1是b
	double maxa=-10000, mina=10000, maxb=-10000, minb=10000;
	double side;
	vector temp4;
	vector gline;//质心连线向量

	temp3 = setvector(0, 0);
	tempn = setvector(0, 0);
	templiner = setvector(0, 0);
	templine = setvector(0, 0);
	
	double vr;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j, k=0, l=0;//i是a边节点个数计数，jk是b边向量个数计数,havedone用于判断是否执行anticollide，havedone=1时不执行
	double averagex, averagey, averagevr = 0;

	for (i = 0;i <= a->dotnum - 1;i++) {//a(i)
		maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
		if (i == a->dotnum - 1) {
			line = fillvector(a->xact[i], a->yact[i], a->xact[0], a->yact[0]);
		}
		else {
			line = fillvector(a->xact[i], a->yact[i], a->xact[i + 1], a->yact[i + 1]);
		}

		//求出该a边向量的单位方向向量和单位法向量
		r = normalize(line);
		n = vertical(line);
		
		for (j = 0;j <= a->dotnum - 1;j++) {//将a的所有点投影到该法向量,并计算最大最小值
			temp4 = fillvector(a->xact[i], a->yact[i], a->xact[j], a->yact[j]);
			if (dmuti(temp4, n) >= maxa) {
				maxa = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= mina) {
				mina = dmuti(temp4, n);
			}
		}

		for (j = 0;j <= b->dotnum - 1;j++) {//将b的所有点投影到a法向量,并计算最大最小值
			temp4 = fillvector(a->xact[i], a->yact[i], b->xact[j], b->yact[j]);
			if (dmuti(temp4, n) >= maxb) {
				maxb = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= minb) {
				minb = dmuti(temp4, n);
			}
		}

		//接下来判断ab最值是否相交，如果否，立刻终止程序
		if (maxa <= minb or maxb <= mina) {
			return ;
		}
		//计算重叠距离,标记当前边
		gline = fillvector(a->xg, a->yg, b->xg, b->yg);
		if (min(abs(maxb - mina), abs(maxa - minb)) <= distance and dmuti(gline,n)>=0) {
			distance = min(abs(maxb - mina), abs(maxa - minb));
			templine = line;
			templiner = r;
			tempn = n;
			tex = a->xact[i];
			tey = a->yact[i];
			type = 0;
		}
	}

	
	//将上述操作翻转，b边对a判断
	for (i = 0;i <= b->dotnum - 1;i++) {//b(i)
		maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
		if (i == b->dotnum - 1) {
			line = fillvector(b->xact[i], b->yact[i], b->xact[0], b->yact[0]);
		}
		else {
			line = fillvector(b->xact[i], b->yact[i], b->xact[i + 1], b->yact[i + 1]);
		}

		//求出该b边向量的单位方向向量和单位法向量
		r = normalize(line);
		n = vertical(line);

		for (j = 0;j <= b->dotnum - 1;j++) {//将b的所有点投影到该法向量,并计算最大最小值
			temp4 = fillvector(b->xact[i], b->yact[i], b->xact[j], b->yact[j]);
			if (dmuti(temp4, n) >= maxb) {
				maxb = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= minb) {
				minb = dmuti(temp4, n);
			}
		}

		for (j = 0;j <= a->dotnum - 1;j++) {//将a的所有点投影到b法向量,并计算最大最小值
			temp4 = fillvector(b->xact[i], b->yact[i], a->xact[j], a->yact[j]);
			if (dmuti(temp4, n) >= maxa) {
				maxa = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= mina) {
				mina = dmuti(temp4, n);
			}
		}

		//接下来判断ab最值是否相交，如果否，立刻终止程序
		if (maxa <= minb or maxb <= mina) {
			return ;
		}
		//计算重叠距离,标记当前边
		gline = fillvector(b->xg, b->yg, a->xg, a->yg);
		if (min(abs(maxb - mina), abs(maxa - minb)) <= distance and dmuti(gline, n) >= 0) {
			//存储所有需要的信息
			distance = min(abs(maxb - mina), abs(maxa - minb));
			templine = line;
			templiner = r;
			tempn = n;
			tex = b->xact[i];
			tey = b->yact[i];
			type = 1;
		}
	}

	//至此，我们确定了碰撞边及其法向量，以及沿着该法向量的跨越距离，之后进行a和b的参数更新即可

	
	if (type == 0) {//说明是a的边，此时遍历一遍b的边节点，a做出参数更新
		for (i = 0;i <= b->dotnum - 1;i++) {
			s = fillvector(tex, tey, b->xact[i], b->yact[i]);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始参数更新
					ra = fillvector(a->xg, a->yg, b->xact[i], b->yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a->v, muti(rafa, a->wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					rb = fillvector(b->xg, b->yg, b->xact[i], b->yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b->v, muti(rbfa, b->wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(tempn, plus(dotspeeda, muti(dotspeedb, -1)));
					if (vr > 0) {//法向相对速度判断
						k = k + 1;
						temp1 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp1 = muti(temp1, 1 / a->i);
						temp2 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp2 = muti(temp2, 1 / b->i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a->m) + (1 / b->m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小
						
						if (dmuti(plus(b->v, muti(a->v, -1)), templiner) <= 0) {
							side = -1;
						}
						else {
							side = 1;
						}
						a->collidev = plus(a->collidev, muti(tempn, J / a->m));
						a->collidewv = a->collidewv + (cmuti(ra, muti(tempn, J)) / a->i);
						b->collidev = plus(b->collidev, muti(tempn, -J / b->m));
						b->collidewv = b->collidewv + (cmuti(rb, muti(tempn, -J)) / b->i);
					}
				}
			}
		}
		if (k > 0) {
			a->collidev.x = a->collidev.x / k;
			a->collidev.y = a->collidev.y / k;
			a->collidewv = a->collidewv / k;
			b->collidev.x = b->collidev.x / k;
			b->collidev.y = b->collidev.y / k;
			b->collidewv = b->collidewv / k;
		}
	}

	if (type == 1) {//说明是b的边，此时遍历一遍a的边节点，a做出参数更新
		for (i = 0;i <= a->dotnum - 1;i++) {
			s = fillvector(tex, tey, a->xact[i], a->yact[i]);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始参数更新
					rb = fillvector(b->xg, b->yg, a->xact[i], a->yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b->v, muti(rbfa, b->wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					ra = fillvector(a->xg, a->yg, a->xact[i], a->yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a->v, muti(rafa, a->wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					vr = dmuti(tempn, plus(dotspeedb, muti(dotspeeda, -1)));
					if (vr > 0) {//法向相对速度判断
						k = k + 1;
						temp1 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp1 = muti(temp1, 1 / b->i);
						temp2 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp2 = muti(temp2, 1 / a->i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a->m) + (1 / b->m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小
						
						if (dmuti(plus(b->v,muti(a->v,-1)), templiner) <= 0) {
							side = -1;
						}
						else {
							side = 1;
						}

						//a->collidev = plus(a->collidev, plus(muti(tempn, -J / a->m), muti(templiner, -J * EFRICTION * side / a->m)));
						a->collidev = plus(a->collidev, muti(tempn, -J / a->m));
						a->collidewv = a->collidewv + (cmuti(ra, muti(tempn, -J)) / a->i);
						b->collidev = plus(b->collidev, muti(tempn, J / b->m));
						b->collidewv = b->collidewv + (cmuti(rb, muti(tempn, J)) / b->i);

					}
				}
			}
		}
		if (k > 0) {
			a->collidev.x = a->collidev.x / k;
			a->collidev.y = a->collidev.y / k;
			a->collidewv = a->collidewv / k;
			b->collidev.x = b->collidev.x / k;
			b->collidev.y = b->collidev.y / k;
			b->collidewv = b->collidewv / k;
		}

	}

	//最后，在速度更新完毕后沿着分离轴推离两形状
	if (type == 0) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, b->m / (a->m + b->m));
		a->xg = a->xg - temp3.x;
		a->yg = a->yg - temp3.y;
		b->xg = b->xg + temp3.x;
		b->yg = b->yg + temp3.y;
	}
	if (type == 1) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, b->m / (a->m + b->m));
		a->xg = a->xg + temp3.x;
		a->yg = a->yg + temp3.y;
		b->xg = b->xg - temp3.x;
		b->yg = b->yg - temp3.y;
	}
	a->v = plus(a->v, a->collidev);
	a->wv = a->wv + a->collidewv;
	b->v = plus(b->v, b->collidev);
	b->wv = b->wv + b->collidewv;
	a->collidev = setvector(0, 0);
	a->collidewv = 0;
	b->collidev = setvector(0, 0);
	b->collidewv = 0;
	
}

void fixcirtodbx(circles* a, dbx* b, double xcon, double ycon) {
	vector ra,rb,rbfa,temp1,temp2,tempn,tempr;
	vector dotspeeda, dotspeedb;
	double xact, yact,vr,J;
	xact = xcon * cos(-b->st) - ycon * sin(-b->st) + b->xg;
	yact = xcon * sin(-b->st) + ycon * cos(-b->st) + b->yg;
	ra = fillvector(a->xg, a->yg, xact, yact);
	tempn = normalize(ra);
	
	dotspeeda = a->v;
	rb = fillvector(b->xg, b->yg, xact, yact);
	rbfa = vertical(rb);
	dotspeedb = plus(b->v, muti(rbfa, b->wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
	vr=dmuti(tempn,plus(dotspeedb, muti(dotspeeda, -1)));

	temp1 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
	temp1 = muti(temp1, 1 / b->i);
	temp2 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
	temp2 = muti(temp2, 1 / a->i);
	J = (-vr * (1)) / ((1 / a->m) + (1 / b->m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));

	a->collidev = plus(a->collidev, muti(tempn, -J / a->m));
	b->collidev = plus(b->collidev, muti(tempn, J / b->m));
	b->collidewv = b->collidewv + (cmuti(rb, muti(tempn, J)) / b->i);
	a->v = plus(a->v, a->collidev);
	b->v = plus(b->v, b->collidev);
	b->wv = b->wv + b->collidewv;
	a->collidev = setvector(0, 0);
	b->collidev = setvector(0, 0);
	b->collidewv = 0;

	ra = fillvector(a->xg, a->yg, xact, yact);
	ra = muti(ra, 0.4);
	a->xg = a->xg + ra.x;
	a->yg = a->yg + ra.y;
	b->xg = b->xg - ra.x;
	b->yg = b->yg - ra.y;
}

dbx onesepaxis(dbx *a, dbx *b) {//与sepaxis的不同在于它只改变a的参数，当b是一个被固定的刚体时使用该函数
	vector ra;//a的质心到a的当前研究边节点的向量
	ra = setvector(0, 0);
	vector rafa;//ra的法向量
	vector rb;//b的质心到a的当前研究边节点的向量
	vector rbfa;//rb的法向量
	vector line;//当前研究边向量
	vector n;//b的当前研究边向量的单位法向量
	vector r;//b的当前研究边向量的单位方向向量
	vector s;//从b的研究边起始点指向a的当前研究边节点的向量
	vector dotspeeda;//a的当前研究边节点对于a质心的速度向量
	vector dotspeedb;//a的当前研究边节点的投影对于b质心的速度向量
	vector temp1;
	vector temp2;
	vector temp3;
	vector templine;vector templiner;vector tempn;double tex = 0;double tey = 0; double distance = 10000;int type = 0;//type=0是a，1是b
	double maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
	double side;
	vector temp4;
	vector gline;//质心连线向量

	temp3 = setvector(0, 0);
	tempn = setvector(0, 0);
	templiner = setvector(0, 0);
	templine = setvector(0, 0);

	double vr;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j, k = 0, l = 0;//i是a边节点个数计数，jk是b边向量个数计数,havedone用于判断是否执行anticollide，havedone=1时不执行
	double averagex, averagey, averagevr = 0;

	for (i = 0;i <= a->dotnum - 1;i++) {//a(i)
		maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
		if (i == a->dotnum - 1) {
			line = fillvector(a->xact[i], a->yact[i], a->xact[0], a->yact[0]);
		}
		else {
			line = fillvector(a->xact[i], a->yact[i], a->xact[i + 1], a->yact[i + 1]);
		}

		//求出该a边向量的单位方向向量和单位法向量
		r = normalize(line);
		n = vertical(line);

		for (j = 0;j <= a->dotnum - 1;j++) {//将a的所有点投影到该法向量,并计算最大最小值
			temp4 = fillvector(a->xact[i], a->yact[i], a->xact[j], a->yact[j]);
			if (dmuti(temp4, n) >= maxa) {
				maxa = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= mina) {
				mina = dmuti(temp4, n);
			}
		}

		for (j = 0;j <= b->dotnum - 1;j++) {//将b的所有点投影到a法向量,并计算最大最小值
			temp4 = fillvector(a->xact[i], a->yact[i], b->xact[j], b->yact[j]);
			if (dmuti(temp4, n) >= maxb) {
				maxb = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= minb) {
				minb = dmuti(temp4, n);
			}
		}

		//接下来判断ab最值是否相交，如果否，立刻终止程序
		if (maxa < minb or maxb < mina) {
			return *a;
		}
		//计算重叠距离,标记当前边
		gline = fillvector(a->xg, a->yg, b->xg, b->yg);
		if (min(abs(maxb - mina), abs(maxa - minb)) <= distance and dmuti(gline, n) >= 0) {
			distance = min(abs(maxb - mina), abs(maxa - minb));
			templine = line;
			templiner = r;
			tempn = n;
			tex = a->xact[i];
			tey = a->yact[i];
			type = 0;
		}
	}


	//将上述操作翻转，b边对a判断
	for (i = 0;i <= b->dotnum - 1;i++) {//b(i)
		maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
		if (i == b->dotnum - 1) {
			line = fillvector(b->xact[i], b->yact[i], b->xact[0], b->yact[0]);
		}
		else {
			line = fillvector(b->xact[i], b->yact[i], b->xact[i + 1], b->yact[i + 1]);
		}

		//求出该b边向量的单位方向向量和单位法向量
		r = normalize(line);
		n = vertical(line);

		for (j = 0;j <= b->dotnum - 1;j++) {//将b的所有点投影到该法向量,并计算最大最小值
			temp4 = fillvector(b->xact[i], b->yact[i], b->xact[j], b->yact[j]);
			if (dmuti(temp4, n) >= maxb) {
				maxb = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= minb) {
				minb = dmuti(temp4, n);
			}
		}

		for (j = 0;j <= a->dotnum - 1;j++) {//将a的所有点投影到b法向量,并计算最大最小值
			temp4 = fillvector(b->xact[i], b->yact[i], a->xact[j], a->yact[j]);
			if (dmuti(temp4, n) >= maxa) {
				maxa = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= mina) {
				mina = dmuti(temp4, n);
			}
		}

		//接下来判断ab最值是否相交，如果否，立刻终止程序
		if (maxa < minb or maxb < mina) {
			return *a;
		}
		//计算重叠距离,标记当前边
		gline = fillvector(b->xg, b->yg, a->xg, a->yg);
		if (min(abs(maxb - mina), abs(maxa - minb)) <= distance and dmuti(gline, n) >= 0) {
			//存储所有需要的信息
			distance = min(abs(maxb - mina), abs(maxa - minb));
			templine = line;
			templiner = r;
			tempn = n;
			tex = b->xact[i];
			tey = b->yact[i];
			type = 1;
		}
	}

	//至此，我们确定了碰撞边及其法向量，以及沿着该法向量的跨越距离，之后进行a和b的参数更新即可


	if (type == 0) {//说明是a的边，此时遍历一遍b的边节点，a做出参数更新
		for (i = 0;i <= b->dotnum - 1;i++) {
			s = fillvector(tex, tey, b->xact[i], b->yact[i]);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始参数更新
					ra = fillvector(a->xg, a->yg, b->xact[i], b->yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a->v, muti(rafa, a->wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					rb = fillvector(b->xg, b->yg, b->xact[i], b->yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b->v, muti(rbfa, b->wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(tempn, plus(dotspeeda, muti(dotspeedb, -1)));
					if (vr > 0) {//法向相对速度判断
						k = k + 1;
						temp1 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp1 = muti(temp1, 1 / a->i);
						temp2 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp2 = muti(temp2, 1 / b->i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a->m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小

						if (dmuti(plus(b->v, muti(a->v, -1)), templiner) <= 0) {
							side = -1;
						}
						else {
							side = 1;
						}
						a->collidev = plus(a->collidev, muti(tempn, J / a->m));
						a->collidewv = a->collidewv + (cmuti(ra, muti(tempn, J)) / a->i);
						
					}
				}
			}
		}
		if (k > 0) {
			a->collidev.x = a->collidev.x / k;
			a->collidev.y = a->collidev.y / k;
			a->collidewv = a->collidewv / k;
			
		}
	}

	if (type == 1) {//说明是b的边，此时遍历一遍a的边节点，a做出参数更新
		for (i = 0;i <= a->dotnum - 1;i++) {
			s = fillvector(tex, tey, a->xact[i], a->yact[i]);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始参数更新
					rb = fillvector(b->xg, b->yg, a->xact[i], a->yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b->v, muti(rbfa, b->wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					ra = fillvector(a->xg, a->yg, a->xact[i], a->yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a->v, muti(rafa, a->wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					vr = dmuti(tempn, plus(dotspeedb, muti(dotspeeda, -1)));
					if (vr > 0) {//法向相对速度判断
						k = k + 1;
						temp1 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp1 = muti(temp1, 1 / b->i);
						temp2 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp2 = muti(temp2, 1 / a->i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a->m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小

						if (dmuti(plus(b->v, muti(a->v, -1)), templiner) <= 0) {
							side = -1;
						}
						else {
							side = 1;
						}

						//a->collidev = plus(a->collidev, plus(muti(tempn, -J / a->m), muti(templiner, -J * EFRICTION * side / a->m)));
						a->collidev = plus(a->collidev, muti(tempn, -J / a->m));
						a->collidewv = a->collidewv + (cmuti(ra, muti(tempn, -J)) / a->i);
					}
				}
			}
		}
		if (k > 0) {
			a->collidev.x = a->collidev.x / k;
			a->collidev.y = a->collidev.y / k;
			a->collidewv = a->collidewv / k;
		}

	}

	//最后，在速度更新完毕后沿着分离轴推离两形状
	if (type == 0) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, 1);
		a->xg = a->xg - temp3.x;
		a->yg = a->yg - temp3.y;
		//a->havedonesep = 1;
	}
	if (type == 1) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, 1);
		a->xg = a->xg + temp3.x;
		a->yg = a->yg + temp3.y;
		//a->havedonesep = 1;
	}
	a->v = plus(a->v, a->collidev);
	a->wv = a->wv + a->collidewv;
	
	a->collidev = setvector(0, 0);
	a->collidewv = 0;
	return *a;
}

circles sepcircle(circles* a, dbx* b) {
	vector ra;//b的当前研究边节点--a的质心的向量
	ra = setvector(0, 0);
	vector rafa;//ra的法向量
	vector rb;//b的质心到a的当前研究边节点的向量
	vector rbfa;//rb的法向量
	vector line;//当前研究边向量
	vector n;//b的当前研究边向量的单位法向量
	vector r;//b的当前研究边向量的单位方向向量
	vector s;//从b的研究边起始点指向a的当前研究边节点的向量
	vector dotspeeda;//a的当前研究边节点对于a质心的速度向量
	vector dotspeedb;//a的当前研究边节点的投影对于b质心的速度向量
	vector temp1;
	vector temp2;
	vector temp3;
	vector templine; vector templiner; vector tempn; double tex = 0; double tey = 0; double distance = 10000; int type = 0;//type=0是a，1是b
	double maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
	double side;
	vector temp4;
	vector temp99;
	vector gline;//质心连线向量

	temp3 = setvector(0, 0);
	tempn = setvector(0, 0);
	templiner = setvector(0, 0);
	templine = setvector(0, 0);

	double vr,vt;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j, k = 0, l = 0;//i是a边节点个数计数，jk是b边向量个数计数,havedone用于判断是否执行anticollide，havedone=1时不执行
	double averagex, averagey, averagevr = 0;
	double xact, yact;

	


	//将上述操作翻转，b边对a判断
	for (i = 0; i <= b->dotnum - 1; i++) {//b(i)
		maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
		if (i == b->dotnum - 1) {
			line = fillvector(b->xact[i], b->yact[i], b->xact[0], b->yact[0]);
		}
		else {
			line = fillvector(b->xact[i], b->yact[i], b->xact[i + 1], b->yact[i + 1]);
		}

		//求出该b边向量的单位方向向量和单位法向量
		r = normalize(line);
		n = vertical(line);

		for (j = 0; j <= b->dotnum - 1; j++) {//将b的所有点投影到该法向量,并计算最大最小值
			temp4 = fillvector(b->xact[i], b->yact[i], b->xact[j], b->yact[j]);
			if (dmuti(temp4, n) >= maxb) {
				maxb = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= minb) {
				minb = dmuti(temp4, n);
			}
		}

		//将a的所有点投影到b法向量,并计算最大最小值
		temp4 = fillvector(b->xact[i], b->yact[i], a->xg, a->yg);
			
		maxa = dmuti(temp4, n)+a->r;
			
		mina = dmuti(temp4, n)-a->r;
			
		

		//接下来判断ab最值是否相交，如果否，立刻终止程序
		if (maxa < minb or maxb < mina) {
			return *a;
		}
		//计算重叠距离,标记当前边
		gline = fillvector(b->xg, b->yg, a->xg, a->yg);
		ra = fillvector( b->xact[i], b->yact[i], a->xg, a->yg);
		if (min(abs(maxb - mina), abs(maxa - minb)) <= distance and dmuti(gline, n) >= 0 and dmuti(ra,r)>=0 and dmuti(ra, r)<=sqrt(line.x * line.x+ line.y * line.y)) {
			//存储所有需要的信息
			distance = min(abs(maxb - mina), abs(maxa - minb));
			templine = line;
			templiner = r;
			tempn = n;
			if (i == 1 and b->dotnum == 2) {
				tempn = muti(tempn, -1);
			}
			tex = b->xact[i];
			tey = b->yact[i];
			type = 1;
		}
	}

	//至此，我们确定了碰撞边及其法向量，以及沿着该法向量的跨越距离，之后进行a和b的参数更新即可


	if (type == 0) {//说明a在边界，此时遍历一遍b的边节点，a做出参数更新
		for (i = 0; i <= b->dotnum - 1; i++) {
			s = fillvector(tex, tey, b->xact[i], b->yact[i]);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始参数更新
					ra = fillvector(a->xg, a->yg, b->xact[i], b->yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a->v, muti(rafa, a->wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					rb = fillvector(b->xg, b->yg, b->xact[i], b->yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b->v, muti(rbfa, b->wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(tempn, plus(dotspeeda, muti(dotspeedb, -1)));
					if (vr > 0) {//法向相对速度判断
						k = k + 1;
						temp1 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp1 = muti(temp1, 1 / a->i);
						temp2 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp2 = muti(temp2, 1 / b->i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a->m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小

						if (dmuti(plus(b->v, muti(a->v, -1)), templiner) <= 0) {
							side = -1;
						}
						else {
							side = 1;
						}
						a->collidev = plus(a->collidev, muti(tempn, J / a->m));
						a->collidewv = a->collidewv + (cmuti(ra, muti(tempn, J)) / a->i);
						//a->collidev = plus(a->collidev, plus(muti(tempn, -J / a->m), muti(templiner, -J * EFRICTION * side / a->m)));
					}
				}
			}
		}
		if (k > 0) {
			a->collidev.x = a->collidev.x / k;
			a->collidev.y = a->collidev.y / k;
			a->collidewv = a->collidewv / k;

		}
	}

	if (type == 1) {//说明是b的边，此时a的碰撞点只有一个，为切点，a做出参数更新
		

			
			temp99 = muti(tempn, -a->r);
			xact = a->xg + temp99.x;
			yact = a->yg + temp99.y;
			s = fillvector(tex, tey, xact, yact);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始参数更新
					rb = fillvector(b->xg, b->yg, xact, yact);
					rbfa = vertical(rb);
					dotspeedb = plus(b->v, muti(rbfa, b->wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					ra = fillvector(a->xg, a->yg, xact, yact);
					rafa = vertical(ra);
					dotspeeda = plus(a->v, muti(rafa, a->wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					vr = dmuti(tempn, plus(dotspeedb, muti(dotspeeda, -1)));
					if (vr > 0) {//法向相对速度判断
						
						temp1 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp1 = muti(temp1, 1 / b->i);
						temp2 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp2 = muti(temp2, 1 / a->i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a->m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小

						

						vt= dmuti(templiner, plus(dotspeedb, muti(dotspeeda, -1)));
						if (vt >= 0) {
							side = 1;
						}
						else {
							side = -1;
						}
						a->collidev = plus(a->collidev, plus(muti(tempn, -J / a->m), muti(templiner, -J * EFRICTION * side * sqrt(sqrt(abs(vt))) / a->m)));
						
						a->collidewv = a->collidewv + (cmuti(ra, plus(muti(tempn, -J), muti(templiner, -J * EFRICTION * side * sqrt(sqrt(abs(vt)))))));
						a->collidewv = a->collidewv / a->i;
						//a->collidev = plus(a->collidev, muti(tempn, -J / a->m));
						//a->collidewv = a->collidewv + (cmuti(ra, muti(tempn, -J)) / a->i);
					}
				}
			}

	}

	//最后，在速度更新完毕后沿着分离轴推离两形状
	if (type == 0) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, 1);
		a->xg = a->xg - temp3.x;
		a->yg = a->yg - temp3.y;
		
	}
	if (type == 1) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, 1);
		a->xg = a->xg + temp3.x;
		a->yg = a->yg + temp3.y;
		
	}
	a->v = plus(a->v, a->collidev);
	a->wv = a->wv + a->collidewv;

	a->collidev = setvector(0, 0);
	a->collidewv = 0;
	return *a;
	
}

dbx fillgndtodbx(dbx a, ground gnd, int i) {
	a.m = 100000000;
	a.i = 100000000;
	a.v = setvector(0, 0);
	a.wv = 0;
	a.collidev= setvector(0, 0);
	a.collidewv = 0;
	a.dotnum = 2;
	a.xcon[0] = 0;
	a.ycon[0] = 0;
	a.xcon[1] = gnd.x[i + 1]-gnd.x[i];
	a.ycon[1] = gnd.y[i + 1]-gnd.y[i];

	a.xg = gnd.x[i];
	a.yg = gnd.y[i];
	a = initxyact(a);
	return a;

}

dbx gndcollide(dbx a, ground gnd, dbx temp) {//只针对ground的分离函数
	
	int i=0;
	temp = fillgndtodbx(temp, gnd, 249);
	a = onesepaxis(&a, &temp);
	a = dbxrotate(a);
	temp = fillgndtodbx(temp, gnd, 250);
	a = onesepaxis(&a, &temp);
	a = dbxrotate(a);
	temp = fillgndtodbx(temp, gnd, 251);
	a = onesepaxis(&a, &temp);
	a = dbxrotate(a);

	if (a.v.x >= 0) {
		for (i = 0;i <350;i++) {
				temp = fillgndtodbx(temp, gnd, i);
				a = onesepaxis(&a, &temp);
				a = dbxrotate(a);
		
		}
	}
	if (a.v.x <= 0) {
		for (i = 350; i >0 ; i--) {
			temp = fillgndtodbx(temp, gnd, i);
			a = onesepaxis(&a, &temp);
			a = dbxrotate(a);

		}
	}
	
	

	return a;
}

circles cirgndcollide(circles a, ground gnd, dbx temp) {
	int i = 0;
	
	
	
	if (a.v.x >= 0) {
		for (i = 0; i < 350; i++) {
			temp = fillgndtodbx(temp, gnd, i);
			a = sepcircle(&a, &temp);
			
		}
	}
	if (a.v.x <= 0) {
		for (i = 350; i > 0; i--) {
			temp = fillgndtodbx(temp, gnd, i);
			a = sepcircle(&a, &temp);
			

		}
	}

	return a;
}

dbx dbxphyupd(dbx ply) {
	ply.fm = 0;
	ply.f = setvector(0, G*ply.m);
	//ply.f = setvector(0, 0);
	ply.a = muti(ply.f, 1 / ply.m);
	ply.wa = ply.fm / ply.i;
	ply.v = plus(ply.v, ply.a);
	ply.wv = ply.wv + ply.wa;
	ply.xg = ply.xg + ply.v.x;
	ply.yg = ply.yg + ply.v.y;
	ply.st = ply.st + ply.wv;//角度更新
	
	ply = dbxrotate(ply);

	//ply.v.x = ply.v.x - 0.02 * ply.v.x;//阻尼测试，在受力模块完善后应该删除掉。
	//ply.v.y = ply.v.y - 0.02 * ply.v.y;//阻尼测试，在受力模块完善后应该删除掉。

	if (ply.yg > 1570) {
		ply.yg = 1570;
		ply.v.y = 0;
	}
	return ply;
}

circles cirphyupd(circles ply) {
	ply.fm = 0;
	ply.f = setvector(0, G * ply.m);
	//ply.f = setvector(0, 0);
	ply.a = muti(ply.f, 1 / ply.m);
	ply.wa = ply.fm / ply.i;
	ply.v = plus(ply.v, ply.a);
	ply.wv = ply.wv + ply.wa;
	ply.xg = ply.xg + ply.v.x;
	ply.yg = ply.yg + ply.v.y;
	ply.st = ply.st + ply.wv;//角度更新

	

	//ply.v.x = ply.v.x - 0.02 * ply.v.x;//阻尼测试，在受力模块完善后应该删除掉。
	//ply.v.y = ply.v.y - 0.02 * ply.v.y;//阻尼测试，在受力模块完善后应该删除掉。

	if (ply.yg > 1570) {
		ply.yg = 1570;
		ply.v.y = 0;
	}
	
	return ply;
}
//以上是碰撞判定与赋值函数


void updwithinput() {

	
	if (GetAsyncKeyState(VK_LEFT)){
		ply.v.x=ply.v.x-0.2;
		ply.wv = ply.wv + 0.007;
	}

	if (GetAsyncKeyState(VK_RIGHT)){
		ply.v.x = ply.v.x + 0.2;
		ply.wv = ply.wv - 0.007;
	}

	if (GetAsyncKeyState(VK_UP)){
		//ply.v.y=ply.v.y-0.2;
		wheel.wv = wheel.wv - 0.06 - 0.018 * wheel.wv;
		wheel2.wv = wheel2.wv - 0.06 - 0.018 * wheel2.wv;
		fueltank = fueltank - 0.01;
	}
	
	if (GetAsyncKeyState(VK_DOWN)){
		//ply.v.y = ply.v.y + 0.2;
		wheel.wv = wheel.wv + 0.06 - 0.018 * wheel.wv;
		wheel2.wv = wheel2.wv + 0.06 - 0.018 * wheel2.wv;
		fueltank = fueltank - 0.01;
	}
	if (GetAsyncKeyState(VK_ESCAPE)) {
		

	}
	if (GetAsyncKeyState('W')) {
		//fueltank ++;
		//drotate(st);
		//st = st + 0.1;
		test = 1;
	}
}

void updwithoutinput() {//without为物理约束模块,在一个dt间的事件
	//sepaxis(&dimian,&rec);
	//sepaxis(&rec, &ply);
	ply=onesepaxis(&ply, &dimian);
	ply=onesepaxis(&ply, &rec);
	
	//dimian = sepaxis(dimian, ply);
	//dimian = collide(dimian, ply);
	//ply = groundcollide(ply, gnd);
	//wheel = sepcircle(&wheel, &ply);
	fixcirtodbx(&wheel, &ply, -52, 27);
	fixcirtodbx(&wheel2, &ply, 52, 27);
	
	wheel = sepcircle(&wheel, &rec);
	wheel2 = sepcircle(&wheel2, &rec);
	wheel = sepcircle(&wheel, &dimian);
	wheel2 = sepcircle(&wheel2, &dimian);
	wheel = cirgndcollide(wheel, gnd, temp2);
	wheel2 = cirgndcollide(wheel2, gnd, temp2);

	
	
	if (fuelblock.v.x == 0 and fuelblock.xg >= gnd.x[0]) {
		fuelblock = onesepaxis(&fuelblock,&ply);
		fuelblock.xg = fuelblock.xg + fuelblock.v.x;
		fuelblock.yg = fuelblock.yg + fuelblock.v.y;
		fuelblock.st = fuelblock.st + fuelblock.wv;
		fuelblock = dbxrotate(fuelblock);
		if (fuelblock.xg <= gnd.x[499] and fuelblock.xg >= gnd.x[490]) {
			fuelblock.yg = gnd.y[499] - 50;
		}
	}
	else {
		if (fuelblock.v.x != 0) {//如果在地形中被吃到了
			fuelblock.xg = fuelblock.xg + 36000;
			if (fuelblock.xg <= gnd.x[499] and fuelblock.xg >= gnd.x[490]) {
				fuelblock.yg = gnd.y[499] - 50;
			}
			
			fuelblock.v = setvector(0, 0);
			fuelblock.wv = 0;
			fuelblock.st = 0;
			fuelblock = dbxrotate(fuelblock);
			fuelx = fuelblock.xg - camera.x;
			fuely = fuelblock.yg - camera.y;
			fueltank = 100;
		}
		else {//如果没吃到
			fuelblock.xg = fuelblock.xg + 36000;
			if (fuelblock.xg <= gnd.x[499] and fuelblock.xg >= gnd.x[490]) {
				fuelblock.yg = gnd.y[499] - 50;
			}
			fuelblock = dbxrotate(fuelblock);
		}
		
	}

	ply=gndcollide(ply, gnd, temp);
	
	wheel = cirphyupd(wheel);
	wheel2 = cirphyupd(wheel2);
	//head = cirphyupd(head);
	ply=dbxphyupd(ply);
	
	//rec = dbxphyupd(rec);
	if (test == 1) {
		dimian = dbxphyupd(dimian);
	}

	head.xg = (0) * cos(-ply.st) - (-40) * sin(-ply.st) + ply.xg;
	head.yg = (0) * sin(-ply.st) + (-40) * cos(-ply.st) + ply.yg;
	head.st = head.st + 0.08*(ply.st - head.st);
	head = cirgndcollide(head, gnd, temp2);
	head = sepcircle(&head, &rec);



	gnd = groundupdate(gnd,ply);
	rec.xg = gnd.x[0];
	rec.yg = gnd.y[0];
	rec = initxyact(rec);

	//Sleep(1);
}

void startup() {
	pregnd[0] = 800;
	pregnd[1] = 500;
	perlinnoise(pregnd);
	gnd = groundinit(gnd);
	//测试用，将rec多边形的形状初始化
	rec.xg = gnd.x[0];
	rec.yg = gnd.y[0];
	rec.dotnum = 4;
	rec.xcon[0] = -30;
	rec.ycon[0] = -2000;
	rec.xcon[1] = 30;
	rec.ycon[1] = -2000;
	rec.xcon[2] = 30;
	rec.ycon[2] = 2000;
	rec.xcon[3] = -30;
	rec.ycon[3] = 2000;
	rec.m = 100;
	rec.i = 8000 * rec.m / 3;
	rec = initxyact(rec);

	dimian.xg = 1000;
	dimian.yg = 550;
	dimian.dotnum = 4;
	dimian.xcon[0] = -150;
	dimian.ycon[0] = -300;
	dimian.xcon[1] = 150;
	dimian.ycon[1] = -300;
	dimian.xcon[2] = 150;
	dimian.ycon[2] = 300;
	dimian.xcon[3] = -150;
	dimian.ycon[3] = 300;
	dimian.v = setvector(0, 0);
	dimian.wv = 0;
	dimian.m = 400;
	dimian.i = dimian.m * 50000 / 3;
	dimian = initxyact(dimian);
	

	ply.xcon[0] = -80;
	ply.ycon[0] = -25;
	ply.xcon[1] = 80;
	ply.ycon[1] = -25;
	ply.xcon[2] = 80;
	ply.ycon[2] = 25;
	ply.xcon[3] = -80;
	ply.ycon[3] = 25;
	ply.xg = 200;
	ply.yg = 600;
	ply.dotnum = 4;
	ply.st = 0.000001;
	ply.v=setvector(0, 0);
	ply.wv = 0;
	ply.m = 20;
	ply.i = ply.m * 2 * ply.xcon[2] * ply.ycon[2] / 3;

	wheel.r = WHEELR;
	wheel.i = 5 * wheel.r * wheel.r;
	wheel.m = 10;
	wheel.st = 0;
	wheel.xg = 125;
	wheel.yg = 600;
	wheel.v = setvector(0,0);
	wheel.wv = 0;

	wheel2.r = WHEELR;
	wheel2.i = 5 * wheel2.r * wheel2.r;
	wheel2.m = 10;
	wheel2.st = 0;
	wheel2.xg = 375;
	wheel2.yg = 600;
	wheel2.v = setvector(0, 0);
	wheel2.wv = 0;

	head.r = 14;
	head.i = 3125;
	head.m = 0.2;
	head.st = 0;
	head.xg = 375;
	head.yg = 600;
	head.v = setvector(0, 0.002);
	head.wv = 0;

	fuelblock.xg = 1100;
	fuelblock.yg = 750;
	fuelblock.dotnum = 4;
	fuelblock.xcon[0] = -30;
	fuelblock.ycon[0] = -30;
	fuelblock.xcon[1] = 30;
	fuelblock.ycon[1] = -30;
	fuelblock.xcon[2] = 30;
	fuelblock.ycon[2] = 30;
	fuelblock.xcon[3] = -30;
	fuelblock.ycon[3] = 30;
	fuelblock.m = 1;
	fuelblock.i = 2500;
	fuelblock = initxyact(fuelblock);
	
	preleft = 0;
	fueltank = 100;
	test = 0;
	
	
	setfillcolor(YELLOW);
	setlinecolor(RED);
	setlinestyle(PS_SOLID, 3);
	setbkmode(1);
	settextstyle(80,0,_T("Arial Black"));
	settextcolor(RGB(220,220,220));
	//setcolor(YELLOW);
	load();
	BeginBatchDraw();
}



int main() {
	int i;
	char ch = '-';
	int restart = 0;
	double tempx, tempy;
	initgraph(width, high);

	while (1) {
		startup();
		ch = 0;
		//GAMEPLAY%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		while (1) {
			show();
			updwithinput();
			updwithoutinput();
			//camerachange(ply.x-ply.vx,ply.y-ply.vy);
			smocamerachange(ply.xg, ply.yg);

			if (head.v.y != 0.002) {
				break;
			}
			fueltank = fueltank - 0.03;
			if (fueltank <= 0) {
				fueltank = 0;
				for (i = 0;i <= 150;i++) {
					show();
					updwithoutinput();
					smocamerachange(ply.xg, ply.yg);
					if (fueltank == 100) {
						break;
					}
				}
			}
			if (fueltank <= 0) {
				break;
			}
		}

		//END%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		tempx = ply.xg + 10 * ply.v.x;
		tempy = ply.yg + 10 * ply.v.y;
		for (i = 0;i <= 30;i++) {
			show();
			updwithoutinput();
			smocamerachange(tempx, tempy);
		}

		for (i = 0;i <= 50;i++) {
			putimage(0, 800 - 16 * i, 1250, 850, &black, 0, 0);
			FlushBatchDraw();
			Sleep(1);
		}
		settextstyle(100, 0, _T("Arial Black"));
		settextcolor(RGB(240, 240, 240));
		TCHAR exit[20];
		_stprintf_s(exit, _T("GAME OVER :D"));
		outtextxy(320, 200, exit);
		_stprintf_s(exit, _T("PRESS R TO RESTART"));
		outtextxy(180, 400, exit);
		_stprintf_s(exit, _T("PRESS ESC TO EXIT"));
		outtextxy(230, 500, exit);
		settextcolor(RGB(100, 220, 0));
		_stprintf_s(exit, _T("%.0lf m"), ply.xg / 60);
		outtextxy(450, 300, exit);
		settextstyle(200, 0, _T("等线"));
		settextcolor(RGB(255, 0, 0));
		outtextxy(500, 20, _T("寄"));
		
		FlushBatchDraw();
		

		while (1) {

			if (GetAsyncKeyState('R')) {
				restart = 1;
				break;
			}
			if (GetAsyncKeyState(VK_ESCAPE)) {
				restart = 0;
				break;
			}
		}

		if (restart == 1) {
			restart = 0;
		}
		else {
			break;
		}
	}

	//getchar();
	closegraph();
	return 0;
}
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
#define ECOLLIDE 0.2//碰撞恢复系数
#define EFRICTION 0.7//动摩擦因数
#define G 1//重力加速度
#define GND 6;

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
IMAGE head;
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
MOUSEMSG m;

struct vector {
	double x;
	double y;
};

struct player {//player不受旋转力矩的影响
	double x, y,st;//重心坐标,角度
	double w, h;//高，宽   右下x+w y+h 右上x+w y-h左下x-w y+h 左上x-w y-h
	double zsx, zsy,zxx,zxy,ysx,ysy,yxx,yxy;
	vector v;//xy速度
	vector a;//xy加速度
	double wv,wa;//角速度，角加速度
	vector f;
	double fm;//合力，合力矩,力矩为正时，逆时针旋转
	double m;//质量
	double j;//转动惯量
};

struct npc {
	double x, y;
	double h, w;
	double vx, vy;
	double ax, ay;
	double m;
};

struct bullet {

};

struct entity {

};


struct circles {
	double x, y;
	double r;
	double vx, vy;
	double w;
	double m;
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
	double test;
};

struct map {

};

struct camera {
	double x, y;
};

struct ground {//铺设宽度为2000
	double x[500];//一个ground最多拥有500个节点
	double y[500];
};

struct car {
	circles backwheel;
	circles frontwheel;
	dbx carbody;
};

//player ply;
dbx ply;
dbx rec;
dbx dimian;
camera camera;
ground gnd;
vector rp;
double st = 0;



//判断两条线段是否相交的函数
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
ground groundinit(ground gnd) {
	int i;
	for (i = 0;i <= 499;i++) {
		gnd.x[i] = gnd.x[i] + i*GND;
		gnd.y[i] = 800;
	}
	return gnd;
}
ground groundupdate(ground gnd,dbx ply) {
	int i,random;
	while (gnd.x[250] <= ply.xg) {
		for (i = 0;i <= 498;i++) {
			gnd.x[i] = gnd.x[i] + GND;
			gnd.y[i] = gnd.y[i + 1];
		}
		gnd.x[499] = gnd.x[499] + GND;
		gnd.y[499] = gnd.y[499] - rand() %5;
	}
	
	return gnd;
}



void drawdbx(dbx a) {
	int i;
	for (i = 0;i <= a.dotnum - 2;i++) {
		line(a.xact[i] - camera.x, a.yact[i] - camera.y, a.xact[i + 1] - camera.x, a.yact[i + 1] - camera.y);
	}
	line(a.xact[a.dotnum - 1] - camera.x, a.yact[a.dotnum - 1] - camera.y, a.xact[0] - camera.x, a.yact[0] - camera.y);
}



void drawcircles(circles a) {
	circle(a.x, a.y, a.r);
	
}

void drawground(ground gnd){
	int i;
	for (i = 0;i <= 498;i++) {
		line(gnd.x[i] - camera.x, gnd.y[i] - camera.y, gnd.x[i + 1] - camera.x, gnd.y[i + 1] - camera.y);
	}
}

void show() {
	putimage(0,0, 1250, 850, &black, 0, 0);
	putimage(-camera.x, -camera.y, 4000, 1600, &background, 0, 0);
	//bar(ply.x-50 - camera.x, ply.y-50 - camera.y, ply.x - camera.x + 50, ply.y - camera.y + 50);
	loadimage(&gmod, _T("./images/gmod.jpg"), 50, 50, true);
	//rotateimage(&gmod, &gmod,-ply.st,true);
	
	//putimage(ply.xg - 25 - camera.x, ply.yg - 25 - camera.y, 50, 50, &gmod, 0, 0);
	
	circle(-camera.x, -camera.y,3);
	
	drawdbx(ply);
	drawdbx(rec);
	drawdbx(dimian);
	drawground(gnd);
	//line(ply.x - ply.w - camera.x, ply.y + ply.h - camera.y, ply.x - ply.w - camera.x, ply.y - ply.h - camera.y);
	//line(ply.x - ply.w - camera.x, ply.y + ply.h - camera.y, ply.x - ply.w - camera.x, ply.y - ply.h - camera.y);
	//line(ply.x - ply.w - camera.x, ply.y + ply.h - camera.y, ply.x - ply.w - camera.x, ply.y - ply.h - camera.y);
	TCHAR p[20];
	_stprintf_s(p, _T("%f"),ply.xg);
	outtextxy(10,30, p);
	_stprintf_s(p, _T("%f"),ply.yg);
	outtextxy(10, 50, p);
	_stprintf_s(p, _T("%f"),rp.x);
	outtextxy(10, 70, p);
	_stprintf_s(p, _T("%f"), rp.y);
	outtextxy(10, 90, p);
	_stprintf_s(p, _T("%f"), ply.wv);
	outtextxy(10, 130, p);
	_stprintf_s(p, _T("%f"), ply.collidewv);
	outtextxy(10, 150, p);

	FlushBatchDraw();
	Sleep(1);
}

void load() {
	loadimage(&background, _T("./images/background.jpg"), 4000, 1600, true);
	loadimage(&black, _T("./images/black.jpg"), 1250, 850, true);
	loadimage(&facebook, _T("./images/facebook.jpg"), 50, 50, true);
	loadimage(&gmod, _T("./images/gmod.jpg"), 50, 50, true);
	loadimage(&gmod, _T("./images/gmod.jpg"), 50, 50, true);
}

void startup() {
	gnd = groundinit(gnd);
	//测试用，将rec多边形的形状初始化
	rec.xg = 500;
	rec.yg = 100;
	rec.dotnum = 4;
	rec.xcon[0] = -40;
	rec.ycon[0] = -20;
	rec.xcon[1] = 40;
	rec.ycon[1] = -20;
	rec.xcon[2] = 40;
	rec.ycon[2] = 20;
	rec.xcon[3] = -40;
	rec.ycon[3] = 20;
	rec.m = 100;
	rec.i = 8000 * rec.m / 3;
	rec.test = 0;

	dimian.xg = 1000;
	dimian.yg = 800;
	dimian.dotnum = 4;
	dimian.xcon[0] = -100;
	dimian.ycon[0] = -200;
	dimian.xcon[1] = 100;
	dimian.ycon[1] = -200;
	dimian.xcon[2] = 100;
	dimian.ycon[2] = 200;
	dimian.xcon[3] = -100;
	dimian.ycon[3] = 200;
	dimian.m = 400;
	dimian.i = dimian.m * 50000 / 3;

	ply.xcon[0] = -25;
	ply.ycon[0] = -25;
	ply.xcon[1] = 25;
	ply.ycon[1] = -25;
	ply.xcon[2] = 25;
	ply.ycon[2] = 25;
	ply.xcon[3] = -25;
	ply.ycon[3] = 25;
	ply.dotnum = 4;
	ply.m = 100;
	ply.i = ply.m * 1250 / 3;
	initgraph(width, high);
	setfillcolor(YELLOW);
	setlinecolor(RED);
	setlinestyle(PS_SOLID, 3);
	setcolor(YELLOW);
	load();
	BeginBatchDraw();
}

void startmenu() {

}

void pausemenu() {

}

void rotate(double x1, double y1, double x2, double y2, double st) {//1绕2旋转计算
	
	x1 = (x1 - x2) * cos(-st) - (y1 - y2) * sin(-st) + x2;
	
	y1 = (x1 - x2) * sin(-st) + (y1 - y2) * cos(-st) + y2;
}



player drotate(player ply) {//1绕2旋转计算
	ply.zsx = (-ply.w) * cos(-ply.st) - (-ply.h) * sin(-ply.st) + ply.x;
	ply.zsy = (-ply.w) * sin(-ply.st) + (-ply.h) * cos(-ply.st) + ply.y;
	ply.zxx = (-ply.w) * cos(-ply.st) - (+ply.h) * sin(-ply.st) + ply.x;
	ply.zxy = (-ply.w) * sin(-ply.st) + (+ply.h) * cos(-ply.st) + ply.y;
	ply.ysx = (ply.w) * cos(-ply.st) - (-ply.h) * sin(-ply.st) + ply.x;
	ply.ysy = (ply.w) * sin(-ply.st) + (-ply.h) * cos(-ply.st) + ply.y;
	ply.yxx = (ply.w) * cos(-ply.st) - (+ply.h) * sin(-ply.st) + ply.x;
	ply.yxy = (ply.w) * sin(-ply.st) + (+ply.h) * cos(-ply.st) + ply.y;
	return ply;
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

void force() {

}


//碰撞判定与赋值函数  collide and anticollide
dbx anticollide(dbx a, dbx b) {//anti改变b的collide参数而不是a的,其余部分直接照搬collide的所有东西

	vector ra;//a的质心到a的当前研究边节点的向量
	ra = setvector(0, 0);
	vector rafa;//ra的法向量
	vector rb;//b的质心到a的当前研究边节点的向量
	vector rbfa;//rb的法向量
	vector lineb;//b的当前研究边向量
	vector n;//b的当前研究边向量的单位法向量
	vector r;//b的当前研究边向量的单位方向向量
	vector s;//从b的研究边起始点指向a的当前研究边节点的向量
	vector dotspeeda;//a的当前研究边节点对于a质心的速度向量
	vector dotspeedb;//a的当前研究边节点的投影对于b质心的速度向量
	vector temp1;
	vector temp2;
	vector temp3;

	double vr;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j;//i是a边节点个数计数，j是b边向量个数计数
	for (i = 0;i <= a.dotnum - 1;i++) {
		for (j = 0;j <= b.dotnum - 1;j++) {
			//设定要研究的b边向量
			if (j == b.dotnum - 1) {
				lineb = fillvector(b.xact[j], b.yact[j], b.xact[0], b.yact[0]);
			}
			else {
				lineb = fillvector(b.xact[j], b.yact[j], b.xact[j + 1], b.yact[j + 1]);
			}
			//求出该b边向量的单位方向向量和单位法向量
			r = normalize(lineb);
			n = vertical(lineb);
			s = fillvector(b.xact[j], b.yact[j], a.xact[i], a.yact[i]);



			if (dmuti(s, n) <= 0 and dmuti(s, n) >= -15) {

				if (dmuti(s, r) >= 0 and dmuti(s, r) <= sqrt(lineb.x * lineb.x + lineb.y * lineb.y)) {
					ra = fillvector(a.xg, a.yg, a.xact[i], a.yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a.v, muti(rafa, a.wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					rb = fillvector(b.xg, b.yg, a.xact[i], a.yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b.v, muti(rbfa, b.wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(n, plus(dotspeeda, muti(dotspeedb, -1)));
					if (vr < 0 and dmuti(ra, n) <= 0) {//法向相对速度判断
						temp1 = setvector(-ra.y * (ra.x * n.y - ra.y * n.x), ra.x * (ra.x * n.y - n.x * ra.y));
						temp1 = muti(temp1, 1 / a.i);
						temp2 = setvector(-rb.y * (rb.x * n.y - rb.y * n.x), rb.x * (rb.x * n.y - n.x * rb.y));
						temp2 = muti(temp2, 1 / b.i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a.m) + (1 / b.m) + dmuti(n, temp1) + dmuti(n, temp2));
						b.collidev = plus(b.collidev, muti(n, -J / b.m));
						b.collidewv = b.collidewv + (cmuti(rb, muti(n, -J)) / b.i);
						temp3 = muti(n, dmuti(s, n));
						temp3 = muti(temp3, a.m / ((a.m + b.m)*4));
						b.xg = b.xg + temp3.x;
						b.yg = b.yg + temp3.y;
					}
				}
			}



		}
	}

	return b;
}









dbx collide(dbx a,dbx b) {//判断a与b是否发生碰撞，并且只更新a的collide参数
	vector ra;//a的质心到a的当前研究边节点的向量
	ra = setvector(0, 0);
	vector rafa;//ra的法向量
	vector rb;//b的质心到a的当前研究边节点的向量
	vector rbfa;//rb的法向量
	vector lineb;//b的当前研究边向量
	vector n;//b的当前研究边向量的单位法向量
	vector r;//b的当前研究边向量的单位方向向量
	vector s;//从b的研究边起始点指向a的当前研究边节点的向量
	vector dotspeeda;//a的当前研究边节点对于a质心的速度向量
	vector dotspeedb;//a的当前研究边节点的投影对于b质心的速度向量
	vector temp1;
	vector temp2;
	vector temp3;
	
	double vr;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j,k;//i是a边节点个数计数，jk是b边向量个数计数,havedone用于判断是否执行anticollide，havedone=1时不执行
	int havedone = 0;

	for (i = 0;i <= a.dotnum - 1;i++) {
		for (j = 0;j <= b.dotnum - 1;j++) {
			//设定要研究的b边向量
			if (j == b.dotnum - 1) {
				lineb = fillvector(b.xact[j], b.yact[j], b.xact[0], b.yact[0]);
			}
			else {
				lineb = fillvector(b.xact[j], b.yact[j], b.xact[j + 1], b.yact[j + 1]);
			}
			//求出该b边向量的单位方向向量和单位法向量
			r = normalize(lineb);
			n = vertical(lineb);
			s = fillvector(b.xact[j], b.yact[j], a.xact[i], a.yact[i]);
			
			

			if (dmuti(s, n) <= 0 and dmuti(s, n)>=-15) {
				
				if (dmuti(s, r) >= 0 and dmuti(s, r) <= sqrt(lineb.x * lineb.x + lineb.y * lineb.y)) {
					ra = fillvector(a.xg, a.yg, a.xact[i], a.yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a.v,muti(rafa, a.wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					rb = fillvector(b.xg, b.yg, a.xact[i], a.yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b.v,muti(rbfa, b.wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(n, plus(dotspeeda, muti(dotspeedb, -1)));
					if (vr < 0 and dmuti(ra, n) <= 0) {//法向相对速度判断
						temp1 = setvector(-ra.y * (ra.x * n.y - ra.y * n.x), ra.x * (ra.x * n.y - n.x * ra.y));
						temp1 = muti(temp1, 1 / a.i);
						temp2 = setvector(-rb.y * (rb.x * n.y - rb.y * n.x), rb.x * (rb.x * n.y - n.x * rb.y));
						temp2 = muti(temp2, 1 / b.i);
						J = (-vr * (1+ECOLLIDE)) / ((1 / a.m) + (1 / b.m) + dmuti(n, temp1) + dmuti(n, temp2));//这个J实际上是受力大小
						a.collidev = plus(a.collidev, muti(n, J / a.m));
						a.collidewv = a.collidewv + (cmuti(ra, muti(n, J)) / a.i);
						temp3 = muti(n, dmuti(s, n));
						temp3 = muti(temp3, b.m / (a.m + b.m)/4);
						a.xg = a.xg - temp3.x;
						a.yg = a.yg - temp3.y;
						havedone = 1;

						//记得在这里加上xy回退坐标修正法，所有问题将迎刃而解!!!!!!!!!!!
					}
				}
			}

			
			
		}
	}

	if (havedone == 0) {
		a=anticollide(b, a);
	}

	return a;
}

dbx sepaxis(dbx a, dbx b) {//WOW WOW WOW 一个非常成功的碰撞解决函数就此诞生！！！！！
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
	vector temp4;
	vector gline;//质心连线向量
	dbx* td;

	temp3 = setvector(0, 0);
	tempn = setvector(0, 0);
	templiner = setvector(0, 0);
	templine = setvector(0, 0);
	
	double vr;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j, k,l;//i是a边节点个数计数，jk是b边向量个数计数,havedone用于判断是否执行anticollide，havedone=1时不执行
	int havedone = 0;

	for (i = 0;i <= a.dotnum - 1;i++) {//a(i)
		maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
		if (i == a.dotnum - 1) {
			line = fillvector(a.xact[i], a.yact[i], a.xact[0], a.yact[0]);
		}
		else {
			line = fillvector(a.xact[i], a.yact[i], a.xact[i + 1], a.yact[i + 1]);
		}

		//求出该a边向量的单位方向向量和单位法向量
		r = normalize(line);
		n = vertical(line);
		
		for (j = 0;j <= a.dotnum - 1;j++) {//将a的所有点投影到该法向量,并计算最大最小值
			temp4 = fillvector(a.xact[i], a.yact[i], a.xact[j], a.yact[j]);
			if (dmuti(temp4, n) >= maxa) {
				maxa = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= mina) {
				mina = dmuti(temp4, n);
			}
		}

		for (j = 0;j <= b.dotnum - 1;j++) {//将b的所有点投影到a法向量,并计算最大最小值
			temp4 = fillvector(a.xact[i], a.yact[i], b.xact[j], b.yact[j]);
			if (dmuti(temp4, n) >= maxb) {
				maxb = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= minb) {
				minb = dmuti(temp4, n);
			}
		}

		//接下来判断ab最值是否相交，如果否，立刻终止程序
		if (maxa <= minb or maxb <= mina) {
			return a;
		}
		//计算重叠距离,标记当前边
		gline = fillvector(a.xg, a.yg, b.xg, b.yg);
		if (min(abs(maxb - mina), abs(maxa - minb)) <= distance and dmuti(gline,n)>=0) {
			distance = min(abs(maxb - mina), abs(maxa - minb));
			templine = line;
			templiner = r;
			tempn = n;
			tex = a.xact[i];
			tey = a.yact[i];
			type = 0;
		}
	}

	
	//将上述操作翻转，b边对a判断
	for (i = 0;i <= b.dotnum - 1;i++) {//b(i)
		maxa = -10000, mina = 10000, maxb = -10000, minb = 10000;
		if (i == b.dotnum - 1) {
			line = fillvector(b.xact[i], b.yact[i], b.xact[0], b.yact[0]);
		}
		else {
			line = fillvector(b.xact[i], b.yact[i], b.xact[i + 1], b.yact[i + 1]);
		}

		//求出该b边向量的单位方向向量和单位法向量
		r = normalize(line);
		n = vertical(line);

		for (j = 0;j <= b.dotnum - 1;j++) {//将b的所有点投影到该法向量,并计算最大最小值
			temp4 = fillvector(b.xact[i], b.yact[i], b.xact[j], b.yact[j]);
			if (dmuti(temp4, n) >= maxb) {
				maxb = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= minb) {
				minb = dmuti(temp4, n);
			}
		}

		for (j = 0;j <= a.dotnum - 1;j++) {//将a的所有点投影到b法向量,并计算最大最小值
			temp4 = fillvector(b.xact[i], b.yact[i], a.xact[j], a.yact[j]);
			if (dmuti(temp4, n) >= maxa) {
				maxa = dmuti(temp4, n);
			}
			if (dmuti(temp4, n) <= mina) {
				mina = dmuti(temp4, n);
			}
		}

		//接下来判断ab最值是否相交，如果否，立刻终止程序
		if (maxa <= minb or maxb <= mina) {
			return a;
		}
		//计算重叠距离,标记当前边
		gline = fillvector(b.xg, b.yg, a.xg, a.yg);
		if (min(abs(maxb - mina), abs(maxa - minb)) <= distance and dmuti(gline, n) >= 0) {
			//存储所有需要的信息
			distance = min(abs(maxb - mina), abs(maxa - minb));
			templine = line;
			templiner = r;
			tempn = n;
			tex = b.xact[i];
			tey = b.yact[i];
			type = 1;
		}
	}

	//至此，我们确定了碰撞边及其法向量，以及沿着该法向量的跨越距离，之后进行a和b的参数更新即可

	
	if (type == 0) {//说明是a的边，此时遍历一遍b的边节点，a做出参数更新
		for (i = 0;i <= b.dotnum - 1;i++) {
			s = fillvector(tex, tey, b.xact[i], b.yact[i]);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始同时进行a和b的参数更新
					ra = fillvector(a.xg, a.yg, b.xact[i], b.yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a.v, muti(rafa, a.wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					rb = fillvector(b.xg, b.yg, b.xact[i], b.yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b.v, muti(rbfa, b.wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(tempn, plus(dotspeeda, muti(dotspeedb, -1)));
					if (vr > 0) {//法向相对速度判断
						temp1 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp1 = muti(temp1, 1 / a.i);
						temp2 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp2 = muti(temp2, 1 / b.i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a.m) + (1 / b.m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小
						
						a.collidev = plus(a.collidev, muti(tempn, J / a.m));
						a.collidewv = a.collidewv + (cmuti(ra, muti(tempn, J)) / a.i);

					}
				}
			}
		}
	}

	if (type == 1) {//说明是b的边，此时遍历一遍a的边节点，a做出参数更新
		for (i = 0;i <= a.dotnum - 1;i++) {
			s = fillvector(tex, tey, a.xact[i], a.yact[i]);
			if (dmuti(s, tempn) <= 0) {
				if (dmuti(s, templiner) >= 0 and dmuti(s, templiner) <= sqrt(templine.x * templine.x + templine.y * templine.y)) {
					//此处开始同时进行a和b的参数更新
					rb = fillvector(b.xg, b.yg, a.xact[i], a.yact[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b.v, muti(rbfa, b.wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					ra = fillvector(a.xg, a.yg, a.xact[i], a.yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a.v, muti(rafa, a.wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					vr = dmuti(tempn, plus(dotspeedb, muti(dotspeeda, -1)));
					if (vr > 0) {//法向相对速度判断
						temp1 = setvector(-rb.y * (rb.x * tempn.y - rb.y * tempn.x), rb.x * (rb.x * tempn.y - tempn.x * rb.y));
						temp1 = muti(temp1, 1 / b.i);
						temp2 = setvector(-ra.y * (ra.x * tempn.y - ra.y * tempn.x), ra.x * (ra.x * tempn.y - tempn.x * ra.y));
						temp2 = muti(temp2, 1 / a.i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a.m) + (1 / b.m) + dmuti(tempn, temp1) + dmuti(tempn, temp2));//这个J实际上是受力大小
						a.collidev = plus(a.collidev, muti(tempn, -J / a.m));
						a.collidewv = a.collidewv - (cmuti(ra, muti(tempn, J)) / a.i);
						

					}
				}
			}
		}
	}

	//最后，在速度更新完毕后沿着分离轴推离两形状
	if (type == 0) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, b.m / (a.m + b.m));
		a.xg = a.xg - temp3.x;
		a.yg = a.yg - temp3.y;
		
	}
	if (type == 1) {
		temp3 = muti(tempn, distance);
		temp3 = muti(temp3, b.m / (a.m + b.m));
		a.xg = a.xg + temp3.x;
		a.yg = a.yg + temp3.y;
	}
	return a;
}



dbx groundanticollide(ground a, dbx b) {//anti改变b的collide参数而不是a的,其余部分直接照搬collide的所有东西

	vector ra;//a的质心到a的当前研究边节点的向量
	ra = setvector(0, 0);
	vector rafa;//ra的法向量
	vector rb;//b的质心到a的当前研究边节点的向量
	vector rbfa;//rb的法向量
	vector lineb;//b的当前研究边向量
	vector n;//b的当前研究边向量的单位法向量
	vector r;//b的当前研究边向量的单位方向向量
	vector s;//从b的研究边起始点指向a的当前研究边节点的向量
	vector dotspeeda;//a的当前研究边节点对于a质心的速度向量
	vector dotspeedb;//a的当前研究边节点的投影对于b质心的速度向量
	vector temp1;
	vector temp2;
	vector temp3;

	double vr;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j;//i是a边节点个数计数，j是b边向量个数计数
	for (i = 100;i <= 399;i++) {
		for (j = 0;j <= b.dotnum - 1;j++) {
			//设定要研究的b边向量
			if (j == b.dotnum - 1) {
				lineb = fillvector(b.xact[j], b.yact[j], b.xact[0], b.yact[0]);
			}
			else {
				lineb = fillvector(b.xact[j], b.yact[j], b.xact[j + 1], b.yact[j + 1]);
			}
			//求出该b边向量的单位方向向量和单位法向量
			r = normalize(lineb);
			n = vertical(lineb);
			s = fillvector(b.xact[j], b.yact[j], a.x[i], a.y[i]);



			if (dmuti(s, n) <= 0 and dmuti(s, n) >= -15) {

				if (dmuti(s, r) >= 0 and dmuti(s, r) <= sqrt(lineb.x * lineb.x + lineb.y * lineb.y)) {
					ra = fillvector(0, 2000, a.x[i], a.y[i]);
					rafa = vertical(ra);
					
					rb = fillvector(b.xg, b.yg, a.x[i], a.y[i]);
					rbfa = vertical(rb);
					dotspeedb = plus(b.v, muti(rbfa, b.wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(n, muti(dotspeedb, -1));
					if (vr < 0 and dmuti(ra, n) <= 0) {//法向相对速度判断
						
						temp2 = setvector(-rb.y * (rb.x * n.y - rb.y * n.x), rb.x * (rb.x * n.y - n.x * rb.y));
						temp2 = muti(temp2, 1 / b.i);
						J = (-vr * (1 + ECOLLIDE)) / ((1 / b.m) + dmuti(n, temp2));
						b.collidev = plus(b.collidev, muti(n, -J / b.m));
						b.collidewv = b.collidewv + (cmuti(rb, muti(n, -J)) / b.i);
						temp3 = muti(n, dmuti(s, n));
						temp3 = muti(temp3, 1/4);
						b.xg = b.xg + temp3.x;
						b.yg = b.yg + temp3.y;
					}
				}
			}



		}
	}

	return b;
}

dbx groundcollide(dbx a, ground gnd) {
	vector ra;//a的质心到a的当前研究边节点的向量
	ra = setvector(0, 0);
	vector rafa;//ra的法向量
	vector rb;//b的质心到a的当前研究边节点的向量
	vector rbfa;//rb的法向量
	vector lineb;//b的当前研究边向量
	vector n;//b的当前研究边向量的单位法向量
	vector r;//b的当前研究边向量的单位方向向量
	vector s;//从b的研究边起始点指向a的当前研究边节点的向量
	vector dotspeeda;//a的当前研究边节点对于a质心的速度向量
	vector dotspeedb;//a的当前研究边节点的投影对于b质心的速度向量
	vector temp1;
	vector temp2;
	vector temp3;

	double vr;//a的当前研究边节点与b的当前研究边向量间的法向相对速度
	double J;//计算得出最终的法向冲量

	int i, j, k;//i是a边节点个数计数，jk是b边向量个数计数,havedone用于判断是否执行anticollide，havedone=1时不执行
	int havedone = 0;

	for (i = 0;i <= a.dotnum - 1;i++) {
		for (j = 100;j <= 399;j++) {
			//设定要研究的gnd边向量
			
			lineb = fillvector(gnd.x[j], gnd.y[j], gnd.x[j + 1], gnd.y[j + 1]);
			
			//求出该gnd边向量的单位方向向量和单位法向量
			r = normalize(lineb);
			n = vertical(lineb);
			s = fillvector(gnd.x[j], gnd.y[j], a.xact[i], a.yact[i]);



			if (dmuti(s, n) <= 0 and dmuti(s, n) >= -15) {

				if (dmuti(s, r) >= 0 and dmuti(s, r) <= sqrt(lineb.x * lineb.x + lineb.y * lineb.y)) {
					ra = fillvector(a.xg, a.yg, a.xact[i], a.yact[i]);
					rafa = vertical(ra);
					dotspeeda = plus(a.v, muti(rafa, a.wv * sqrt(ra.x * ra.x + ra.y * ra.y)));
					rb = fillvector(0, 2000, a.xact[i], a.yact[i]);
					rbfa = vertical(rb);
					//dotspeedb = plus(b.v, muti(rbfa, b.wv * sqrt(rb.x * rb.x + rb.y * rb.y)));
					vr = dmuti(n,dotspeeda);
					if (vr < 0 and dmuti(ra, n) <= 0) {//法向相对速度判断
						temp1 = setvector(-ra.y * (ra.x * n.y - ra.y * n.x), ra.x * (ra.x * n.y - n.x * ra.y));
						temp1 = muti(temp1, 1 / a.i);
						
						J = (-vr * (1 + ECOLLIDE)) / ((1 / a.m) + dmuti(n, temp1));//这个J实际上是受力大小
						a.collidev = plus(a.collidev, muti(n, J / a.m));
						a.collidewv = a.collidewv + (cmuti(ra, muti(n, J)) / a.i);
						temp3 = muti(n, dmuti(s, n));
						temp3 = muti(temp3, 1 / 4);
						a.xg = a.xg - temp3.x;
						a.yg = a.yg - temp3.y;
						havedone = 1;

						//记得在这里加上xy回退坐标修正法，所有问题将迎刃而解!!!!!!!!!!!
					}
				}
			}



		}
	}

	if (havedone == 0) {
		a = groundanticollide(gnd, a);
	}

	return a;
	
}

dbx dbxphyupd(dbx ply) {
	ply.fm = 0;
	ply.f = setvector(0, G*ply.m);
	ply.a = muti(ply.f, 1 / ply.m);
	ply.wa = ply.fm / ply.i;
	ply.v = plus(ply.v, ply.a);
	ply.v = plus(ply.v, ply.collidev);
	ply.wv = ply.wv + ply.wa;
	ply.wv = ply.wv + ply.collidewv;
	ply.xg = ply.xg + ply.v.x;
	ply.yg = ply.yg + ply.v.y;
	ply.st = ply.st + ply.wv;//角度更新

	ply = dbxrotate(ply);


	ply.v.x = ply.v.x - 0.06 * ply.v.x;//阻尼测试，在受力模块完善后应该删除掉。
	ply.v.y = ply.v.y - 0.03 * ply.v.y;//阻尼测试，在受力模块完善后应该删除掉。


	if (ply.v.x <= 0.0001 and ply.v.x > 0) {
		ply.v.x = 0;
	}
	if (ply.v.y <= 0.0001 and ply.v.y > 0) {
		ply.v.y = 0;
	}


	
	//在每次物理更新的最后需要把collide参数重置，才能进行下一个dt的运算
	ply.collidev = setvector(0, 0);
	ply.collidewv = 0;

	if (ply.yg > 1570) {
		ply.yg = 1570;
		ply.v.y = 0;
	}
	return ply;
}


player playerphyupd(player ply) {
	force();
	ply.fm = 10;
	ply.f = setvector(0, 1 * ply.m);
	ply.a = muti(ply.f, 1 / ply.m);
	ply.wa = ply.fm / ply.j;
	ply.v = plus(ply.v, ply.a);
	ply.wv = ply.wv + ply.wa;
	ply.x = ply.x + ply.v.x;
	ply.y = ply.y + ply.v.y;
	ply.st = ply.st + ply.wv;//角度更新

	ply=drotate(ply);


	ply.v.x = ply.v.x - 0.06 * ply.v.x;//阻尼测试，在受力模块完善后应该删除掉。
	ply.v.y = ply.v.y - 0.03 * ply.v.y;//阻尼测试，在受力模块完善后应该删除掉。
	

	if (ply.v.x <= 0.0001 and ply.v.x > 0) {
		ply.v.x = 0;
	}
	if (ply.v.y <= 0.0001 and ply.v.y > 0) {
		ply.v.y = 0;
	}
	

	if (ply.y > 1570) {
		ply.y = 1570;
		ply.v.y = 0;
	}

	/*if (cross(0, 100, 1000, 500, ply.x + 25, ply.y + 25, ply.x - 25, ply.y - 25) == 1) {
		ply.vy = 0;
	}*/
	return ply;
}

void npcphyupd() {

}

void entityphyupd() {

}

void bulletphyupd() {

}


void updwithinput() {

	


	if (GetAsyncKeyState(VK_LEFT)){
		
		ply.v.x=ply.v.x-1;
	}

	if (GetAsyncKeyState(VK_RIGHT)){
		ply.v.x = ply.v.x + 1;
		
	}

	
	if (GetAsyncKeyState(VK_UP)){
		ply.v.y=ply.v.y-1;
		
	}
	
	if (GetAsyncKeyState(VK_DOWN)){
		ply.v.y = ply.v.y + 1;
		
	}
	if (GetAsyncKeyState(VK_SPACE)) {
		ply.v.y = -16;

	}
	if (GetAsyncKeyState('W')) {
		rotate(ply.xg,ply.yg,0,0,1);
		//drotate(st);
		st = st + 0.1;
		
	}
}

void updwithoutinput() {//without为物理约束模块,在一个dt间的事件
	rec=sepaxis(rec, dimian);
	dimian=sepaxis(dimian,rec);
	ply = sepaxis(ply, rec);
	rec = sepaxis(rec, ply);
	ply = sepaxis(ply, dimian);
	dimian = sepaxis(dimian, ply);
	//dimian = collide(dimian, ply);
	//ply = groundcollide(ply, gnd);


	ply=dbxphyupd(ply);
	rec = dbxphyupd(rec);
	dimian= dbxphyupd(dimian);

	gnd = groundupdate(gnd,ply);
	npcphyupd();
	entityphyupd();
	bulletphyupd();
	Sleep(1);
}


int main() {
	double x=200, y=100;
	ply.xg = 50;ply.yg = 50;
	printf("666");
	startup();

	
	//putimage(500,100,100,100,&gmod,20,20, NOTSRCERASE);
	//putimage(2, 2, 100, 100, &gmod, 50, 50, SRCINVERT);
	//putimage(0, 0, 4000, 1600, &background, 0, 0);//右下，左上
	//bar(200, 100, 300, 300);
	

	while (1) {
		show();
		updwithinput();
		updwithoutinput();
		//camerachange(ply.x-ply.vx,ply.y-ply.vy);
		smocamerachange(ply.xg, ply.yg);
	}

	FlushBatchDraw();

	getchar();
	closegraph();
	return 0;
}
clear all;
clc;
filename = 'data.txt';  
data = load(filename);

y = data(:, 1);
x = data(:, 2);
z = data(:, 3);
 
 
%%%%%%%%%%%%%空间二次曲面拟合算法
 
%数据长度
num_points = length(x);
 
%一次项统计平均
x_avr =sum(x)/num_points ;
y_avr =sum(y)/num_points ;
z_avr =sum(z)/num_points ;
%二次项统计平均
xx_avr =sum(x.*x)/num_points;
yy_avr =sum(y.*y)/num_points;
zz_avr =sum(z.*z)/num_points;
xy_avr =sum(x.*y)/num_points;
xz_avr =sum(x.*z)/num_points;
yz_avr =sum(y.*z)/num_points;
%三次项统计平均
xxx_avr =sum(x.*x.*x)/num_points;
xxy_avr =sum(x.*x.*y)/num_points;
xxz_avr =sum(x.*x.*z)/num_points;
xyy_avr =sum(x.*y.*y)/num_points;
xzz_avr =sum(x.*z.*z)/num_points;
yyy_avr =sum(y.*y.*y)/num_points;
yyz_avr =sum(y.*y.*z)/num_points;
yzz_avr =sum(y.*z.*z)/num_points;
zzz_avr =sum(z.*z.*z)/num_points;
%四次项统计平均
yyyy_avr =sum(y.*y.*y.*y)/num_points;
zzzz_avr =sum(z.*z.*z.*z)/num_points;
xxyy_avr =sum(x.*x.*y.*y)/num_points;
xxzz_avr =sum(x.*x.*z.*z)/num_points;
yyzz_avr =sum(y.*y.*z.*z)/num_points;
 
%计算求解线性方程的系数矩阵
A0 = [yyyy_avr yyzz_avr xyy_avr yyy_avr yyz_avr yy_avr;
      yyzz_avr zzzz_avr xzz_avr yzz_avr zzz_avr zz_avr;
      xyy_avr  xzz_avr  xx_avr  xy_avr  xz_avr  x_avr;
      yyy_avr  yzz_avr  xy_avr  yy_avr  yz_avr  y_avr;
      yyz_avr  zzz_avr  xz_avr  yz_avr  zz_avr  z_avr;
      yy_avr   zz_avr   x_avr   y_avr   z_avr   1;];
%计算非齐次项
b = [-xxyy_avr;
     -xxzz_avr;
     -xxx_avr;
     -xxy_avr;
     -xxz_avr;
     -xx_avr];
 
%求拟合系数
A0_inv=inv(A0);
resoult =A0_inv *b;
 
x00 = -resoult(3)/2;                  %拟合出的x坐标
y00 = -resoult(4)/(2*resoult(1));     %拟合出的y坐标
z00 = -resoult(5)/(2*resoult(2));     %拟合出的z坐标
 
AA = sqrt(x00*x00 + resoult(1)*y00*y00 + resoult(2)*z00*z00 - resoult(6));   % 拟合出的x方向上的轴半径
BB = AA/sqrt(resoult(1));                                                    % 拟合出的y方向上的轴半径
CC = AA/sqrt(resoult(2));                                                    % 拟合出的z方向上的轴半径
 
fprintf('拟合结果\n');
fprintf('x0 = %f \n',x00);
fprintf('y0 = %f \n',y00);
fprintf('z0 = %f \n',z00);
fprintf('A = %f  \n',AA);
fprintf('B = %f  \n',BB);
fprintf('C = %f  \n',CC);
 
%%拟合出的椭球面数据生成
A = AA;     % x方向上的轴半径
B = BB;     % y方向上的轴半径
C = CC;     % z方向上的轴半径
x0 = x00;   %椭球球心x坐标
y0 = y00;   %椭球球心y坐标
z0 = z00;   %椭球球心z坐标
num_alfa = 100;
num_sita = 50;
alfa = (0:num_alfa-1)*1*pi/num_alfa;
sita = (0:num_sita-1)*2*pi/num_sita;
X = zeros(num_alfa,num_sita);
Y = zeros(num_alfa,num_sita);
Z = zeros(num_alfa,num_sita);
for i = 1:num_alfa
    for j = 1:num_sita
        X(i,j) = x0 + A*sin(alfa(i))*cos(sita(j));
        Y(i,j) = y0 + B*sin(alfa(i))*sin(sita(j));
        Z(i,j) = z0 + C*cos(alfa(i));
    end
end
X = reshape(X,num_alfa*num_sita,1);    %转换成一维的数组便于后续的处理
Y = reshape(Y,num_alfa*num_sita,1);
Z = reshape(Z,num_alfa*num_sita,1);
 
%%实测、拟合对比
figure;
plot3(x,y,z,'*');
hold on;
plot3(X,Y,Z,'*');
title('实测、拟合对比');
 
%%xy投影对比
figure;
plot(x,y,'*');
hold on;
plot(X,Y,'*');
title('xy投影');
 
%%xz投影对比
figure;
plot(x,z,'*');
hold on;
plot(X,Z,'*');
title('xz投影');
 
%%yz投影对比
figure;
plot(y,z,'*');
hold on;
plot(Y,Z,'*');
title('yz投影');
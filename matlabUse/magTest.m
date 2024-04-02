% 读取 txt 文件中的数据
filename = 'data.txt';  
data = load(filename);

column1 = data(:, 1);
column2 = data(:, 2);
column3 = data(:, 3);

figure(1)
scatter3(column1,column2,column3);
figure(2)
plot(column1,column2);
figure(3)
plot(column1,column3);

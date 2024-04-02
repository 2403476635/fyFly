clear;clc;
filename = 'output.xlsx';  % 请将文件名替换为实际的 Excel 文件名
data = xlsread(filename);

% 将数据分为三列
column1 = data(:, 1);
column2 = data(:, 2);
column3 = data(:, 3);

t=0:0.005:5;
Fs=200;%采样频率
T=1/Fs;%时间间隔

[filtered_signal,filtb,filta]=lopass_butterworth(column1,45,Fs,4);
figure(1)
subplot(6,1,1)
plot(column1)
title('原始信号')
subplot(6,1,2)
plot(filtered_signal)
title('滤波后信号')

subplot(6,1,3)
plot(column1, 'b')  % 绘制原始信号
hold on
plot(filtered_signal, 'r')  % 绘制滤波后的信号
hold off
title('Original and Filtered Signals')
xlabel('Sample')
ylabel('Amplitude')
legend('Original', 'Filtered')


L = 4000; 

Y = fft(column1);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Fs/L*(0:(L/2));

subplot(6,1,4)
plot(f,P1,"LineWidth",3) 
title("Single-Sided Amplitude Spectrum of S(t)")
xlabel("f (Hz)")
ylabel("|P1(f)|")

a1 = P1;

Y = fft(filtered_signal);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

subplot(6,1,5)
plot(f,P1,"LineWidth",3) 
title("Single-Sided Amplitude Spectrum of S(t)")
xlabel("f (Hz)")
ylabel("|P1(f)|")

a2 = P1;

subplot(6,1,6)
plot(f,a1, 'b')  % 绘制原始信号
hold on
plot(f,a2, 'r')  % 绘制滤波后的信号
hold off
title('Original and Filtered Signals')
xlabel('Sample')
ylabel('Amplitude')
legend('Original', 'Filtered')
